/* industrial-digital-out-hs-4-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * config_ido4.h: Config for output and led pins
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CONFIG_IDO4_H
#define CONFIG_IDO4_H

#include "xmc_gpio.h"

#define IDO4_OUT0_PORT XMC_GPIO_PORT1
#define IDO4_OUT0_PIN  0
#define IDO4_OUT1_PORT XMC_GPIO_PORT0
#define IDO4_OUT1_PIN  0
#define IDO4_OUT2_PORT XMC_GPIO_PORT0
#define IDO4_OUT2_PIN  5
#define IDO4_OUT3_PORT XMC_GPIO_PORT0
#define IDO4_OUT3_PIN  6

#define IDO4_LED0_PORT XMC_GPIO_PORT0
#define IDO4_LED0_PIN  12
#define IDO4_LED1_PORT XMC_GPIO_PORT0
#define IDO4_LED1_PIN  9
#define IDO4_LED2_PORT XMC_GPIO_PORT0
#define IDO4_LED2_PIN  8
#define IDO4_LED3_PORT XMC_GPIO_PORT0
#define IDO4_LED3_PIN  7

#endif