#ifndef HW_ARM_STM32H735_SOC_H
#define HW_ARM_STM32H735_SOC_H

#include "hw/char/stm32h735_usart.h"
#include "hw/arm/armv7m.h"
#include "qom/object.h"
#include "hw/clock.h"

#define TYPE_STM32H735_SOC "stm32h735-soc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32H735State, STM32H735_SOC)

#define STM_NUM_USARTS 3

#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE (128 * 1024)
/*
#define SRAM_BASE_ADDRESS 0x20000000
#define SRAM_SIZE (256 * 1024)
*/
#define RAM_EXEC_ADRESS 0x24000000
#define RAM_EXEC_SIZE (312 * 1024)
#define DTCMRAM_ADRESS 0x20000000
#define DTCMRAM_SIZE (125 * 1024)
#define ITCMRAM_ADRESS 0x00000000
#define ITCMRAM_SIZE (62 * 1024)
#define RAM_D2_ADRESS 0x30000000
#define RAM_D2_SIZE (31 * 1024)
#define RAM_D3_ADRESS 0x38000000
#define RAM_D3_SIZE (15 * 1024)


struct STM32H735State 
{
    /*< private >*/
    SysBusDevice parent_obj;

    /*< public >*/
    char *cpu_type;

    ARMv7MState armv7m;

    STM32H735UsartState usart[STM_NUM_USARTS];

    //MemoryRegion sram;
    MemoryRegion flash;
    MemoryRegion flash_alias;

    MemoryRegion ram_exec;
    MemoryRegion dtcmram;
    MemoryRegion itcmram;
    MemoryRegion ram_d2;
    MemoryRegion ram_d3;

    Clock *sysclk;
    Clock *refclk;
};

#endif