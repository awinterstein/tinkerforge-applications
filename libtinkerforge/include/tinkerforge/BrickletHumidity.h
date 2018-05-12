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

#ifndef BRICKLETHUMIDITY_H_
#define BRICKLETHUMIDITY_H_

#include "AbstractSensor.h"

namespace tinkerforge {

class BrickletHumidity: public tinkerforge::AbstractSensor {

public:
    BrickletHumidity(const char* uid, ConnectionHandler &connection);
	virtual ~BrickletHumidity();

    static uint32_t DeviceIdentifier();
    const std::string& type() const override;

	uint16_t getHumidity();
    void registerCallback(ValueChangedCallback callback) override;

private:
    friend void humidityCallback(uint16_t, void*);
    void humidityUpdated(uint16_t newHumidity);

    Device                        m_bricklet;
    uint8_t                       m_tolerance;
    ValueChangedCallback          m_callback;
};

} /* namespace tinkerforge */
#endif /* BRICKLETHUMIDITY_H_ */
