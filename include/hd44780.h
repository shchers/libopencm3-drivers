/**
 * Copyright (C) 2019, Sergey Shcherbakov
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
 *
 * @brief Minimalistic driver for HD44780-based LCD displays
 *
 * As a reference used HITACHI dataheet version ADE-207-272(Z) '99.9 Rev. 0.0
 * for HD44780U (LCD-II)
 */

#ifndef _HD44780_H_
#define _HD44780_H_

/* Std headers */
#include <stdint.h>
#include <stdbool.h>

struct hd44780_gpio {
	/// GPIO port id
	uint32_t port;
	/// GPIO pin - should be used only one pin
	uint16_t gpio;
};

struct hd44780_bus {
	struct hd44780_gpio rs;
	struct hd44780_gpio e;
	struct hd44780_gpio rnw;
	struct hd44780_gpio db7;
	struct hd44780_gpio db6;
	struct hd44780_gpio db5;
	struct hd44780_gpio db4;
	struct hd44780_gpio db3;
	struct hd44780_gpio db2;
	struct hd44780_gpio db1;
	struct hd44780_gpio db0;
};

// Enable converting for russian fonts
//#define hd44780_DO_CONVERT_RUS

/**
 * @brief Clear Display
 */
void hd44780_clear(void);

/**
 * @brief Return Home
 */
void hd44780_home(void);

/**
 * @brief Entry Mode Set
 * @param	inc		DDRAM address to be increamented if true
 * @param	shift	Shift display if true
 * @return None
 */
void hd44780_mode(bool inc, bool shift);

/**
 * @brief Display On/Off Control
 * @param	display_on		Display is on if true
 * @param	show_cursor		Show cursor if true
 * @param	cursor_blink	Enable cursor blinking if true
 *
 */
void hd44780_dispay_ctrl(bool display_on, bool show_cursor, bool cursor_blink);

/**
 * @brief Cursor or Display Shift
 * @param	display		Shift display if true, otherwise cursor
 * @param	right		Shift right if true, otherwise left
 */
void hd44780_cursor_ctrl(bool display, bool right);

/**
 * @brief Function Set
 * @param	bus8		8-bits long bus
 * @param	num_lines	Number of display lines
 * @param	big_fonts	5x10 dots fonts if true, otherwise 5x8
 */
void hd44780_fnc(bool bus8, uint8_t num_lines, bool big_fonts);

/**
 * @brief Set CGRAM address
 * @param	addr	CGRAM address
 */
void hd44780_set_CGRAM_addr(uint8_t addr);

/**
 * @brief Set DDRAM address
 * @param	addr	DDRAM address
 */
void hd44780_set_DDRAM_addr(uint8_t addr);

/**
 * @brief Init of HT44780 display and its data bus lines
 * @param	bus_props	Data bus GPIO descriptor
 * @param	bus8		8-bits long bus
 * @param	width		Display width
 * @param	num_lines	Number of display lines
 * @param	big_fonts	5x10 dots fonts if true, otherwise 5x8
 */
void hd44780_init(struct hd44780_bus *bus_props, uint8_t width, bool bus8, uint8_t num_lines, bool big_fonts);

/**
 * @brief Put character on a display
 * @param	ch	Character to be printed
 */
void hd44780_putchar(int ch);

/**
 * @brief Put character on a display at point[x,y]
 * @param	ch	Character to be printed
 * @param	x	X-axis, starts at 0
 * @param	y	Y-axis, starts at 0
 */
void hd44780_putchar_xy(uint8_t x, uint8_t y, int ch);

/**
 * Basic printf implementation for LCD
 * @param	fmt		Text and formating
 * @param	...		Arguments
 */
void hd44780_printf(const char *fmt, ...);

/**
 * Basic printf implementation for LCD with initial point definition
 * @param	x		X-axis, starts at 0
 * @param	y		Y-axis, starts at 0
 * @param	fmt		Text and formating
 * @param	...		Arguments
 */
void hd44780_printf_xy(uint8_t x, uint8_t y, const char *fmt, ...);

/**
 * @brief Put user-defined character pattern for the character to the character generator RAM
 * @param	addr		Character address in the character generator RAM
 * @param	pattern		Character pattern
 * @param	size		Pattern size
 */
void hd44780_define_char(uint8_t addr, uint8_t* pattern, uint8_t size);

#endif // _HD44780_H_
