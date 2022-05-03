#ifndef HW_STM32H735_FLASH_REG_H
#define HW_STM32H735_FLASH_REG_H

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

#define FLASH_ACR 0x000
#define FLASH_KEYR 0x004
#define FLASH_OPTKEYR 0x008
#define FLASH_CR 0x00C
#define FLASH_SR 0x010
#define FLASH_CCR 0x014
#define FLASH_OPTCR 0x018
#define FLASH_OPTSR_CUR 0x01C
#define FLASH_OPTSR_PRG 0x020
#define FLASH_OPTCCR 0x024
#define FLASH_PRAR_CUR 0x028
#define FLASH_PRAR_PRG 0x02C
#define FLASH_SCAR_CUR 0x030
#define FLASH_SCAR_PRG 0x034
#define FLASH_WPSN_CUR 0x038
#define FLASH_WPSN_PRG 0x03C
#define FLASH_BOOT_CUR 0x040
#define FLASH_BOOT_PRG 0x044
#define FLASH_CRCCR 0x050
#define FLASH_CRCSADDR 0x054
#define FLASH_CRCEADDR 0x058
#define FLASH_CRCDATAR 0x05C
#define FLASH_ECC_FAR 0x060
#define FLASH_OPTSR2_CUR 0x070
#define FLASH_OPTSR2_PRG 0x074

//latency
#define FLASH_ACR_MASK ( 1 << 0 | 1 << 1 | 1 << 2 | 1 << 3)

#define FLASH_KEYR_KEY1 0x45670123
#define FLASH_KEYR_KEY2 0xCDEF89AB
#define FLASH_CR_PG (1 << 1)
#define FLASH_CR_LOCK (0 << 0)
#define FLASH_CR_FW (1 << 6)


#define TYPE_STM32H735_FLASH_REG "stm32h735-flash-reg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32H735FlashRegState, STM32H735_FLASH_REG)

struct STM32H735FlashRegState 
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t flash_acr;
    uint32_t flash_keyr;
    uint32_t flash_optkeyr;
    uint32_t flash_cr;
    uint32_t flash_sr;
    uint32_t flash_ccr;
    uint32_t flash_optcr;
    uint32_t flash_optsr_cur;
    uint32_t flash_optsr_prg;
    uint32_t flash_optccr;
    uint32_t flash_prar_cur;
    uint32_t flash_prar_prg;
    uint32_t flash_scar_cur;
    uint32_t flash_scar_prg;
    uint32_t flash_wpsn_cur;
    uint32_t flash_wpsn_prg;
    uint32_t flash_boot_cur;
    uint32_t flash_boot_prg;
    uint32_t flash_crccr;
    uint32_t flash_crcsaddr;
    uint32_t flash_crceaddr;
    uint32_t flash_crcdatar;
    uint32_t flash_ecc_far;
    uint32_t flash_optsr2_cur;
    uint32_t flash_optsr2_prg;


    CharBackend chr;
    qemu_irq irq;
};
#endif/* HW_STM32H735_FLASH_REG_H */