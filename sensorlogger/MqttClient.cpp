/*
 * Tinkerforge Sensorlogger - Logging Tinkerforge sensor values via MQTT
 * Copyright (C) 2018 Adrian Winterstein
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "MqttClient.h"

#include <mosquitto.h>
#include <spdlog/spdlog.h>

struct MosquittoCallbacks {

    static void on_connect_wrapper(struct mosquitto*, void* userdata, int rc)
    {
        static_cast<MqttClient*>(userdata)->onConnect(rc);
    }

    static void on_disconnect_wrapper(struct mosquitto*, void* userdata, int rc)
    {
        static_cast<MqttClient*>(userdata)->onDisconnect(rc);
    }

    static void on_publish_wrapper(struct mosquitto*, void* userdata, int mid)
    {
        static_cast<MqttClient*>(userdata)->onPublish(mid);
    }

    static void on_message_wrapper(struct mosquitto*, void* userdata, const struct mosquitto_message* message)
    {
        static_cast<MqttClient*>(userdata)->onMessage(message);
    }

    static void on_subscribe_wrapper(struct mosquitto*, void* userdata, int mid, int qos_count, const int* granted_qos)
    {
        static_cast<MqttClient*>(userdata)->onSubscribe(mid, qos_count, granted_qos);
    }

    static void on_unsubscribe_wrapper(struct mosquitto*, void *userdata, int mid)
    {
        static_cast<MqttClient*>(userdata)->onUnsubscribe(mid);
    }

    static void on_log_wrapper(struct mosquitto*, void *userdata, int level, const char *str)
    {
        static_cast<MqttClient*>(userdata)->onLog(level, str);
    }
};

MqttClient::MqttClient(const Configuration& configuration)
    : m_keepalive(configuration.keepalive)
{
    m_mosq = mosquitto_new(configuration.id.c_str(), true, this);
    if (m_mosq == nullptr)
    {
        if (spdlog::get("mqtt"))
        {
            spdlog::get("mqtt")->info("MQTT client error on creating an instance: {}.", strerror(errno));
        }
    }

    mosquitto_connect_callback_set(m_mosq, MosquittoCallbacks::on_connect_wrapper);
    mosquitto_disconnect_callback_set(m_mosq, MosquittoCallbacks::on_disconnect_wrapper);
    mosquitto_publish_callback_set(m_mosq, MosquittoCallbacks::on_publish_wrapper);
    mosquitto_message_callback_set(m_mosq, MosquittoCallbacks::on_message_wrapper);
    mosquitto_subscribe_callback_set(m_mosq, MosquittoCallbacks::on_subscribe_wrapper);
    mosquitto_unsubscribe_callback_set(m_mosq, MosquittoCallbacks::on_unsubscribe_wrapper);
    mosquitto_log_callback_set(m_mosq, MosquittoCallbacks::on_log_wrapper);

    // TODO: check return value
    mosquitto_lib_init();                                             // Initialize libmosquitto

    if (!configuration.user.empty())
    {
        mosquitto_username_pw_set(m_mosq, configuration.user.c_str(), configuration.password.c_str());                  // Set username and password before connecting
    }

    // TODO: check return value
    mosquitto_connect(m_mosq, configuration.broker.c_str(), configuration.port, static_cast<int>(configuration.keepalive.count())); // Connect to MQTT Broker
}

MqttClient::~MqttClient()
{
    mosquitto_disconnect(m_mosq);
    mosquitto_lib_cleanup(); // TODO: check return value

    stop();
    join();
}

void MqttClient::run()
{
    m_running = true;

    m_thread = std::thread([this](){
        while(m_running)
        {
            const auto result = mosquitto_loop(m_mosq, -1, 1);

            // try to reconnect if an error happened
            if (result != MOSQ_ERR_SUCCESS)
            {
                if (spdlog::get("mqtt")) { spdlog::get("mqtt")->info("MQTT client error: {} Trying to reconnect.", errorCodeToString(result)); }
                mosquitto_reconnect(m_mosq);
            }
        }
    });
}

void MqttClient::stop()
{
    m_running = false;
}

void MqttClient::join()
{
    m_thread.join();
}

int MqttClient::subscribe(const std::string& subscription_pattern, int qos, MessageCallback callback)
{
    return subscribe(nullptr, subscription_pattern, qos, callback);
}

int MqttClient::unsubscribe(const std::string& subscription_pattern)
{
    return unsubscribe(nullptr, subscription_pattern);
}

void MqttClient::onConnect(int rc)
{
    if (spdlog::get("mqtt")) { spdlog::get("mqtt")->info("MQTT client connected with result: {}", errorCodeToString(rc)); }
}

void MqttClient::onDisconnect(int rc)
{
    if (spdlog::get("mqtt")) { spdlog::get("mqtt")->warn("MQTT client disconnected with result: {}", errorCodeToString(rc)); }
}

void MqttClient::onPublish(int /*mid*/)
{

}

