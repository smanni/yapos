#ifndef YAPOS_H
#define YAPOS_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "yapos_config.h"

typedef enum {
	YAPOS_ERR_OK = 0,
	YAPOS_ERR_WRONG_STATE,
	YAPOS_ERR_NO_MEM,
	YAPOS_ERR_INVALID_PARAM,
} yapos_err_t;

yapos_err_t yapos_init(void);
yapos_err_t yapos_add_task(void (*handler)(void *params), void *params,
		uint32_t *stack, size_t stack_size);
yapos_err_t yapos_start(uint32_t systick_ticks);

#endif
