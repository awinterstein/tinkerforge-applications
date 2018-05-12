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

#ifndef BRICKLETAMBIENTLIGHT_H_
#define BRICKLETAMBIENTLIGHT_H_

#include "AbstractSensor.h"

namespace tinkerforge {

class BrickletAmbientLight: public AbstractSensor {

public:
    BrickletAmbientLight(const char* uid, ConnectionHandler &connection);
    virtual ~BrickletAmbientLight();

    static uint32_t DeviceIdentifier();

    const std::string& type() const override;

    uint16_t getAmbientLight();
    void registerCallback(ValueChangedCallback callback) override;

private:
    friend void ambientLightCallback(uint16_t, void*);
    void valueUpdated(uint16_t newValue);

    Device                        m_bricklet;
    uint8_t                       m_tolerance;
    ValueChangedCallback          m_callback;
};

} /* namespace tinkerforge */

#endif /* BRICKLETAMBIENTLIGHT_H_ */
