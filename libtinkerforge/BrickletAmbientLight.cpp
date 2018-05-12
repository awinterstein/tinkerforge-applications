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

#include <tinkerforge/BrickletAmbientLight.h>

extern "C" {
#include "bindings/bricklet_ambient_light.h"
}

namespace tinkerforge {

  void ambientLightCallback(uint16_t newValue, void* object)
  {
    static_cast<BrickletAmbientLight*>(object)->valueUpdated(newValue);
  }

  BrickletAmbientLight::BrickletAmbientLight(const char* uid, ConnectionHandler &connection)
      : AbstractSensor(uid)
      , m_tolerance(10)
  {
    ambient_light_create(&m_bricklet, uid, connection.getConnection());
  }

  BrickletAmbientLight::~BrickletAmbientLight()
  {
    ambient_light_destroy(&m_bricklet);
  }

  uint32_t BrickletAmbientLight::DeviceIdentifier()
  {
      return AMBIENT_LIGHT_DEVICE_IDENTIFIER;
  }

  const std::string& BrickletAmbientLight::type() const
  {
      static std::string type("ambient-light");
      return type;
  }

  uint16_t BrickletAmbientLight::getAmbientLight() {
    uint16_t value;
    ambient_light_get_illuminance(&m_bricklet, &value);
    return value;
  }

  void BrickletAmbientLight::registerCallback(ValueChangedCallback callback)
  {
    m_callback = std::move(callback);

    ambient_light_register_callback(&m_bricklet,
        AMBIENT_LIGHT_CALLBACK_ILLUMINANCE_REACHED,
        reinterpret_cast<void*>(ambientLightCallback), this);

    // already update the humidity with the current value after registering the callback
    valueUpdated(getAmbientLight());
  }

  void BrickletAmbientLight::valueUpdated(uint16_t newValue)
  {
      ambient_light_set_illuminance_callback_threshold(&m_bricklet, 'o',
          newValue >= m_tolerance ? newValue - m_tolerance : 0,
          newValue + m_tolerance);

      if (m_callback) { m_callback(type(), newValue); }
  }

} /* namespace tinkerforge */
