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
 */

// Std headers
#include <stddef.h>

// libopencm3 headers
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

// Local headers
#include "include/keys.h"
#include "include/helper.h"

void keys_setup(struct keys_s *keys, int n)
{
	volatile int id;
	uint8_t pullup;

	// Configuring buttons array
	for (id = 0; id < n; id++) {
		// Enable clock on port
		rcc_periph_clock_enable(port2RCC(keys[id].port));

		if (keys[id].pup) {
			pullup = GPIO_PUPD_PULLUP;
		} else {
			pullup = GPIO_PUPD_NONE;
		}

		// Configure pin as input
		gpio_mode_setup(
				keys[id].port,
				GPIO_MODE_INPUT,
				pullup,
				keys[id].gpio);

		// Enable pull-up
		gpio_set(keys[id].port, keys[id].gpio);
	}
}

bool key_pressed(struct keys_s *keys, int id)
{
	bool val = !gpio_get(keys[id].port, keys[id].gpio);
	return val ^ keys[id].nc;
}
