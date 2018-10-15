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

#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include <sstream>
#include <vector>

class MqttClient {
public:
    struct Configuration {
        std::string          id;
        std::string          broker;
        uint16_t             port      {1883};
        std::chrono::seconds keepalive {10};
        std::string          user;
        std::string          password;
    };

    using MessageCallback = std::function<void(std::string topic, std::string message)>;

    MqttClient(const Configuration& configuration);
    ~MqttClient();

    void run();
    void stop();
    void join();

    template<typename T>
    int publish(const std::string& topic, const T& payload, int qos, bool retain)
    {
        std::stringstream sstream;
        sstream << payload;
        const std::string payloadStr = sstream.str();

        return publish(nullptr, topic, payloadStr, qos, retain);
    }

    int subscribe(const std::string& subscription_pattern, int qos, MessageCallback callback);
    int unsubscribe(const std::string& subscription_pattern);

    void onConnect(int rc);
    void onDisconnect(int rc);
    void onPublish(int mid);
    void onMessage(const struct mosquitto_message *message);
    void onSubscribe(int mid, int qos_count, const int *granted_qos);
    void onUnsubscribe(int mid);
    void onLog(int level, const char *str);

private:
    int publish(int *mid, const std::string& topic, const std::string& payload, int qos, bool retain);
    int subscribe(int *mid, const std::string& subscription_pattern, int qos, MessageCallback callback);
    int unsubscribe(int *mid, const std::string& subscription_pattern);
    const char* errorCodeToString(int error);

    struct mosquitto*    m_mosq;

    std::chrono::seconds m_keepalive;
    std::thread          m_thread;
    bool                 m_running = false;

    struct MessageCallbackConfiguration {
        std::string          pattern;
        MessageCallback      callback;

        MessageCallbackConfiguration(std::string pattern, MessageCallback callback)
            : pattern(pattern), callback(callback) {}
    };

    std::vector<MessageCallbackConfiguration> m_messageCallbacks;
};


#endif // MQTTCLIENT_H
