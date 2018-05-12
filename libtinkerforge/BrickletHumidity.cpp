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

#include <tinkerforge/BrickletHumidity.h>

extern "C" {
#include "bindings/bricklet_humidity.h"
}

namespace tinkerforge {

  void humidityCallback(uint16_t newHumidity, void* object)
  {
    static_cast<BrickletHumidity*>(object)->humidityUpdated(newHumidity);
  }

  BrickletHumidity::BrickletHumidity(const char* uid, ConnectionHandler &connection)
      : AbstractSensor(uid)
      , m_tolerance(3)
  {
    humidity_create(&m_bricklet, uid, connection.getConnection());
  }

  BrickletHumidity::~BrickletHumidity()
  {
    humidity_destroy(&m_bricklet);
  }

  uint32_t BrickletHumidity::DeviceIdentifier()
  {
      return HUMIDITY_DEVICE_IDENTIFIER;
  }

  const std::string& BrickletHumidity::type() const
  {
      static std::string type("humidity");
      return type;
  }

  uint16_t BrickletHumidity::getHumidity() {
    uint16_t humidityValue;
    humidity_get_humidity(&m_bricklet, &humidityValue);
    return humidityValue;
  }

  void BrickletHumidity::registerCallback(ValueChangedCallback callback)
  {
    m_callback = std::move(callback);

    humidity_register_callback(&m_bricklet,
        HUMIDITY_CALLBACK_HUMIDITY_REACHED,
        reinterpret_cast<void*>(humidityCallback), this);

    // already update the humidity with the current value after registering the callback
    humidityUpdated(getHumidity());
  }

  void BrickletHumidity::humidityUpdated(uint16_t newHumidity)
  {
      humidity_set_humidity_callback_threshold(&m_bricklet, 'o',
          newHumidity >= m_tolerance ? newHumidity - m_tolerance : 0,
          newHumidity + m_tolerance);

      if (m_callback) { m_callback(type(), newHumidity); }
  }

} /* namespace tinkerforge */
