/* industrial-digital-out-hs-4-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * vnh140aj.c: Driver for VN7140AJ adc handling
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

#include "vn7140aj.h"

#include "bricklib2/hal/system_timer/system_timer.h"

#include "configs/config_vn7140aj.h"

#include "xmc_vadc.h"

VN7140AJ vn7140aj;

int8_t vn7140_channel_to_value_index[16] = {
	4, // P2_6  -> msense2  (ch 0)
	1, // P2_7  -> vsense1  (ch 1)
	3, // P2_9  -> msense1  (ch 2)
	2, // P2_10 -> msense0  (ch 3)
	0, // P2_11 -> vsense0  (ch 4)
	-1, -1,
	5, // P2_2  -> msense3  (ch 7)
	-1, -1, -1, -1, -1, -1, -1, -1
};

#define VN7140AJ_MAX_ADC_VALUE ((1  << 12) - 1)

// resistor divider 1k/10k,  12 bit -> mV => 3300*11/4095 = 2420/273
#define VN7140AJ_ADC_TO_VSENSE(value) ((value)*2420/273)

// V_SENSE = R_SENSE · I_OUT / K
// R_SENSE = 1000, K = 550
// => I_OUT = V_SENSE*550/1000 = 11/20
#define VN7140AJ_ADC_TO_MSENSE(value) ((value)*11/20)

void vn7140aj_log_values(void) {
	static uint32_t t = 0;

	if(system_timer_is_time_elapsed_ms(t, 250)) {
		uartbb_printf("ADC values:\n\r");
		uartbb_printf(" * vsense 0: %d mV\n\r", vn7140aj.values[VN7140AJ_VSENSE0_INDEX]);
		uartbb_printf(" * vsense 1: %d mV\n\r", vn7140aj.values[VN7140AJ_VSENSE1_INDEX]);
		uartbb_printf("\n\r");
		uartbb_printf(" * msense 0: %d mA\n\r", vn7140aj.values[VN7140AJ_MSENSE0_INDEX]);
		uartbb_printf(" * msense 1: %d mA\n\r", vn7140aj.values[VN7140AJ_MSENSE1_INDEX]);
		uartbb_printf(" * msense 2: %d mA\n\r", vn7140aj.values[VN7140AJ_MSENSE2_INDEX]);
		uartbb_printf(" * msense 3: %d mA\n\r", vn7140aj.values[VN7140AJ_MSENSE3_INDEX]);
		uartbb_printf("\n\r");
		uartbb_printf("\n\r");

		t = system_timer_get_ms();
	}
}

void vn7140aj_tick(void) {
	const uint32_t value = VADC->GLOBRES;
	if(value & (1 << 31)) {
		const uint32_t channel = (value >> 20) & 0b1111;
		const int8_t index = vn7140_channel_to_value_index[channel];
		if(index != -1) {
			vn7140aj.values[index] = index < 2 ? VN7140AJ_ADC_TO_VSENSE(value & 0xFFFF) : VN7140AJ_ADC_TO_MSENSE(value & 0xFFFF);
		}
	}

	vn7140aj_log_values();
}

void vn7140aj_init(void) {
	memset(&vn7140aj, 0, sizeof(VN7140AJ));

	// This structure contains the Global related Configuration.
	const XMC_VADC_GLOBAL_CONFIG_t adc_global_config = {
		.boundary0 = (uint32_t) 0, // Lower boundary value for Normal comparison mode
		.boundary1 = (uint32_t) 0, // Upper boundary value for Normal comparison mode

		.class0 = {
			.sample_time_std_conv     = 31,                      // The Sample time is (2*tadci)
			.conversion_mode_standard = XMC_VADC_CONVMODE_12BIT, // 12bit conversion Selected

		},
		.class1 = {
			.sample_time_std_conv     = 31,                      // The Sample time is (2*tadci)
			.conversion_mode_standard = XMC_VADC_CONVMODE_12BIT, // 12bit conversion Selected

		},

		.data_reduction_control         = 0b11, // Accumulate 4 result values
		.wait_for_read_mode             = 0, // GLOBRES Register will not be overwritten until the previous value is read
		.event_gen_enable               = 1, // Result Event from GLOBRES is enabled
		.disable_sleep_mode_control     = 0  // Sleep mode is enabled
	};


	// Global iclass0 configuration
	const XMC_VADC_GLOBAL_CLASS_t adc_global_iclass_config = {
		.conversion_mode_standard = XMC_VADC_CONVMODE_12BIT,
		.sample_time_std_conv	  = 31,
	};

	// Global Result Register configuration structure
	XMC_VADC_RESULT_CONFIG_t adc_global_result_config = {
		.data_reduction_control = 0, //0b11, // Accumulate 4 result values
		.post_processing_mode   = XMC_VADC_DMM_REDUCTION_MODE,
		.wait_for_read_mode  	= 1, // Enabled
		.part_of_fifo       	= 0, // No FIFO
		.event_gen_enable   	= 1  // Enable Result event
	};

	// LLD Background Scan Init Structure
	const XMC_VADC_BACKGROUND_CONFIG_t adc_background_config = {
		.conv_start_mode   = XMC_VADC_STARTMODE_CIR,       // Conversion start mode selected as cancel inject repeat
		.req_src_priority  = XMC_VADC_GROUP_RS_PRIORITY_1, // Priority of the Background request source in the VADC module
		.trigger_signal    = XMC_VADC_REQ_TR_A,            // If Trigger needed then this denotes the Trigger signal
		.trigger_edge      = XMC_VADC_TRIGGER_EDGE_NONE,   // If Trigger needed then this denotes Trigger edge selected
		.gate_signal       = XMC_VADC_REQ_GT_A,			   // If Gating needed then this denotes the Gating signal
		.timer_mode        = 0,							   // Timer Mode Disabled
		.external_trigger  = 0,                            // Trigger is Disabled
		.req_src_interrupt = 0,                            // Background Request source interrupt Disabled
		.enable_auto_scan  = 1,
		.load_mode         = XMC_VADC_SCAN_LOAD_OVERWRITE
	};


	XMC_VADC_GLOBAL_Init(VADC, &adc_global_config);
	XMC_VADC_GLOBAL_StartupCalibration(VADC);

	// Initialize the Global Conversion class 0
	XMC_VADC_GLOBAL_InputClassInit(VADC, adc_global_iclass_config, XMC_VADC_GROUP_CONV_STD, 0);
	// Initialize the Global Conversion class 1
	XMC_VADC_GLOBAL_InputClassInit(VADC, adc_global_iclass_config, XMC_VADC_GROUP_CONV_STD, 1);

	// Initialize the Background Scan hardware
	XMC_VADC_GLOBAL_BackgroundInit(VADC, &adc_background_config);

	// Initialize the global result register
	XMC_VADC_GLOBAL_ResultInit(VADC, &adc_global_result_config);

	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, VN7140AJ_VSENSE0_CHANNEL);
	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, VN7140AJ_VSENSE1_CHANNEL);
	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, VN7140AJ_MSENSE0_CHANNEL);
	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, VN7140AJ_MSENSE1_CHANNEL);
	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, VN7140AJ_MSENSE2_CHANNEL);
	XMC_VADC_GLOBAL_BackgroundAddChannelToSequence(VADC, 0, VN7140AJ_MSENSE3_CHANNEL);

	XMC_VADC_GLOBAL_BackgroundTriggerConversion(VADC);
}