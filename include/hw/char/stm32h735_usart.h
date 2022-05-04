#ifndef HW_STM32H735_USART_H
#define HW_STM32H735_USART_H

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

#define USART_CR1   0x00
#define USART_CR2   0x04
#define USART_CR3   0x08
#define USART_BRR   0x0C
#define USART_GTPR  0x10
#define USART_RTOR  0x14
#define USART_RQR   0x18
#define USART_ISR   0x1C
#define USART_ICR   0x20
#define USART_RDR   0x24
#define USART_TDR   0x28
#define USART_PRESC 0x2C

#define USART_ISR_RESET (USART_ISR_TXE | USART_ISR_TC)

#define USART_ISR_TXE  (1 << 7)
#define USART_ISR_TC   (1 << 6)
#define USART_ISR_RXNE (1 << 5)
#define USART_ISR_TEACK (1 << 21)
#define USART_ISR_RTOF (1 << 11)

#define USART_CR1_UE  (1 << 0)
#define USART_CR1_RXNEIE  (1 << 5)
#define USART_CR1_TE  (0 << 3)
#define USART_CR1_RE  (0 << 2)

#define USART_CR2_RTOEN (0 << 23)

#define TYPE_STM32H735_USART "stm32h735-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32H735UsartState, STM32H735_USART)

struct STM32H735UsartState 
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t usart_cr1;
    uint32_t usart_cr2;
    uint32_t usart_cr3;
    uint32_t usart_brr;
    uint32_t usart_gtpr;
    uint32_t usart_rtor;
    uint32_t usart_rqr;
    uint32_t usart_isr;
    uint32_t usart_icr;
    uint32_t usart_rdr;
    uint32_t usart_tdr;
    uint32_t usart_presc;

    CharBackend chr;
    qemu_irq irq;
};
#endif/* HW_STM32H735_USART_H */