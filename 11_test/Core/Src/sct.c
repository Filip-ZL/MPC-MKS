#include <stdint.h>
#include "main.h"
#include "sct.h"

static const uint32_t reg_values[1][6] = {

		{
				//PCDE--------GFAB @ DIS3
				0b0000000000000010 << 0,
				0b0000000000000001 << 0,
				0b0100000000000000 << 0,
				0b0010000000000000 << 0,
				0b0001000000000000 << 0,
				0b0000000000000100 << 0,
		},

};

void sct_init(void)
{
	sct_led(0);
	HAL_GPIO_WritePin(SCT_NOE_GPIO_Port, SCT_NOE_Pin, 0);
}

void sct_led(uint32_t value)
{
	for (uint8_t i = 0; i < 32; i++) {
		HAL_GPIO_WritePin(SCT_SDI_GPIO_Port, SCT_SDI_Pin, value & 1);
		value >>= 1;
		HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 1);
		HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin, 0);
	}
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 1);
	HAL_GPIO_WritePin(SCT_NLA_GPIO_Port, SCT_NLA_Pin, 0);
}

void sct_value(uint16_t value)
{
	uint32_t reg = 0;

	reg |= reg_values[0][value / 1 % 10];

	sct_led(reg);
}
