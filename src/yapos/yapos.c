#include "yapos.h"

/* Task descriptor */
struct task {
	/* The stack pointer (sp) has to be the first element as it is located
	   at the same address as the structure itself (which makes it possible
	   to locate it safely from assembly implementation of PendSV_Handler).
	   The compiler might add padding between other structure elements. */
	volatile uint32_t sp;
	void (*handler)(void *params);
	void *params;
};

/* Tasks table */
struct tasks_table {
	struct task tasks[YAPOS_CONF_MAX_TASKS];
	volatile uint32_t current_task;
	uint32_t size;
};

/* Members */
static struct tasks_table tasks_tab;
volatile struct task *yapos_curr_task;
volatile struct task *yapos_next_task;
static bool init = false;

/* Function called when some task handler unexpectedly returns */
static void task_finished(void)
{
	volatile uint32_t i = 0;
	while (1)
		i++;
}

/* Init scheduler */
yapos_err_t yapos_init(void)
{
	/* Must be called once */
	if (init)
		return YAPOS_ERR_WRONG_STATE;
	init = true;

	memset(&tasks_tab, 0, sizeof(tasks_tab));

	return YAPOS_ERR_OK;
}

/* Register new task */
yapos_err_t yapos_add_task(void (*handler)(void *params), void *params,
		uint32_t *stack, size_t stack_size)
{
	/* Must be already initialized */
	if (!init)
		return YAPOS_ERR_WRONG_STATE;

	if (tasks_tab.size >= YAPOS_CONF_MAX_TASKS-1)
		return YAPOS_ERR_NO_MEM;

	/* Initialize the task structure and set SP to the top of the stack
	   minus 16 words (64 bytes) to leave space for storing 16 registers */
	struct task *p_task = &tasks_tab.tasks[tasks_tab.size];
	p_task->handler = handler;
	p_task->params = params;
	p_task->sp = (uint32_t)(stack+stack_size-16);

	/* Save init. values of registers which will be restored on exc. return:
	   - XPSR: Default value (0x01000000)
	   - PC: Point to the handler function
	   - LR: Point to a function to be called when the handler returns
	   - R0: Point to the handler function's parameter */
	stack[stack_size-1] = 0x01000000;
	stack[stack_size-2] = (uint32_t)handler;
	stack[stack_size-3] = (uint32_t)&task_finished;
	stack[stack_size-8] = (uint32_t)params;

#ifdef YAPOS_CONF_DEBUG
	uint32_t base = (tasks_tab.size+1)*1000;
	stack[stack_size-4] = base+12;  /* R12 */
	stack[stack_size-5] = base+3;   /* R3  */
	stack[stack_size-6] = base+2;   /* R2  */
	stack[stack_size-7] = base+1;   /* R1  */
	/* p_stack[stack_size-8] is R0 */
	stack[stack_size-9] = base+7;   /* R7  */
	stack[stack_size-10] = base+6;  /* R6  */
	stack[stack_size-11] = base+5;  /* R5  */
	stack[stack_size-12] = base+4;  /* R4  */
	stack[stack_size-13] = base+11; /* R11 */
	stack[stack_size-14] = base+10; /* R10 */
	stack[stack_size-15] = base+9;  /* R9  */
	stack[stack_size-16] = base+8;  /* R8  */
#endif

	tasks_tab.size++;

	return YAPOS_ERR_OK;
}

/* Start scheduler */
yapos_err_t yapos_start(uint32_t systick_ticks)
{
	/* Must be already initialized */
	if (!init)
		return YAPOS_ERR_WRONG_STATE;

	/* Lowest possible priority */
	NVIC_SetPriority(PendSV_IRQn, 0xff);
	/* Highest possible priority */
	NVIC_SetPriority(SysTick_IRQn, 0x00);

	/* Start the SysTick timer */
	uint32_t ret_val = SysTick_Config(systick_ticks);
	if (ret_val != 0)
		return YAPOS_ERR_INVALID_PARAM;

	/* Start the first task */
	yapos_curr_task = &tasks_tab.tasks[tasks_tab.current_task];

	/* Set PSP to the top of task's stack */
	__set_PSP(yapos_curr_task->sp + 64);
	/* Switch to Unprivileged Thread Mode with PSP */
	__set_CONTROL(0x03);
	/* Execute ISB after changing CONTORL (recommended) */
	__ISB();

	/* Call first task handler */
	yapos_curr_task->handler(yapos_curr_task->params);

	return YAPOS_ERR_OK;
}

/* Systick interrupt handler */
void SysTick_Handler(void)
{
	yapos_curr_task = &tasks_tab.tasks[tasks_tab.current_task];

	/* Select next task */
	tasks_tab.current_task++;
	if (tasks_tab.current_task >= tasks_tab.size)
		tasks_tab.current_task = 0;

	yapos_next_task = &tasks_tab.tasks[tasks_tab.current_task];

	/* Trigger PendSV which performs the actual context switch */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
