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

#include <algorithm>
#include <spdlog/spdlog.h>

#include <tinkerforge/BrickletTemperature.h>
#include <tinkerforge/BrickletHumidity.h>
#include <tinkerforge/BrickletAmbientLight.h>
#include <tinkerforge/BrickletDistanceIr.h>

using namespace tinkerforge;
using namespace std::placeholders;

SensorLogger::SensorLogger(std::string topic, std::unique_ptr<MqttClient> mqttClient)
    : m_topic(std::move(topic))
    , m_mqttClient(std::move(mqttClient))
{
    m_sensorsConnection.setEnumerateCallback(
                std::bind(&SensorLogger::enumerationCallback, this, _1, _2, _3));

    if (m_topic.back() != '/') { m_topic.push_back('/'); }
}

void SensorLogger::run()
{   
    m_mqttClient->run();
    m_sensorsConnection.joinThread();
}

void SensorLogger::enumerationCallback(const char *uid, uint16_t device_identifier, uint8_t enumeration_type)
{
    // Workaround for a strange character at the end of the
    // uid, when this is compiled for 32 bit.
    std::string s = std::string(uid);
    if (s.length() > 3)
    {
        s.erase(s.end()-1);
        uid = const_cast<char*>(s.c_str());
    }

    if (enumeration_type >= 2)
    {
        // Remove the entry from the container
        const auto it = std::find_if(m_sensors.begin(), m_sensors.end(),
                                     [&uid](const std::unique_ptr<AbstractSensor>& b) { return *b == AbstractSensor::UID(uid); });
        if (it != m_sensors.end()) {
            if (spdlog::get("main")) { spdlog::get("main")->info("Sensor '{}' was removed.", (*it)->type()); }
            m_sensors.erase(it);
        }
    }
    else
    {
        std::unique_ptr<AbstractSensor> sensor = nullptr;

        if (device_identifier == BrickletTemperature::DeviceIdentifier())
        {
            sensor = std::make_unique<BrickletTemperature>(uid, m_sensorsConnection);
        }
        else if (device_identifier == BrickletHumidity::DeviceIdentifier())
        {
            sensor = std::make_unique<BrickletHumidity>(uid, m_sensorsConnection);
        }
        else if (device_identifier == BrickletAmbientLight::DeviceIdentifier())
        {
            sensor = std::make_unique<BrickletAmbientLight>(uid, m_sensorsConnection);
        }
        else if (device_identifier == BrickletDistanceIr::DeviceIdentifier())
        {
            sensor = std::make_unique<BrickletDistanceIr>(uid, m_sensorsConnection);
        }

        if (sensor)
        {
            sensor->registerCallback([this](const std::string& type, int32_t value){
                m_mqttClient->publish(m_topic+type, value, 0, true);
            });

            if (spdlog::get("main")) { spdlog::get("main")->info("Sensor '{}' was added.", sensor->type()); }
            m_sensors.push_back(std::move(sensor));
        }
    }
}
