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

#include <tinkerforge/ConnectionHandler.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>

#include <iostream>

namespace tinkerforge {

void enumerateCallback(const char* uid, const char*, char, uint8_t*, uint8_t*, uint16_t deviceIdentifier, uint8_t enumerationType, void* object)
{
    auto connectionHandler = static_cast<ConnectionHandler*>(object);
    if (connectionHandler->m_callback)
    {
        connectionHandler->m_callback(uid, deviceIdentifier, enumerationType);
    }
}

  ConnectionHandler::ConnectionHandler (const char* host, uint16_t port)
  {  
    // Create ip connection to brickd.
    ipcon_create(&m_ipcon);

    // Try to connect until it is connected to the brick daemon.
    uint8_t connectionTries = 0;
    while(ipcon_connect(&m_ipcon, host, port) < 0) {
        if (connectionTries < 60) { ++connectionTries; }

        if (spdlog::get("main"))
        {
            spdlog::get("main")->error("Connection to brick deamon on '{}' (port {}) failed. Will try again in {} seconds.", host, port, 5*connectionTries);
        }

        std::this_thread::sleep_for(std::chrono::seconds(5*connectionTries));
    }

    // Necessary at the moment for the second brick not to fail connecting.
    std::this_thread::sleep_for(std::chrono::milliseconds(5*connectionTries));
  }

  ConnectionHandler::~ConnectionHandler ()
  {
    ipcon_destroy(&m_ipcon);
  }

  IPConnection* ConnectionHandler::getConnection ()
  {
    return &m_ipcon;
  }

  void ConnectionHandler::setEnumerateCallback (EnumerateCallback callback)
  {
      m_callback = std::move(callback);

      ipcon_register_callback(&m_ipcon, IPCON_CALLBACK_ENUMERATE,
                              reinterpret_cast<void*>(enumerateCallback), this);
      ipcon_enumerate(&m_ipcon);
  }

  void ConnectionHandler::joinThread ()
  {   
    ipcon_wait(&m_ipcon);
  }

} /* namespace tinkerforge */
