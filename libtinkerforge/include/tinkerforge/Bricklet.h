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

#ifndef BRICKLET_H_
#define BRICKLET_H_

#include "ConnectionHandler.h"
#include <array>

struct Device_;

namespace tinkerforge {

  class Bricklet
  {

  public:
      class UID : public std::array<char,3> {
      public:
          UID(const char* uid)
          {
              operator[](0) = uid[0];
              operator[](1) = uid[1];
              operator[](2) = uid[2];
          }
      };

    Bricklet (const char* uid);
    virtual ~Bricklet () = default;

    const UID& getUid() const;

  private:
    UID m_uid;
  };

  bool operator==(const Bricklet& bricket, const Bricklet::UID& uid);

} /* namespace tinkerforge */
#endif /* BRICKLET_H_ */

