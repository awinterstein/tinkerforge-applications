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

#ifndef SENSORLOGGER_H
#define SENSORLOGGER_H

#include <vector>
#include <memory>

#include <tinkerforge/AbstractSensor.h>
#include <tinkerforge/ConnectionHandler.h>

#include "MqttClient.h"

#ifndef __cpp_lib_make_unique
namespace std {
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}
#endif

class SensorLogger
{
public:
    SensorLogger(std::string topic, std::unique_ptr<MqttClient> mqttClient);

    void run();

private:
    void enumerationCallback(const char *uid, uint16_t device_identifier, uint8_t enumeration_type);

    std::string                                               m_topic;

    tinkerforge::ConnectionHandler                            m_sensorsConnection;
    std::vector<std::unique_ptr<tinkerforge::AbstractSensor>> m_sensors;
    std::unique_ptr<MqttClient>                               m_mqttClient;
};

#endif // SENSORLOGGER_H
