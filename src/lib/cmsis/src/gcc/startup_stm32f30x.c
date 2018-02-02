/** \file  startup_stm32f30x.c
 ** \brief  STM32F30x system start-up
 ** \author S. Rossi
 **
 **  Version Control Informations:
 **     $Revision: 747 $
 **     $LastChangedDate: 2015-12-18 18:28:36 +0100 (ven, 18 dic 2015) $
 **     $LastChangedBy: Stefano.Rossi $
 */

#include <stdint.h>
#include <sys/types.h>
#include "stm32f30x.h"


#ifdef STM32F30X

/* LINKER SYMBOLS */
extern unsigned int _sidata;        /* Start address of the init values of the .data section */
extern unsigned int _sdata;         /* Start address of .data section */
extern unsigned int _edata;         /* End address of .data section */
extern unsigned int __bss_start__;  /* Start address of the .bss section */
extern unsigned int __bss_end__;    /* End address of the .bss section */
extern unsigned int _estack;        /* Stack pointer reset address */

extern void (*__preinit_array_start[])(void) __attribute__((weak));
extern void (*__preinit_array_end[])(void)   __attribute__((weak));
extern void (*__init_array_start[])(void)    __attribute__((weak));
extern void (*__init_array_end[])(void)      __attribute__((weak));
extern void(*__fini_array_start[])(void)     __attribute__((weak));
extern void (*__fini_array_end[])(void)      __attribute__((weak));

extern void SystemInit(void);       /* hardware initialization (clock & IVT base address) user defined */
extern void main(void);             /* user main application */

/** \brief Init ".data" section */
static inline void __attribute__((always_inline)) __initialize_data(unsigned int* from, unsigned int* section_begin, unsigned int* section_end)
{
    unsigned int *p = section_begin;
    while (p < section_end)
    {
        *p++ = *from++;
    }
}

/** \brief Init ".bss" section */
static inline void __attribute__((always_inline)) __initialize_bss(unsigned int* section_begin, unsigned int* section_end)
{
    unsigned int *p = section_begin;
    while (p < section_end)
    {
        *p++ = 0;
    }
}

/** \brief  Iterate over all the preinit/init routines (mainly static constructors of C++ objects) */
static inline void __attribute__((always_inline)) __run_init_array(void)
{
    int count;
    int i;

    count = __preinit_array_end - __preinit_array_start;
    for (i = 0; i < count; i++)
    __preinit_array_start[i]();

    /*
     * If you need to run the code in the .init section, please use
     * the startup files, since this requires the code in crti.o and crtn.o
     * to add the function prologue/epilogue.
     * _init(); // DO NOT ENABE THIS!
     */

    count = __init_array_end - __init_array_start;
    for (i = 0; i < count; i++)
    {
        __init_array_start[i]();
    }
}

/** \brief  Run static destructors */
static inline void __attribute__((always_inline)) __run_fini_array(void)
{
    int count;
    int i;

    count = __fini_array_end - __fini_array_start;
    for (i = count; i > 0; i--)
    {
        __fini_array_start[i - 1]();
    }
}

/** \brief Initialize MCU base hardware (clock & IVT) */
static void __initialize_hardware(void)
{
    SystemInit();

#if defined (__VFP_FP__) && !defined (__SOFTFP__)
    /*
     * Enable the Cortex-M4 FPU only when -mfloat-abi=hard.
     * Code taken from Section 7.1, Cortex-M4 TRM (DDI0439C)
     */
    SCB->CPACR |= (0xF << 20);
#endif
}


/** \brief Start-Up routine
 * \note Cortex-M start code execution from here
 * \note \c _start symbol cannot be declared static
 */
void __attribute__ ((section(".after_vectors"),noreturn)) _start(void)
{
    /* Zero-fill the bss segment */
    __initialize_bss(&__bss_start__, &__bss_end__);

    /*
    * Copy the data segment from Flash to RAM.
    * When using startup files, this code is executed via the preinit array.
    */
    __initialize_data(&_sidata, &_sdata, &_edata);

    /* Set-Up base hardware */
    __initialize_hardware();

    /*
    * Standard library initialisation
    */
    __run_init_array();

    /* call user main */
    main();

    /*
    *  Run the C++ static destructors
    */
    __run_fini_array();

    /* Code should never reach here */
    for(;;){ }
}


