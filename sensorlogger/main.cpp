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

#include "SensorLogger.h"

#include <iostream>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

bool checkParameters(const MqttClient::Configuration& config, std::string& errorMessage)
{
    if (config.id.empty())
    {
        errorMessage = "client id not set";
        return false;
    }

    if (config.broker.empty())
    {
        errorMessage = "broker address not set";
        return false;
    }

    return true;
}

bool checkTopic(const std::string& topic, std::string& errorMessage)
{
    if (topic.empty())
    {
        errorMessage = "topic not set";
        return false;
    }

    return true;
}

inline std::shared_ptr<spdlog::logger> createLogger(const std::string& logger_name, bool stdout)
{
    if (stdout)
    {
        return spdlog::create(logger_name, {spdlog::sinks::stdout_sink_mt::instance(), std::make_shared<spdlog::sinks::syslog_sink>()});
    }
    else
    {
        return spdlog::create(logger_name, {std::make_shared<spdlog::sinks::syslog_sink>()});
    }
}

int main(int argc, char** argv)
{
    MqttClient::Configuration mqttConfig;
    std::string mqttTopic;

    // Declare the supported command line options.
    po::options_description desc("Command line options");
    desc.add_options()
        ("help", "Shows this help message")
        ("quiet,q", "Don't print to the standard output")
        ("id,i", po::value<std::string>(&mqttConfig.id), "MQTT client id")
        ("host,h", po::value<std::string>(&mqttConfig.broker), "MQTT broker address")
        ("port,p", po::value<uint16_t>(&mqttConfig.port), "MQTT broker port")
        ("topic,t", po::value<std::string>(&mqttTopic), "MQTT topic")
        ("user,u", po::value<std::string>(&mqttConfig.user), "MQTT user name")
        ("password,P", po::value<std::string>(&mqttConfig.password), "MQTT password")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << std::endl;
        return 0;
    }

    std::string errorMessage;
    if (!checkParameters(mqttConfig, errorMessage) || !checkTopic(mqttTopic, errorMessage))
    {
        std::cout << "Wrong command line parameters used (" << errorMessage << ").\n" << std::endl;
        std::cout << desc << std::endl;
        return 1;
    }


    createLogger("main", !vm.count("quiet"));
    createLogger("mqtt", !vm.count("quiet"));

    auto mqttClient = std::make_unique<MqttClient>(mqttConfig);
    SensorLogger(mqttTopic, std::move(mqttClient)).run();
    return 0;
}
