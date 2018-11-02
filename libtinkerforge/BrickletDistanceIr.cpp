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

#include <tinkerforge/BrickletDistanceIr.h>

extern "C" {
#include "bindings/bricklet_distance_ir.h"
}

namespace tinkerforge {

  void distanceIrCallback(uint16_t curDistance, void* object)
  {
    static_cast<BrickletDistanceIr*>(object)->valueUpdated(curDistance);
  }

  BrickletDistanceIr::BrickletDistanceIr(const char* uid, ConnectionHandler &connection)
      : AbstractSensor(uid)
  {
    distance_ir_create(&m_bricklet, uid, connection.getConnection());
  }

  BrickletDistanceIr::~BrickletDistanceIr()
  {
    distance_ir_destroy(&m_bricklet);
  }

  uint32_t BrickletDistanceIr::DeviceIdentifier()
  {
      return DISTANCE_IR_DEVICE_IDENTIFIER;
  }

  const std::string& BrickletDistanceIr::type() const
  {
      static std::string type("distance");
      return type;
  }

  uint16_t BrickletDistanceIr::getDistance()
  {
    uint16_t distanceValue;
    distance_ir_get_distance(&m_bricklet, &distanceValue);
    return distanceValue;
  }

  void BrickletDistanceIr::registerCallback(ValueChangedCallback callback)
  {
    m_callback = std::move(callback);

    // Activate the callback (set 1s period as minimum)
    distance_ir_set_distance_callback_period(&m_bricklet, CALLBACK_PERIOD);

    // Set the callback to the "distanceCallback" member function
    distance_ir_register_callback(&m_bricklet,
        DISTANCE_IR_CALLBACK_DISTANCE,
        reinterpret_cast<void*>(distanceIrCallback), this);
  }

  void BrickletDistanceIr::valueUpdated(uint16_t newValue)
  {
      // The distance bricklet isn't working right for values larger than some
      // value. This value would need to be adapted for different sensors.
      newValue = newValue > MAXIMUM_VALUE ? MAXIMUM_VALUE : newValue;

      if (newValue == m_lastValue) { return; }
      m_lastValue = newValue;

      if (m_callback) { m_callback(type(), newValue); }
  }

} /* namespace tinkerforge */
