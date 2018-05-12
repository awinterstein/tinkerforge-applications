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

#ifndef LCD_H_
#define LCD_H_

extern "C" {
#include <tinkerforge/bindings/bricklet_lcd_20x4.h>
}

#include "Bricklet.h"

#include <string>

namespace tinkerforge {

class Lcd: public tinkerforge::Bricklet {
	LCD20x4*	m_lcd;
	char*		m_statusbar;
	uint8_t		m_linewidth;
public:
    Lcd(const char* uid, ConnectionHandler &connection, bool statusbar = false);
	virtual ~Lcd();
    virtual Device* getDevice() const;
	int setBacklight(const bool backlight = true);
	int writeTextToDisplay(const std::string text);

	int writeLine(uint8_t line, uint8_t position, const char *text);
	int writeTextToLine(uint8_t line, const std::string text);
	int writeStatusbar(uint8_t position, char symbol);
	int clearDisplay(bool preserveStatusbar = false);

private:
	std::string unicodeToKs0066u(const std::string&) const;
	char multibyteToLcdByte(char high, char low) const;
	void wchar_to_ks0066u(const wchar_t *wchar, char *ks0066u, int ks0066u_length) const;
};

} /* namespace tinkerforge */
#endif /* LCD_H_ */
