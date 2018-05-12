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

#include <tinkerforge/Lcd.h>

#include <locale>

namespace tinkerforge {

  Lcd::Lcd(const char* uid, ConnectionHandler &connection, bool statusbar)
      : Bricklet(uid)
  {
    m_lcd = new LCD20x4();
    lcd_20x4_create(m_lcd, uid, connection.getConnection());

    if (statusbar)
      {
        m_statusbar = new char[4];
        for (uint8_t n = 0; n < 4; n++) m_statusbar[n] = ' ';
        m_linewidth = 19;
      }
    else
      {
        m_statusbar = 0;
        m_linewidth = 20;
      }
  }

  Lcd::~Lcd() {
    setBacklight(false);
    clearDisplay();

    delete m_statusbar;
    delete m_lcd;
  }

  Device* Lcd::getDevice() const {
    return m_lcd;
  }

  int Lcd::setBacklight(const bool backlight) {
    bool backlightOn;
    lcd_20x4_is_backlight_on(m_lcd, &backlightOn);

    int success = true;
    if (!backlight && backlightOn)
      {
        success = lcd_20x4_backlight_off(m_lcd);
      }
    else if (backlight && !backlightOn)
      {
        success = lcd_20x4_backlight_on(m_lcd);
      }

    return success;
  }

  int Lcd::writeTextToDisplay(const std::string text) {
    clearDisplay(m_statusbar != 0);

    std::string displayText = unicodeToKs0066u(text);

    int success = 0;
    for (uint8_t n = 0; n < 4; n++)
      {
        if (m_linewidth*n > displayText.size()) break;

        char lcdText[m_linewidth+1];
        memset( lcdText, 0, (m_linewidth+1)*sizeof(char) );

        displayText.copy(lcdText, m_linewidth, m_linewidth*n);
        success = writeLine(n, 0, lcdText);
      }

    return success;
  }

  int Lcd::writeLine(uint8_t line, uint8_t position, const char *text) {
    return lcd_20x4_write_line(m_lcd, line, position, text);
  }

  int Lcd::writeStatusbar(uint8_t position, char symbol) {
    if (m_statusbar == 0) return -11;

    m_statusbar[position] = symbol;
    return lcd_20x4_write_line(m_lcd, position, m_linewidth, &symbol);
  }

  int Lcd::writeTextToLine(uint8_t line, const std::string text) {
    std::string displayText = unicodeToKs0066u(text);

    char lcdText[20] = {0};
    displayText.copy(lcdText, 20);
    return writeLine(line, 0, lcdText);
  }

  int Lcd::clearDisplay(bool preserveStatusbar) {
    int returnValue = lcd_20x4_clear_display(m_lcd);

    if (preserveStatusbar && m_statusbar != 0)
      {
        for (uint8_t n = 0; n < 4; n++)
          {
            writeStatusbar(n, m_statusbar[n]);
          }
      }

    return returnValue;
  }

  std::string
  Lcd::unicodeToKs0066u(const std::string& inputString) const
  {
    std::string outputString;

    for (auto i = inputString.begin(); i != inputString.end(); i++)
      {
        // Test for 110XXXXX which stands for the first byte of
        // a two byte character (more than two bytes are ignored,
        // because the LCD cannot display any of these characters).
        if ((*i & 0xE0) == 0xC0)
          {
            // Get the two bytes of the character and encode them
            // for the LCD
            char high = *i;
            char low = *(++i);
            outputString.push_back(multibyteToLcdByte(high, low));
          }
        else
          {
            outputString.push_back(*i);
          }
      }

    return outputString;
  }

  char
  Lcd::multibyteToLcdByte(char high, char low) const
  {
    char c = 0xff; // BLACK SQUARE as default

    if (high == (char)0xc3)
      {
        // LATIN LETTER A WITH DIAERESIS
        if (low == (char)0xa4 || low == (char)0x84) c = 0xe1;

        // LATIN LETTER O WITH DIAERESIS
        else if (low == (char)0xb6 || low == (char)0x96) c = 0xef;

        // LATIN LETTER U WITH DIAERESIS
        else if (low == (char)0xbc || low == (char)0x9c) c = 0xf5;

        // LATIN LETTER SHARP S
        else if (low == (char)0x9f) c = 0xe2;
      }

    return c;
  }

