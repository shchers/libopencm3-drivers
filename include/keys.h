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
 * @brief Minimalistic driver for binary keys/buttons management
 */

// Std headers
#include <stdint.h>
#include <stdbool.h>

#ifndef KEYS_H
#define KEYS_H

/// Descriptor of buttons
struct keys_s {
	/// GPIO port id
	uint32_t port;
	/// GPIO pin - should be used only one pin
	uint16_t gpio;
	/// Enable Pull-Up on GPIO Pin
	bool pup;
	/// Should be true is button is "normally closed"
	bool nc;
#ifdef KEYS_ENABLE_KEY_CODES
	/// Assigned key code
	uint8_t key;
#endif
#ifdef KEYS_ENABLE_NAMES
	/// Key namespace
	const char *name;
#endif
};

/**
 * @brief Setup keys
 * @param	keys	Array of keys descriptors
 * @param	n	Number of descriptors in array
 * @return	None
 */
void keys_setup(struct keys_s *keys, int n);

/**
 * @brief Check if key is pressed
 * @param	keys	Array of keys descriptors
 * @param	id	Key id in descriptor
 * @return	True if key is pressed
 */
bool key_pressed(struct keys_s *keys, int id);

#endif // KEYS_H
