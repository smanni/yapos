/** \file main.c
 ** \brief Application entry point
 ** \author S.Rossi
 **
 **  Version Control Informations:
 **     $Revision: 750 $
 **     $LastChangedDate: 2015-12-18 18:53:31 +0100 (ven, 18 dic 2015) $
 **     $LastChangedBy: Stefano.Rossi $
 */

#include "stm32f30x_rcc.h"
#include "stm32f30x_gpio.h"
#include "yapos.h"


#define ERR_TRAP(err_code) \
	do { \
		if (err_code != YAPOS_ERR_OK) \
			while(1); \
	} while (0)


static void delay(volatile uint32_t time)
{
	while (time > 0)
		time--;
}

static void task_blue(void *p_params)
{
	while (1) {
		__disable_irq();
		GPIOE->ODR ^= GPIO_Pin_8;
		__enable_irq();

		delay(0x100000);
	}
}

static void task_red(void *p_params)
{
	while (1) {
		__disable_irq();
		GPIOE->ODR ^= GPIO_Pin_9;
		__enable_irq();

		delay(0x200000);
	}
}

static void task_orange(void *p_params)
{
	while (1) {
		__disable_irq();
		GPIOE->ODR ^= GPIO_Pin_10;
		__enable_irq();

		delay(0x100000);
	}
}

int main(void)
{
	yapos_err_t err_code;

	GPIO_InitTypeDef gpio;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOE, ENABLE);

	GPIO_StructInit(&gpio);
	gpio.GPIO_Pin= GPIO_Pin_8;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_OType = GPIO_OType_PP;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &gpio);

	gpio.GPIO_Pin= GPIO_Pin_9;
	GPIO_Init(GPIOE, &gpio);

	gpio.GPIO_Pin= GPIO_Pin_10;
	GPIO_Init(GPIOE, &gpio);

	/* Initialize task stacks: */
	static uint32_t stack1[128];
	static uint32_t stack2[128];
	static uint32_t stack3[128];

	err_code = yapos_init();
	ERR_TRAP(err_code);

	err_code = yapos_add_task(&task_blue, NULL, stack1, 128);
	ERR_TRAP(err_code);
	err_code = yapos_add_task(&task_red, NULL, stack2, 128);
	ERR_TRAP(err_code);
	err_code = yapos_add_task(&task_orange, NULL, stack3, 128);
	ERR_TRAP(err_code);

	/* Context switch every second: */
	err_code = yapos_start(0x8ffffUL);
	ERR_TRAP(err_code);

	/* The program should never reach there: */
	while (1);
}