void MqttClient::onMessage(const struct mosquitto_message *message)
{
    // find callback configuration for the given topic and call the corresponding callback
    const auto it = std::find_if(m_messageCallbacks.begin(), m_messageCallbacks.end(), [&message](const MessageCallbackConfiguration& messageCallback){
        bool matches;
        const auto result = mosquitto_topic_matches_sub(message->topic, messageCallback.pattern.c_str(), &matches);
        return result == MOSQ_ERR_SUCCESS && matches;
    });

    if (it != m_messageCallbacks.end())
    {
        it->callback({message->topic, std::string(static_cast<char*>(message->payload), static_cast<size_t>(message->payloadlen))});
    }
}

void MqttClient::onSubscribe(int /*mid*/, int /*qos_count*/, const int */*granted_qos*/)
{

}

void MqttClient::onUnsubscribe(int /*mid*/)
{

}

void MqttClient::onLog(int level, const char *str)
{
    if (!spdlog::get("mqtt")) { return; }

    switch(level)
    {
    case MOSQ_LOG_DEBUG:
        spdlog::get("mqtt")->debug(str);
        break;
    case MOSQ_LOG_NOTICE:
        //spdlog::get("mqtt")->notice(str);
        break;
    case MOSQ_LOG_INFO:
        spdlog::get("mqtt")->info(str);
        break;
    case MOSQ_LOG_WARNING:
        spdlog::get("mqtt")->warn(str);
        break;
    case MOSQ_LOG_ERR:
    default:
        spdlog::get("mqtt")->error(str);
        break;
    }
}

int MqttClient::publish(int *mid, const std::string& topic, const std::string& payload, int qos, bool retain)
{
    return mosquitto_publish(m_mosq, mid, topic.c_str(),
                             static_cast<int>(payload.length()), payload.c_str(),
                             qos, retain);
}

int MqttClient::subscribe(int *mid, const std::string& subscription_pattern, int qos, MessageCallback callback)
{
    const auto subscribe_result = mosquitto_subscribe(m_mosq, mid, subscription_pattern.c_str(), qos);
    if (subscribe_result == MOSQ_ERR_SUCCESS)
    {
        // add callback configuration to the callbacks vector
        m_messageCallbacks.emplace_back(subscription_pattern, callback);
    }

    return subscribe_result;
}

int MqttClient::unsubscribe(int *mid, const std::string& subscription_pattern)
{
    const auto unsubscribe_result = mosquitto_unsubscribe(m_mosq, mid, subscription_pattern.c_str());
    if (unsubscribe_result == MOSQ_ERR_SUCCESS)
    {
        // find callback configuration for given pattern and remove it from the callbacks vector
        const auto it = std::find_if(m_messageCallbacks.begin(), m_messageCallbacks.end(), [&subscription_pattern](const MessageCallbackConfiguration& messageCallback){
            return messageCallback.pattern == subscription_pattern;
        });
        if (it != m_messageCallbacks.end()) { m_messageCallbacks.erase(it); }
    }

    return unsubscribe_result;
}

const char* MqttClient::errorCodeToString(int error)
{
    return mosquitto_strerror(error);
}
