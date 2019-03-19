/* industrial-digital-out-hs-4-bricklet
 * Copyright (C) 2018 Ishraq Ibne Ashraf <ishraq@tinkerforge.com>
 * Copyright (C) 2018-2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * ido4.h: Implementation of Industrial Digital Out 4 V2 Bricklet
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

#ifndef IDO4_H
#define IDO4_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/utility/ringbuffer.h"
#include "bricklib2/utility/led_flicker.h"

#include "xmc_gpio.h"

#define NUMBER_OF_CHANNELS 4

#define MONOFLOP_CB_BUFFER_UNIT_SIZE 2
#define MONOFLOP_CB_BUFFER_SIZE (64 * MONOFLOP_CB_BUFFER_UNIT_SIZE)

typedef struct {
	uint8_t pin;
	uint8_t config;
	XMC_GPIO_PORT_t *port;
	LEDFlickerState channel_led_flicker_state;
} CH_STATUS_LED_CONFIG_t;

typedef struct {
	uint32_t time;
	uint32_t time_start;
	uint32_t time_remaining;
} CH_MONOFLOP_t;

typedef struct {
  uint32_t frequency;
  uint16_t duty_cycle;
} IO_CH_PWM_t;

typedef struct {
	// Generic channel config
	bool value;
	uint8_t pin;
	XMC_GPIO_PORT_t *port;

	// Channel monoflop config
	CH_MONOFLOP_t monoflop;

	// Channel PWM config
	IO_CH_PWM_t pwm;

	// Channel LED config
	CH_STATUS_LED_CONFIG_t status_led;
} CH_t;

typedef struct {
	CH_t channels[NUMBER_OF_CHANNELS];

	// Monoflop callback
	Ringbuffer monoflop_cb_rb;
	uint8_t monoflop_cb_buffer[MONOFLOP_CB_BUFFER_SIZE];
} IDO4_t;

extern IDO4_t ido4;

void ido4_pwm_stop(const uint8_t channel);
void ido4_pwm_update(const uint8_t channel, const uint32_t frequency, const uint16_t duty_cycle);
void ido4_init(void);
void ido4_tick(void);

#endif
