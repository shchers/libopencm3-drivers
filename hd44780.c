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

// Std headers
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>

// Local headers
#include "include/helper.h"
#include "include/hd44780.h"

/// Max number of characters in LCD buffer
#define HD44780_MAX_BUFFER_SIZE (80)

/* Registers */
#define CLEAR_DISPLAY           0x01

#define RETURN_HOME             0x02

#define ENTRY_MODE_SET          0x04
#define     ENTRY_MODE_SH       0x01
#define     ENTRY_MODE_ID       0x02

#define DISPLAY_CONTROL         0x08
#define     DISPLAY_CONTROL_B   0x01
#define     DISPLAY_CONTROL_C   0x02
#define     DISPLAY_CONTROL_D   0x04

#define CURSOR_DISPLAY_SHIFT    0x10
#define     CURSOR_DISPLAY_RL   0x04
#define     CURSOR_DISPLAY_SC   0x08

#define FUNCTION_SET            0x20
#define     FUNCTION_SET_F      0x04
#define     FUNCTION_SET_N      0x08
#define     FUNCTION_SET_DL     0x10

#define SET_CG_RAM_ADDR         0x40
#define     CG_RAM_ADDR_MASK    0x3F

#define SET_DD_RAM_ADDR         0x80
#define     DD_RAM_ADDR_MASK    0x7F

/* Read busy flag & address */
/// Busy flag
#define FLAGS_BF_MASK           0x80
/// Address counter
#define FLAGS_AC_MASK           0x7F

extern void sleep_ms(uint32_t ms);

struct position_s {
    uint8_t x;
    uint8_t y;
};

static struct
{
  uint8_t width;
  bool bus8;
  struct hd44780_bus *bus;
  struct position_s position;
} hd44780_params;

/**
 * @brief Write half-byte
 * @param rs	True if data, otherwise instructin register
 * @param	data	Half of data byte
 */
