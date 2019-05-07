/* industrial-digital-out-hs-4-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * communication.c: TFP protocol message handling
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

#include "communication.h"

#include "bricklib2/utility/communication_callback.h"
#include "bricklib2/protocols/tfp/tfp.h"

#include "ido4.h"
#include "vn7140aj.h"

BootloaderHandleMessageResponse handle_message(const void *message, void *response) {
	switch(tfp_get_fid_from_message(message)) {
		case FID_SET_VALUE: return set_value(message);
		case FID_GET_VALUE: return get_value(message, response);
		case FID_GET_MEASUREMENTS: return get_measurements(message, response);
		case FID_SET_SELECTED_VALUE: return set_selected_value(message);
		case FID_SET_MONOFLOP: return set_monoflop(message);
		case FID_GET_MONOFLOP: return get_monoflop(message, response);
		case FID_SET_CHANNEL_LED_CONFIG: return set_channel_led_config(message);
		case FID_GET_CHANNEL_LED_CONFIG: return get_channel_led_config(message, response);
		case FID_SET_PWM_CONFIGURATION: return set_pwm_configuration(message);
		case FID_GET_PWM_CONFIGURATION: return get_pwm_configuration(message, response);
		case FID_SET_MEASUREMENTS_CALLBACK_CONFIGURATION: return set_measurements_callback_configuration(message);
		case FID_GET_MEASUREMENTS_CALLBACK_CONFIGURATION: return get_measurements_callback_configuration(message, response);
		default: return HANDLE_MESSAGE_RESPONSE_NOT_SUPPORTED;
	}
}

BootloaderHandleMessageResponse set_value(const SetValue *data) {
	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		// abort potentially pending monoflop
		ido4.channels[i].monoflop.time_start = 0;
		ido4.channels[i].monoflop.time_remaining = 0;
		ido4.channels[i].monoflop.running = false;

		if(data->value & (1 << i)) {
			ido4.channels[i].value = true;
			XMC_GPIO_SetOutputHigh(ido4.channels[i].port, ido4.channels[i].pin);
		}
		else {
			ido4.channels[i].value = false;
			XMC_GPIO_SetOutputLow(ido4.channels[i].port, ido4.channels[i].pin);
		}
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_value(const GetValue *data, GetValue_Response *response) {
	uint8_t bit_encoded_values = 0;
	response->header.length = sizeof(GetValue_Response);

	for(uint8_t i = 0; i < NUMBER_OF_CHANNELS; i++) {
		if(ido4.channels[i].value) {
			bit_encoded_values |= (1 << i);
		}
		else {
			bit_encoded_values &= ~(1 << i);
		}
	}

	response->value = bit_encoded_values;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse get_measurements(const GetMeasurements *data, GetMeasurements_Response *response) {
	response->header.length = sizeof(GetMeasurements_Response);
	response->voltage[0]    = vn7140aj.values[VN7140AJ_VSENSE0_INDEX];
	response->voltage[1]    = vn7140aj.values[VN7140AJ_VSENSE1_INDEX];
	response->current[0]    = vn7140aj.values[VN7140AJ_MSENSE0_INDEX];
	response->current[1]    = vn7140aj.values[VN7140AJ_MSENSE1_INDEX];
	response->current[2]    = vn7140aj.values[VN7140AJ_MSENSE2_INDEX];
	response->current[3]    = vn7140aj.values[VN7140AJ_MSENSE3_INDEX];

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_selected_value(const SetSelectedValue *data) {
	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// Reset PWM
	ido4_pwm_stop(data->channel);

	// abort potentially pending monoflop
	ido4.channels[data->channel].monoflop.time_start = 0;
	ido4.channels[data->channel].monoflop.time_remaining = 0;
	ido4.channels[data->channel].monoflop.running = false;

	ido4.channels[data->channel].value = data->value;

	if(ido4.channels[data->channel].value) {
		XMC_GPIO_SetOutputHigh(ido4.channels[data->channel].port,
		                       ido4.channels[data->channel].pin);
	}
	else {
		XMC_GPIO_SetOutputLow(ido4.channels[data->channel].port,
		                      ido4.channels[data->channel].pin);
	}

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse set_monoflop(const SetMonoflop *data) {
	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// Reset PWM
	ido4_pwm_stop(data->channel);

	ido4.channels[data->channel].value = data->value;
	ido4.channels[data->channel].monoflop.time = data->time;
	ido4.channels[data->channel].monoflop.time_remaining = data->time;
	ido4.channels[data->channel].monoflop.running = true;

	if(ido4.channels[data->channel].value) {
		XMC_GPIO_SetOutputHigh(ido4.channels[data->channel].port,
		                       ido4.channels[data->channel].pin);
	}
	else {
		XMC_GPIO_SetOutputLow(ido4.channels[data->channel].port,
		                      ido4.channels[data->channel].pin);
	}

	ido4.channels[data->channel].monoflop.time_start = system_timer_get_ms();

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_monoflop(const GetMonoflop *data, GetMonoflop_Response *response) {
	response->header.length = sizeof(GetMonoflop_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->time = ido4.channels[data->channel].monoflop.time;
	response->value = ido4.channels[data->channel].value;
	response->time_remaining = ido4.channels[data->channel].monoflop.time_remaining;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_channel_led_config(const SetChannelLEDConfig *data) {
	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	ido4.channels[data->channel].status_led.config = data->config;

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_channel_led_config(const GetChannelLEDConfig *data, GetChannelLEDConfig_Response *response) {
	response->header.length = sizeof(GetChannelLEDConfig_Response);

	if(data->channel > NUMBER_OF_CHANNELS - 1) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->config = ido4.channels[data->channel].status_led.config;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_pwm_configuration(const SetPWMConfiguration *data) {
	if(data->channel >= NUMBER_OF_CHANNELS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	// Reset monoflop
	ido4.channels[data->channel].monoflop.time_start = 0;
	ido4.channels[data->channel].monoflop.time_remaining = 0;
	ido4.channels[data->channel].monoflop.running = false;

	ido4_pwm_update(data->channel, data->frequency, data->duty_cycle);

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_pwm_configuration(const GetPWMConfiguration *data, GetPWMConfiguration_Response *response) {
	if(data->channel >= NUMBER_OF_CHANNELS) {
		return HANDLE_MESSAGE_RESPONSE_INVALID_PARAMETER;
	}

	response->header.length = sizeof(GetPWMConfiguration_Response);
	response->duty_cycle    = ido4.channels[data->channel].pwm.duty_cycle;
	response->frequency     = ido4.channels[data->channel].pwm.frequency;

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

BootloaderHandleMessageResponse set_measurements_callback_configuration(const SetMeasurementsCallbackConfiguration *data) {

	return HANDLE_MESSAGE_RESPONSE_EMPTY;
}

BootloaderHandleMessageResponse get_measurements_callback_configuration(const GetMeasurementsCallbackConfiguration *data, GetMeasurementsCallbackConfiguration_Response *response) {
	response->header.length = sizeof(GetMeasurementsCallbackConfiguration_Response);

	return HANDLE_MESSAGE_RESPONSE_NEW_MESSAGE;
}

bool handle_monoflop_done_callback(void) {
	static bool is_buffered = false;
	static MonoflopDone_Callback cb;

	if(!is_buffered) {
		if(ringbuffer_is_empty(&ido4.monoflop_cb_rb)) {
			// Nothing to send
			return false;
		}

		tfp_make_default_header(&cb.header,
		                        bootloader_get_uid(),
		                        sizeof(MonoflopDone_Callback),
		                        FID_CALLBACK_MONOFLOP_DONE);

		ringbuffer_get(&ido4.monoflop_cb_rb, &cb.channel);
		ringbuffer_get(&ido4.monoflop_cb_rb, (uint8_t *)&cb.value);
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status,
		                                       (uint8_t*)&cb,
		                                       sizeof(MonoflopDone_Callback));

		is_buffered = false;

		return true;
	}
	else {
		is_buffered = true;
	}

	return false;
}

bool handle_measurements_callback(void) {
	static bool is_buffered = false;
	static Measurements_Callback cb;

	if(!is_buffered) {
		tfp_make_default_header(&cb.header, bootloader_get_uid(), sizeof(Measurements_Callback), FID_CALLBACK_MEASUREMENTS);
		// TODO: Implement Measurements callback handling

		return false;
	}

	if(bootloader_spitfp_is_send_possible(&bootloader_status.st)) {
		bootloader_spitfp_send_ack_and_message(&bootloader_status, (uint8_t*)&cb, sizeof(Measurements_Callback));
		is_buffered = false;
		return true;
	} else {
		is_buffered = true;
	}

	return false;
}

void communication_tick(void) {
	communication_callback_tick();
}

void communication_init(void) {
	communication_callback_init();
}
