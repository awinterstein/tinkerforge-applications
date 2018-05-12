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

#ifndef BRICKLETTEMPERATURE_H_
#define BRICKLETTEMPERATURE_H_

#include "AbstractSensor.h"

namespace tinkerforge {

  class BrickletTemperature: public AbstractSensor {

  public:
    BrickletTemperature (const char* uid, ConnectionHandler &connection);
    virtual ~BrickletTemperature ();

    static uint32_t DeviceIdentifier();

    const std::string& type() const override;

    int16_t getTemperature ();
    void registerCallback(ValueChangedCallback callback) override;

  private:
    friend void temperatureCallback (int16_t temperature, void* object);
    void temperatureUpdated(int16_t newTemperature);

    Device                       m_temperature;
    int8_t                       m_tolerance;
    ValueChangedCallback         m_callback;
  };

} /* namespace tinkerforge */

#endif /* BRICKLETTEMPERATURE_H_ */