  // Maps a wchar_t string to the LCD charset
  void
  Lcd::wchar_to_ks0066u (const wchar_t *wchar, char *ks0066u, int ks0066u_length) const
  {
    const wchar_t *s = wchar;
    char *d = ks0066u;
    char *e = ks0066u + ks0066u_length;
    char c;
    uint32_t code_point;

    while (*s != '\0' && d < e) {
        // If wchar_t is UTF-16 then handle surrogates
        if (sizeof(wchar_t) == 2 && *s >= 0xd800 && *s <= 0xdbff) {
            code_point = 0x10000 + (*s - 0xd800) * 0x400 + (*(s + 1) - 0xdc00);
            s += 2;
        } else {
            code_point = *s++;
        }

        // ASCII subset from JIS X 0201
        if (code_point >= 0x0020 && code_point <= 0x007e) {
            // The LCD charset doesn't include '\' and '~', use similar characters instead
            switch (code_point) {
            case 0x005c: c = 0xa4; break; // REVERSE SOLIDUS maps to IDEOGRAPHIC COMMA
            case 0x007e: c = 0x2d; break; // TILDE maps to HYPHEN-MINUS
            default: c = code_point; break;
            }
        }
        // Katakana subset from JIS X 0201
        else if (code_point >= 0xff61 && code_point <= 0xff9f) {
            c = code_point - 0xfec0;
        }
        // Special characters
        else {
            switch (code_point) {
            case 0x00a5: c = 0x5c; break; // YEN SIGN
            case 0x2192: c = 0x7e; break; // RIGHTWARDS ARROW
            case 0x2190: c = 0x7f; break; // LEFTWARDS ARROW
            case 0x00b0: c = 0xdf; break; // DEGREE SIGN maps to KATAKANA SEMI-VOICED SOUND MARK
            case 0x03b1: c = 0xe0; break; // GREEK SMALL LETTER ALPHA
            case 0x00c4: c = 0xe1; break; // LATIN CAPITAL LETTER A WITH DIAERESIS
            case 0x00e4: c = 0xe1; break; // LATIN SMALL LETTER A WITH DIAERESIS
            case 0x00df: c = 0xe2; break; // LATIN SMALL LETTER SHARP S
            case 0x03b5: c = 0xe3; break; // GREEK SMALL LETTER EPSILON
            case 0x00b5: c = 0xe4; break; // MICRO SIGN
            case 0x03bc: c = 0xe4; break; // GREEK SMALL LETTER MU
            case 0x03c2: c = 0xe5; break; // GREEK SMALL LETTER FINAL SIGMA
            case 0x03c1: c = 0xe6; break; // GREEK SMALL LETTER RHO
            case 0x221a: c = 0xe8; break; // SQUARE ROOT
            case 0x00b9: c = 0xe9; break; // SUPERSCRIPT ONE maps to SUPERSCRIPT (minus) ONE
            case 0x00a4: c = 0xeb; break; // CURRENCY SIGN
            case 0x00a2: c = 0xec; break; // CENT SIGN
            case 0x2c60: c = 0xed; break; // LATIN CAPITAL LETTER L WITH DOUBLE BAR
            case 0x00f1: c = 0xee; break; // LATIN SMALL LETTER N WITH TILDE
            case 0x00d6: c = 0xef; break; // LATIN CAPITAL LETTER O WITH DIAERESIS
            case 0x00f6: c = 0xef; break; // LATIN SMALL LETTER O WITH DIAERESIS
            case 0x03f4: c = 0xf2; break; // GREEK CAPITAL THETA SYMBOL
            case 0x221e: c = 0xf3; break; // INFINITY
            case 0x03a9: c = 0xf4; break; // GREEK CAPITAL LETTER OMEGA
            case 0x00dc: c = 0xf5; break; // LATIN CAPITAL LETTER U WITH DIAERESIS
            case 0x00fc: c = 0xf5; break; // LATIN SMALL LETTER U WITH DIAERESIS
            case 0x03a3: c = 0xf6; break; // GREEK CAPITAL LETTER SIGMA
            case 0x03c0: c = 0xf7; break; // GREEK SMALL LETTER PI
            case 0x0304: c = 0xf8; break; // COMBINING MACRON
            case 0x00f7: c = 0xfd; break; // DIVISION SIGN

            default:
            case 0x25a0: c = 0xff; break; // BLACK SQUARE
            }
        }

        // Special handling for 'x' followed by COMBINING MACRON
        if (c == (char)0xf8) {
            if (d == ks0066u || (d > ks0066u && *(d - 1) != 'x')) {
                c = 0xff; // BLACK SQUARE
            }

            if (d > ks0066u) {
                --d;
            }
        }

        *d++ = c;
    }

    while (d < e) {
        *d++ = '\0';
    }
  }

} /* namespace tinkerforge */
