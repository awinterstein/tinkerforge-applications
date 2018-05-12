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

#ifndef BRICKLETDISTANCEIR_H_
#define BRICKLETDISTANCEIR_H_

#include "AbstractSensor.h"

namespace tinkerforge {

class BrickletDistanceIr : public AbstractSensor
{
public:
    BrickletDistanceIr (const char* uid, ConnectionHandler &connection);
	virtual ~BrickletDistanceIr ();

    static uint32_t DeviceIdentifier();
    const std::string& type() const override;

    uint16_t getDistance ();
    void registerCallback(ValueChangedCallback callback) override;

private:
    friend void distanceIrCallback(uint16_t, void*);
    void valueUpdated(uint16_t newValue);

    Device                  m_bricklet;
    uint8_t                 m_tolerance;
    ValueChangedCallback    m_callback;
    uint16_t                m_lastValue{0};
};

} /* namespace tinkerforge */

#endif /* BRICKLETDISTANCEIR_H_ */
