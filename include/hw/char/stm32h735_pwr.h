#ifndef HW_STM32H735_PWR_H
#define HW_STM32H735_PWR_H

#include "chardev/char-fe.h"
#include "qom/object.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "exec/address-spaces.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "hw/misc/unimp.h"
#include "sysemu/sysemu.h"
#include "qemu/error-report.h"
#include "hw/sysbus.h"

#define PWR_CR1     0x000
#define PWR_CSR1    0x004
#define PWR_CR2     0x008
#define PWR_CR3     0x00C
#define PWR_CPUCR   0x010
#define PWR_D3CR    0x018
#define PWR_WKUPCR  0x020
#define PWR_WKUPFR  0x024
#define PWR_WKUPEPR 0x028

#define PWR_D3CR_VOSRDY  (1 << 13)
#define PWR_D3CR_VOS    (0 << 14 && 0 << 15)
#define PWR_CR3_BYPASS   (1 << 0)
#define PWR_CR3_LDOEN    (0 << 1)
#define PWR_CR3_SDEN     (0 << 2)
#define PWR_CSR1_ACTVOSRDY (1 << 13)


#define TYPE_STM32H735_PWR "stm32h735-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32H735PwrState, STM32H735_PWR)

struct STM32H735PwrState 
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t pwr_cr1;
    uint32_t pwr_csr1;
    uint32_t pwr_cr2;
    uint32_t pwr_cr3;
    uint32_t pwr_cpucr;
    uint32_t pwr_d3cr;
    uint32_t pwr_wkupcr;
    uint32_t pwr_wkupfr;
    uint32_t pwr_wkupepr;

    uint32_t flash_acr;

    CharBackend chr;
    qemu_irq irq;
};
#endif/* HW_STM32H735_PWR_H */