/** \brief MCU Reset Handler
 * \note Called at mcu power-on-reset
 */
void __attribute__ ((section(".after_vectors"), noreturn)) Reset_Handler(void)
{
    _start();
}


/** \brief IRQ Default Handler
 * \note IRQ vectors are defined as weak symbols and are aliased by this default routine
 * \not  If IRQ is not re-defined but called at run-time code exection stops here
 */
void __attribute__ ((section(".after_vectors"))) IRQ_DefaultHandler(void)
{
	for(;;)
    {
    }
}


/* IRQ Default ISR */
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) NMI_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) HardFault_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) MemManage_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) BusFault_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) UsageFault_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) SVC_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DebugMon_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) PendSV_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) SysTick_Handler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) WWDG_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) PVD_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TAMPER_STAMP_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) RTC_WKUP_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) FLASH_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) RCC_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI0_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI1_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI2_TS_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI4_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel1_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel2_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel4_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel5_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel6_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA1_Channel7_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) ADC1_2_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USB_HP_CAN1_TX_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USB_LP_CAN1_RX0_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) CAN1_RX1_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) CAN1_SCE_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI9_5_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM1_BRK_TIM15_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM1_UP_TIM16_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM1_TRG_COM_TIM17_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM1_CC_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM2_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM4_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) I2C1_EV_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) I2C1_ER_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) I2C2_EV_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) I2C2_ER_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) SPI1_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) SPI2_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USART1_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USART2_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USART3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) EXTI15_10_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) RTC_Alarm_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USBWakeUp_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM8_BRK_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM8_UP_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM8_TRG_COM_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM8_CC_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) ADC3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) SPI3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) UART4_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) UART5_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM6_DAC_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) TIM7_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA2_Channel1_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA2_Channel2_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA2_Channel3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA2_Channel4_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) DMA2_Channel5_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) ADC4_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) COMP1_2_3_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) COMP4_5_6_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) COMP7_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USB_HP_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USB_LP_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) USBWakeUp_RMP_IRQHandler(void);
void __attribute__ ((weak, alias ("IRQ_DefaultHandler"))) FPU_IRQHandler(void);



/** \brief IRQ vector type */
typedef void (* const pfnIRQHandler)(void);

