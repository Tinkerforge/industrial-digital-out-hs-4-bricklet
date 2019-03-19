/* industrial-digital-out-hs-4-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.h: TFP protocol message handling
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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib2/protocols/tfp/tfp.h"
#include "bricklib2/bootloader/bootloader.h"

// Default functions
BootloaderHandleMessageResponse handle_message(const void *data, void *response);
void communication_tick(void);
void communication_init(void);

// Constants
#define INDUSTRIAL_DIGITAL_OUT_HS_4_CHANNEL_LED_CONFIG_OFF 0
#define INDUSTRIAL_DIGITAL_OUT_HS_4_CHANNEL_LED_CONFIG_ON 1
#define INDUSTRIAL_DIGITAL_OUT_HS_4_CHANNEL_LED_CONFIG_SHOW_HEARTBEAT 2
#define INDUSTRIAL_DIGITAL_OUT_HS_4_CHANNEL_LED_CONFIG_SHOW_CHANNEL_STATUS 3

#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_MODE_BOOTLOADER 0
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_MODE_FIRMWARE 1
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_MODE_BOOTLOADER_WAIT_FOR_REBOOT 2
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_REBOOT 3
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_MODE_FIRMWARE_WAIT_FOR_ERASE_AND_REBOOT 4

#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_STATUS_OK 0
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_STATUS_INVALID_MODE 1
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_STATUS_NO_CHANGE 2
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_STATUS_ENTRY_FUNCTION_NOT_PRESENT 3
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_STATUS_DEVICE_IDENTIFIER_INCORRECT 4
#define INDUSTRIAL_DIGITAL_OUT_HS_4_BOOTLOADER_STATUS_CRC_MISMATCH 5

#define INDUSTRIAL_DIGITAL_OUT_HS_4_STATUS_LED_CONFIG_OFF 0
#define INDUSTRIAL_DIGITAL_OUT_HS_4_STATUS_LED_CONFIG_ON 1
#define INDUSTRIAL_DIGITAL_OUT_HS_4_STATUS_LED_CONFIG_SHOW_HEARTBEAT 2
#define INDUSTRIAL_DIGITAL_OUT_HS_4_STATUS_LED_CONFIG_SHOW_STATUS 3

// Function and callback IDs and structs
#define FID_SET_VALUE 1
#define FID_GET_VALUE 2
#define FID_GET_MEASUREMENTS 3
#define FID_SET_SELECTED_VALUE 4
#define FID_SET_MONOFLOP 5
#define FID_GET_MONOFLOP 6
#define FID_SET_CHANNEL_LED_CONFIG 8
#define FID_GET_CHANNEL_LED_CONFIG 9
#define FID_SET_PWM_CONFIGURATION 10
#define FID_GET_PWM_CONFIGURATION 11
#define FID_SET_MEASUREMENTS_CALLBACK_CONFIGURATION 12
#define FID_GET_MEASUREMENTS_CALLBACK_CONFIGURATION 13

#define FID_CALLBACK_MONOFLOP_DONE 7
#define FID_CALLBACK_MEASUREMENTS 14

typedef struct {
	TFPMessageHeader header;
	uint8_t value;
} __attribute__((__packed__)) SetValue;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetValue;

typedef struct {
	TFPMessageHeader header;
	uint8_t value;
} __attribute__((__packed__)) GetValue_Response;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetMeasurements;

typedef struct {
	TFPMessageHeader header;
	uint16_t voltage[2];
	uint16_t current[4];
} __attribute__((__packed__)) GetMeasurements_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	bool value;
} __attribute__((__packed__)) SetSelectedValue;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	bool value;
	uint32_t time;
} __attribute__((__packed__)) SetMonoflop;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetMonoflop;

typedef struct {
	TFPMessageHeader header;
	bool value;
	uint32_t time;
	uint32_t time_remaining;
} __attribute__((__packed__)) GetMonoflop_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	bool value;
} __attribute__((__packed__)) MonoflopDone_Callback;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint8_t config;
} __attribute__((__packed__)) SetChannelLEDConfig;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetChannelLEDConfig;

typedef struct {
	TFPMessageHeader header;
	uint8_t config;
} __attribute__((__packed__)) GetChannelLEDConfig_Response;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
	uint32_t frequency;
	uint16_t duty_cycle;
} __attribute__((__packed__)) SetPWMConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint8_t channel;
} __attribute__((__packed__)) GetPWMConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint32_t frequency;
	uint16_t duty_cycle;
} __attribute__((__packed__)) GetPWMConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) SetMeasurementsCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
} __attribute__((__packed__)) GetMeasurementsCallbackConfiguration;

typedef struct {
	TFPMessageHeader header;
	uint32_t period;
	bool value_has_to_change;
} __attribute__((__packed__)) GetMeasurementsCallbackConfiguration_Response;

typedef struct {
	TFPMessageHeader header;
	uint16_t voltage[2];
	uint16_t current[4];
} __attribute__((__packed__)) Measurements_Callback;


// Function prototypes
BootloaderHandleMessageResponse set_value(const SetValue *data);
BootloaderHandleMessageResponse get_value(const GetValue *data, GetValue_Response *response);
BootloaderHandleMessageResponse get_measurements(const GetMeasurements *data, GetMeasurements_Response *response);
BootloaderHandleMessageResponse set_selected_value(const SetSelectedValue *data);
BootloaderHandleMessageResponse set_monoflop(const SetMonoflop *data);
BootloaderHandleMessageResponse get_monoflop(const GetMonoflop *data, GetMonoflop_Response *response);
BootloaderHandleMessageResponse set_channel_led_config(const SetChannelLEDConfig *data);
BootloaderHandleMessageResponse get_channel_led_config(const GetChannelLEDConfig *data, GetChannelLEDConfig_Response *response);
BootloaderHandleMessageResponse set_pwm_configuration(const SetPWMConfiguration *data);
BootloaderHandleMessageResponse get_pwm_configuration(const GetPWMConfiguration *data, GetPWMConfiguration_Response *response);
BootloaderHandleMessageResponse set_measurements_callback_configuration(const SetMeasurementsCallbackConfiguration *data);
BootloaderHandleMessageResponse get_measurements_callback_configuration(const GetMeasurementsCallbackConfiguration *data, GetMeasurementsCallbackConfiguration_Response *response);

// Callbacks
bool handle_monoflop_done_callback(void);
bool handle_measurements_callback(void);

#define COMMUNICATION_CALLBACK_TICK_WAIT_MS 1
#define COMMUNICATION_CALLBACK_HANDLER_NUM 2
#define COMMUNICATION_CALLBACK_LIST_INIT \
	handle_monoflop_done_callback, \
	handle_measurements_callback, \


#endif
