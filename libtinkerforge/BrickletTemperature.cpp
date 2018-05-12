/*
 * Libtinkerforge - Object oriented library for tinkerforge c binings
 * Copyright (C) 2013 Adrian Winterstein
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

#include <tinkerforge/BrickletTemperature.h>

extern "C" {
#include "bindings/bricklet_temperature.h"
}

namespace tinkerforge {

void temperatureCallback(int16_t curTemperature, void* object)
{
    static_cast<BrickletTemperature*>(object)->temperatureUpdated(curTemperature);
}

BrickletTemperature::BrickletTemperature(const char* uid, ConnectionHandler &connection)
    : AbstractSensor(uid)
    , m_tolerance(10)
{
    temperature_create(&m_temperature, uid, connection.getConnection());
}

BrickletTemperature::~BrickletTemperature ()
{
    temperature_destroy(&m_temperature);
}

uint32_t BrickletTemperature::DeviceIdentifier()
{
    return TEMPERATURE_DEVICE_IDENTIFIER;
}

const std::string& BrickletTemperature::type() const
{
    static std::string type("temperature");
    return type;
}

int16_t BrickletTemperature::getTemperature() {
	int16_t temperatureValue;
    temperature_get_temperature(&m_temperature, &temperatureValue);
	return temperatureValue;
}

void BrickletTemperature::registerCallback(ValueChangedCallback callback)
{
    m_callback = std::move(callback);

    temperature_register_callback(&m_temperature,
			TEMPERATURE_CALLBACK_TEMPERATURE_REACHED,
            reinterpret_cast<void*>(temperatureCallback), this);

    // already update the temperature with the current value after registering the callback
    temperatureUpdated(getTemperature());
}

void BrickletTemperature::temperatureUpdated(int16_t newTemperature)
{
    // update callback threshold to trigger again, when outside of current temperature +/- tolerance
    temperature_set_temperature_callback_threshold(&m_temperature, 'o',
                newTemperature - m_tolerance,
                newTemperature + m_tolerance);

    if (m_callback) { m_callback(type(), newTemperature); }
}

} /* namespace tinkerforge */