/** \brief STM32F30x Interrupt Vector Table */
__attribute__ ((section(".isr_vector")))
pfnIRQHandler __isr_vectors[] =
{
    (pfnIRQHandler)&_estack,
    Reset_Handler,                  // Reset Handler

    NMI_Handler,                    // NMI Handler
    HardFault_Handler,              // Hard Fault Handler
    MemManage_Handler,              // MPU Fault Handler
    BusFault_Handler,               // Bus Fault Handler
    UsageFault_Handler,             // Usage Fault Handler
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    SVC_Handler,                    // SVCall Handler
    DebugMon_Handler,               // Debug Monitor Handler
    0,                              // Reserved
    PendSV_Handler,                 // PendSV Handler
    SysTick_Handler,                // SysTick Handler

    // External Interrupts
    WWDG_IRQHandler,                // Window WatchDog
    PVD_IRQHandler,                 // PVD through EXTI Line detection
    TAMPER_STAMP_IRQHandler,        // Tamper and TimeStamps through the EXTI line
    RTC_WKUP_IRQHandler,            // RTC Wakeup through the EXTI line
    FLASH_IRQHandler,               // FLASH
    RCC_IRQHandler,                 // RCC
    EXTI0_IRQHandler,               // EXTI Line0
    EXTI1_IRQHandler,               // EXTI Line1
    EXTI2_TS_IRQHandler,            // EXTI Line2 and Touch Sense
    EXTI3_IRQHandler,               // EXTI Line3
    EXTI4_IRQHandler,               // EXTI Line4
    DMA1_Channel1_IRQHandler,       // DMA1 Channel 1
    DMA1_Channel2_IRQHandler,       // DMA1 Channel 2
    DMA1_Channel3_IRQHandler,       // DMA1 Channel 3
    DMA1_Channel4_IRQHandler,       // DMA1 Channel 4
    DMA1_Channel5_IRQHandler,       // DMA1 Channel 5
    DMA1_Channel6_IRQHandler,       // DMA1 Channel 6
    DMA1_Channel7_IRQHandler,       // DMA1 Channel 7
    ADC1_2_IRQHandler,              // ADC1 and ADC2
    USB_HP_CAN1_TX_IRQHandler,      // USB Device High Priority or CAN1 TX
    USB_LP_CAN1_RX0_IRQHandler,     // USB Device Low Priority or CAN1 RX0
    CAN1_RX1_IRQHandler,            // CAN1 RX1
    CAN1_SCE_IRQHandler,            // CAN1 SCE
    EXTI9_5_IRQHandler,             // External Line[9:5]s
    TIM1_BRK_TIM15_IRQHandler,      // TIM1 Break and TIM15
    TIM1_UP_TIM16_IRQHandler,       // TIM1 Update and TIM16
    TIM1_TRG_COM_TIM17_IRQHandler,  // TIM1 Trigger and Commutation and TIM17
    TIM1_CC_IRQHandler,             // TIM1 Capture Compare
    TIM2_IRQHandler,                // TIM2
    TIM3_IRQHandler,                // TIM3
    TIM4_IRQHandler,                // TIM4
    I2C1_EV_IRQHandler,             // I2C1 Event
    I2C1_ER_IRQHandler,             // I2C1 Error
    I2C2_EV_IRQHandler,             // I2C2 Event
    I2C2_ER_IRQHandler,             // I2C2 Error
    SPI1_IRQHandler,                // SPI1
    SPI2_IRQHandler,                // SPI2
    USART1_IRQHandler,              // USART1
    USART2_IRQHandler,              // USART2
    USART3_IRQHandler,              // USART3
    EXTI15_10_IRQHandler,           // External Line[15:10]s
    RTC_Alarm_IRQHandler,           // RTC Alarm (A and B) through EXTI Line
    USBWakeUp_IRQHandler,           // USB Wakeup through EXTI line
    TIM8_BRK_IRQHandler,            // TIM8 Break
    TIM8_UP_IRQHandler,             // TIM8 Update
    TIM8_TRG_COM_IRQHandler,        // TIM8 Trigger and Commutation
    TIM8_CC_IRQHandler,             // TIM8 Capture Compare
    ADC3_IRQHandler,                // ADC3
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    SPI3_IRQHandler,                // SPI3
    UART4_IRQHandler,               // UART4
    UART5_IRQHandler,               // UART5
    TIM6_DAC_IRQHandler,            // TIM6 and DAC1&2 underrun errors
    TIM7_IRQHandler,                // TIM7
    DMA2_Channel1_IRQHandler,       // DMA2 Channel 1
    DMA2_Channel2_IRQHandler,       // DMA2 Channel 2
    DMA2_Channel3_IRQHandler,       // DMA2 Channel 3
    DMA2_Channel4_IRQHandler,       // DMA2 Channel 4
    DMA2_Channel5_IRQHandler,       // DMA2 Channel 5
    ADC4_IRQHandler,                // ADC4
    0,                              // Reserved
    0,                              // Reserved
    COMP1_2_3_IRQHandler,           // COMP1, COMP2 and COMP3
    COMP4_5_6_IRQHandler,           // COMP4, COMP5 and COMP6
    COMP7_IRQHandler,               // COMP7
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    USB_HP_IRQHandler,              // USB High Priority remap
    USB_LP_IRQHandler,              // USB Low Priority remap
    USBWakeUp_RMP_IRQHandler,       // USB Wakeup remap through EXTI
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    0,                              // Reserved
    FPU_IRQHandler,                 // FPU
};

#endif

