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

// Local headers
#include "include/helper.h"

volatile enum rcc_periph_clken port2RCC(uint32_t port)
{
	switch (port) {
		case GPIOA:
			return RCC_GPIOA;
		case GPIOB:
			return RCC_GPIOB;
		case GPIOC:
			return RCC_GPIOC;
		case GPIOD:
			return RCC_GPIOD;
		case GPIOE:
			return RCC_GPIOE;
		default:
			// Halt - stop on break point and wait for debugger
			__asm__("BKPT");
	}
}