static void hd44780_write_half_byte(bool rs, uint8_t data)
{
	gpio_clear(hd44780_params.bus->rnw.port, hd44780_params.bus->rnw.gpio);

	if (rs) {
		gpio_set(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	} else {
		gpio_clear(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	}

	if (data & 0x08)
		gpio_set(hd44780_params.bus->db7.port, hd44780_params.bus->db7.gpio);
	else
		gpio_clear(hd44780_params.bus->db7.port, hd44780_params.bus->db7.gpio);

	if (data & 0x04)
		gpio_set(hd44780_params.bus->db6.port, hd44780_params.bus->db6.gpio);
	else
		gpio_clear(hd44780_params.bus->db6.port, hd44780_params.bus->db6.gpio);

	if (data & 0x02)
		gpio_set(hd44780_params.bus->db5.port, hd44780_params.bus->db5.gpio);
	else
		gpio_clear(hd44780_params.bus->db5.port, hd44780_params.bus->db5.gpio);

	if (data & 0x01)
		gpio_set(hd44780_params.bus->db4.port, hd44780_params.bus->db4.gpio);
	else
		gpio_clear(hd44780_params.bus->db4.port, hd44780_params.bus->db4.gpio);

	gpio_set(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);
	gpio_clear(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);
}

/**
 * @brief Write byte to LCD
 * @param rs	True if data, otherwise instructin register
 * @param data	Data byte
 */
static void hd44780_write_byte(bool rs, uint8_t data)
{
	gpio_clear(hd44780_params.bus->rnw.port, hd44780_params.bus->rnw.gpio);

	if (rs) {
		gpio_set(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	} else {
		gpio_clear(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	}

	if (data & 0x80)
		gpio_set(hd44780_params.bus->db7.port, hd44780_params.bus->db7.gpio);
	else
		gpio_clear(hd44780_params.bus->db7.port, hd44780_params.bus->db7.gpio);

	if (data & 0x40)
		gpio_set(hd44780_params.bus->db6.port, hd44780_params.bus->db6.gpio);
	else
		gpio_clear(hd44780_params.bus->db6.port, hd44780_params.bus->db6.gpio);

	if (data & 0x20)
		gpio_set(hd44780_params.bus->db5.port, hd44780_params.bus->db5.gpio);
	else
		gpio_clear(hd44780_params.bus->db5.port, hd44780_params.bus->db5.gpio);

	if (data & 0x10)
		gpio_set(hd44780_params.bus->db4.port, hd44780_params.bus->db4.gpio);
	else
		gpio_clear(hd44780_params.bus->db4.port, hd44780_params.bus->db4.gpio);

	if (data & 0x08)
		gpio_set(hd44780_params.bus->db3.port, hd44780_params.bus->db3.gpio);
	else
		gpio_clear(hd44780_params.bus->db3.port, hd44780_params.bus->db3.gpio);

	if (data & 0x04)
		gpio_set(hd44780_params.bus->db2.port, hd44780_params.bus->db2.gpio);
	else
		gpio_clear(hd44780_params.bus->db2.port, hd44780_params.bus->db2.gpio);

	if (data & 0x02)
		gpio_set(hd44780_params.bus->db1.port, hd44780_params.bus->db1.gpio);
	else
		gpio_clear(hd44780_params.bus->db1.port, hd44780_params.bus->db1.gpio);

	if (data & 0x01)
		gpio_set(hd44780_params.bus->db0.port, hd44780_params.bus->db0.gpio);
	else
		gpio_clear(hd44780_params.bus->db0.port, hd44780_params.bus->db0.gpio);

	gpio_set(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);
	gpio_clear(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);
}

/**
 * @brief Write byte to LCD
 * @param rs	True if data, otherwise instructin register
 * @param data	Data byte
 */
static void hd44780_write(bool rs, uint8_t data)
{
	if (hd44780_params.bus8) {
		hd44780_write_byte(rs, data);
	} else {
		hd44780_write_half_byte(rs, data >> 4);
		hd44780_write_half_byte(rs, data);
	}
}

#ifdef hd44780_DO_CONVERT_RUS
/**
 * @brief   Convert Russian symbols codes to LCD codes
 * @param   ch  Russian symbols code
 * @return  LCD symbol code
 */
static int hd44780_convert_rus(int ch)
{
	switch (ch) {
		case 'а': ch = 97;  break;
		case 'б': ch = 178; break;
		case 'в': ch = 179; break;
		case 'г': ch = 180; break;
		case 'д': ch = 227; break;
		case 'е': ch = 101; break;
		case 'ё': ch = 181; break;
		case 'ж': ch = 182; break;
		case 'з': ch = 183; break;
		case 'и': ch = 184; break;
		case 'й': ch = 185; break;
		case 'к': ch = 186; break;
		case 'л': ch = 187; break;
		case 'м': ch = 188; break;
		case 'н': ch = 189; break;
		case 'о': ch = 111; break;
		case 'п': ch = 190; break;
		case 'р': ch = 112; break;
		case 'с': ch = 99;  break;
		case 'т': ch = 191; break;
		case 'у': ch = 121; break;
		case 'ф': ch = 228; break;
		case 'х': ch = 120; break;
		case 'ч': ch = 192; break;
		case 'ц': ch = 229; break;
		case 'ш': ch = 193; break;
		case 'щ': ch = 230; break;
		case 'ъ': ch = 194; break;
		case 'ы': ch = 195; break;
		case 'ь': ch = 196; break;
		case 'э': ch = 197; break;
		case 'ю': ch = 198; break;
		case 'я': ch = 199; break;

		case 'А': ch = 65;  break;
		case 'Б': ch = 160; break;
		case 'В': ch = 66;  break;
		case 'Г': ch = 161; break;
		case 'Д': ch = 224; break;
		case 'Е': ch = 69;  break;
		case 'Ё': ch = 162; break;
		case 'Ж': ch = 163; break;
		case 'З': ch = 164; break;
		case 'И': ch = 165; break;
		case 'Й': ch = 166; break;
		case 'К': ch = 75;  break;
		case 'Л': ch = 167; break;
		case 'М': ch = 77;  break;
		case 'Н': ch = 72;  break;
		case 'О': ch = 79;  break;
		case 'П': ch = 168; break;
		case 'Р': ch = 80;  break;
		case 'С': ch = 67;  break;
		case 'Т': ch = 84;  break;
		case 'У': ch = 169; break;
		case 'Ф': ch = 170; break;
		case 'Х': ch = 88;  break;
		case 'Ч': ch = 171; break;
		case 'Ц': ch = 225; break;
		case 'Ш': ch = 171; break;
		case 'Щ': ch = 226; break;
		case 'Ъ': ch = 172; break;
		case 'Ы': ch = 173; break;
		case 'Ь': ch = 98;  break;
		case 'Э': ch = 174; break;
		case 'Ю': ch = 176; break;
		case 'Я': ch = 177; break;

		default: break;
	}

	return ch;
}
#endif

/**
 * @brief Move cursor to point[x,y]. [0,0] is a top left corner
 * @param	x	X-axis, starts at 0
 * @param	x	Y-axis, starts at 0
 */
static void hd44780_gotoxy(uint8_t x, uint8_t y)
{
	if (x >= hd44780_params.width) {
		y++;
		x = 0;
	}

	switch (y) {
		case 0:
			y = 0x00;
			break;

		case 1:
			y = 0x40;
			hd44780_params.position.y = 1;
			break;

		default:
			y = 0x00;
			x = 0;
			hd44780_params.position.y = 0;
			break;
	}

	hd44780_set_DDRAM_addr(x + y);
	hd44780_params.position.x = x;
}

/**
 * @brief Jump to new line
 */
static void hd44780_nl(void)
{
	uint8_t y = hd44780_params.position.y + 1;
	hd44780_gotoxy(0, y);
}

#warning XXX: hd44780_read yet not implmented!
#if 0
static uint8_t hd44780_read_byte(bool rs)
{
	uint8_t temp = 0;

	gpio_set(hd44780_params.bus->rnw.port, hd44780_params.bus->rnw.gpio);

	if (rs) {
		gpio_set(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	} else {
		gpio_clear(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	}

	gpio_set(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);

	// XXX: Get data from LCD here

	gpio_clear(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);

	return temp;
}

static uint8_t hd44780_read_half_byte(bool rs)
{
	uint8_t temp = 0;

	gpio_set(hd44780_params.bus->rnw.port, hd44780_params.bus->rnw.gpio);

	if (rs) {
		gpio_set(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	} else {
		gpio_clear(hd44780_params.bus->rs.port, hd44780_params.bus->rs.gpio);
	}

	gpio_set(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);

	// XXX: Get HIGH part of data from LCD here

	gpio_clear(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	gpio_set(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);

	// XXX: Get LOW part of data from LCD here

	gpio_clear(hd44780_params.bus->e.port, hd44780_params.bus->e.gpio);
	sleep_ms(1);

	return temp;
}

static uint8_t hd44780_read_half_byte(bool rs)
{
	uint8_t data;

	if (hd44780_params.bus8) {
		data = hd44780_read(rs);
	} else {
		data = hd44780_read(rs, data) << 4;
		data |= hd44780_read(rs);
	}

	return data;
}
#endif

#warning XXX: hd44780_busy yet not implmented!
bool hd44780_busy(void)
{
#if 0
    return (hd44780_read(false) & FLAGS_BF_MASK);
#else
	sleep_ms(1);
	return true;
#endif
}

void hd44780_clear()
{
	hd44780_params.position.x = 0;
	hd44780_params.position.y = 0;

	hd44780_write(false, CLEAR_DISPLAY);
}

void hd44780_home()
{
	hd44780_params.position.x = 0;
	hd44780_params.position.y = 0;

	hd44780_write(false, RETURN_HOME);
	sleep_ms(2);
}

void hd44780_mode(bool inc, bool shift)
{
	uint8_t temp = ENTRY_MODE_SET;

	if (inc) {
		temp |= ENTRY_MODE_ID;
	}

	if (shift) {
		temp |= ENTRY_MODE_SH;
	}

	hd44780_write(false, temp);
}

void hd44780_dispay_ctrl(bool display_on, bool show_cursor, bool cursor_blink)
{
	uint8_t temp = DISPLAY_CONTROL;

	if (display_on) {
		temp |= DISPLAY_CONTROL_D;
	}

	if (show_cursor) {
		temp |= DISPLAY_CONTROL_C;
	}

	if (cursor_blink) {
		temp |= DISPLAY_CONTROL_B;
	}

	hd44780_write(false, temp);
}

void hd44780_cursor_ctrl(bool display, bool right)
{
	uint8_t temp = CURSOR_DISPLAY_SHIFT;

	if (display) {
		temp |= CURSOR_DISPLAY_SC;
	}

	if (right) {
		temp |= CURSOR_DISPLAY_RL;
	}

	hd44780_write(false, temp);
}

void hd44780_fnc(bool bus8, uint8_t num_lines, bool big_fonts)
{
	uint8_t temp = FUNCTION_SET;

	if (bus8) {
		temp |= FUNCTION_SET_DL;
	}

	if ((num_lines - 1) > 0) {
		temp |= FUNCTION_SET_N;
	}

	if (big_fonts) {
		temp |= FUNCTION_SET_F;
	}

	hd44780_write(false, temp);
}

void hd44780_set_CGRAM_addr(uint8_t addr)
{
	uint8_t temp = SET_CG_RAM_ADDR;

	temp += addr & CG_RAM_ADDR_MASK;

	hd44780_write(false, temp);
}

void hd44780_set_DDRAM_addr(uint8_t addr)
{
	uint8_t temp = SET_DD_RAM_ADDR;

	temp += addr & DD_RAM_ADDR_MASK;

	hd44780_write(false, temp);
}

static void hd44780_init_4bits(void)
{
	hd44780_write_half_byte(false, (FUNCTION_SET | FUNCTION_SET_DL) >> 4);
	sleep_ms(15);

	hd44780_write_half_byte(false, (FUNCTION_SET | FUNCTION_SET_DL) >> 4);
	sleep_ms(15);

	hd44780_write_half_byte(false, (FUNCTION_SET | FUNCTION_SET_DL) >> 4);
	sleep_ms(15);

	hd44780_write_half_byte(false, FUNCTION_SET >> 4);
}

void hd44780_init(struct hd44780_bus *bus_props, uint8_t width, bool bus8, uint8_t num_lines, bool big_fonts)
{
    hd44780_params.width = width;
    hd44780_params.position.x = 0;
    hd44780_params.position.y = 0;
	hd44780_params.bus8 = bus8;
	hd44780_params.bus = bus_props;

	// Configuring GPIO used for LCD bus
	rcc_periph_clock_enable(port2RCC(bus_props->rs.port));
	gpio_clear(bus_props->rs.port, bus_props->rs.gpio);
	gpio_mode_setup(bus_props->rs.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->rs.gpio);

	rcc_periph_clock_enable(port2RCC(bus_props->e.port));
	gpio_clear(bus_props->e.port, bus_props->e.gpio);
	gpio_mode_setup(bus_props->e.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->e.gpio);

	rcc_periph_clock_enable(port2RCC(bus_props->rnw.port));
	gpio_clear(bus_props->rnw.port, bus_props->rnw.gpio);
	gpio_mode_setup(bus_props->rnw.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->rnw.gpio);

	rcc_periph_clock_enable(port2RCC(bus_props->db7.port));
	gpio_clear(bus_props->db7.port, bus_props->db7.gpio);
	gpio_mode_setup(bus_props->db7.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db7.gpio);

	rcc_periph_clock_enable(port2RCC(bus_props->db6.port));
	gpio_clear(bus_props->db6.port, bus_props->db6.gpio);
	gpio_mode_setup(bus_props->db6.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db6.gpio);

	rcc_periph_clock_enable(port2RCC(bus_props->db5.port));
	gpio_clear(bus_props->db5.port, bus_props->db5.gpio);
	gpio_mode_setup(bus_props->db5.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db5.gpio);

	rcc_periph_clock_enable(port2RCC(bus_props->db4.port));
	gpio_clear(bus_props->db4.port, bus_props->db4.gpio);
	gpio_mode_setup(bus_props->db4.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db4.gpio);

	if (bus8) {
		rcc_periph_clock_enable(port2RCC(bus_props->db3.port));
		gpio_clear(bus_props->db3.port, bus_props->db3.gpio);
		gpio_mode_setup(bus_props->db3.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db3.gpio);

		rcc_periph_clock_enable(port2RCC(bus_props->db2.port));
		gpio_clear(bus_props->db2.port, bus_props->db2.gpio);
		gpio_mode_setup(bus_props->db2.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db2.gpio);

		rcc_periph_clock_enable(port2RCC(bus_props->db1.port));
		gpio_clear(bus_props->db1.port, bus_props->db1.gpio);
		gpio_mode_setup(bus_props->db1.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db1.gpio);

		rcc_periph_clock_enable(port2RCC(bus_props->db0.port));
		gpio_clear(bus_props->db0.port, bus_props->db0.gpio);
		gpio_mode_setup(bus_props->db0.port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, bus_props->db0.gpio);
	}

	sleep_ms(40);

	if (!bus8) {
		hd44780_init_4bits();
	}

	// Apply default configuration
	hd44780_fnc(false, num_lines, false);
	hd44780_dispay_ctrl(true, false, false);
	hd44780_mode(true, false);
	hd44780_cursor_ctrl(false, false);

	hd44780_clear();
	hd44780_home();
}

void hd44780_putchar(int ch)
{
#ifdef hd44780_DO_CONVERT
	ch = hd44780_convert_rus(ch);
#endif

	hd44780_write(true, ch);
	hd44780_gotoxy(hd44780_params.position.x + 1, hd44780_params.position.y);
}

void hd44780_putchar_xy(uint8_t x, uint8_t y, int ch)
{
	hd44780_gotoxy(x, y);
	hd44780_putchar(ch);
}

static int hd44780_vsprintf(const char *fmt, va_list arg_ptr)
{
	char buf[HD44780_MAX_BUFFER_SIZE];
	int i, len;

	len = vsnprintf(buf, HD44780_MAX_BUFFER_SIZE, fmt, arg_ptr);

	for (i = 0; len > 0 && i < len; i++) {
		if (buf[i] == '\n') {
			hd44780_nl();
			continue;
		}

		hd44780_putchar(buf[i]);
	}

	return len;
}

void hd44780_printf(const char *fmt, ...)
{
	va_list arg_ptr;

	va_start(arg_ptr, fmt);
	hd44780_vsprintf(fmt, arg_ptr);
	va_end(arg_ptr);
}

void hd44780_printf_xy(uint8_t x, uint8_t y, const char *fmt, ...)
{
	va_list arg_ptr;

	hd44780_gotoxy(x, y);

	va_start(arg_ptr, fmt);
	hd44780_vsprintf(fmt, arg_ptr);
	va_end(arg_ptr);
}
