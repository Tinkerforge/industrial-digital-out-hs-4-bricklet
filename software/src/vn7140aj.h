/* industrial-digital-out-hs-4-bricklet
 * Copyright (C) 2019 Olaf Lüke <olaf@tinkerforge.com>
 *
 * vnh140aj.h: Driver for VN7140AJ adc handling
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

#ifndef VN7140AJ_H
#define VN7140AJ_H

#include <stdint.h>

#define VN7140AJ_ADC_NUM 6

#define VN7140AJ_VSENSE0_INDEX 0
#define VN7140AJ_VSENSE1_INDEX 1
#define VN7140AJ_MSENSE0_INDEX 2
#define VN7140AJ_MSENSE1_INDEX 3
#define VN7140AJ_MSENSE2_INDEX 4
#define VN7140AJ_MSENSE3_INDEX 5

typedef struct {
    uint16_t values[VN7140AJ_ADC_NUM];
} VN7140AJ;

extern VN7140AJ vn7140aj;

void vn7140aj_tick(void);
void vn7140aj_init(void);

#endif