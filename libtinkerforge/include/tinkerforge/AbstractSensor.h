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

#ifndef ABSTRACTSENSOR_H_
#define ABSTRACTSENSOR_H_

#include "ConnectionHandler.h"

#include <functional>
#include <array>

struct Device_;

namespace tinkerforge {

  class AbstractSensor
  {

  public:
      using ValueChangedCallback = std::function<void(const std::string type, int32_t value)>;

      class UID : public std::array<char,3> {
      public:
          UID(const char* uid)
          {
              operator[](0) = uid[0];
              operator[](1) = uid[1];
              operator[](2) = uid[2];
          }
      };

    AbstractSensor (const char* uid);
    virtual ~AbstractSensor () = default;

    const UID& getUid() const;
    virtual const std::string& type() const = 0;

    virtual void registerCallback(ValueChangedCallback callback) = 0;

  private:
    UID m_uid;
  };

  bool operator==(const AbstractSensor& bricket, const AbstractSensor::UID& uid);

} /* namespace tinkerforge */
#endif /* ABSTRACTSENSOR_H_ */

