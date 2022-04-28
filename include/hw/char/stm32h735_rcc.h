#ifndef HW_STM32H735_RCC_H
#define HW_STM32H735_RCC_H

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

#define RCC_CR         0x000
#define RCC_HSICFGR    0x004
#define RCC_CRRCR      0x008
#define RCC_CSICFGR    0x00C
#define RCC_CFGR       0x010
#define RCC_D1CFGR     0x018
#define RCC_D2CFGR     0x01C
#define RCC_D3CFGR     0x020
#define RCC_PLLCKSELR  0x028
#define RCC_PLLCFGR    0x02C
#define RCC_PLL1DIVR   0x030
#define RCC_PLL1FRACR  0x034
#define RCC_PLL2DIVR   0x038
#define RCC_PLL2FRACR  0x03C
#define RCC_PLL3DIVR   0x040
#define RCC_PLL3FRACR  0x044
#define RCC_D2CCIP1R   0x050
#define RCC_D2CCIP2R   0x054
#define RCC_D3CCIPR    0x058
#define RCC_CIER       0x060
#define RCC_CIFR       0x64
#define RCC_CICR       0x68
#define RCC_BDCR       0x070
#define RCC_CSR        0x074
#define RCC_AHB3RSTR   0x07C
#define RCC_AHB1RSTR   0x080
#define RCC_AHB2RSTR   0x084
#define RCC_AHB4RSTR   0x088
#define RCC_APB3RSTR   0x08C
#define RCC_APB1LRSTR  0x090
#define RCC_APB1HRSTR  0x094
#define RCC_APB2RSTR   0x098
#define RCC_APB4RSTR   0x09C
#define RCC_GCR        0x0A0
#define RCC_D3AMR      0x0A8
#define RCC_RSR        0x0D0 // can be accessed with this 
//#define RCC_C1_RSR     0x130 // or this 
#define RCC_AHB3ENR    0x0D4 // can be acced with this
//#define RCC_C1_AHB3ENR 0x134 // or this 
#define RCC_AHB1ENR    0x0D8 // can be acced with this
//#define RCC_C1_AHB1ENR 0x138 // or this 
#define RCC_AHB2ENR    0x0DC // can be acced with this
//#define RCC_C1_AHB2ENR 0x13C // or this 
#define RCC_AHB4ENR    0x0E0 // can be acced with this
//#define RCC_C1_AHB4ENR 0x140 // or this 
#define RCC_APB3ENR    0x0E4 // can be acced with this
//#define RCC_C1_APB3ENR 0x144 // or this 
#define RCC_APB1LENR    0x0E8 // can be acced with this
//#define RCC_C1_APB1LENR 0x148 // or this 
#define RCC_APB1HENR    0x0EC // can be acced with this
//#define RCC_C1_APB1HENR 0x14C // or this 
#define RCC_APB2ENR    0x0F0 // can be acced with this
//#define RCC_C1_APB2ENR 0x150 // or this 
#define RCC_APB4ENR    0x0F4 // can be acced with this
//#define RCC_C1_APB4ENR 0x154 // or this 
#define RCC_AHB3LPENR  0x0FC // can be acced with this
//#define RCC_C1_AHB3LPENR 0x15C // or this 
#define RCC_AHB1LPENR  0x100 // can be acced with this
//#define RCC_C1_AHB1LPENR 0x160 // or this 
#define RCC_AHB2LPENR  0x104 // can be acced with this
//#define RCC_C1_AHB2LPENR 0x164 // or this 
#define RCC_AHB4LPENR  0x108 // can be acced with this
//#define RCC_C1_AHB4LPENR 0x168 // or this 
#define RCC_APB3LPENR 0x10C // can be acced with this
//#define RCC_C1_APB3LPENR 0x16C // or this 
#define RCC_APB1LLPENR 0x110 // can be acced with this
//#define RCC_C1_APB1LLPENR 0x170 // or this 
#define RCC_APB1HLPENR 0x114 // can be acced with this
//#define RCC_C1_APB1HLPENR 0x174 // or this 
#define RCC_APB2LPENR 0x118 // can be acced with this
//#define RCC_C1_APB2LPENR 0x178 // or this 
#define RCC_APB4LPENR 0x11C // can be acced with this
//#define RCC_C1_APB4LPENR 0x17C // or this 


