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

#ifndef CONNECTIONHANDLER_H_
#define CONNECTIONHANDLER_H_

#include <functional>
#include <tinkerforge/bindings/ip_connection.h>

#include "Bricklet.h"

namespace tinkerforge {

class ConnectionHandler
{
    using EnumerateCallback = std::function<void(const char*, uint16_t, uint8_t)>;

    typedef void (*EnumerateCallbackFunction)(const char*, const char*, char, uint8_t*, uint8_t*, uint16_t, uint8_t, void*);

public:
    ConnectionHandler(const char* host = "localhost", uint16_t port = 4223);
    virtual ~ConnectionHandler();
    IPConnection* getConnection();

    void setEnumerateCallback(EnumerateCallback callback);
    void joinThread();

private:
    friend void enumerateCallback(const char*, const char*, char, uint8_t*, uint8_t*, uint16_t, uint8_t, void*);

    IPConnection      m_ipcon;
    EnumerateCallback m_callback;
};

} /* namespace tinkerforge */

#endif /* CONNECTIONHANDLER_H_ */