#define RCC_CR_HSEBYP  (1 << 18)
#define RCC_CR_HSERDY  (1 << 17)
#define RCC_CR_HSEON   (1 << 16)
#define RCC_CR_HSIRDY  (1 << 2)
#define RCC_CR_HSION   (1 << 0)
#define RCC_CR_HSIKERON (1 << 1)
#define RCC_CR_PLL1RDY (1 << 25)

#define RCC_CIER_LSIRDYIE (1 << 0)
#define RCC_CIER_LSERDYIE (0 << 1)

#define RCC_CIFR_LSIRDYF (1 << 0)
#define RCC_CIFR_LSERDYF (0 << 1)

#define RCC_CICR_LSERDYC (1 << 1)
#define RCC_CICR_PLL1RDYC (1 << 6)

#define RCC_BDCR_LSEBYP (1 << 2)
#define RCC_BDCR_LSERDY (1 << 1)
#define RCC_BDCR_LSEON  (1 << 0)

#define RCC_CFGR_SWS ( 0 << 3 | 0 << 4 | 0 << 5)
#define RCC_CFGR_SW ( 0 << 0 | 1 << 1 | 1 << 2)


#define TYPE_STM32H735_RCC "stm32h735-rcc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32H735RccState, STM32H735_RCC)

struct STM32H735RccState 
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t rcc_cr;
    uint32_t rcc_hsicfgr;
    uint32_t rcc_crrcr;
    uint32_t rcc_csicfgr;
    uint32_t rcc_cfgr;
    uint32_t rcc_d1cfgr;
    uint32_t rcc_d2cfgr;
    uint32_t rcc_d3cfgr;
    uint32_t rcc_pllckselr;
    uint32_t rcc_pllcfgr;
    uint32_t rcc_pll1divr;
    uint32_t rcc_pll1fracr;
    uint32_t rcc_pll2divr;
    uint32_t rcc_pll2fracr;
    uint32_t rcc_pll3divr;
    uint32_t rcc_pll3fracr;
    uint32_t rcc_d2ccip1r;
    uint32_t rcc_d2ccip2r;
    uint32_t rcc_d3ccipr;
    uint32_t rcc_cier;
    uint32_t rcc_cifr;
    uint32_t rcc_cicr;
    uint32_t rcc_bdcr;
    uint32_t rcc_csr;
    uint32_t rcc_ahb3rstr;
    uint32_t rcc_ahb1rstr;
    uint32_t rcc_ahb2rstr;
    uint32_t rcc_ahb4rstr;
    uint32_t rcc_apb3rstr;
    uint32_t rcc_apb1lrstr;
    uint32_t rcc_apb1hrstr;
    uint32_t rcc_apb2rstr;
    uint32_t rcc_apb4rstr;
    uint32_t rcc_gcr;
    uint32_t rcc_d3amr;
    uint32_t rcc_rsr;
    uint32_t rcc_ahb3enr;
    uint32_t rcc_ahb1enr;
    uint32_t rcc_ahb2enr;
    uint32_t rcc_ahb4enr;
    uint32_t rcc_apb3enr;
    uint32_t rcc_apb1lenr;
    uint32_t rcc_apb1henr;
    uint32_t rcc_apb2enr;
    uint32_t rcc_apb4enr;
    uint32_t rcc_ahb3lpenr;
    uint32_t rcc_ahb1lpenr;
    uint32_t rcc_ahb2lpenr;
    uint32_t rcc_ahb4lpenr;
    uint32_t rcc_apb3lpenr;
    uint32_t rcc_apb1llpenr;
    uint32_t rcc_apb1hlpenr;
    uint32_t rcc_apb2lpenr;
    uint32_t rcc_apb4lpenr;

    CharBackend chr;
    qemu_irq irq;
};
#endif/* HW_STM32H735_RCC_H */