#include "qemu/osdep.h"
#include "hw/char/stm32h735_usart.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef STM_USART_ERR_DEBUG
#define STM_USART_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...) do { \
    if (STM_USART_ERR_DEBUG >= lvl) { \
        qemu_log("%s: " fmt, __func__, ## args); \
    } \
} while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ## args)

static int stm32h735_usart_can_receive(void *opaque)
{
    STM32H735UsartState *s = opaque;

    if (!(s->usart_isr & USART_ISR_RXNE)) {
        return 1;
    }

    return 0;
}

static void stm32h735_usart_receive(void *opaque, const uint8_t *buf, int size)
{
    STM32H735UsartState *s = opaque;

    if (!(s->usart_cr1 & USART_CR1_UE && s->usart_cr1 & USART_CR1_RE)) {
        /* USART not enabled - drop the chars */
        DB_PRINT("Dropping the chars\n");
        return;
    }

    s->usart_rdr = *buf;
    s->usart_isr |= USART_ISR_RXNE;

    if (s->usart_cr1 & USART_CR1_RXNEIE) {
        qemu_set_irq(s->irq, 1);
    }

    DB_PRINT("Receiving: %c\n", s->usart_rdr);
}

static void stm32h735_usart_reset(DeviceState *dev)
{
    STM32H735UsartState *s = STM32H735_USART(dev);

    s->usart_isr = USART_ISR_RESET;
    s->usart_rdr = 0x00000000;
    s->usart_tdr = 0x00000000;
    s->usart_brr = 0x00000000;
    s->usart_cr1 = 0x00000000;
    s->usart_cr2 = 0x00000000;
    s->usart_cr3 = 0x00000000;
    s->usart_gtpr = 0x00000000;
    s->usart_rtor = 0x00000000;
    s->usart_isr = 0x00000000;
    s->usart_icr = 0x00000000;
    s->usart_presc = 0x00000000;

    qemu_set_irq(s->irq, 0);
}

static uint64_t stm32h735_usart_read(void *opaque, hwaddr addr,
                                       unsigned int size)
{
    STM32H735UsartState *s = opaque;
    uint64_t retvalue;

    //HACK: Just to set the flags
    s->usart_isr |= USART_ISR_TC | USART_ISR_TXE;
    //printf("USART-Read 0x%"HWADDR_PRIx"\n", addr);

    switch (addr) {
    case USART_ISR:
        retvalue = s->usart_isr;
        qemu_chr_fe_accept_input(&s->chr);
        return retvalue;
    case USART_RDR:
        printf("Value: 0x%" PRIx32 ", %c\n", s->usart_rdr, (char) s->usart_rdr);
        retvalue = s->usart_rdr & 0x3FF;
        s->usart_isr &= ~USART_ISR_RXNE;
        qemu_chr_fe_accept_input(&s->chr);
        qemu_set_irq(s->irq, 0);
        return retvalue;
    case USART_BRR:
        return s->usart_brr;
    case USART_CR1:
        return s->usart_cr1;
    case USART_CR2:
        return s->usart_cr2;
    case USART_CR3:
        return s->usart_cr3;
    case USART_GTPR:
        return s->usart_gtpr;
    case USART_TDR:
        return s->usart_tdr;
    case USART_RTOR:
        return s->usart_rtor;
    case USART_RQR:
        return s->usart_rqr;
    case USART_ICR:
        return s->usart_icr;
    case USART_PRESC:
        return s->usart_presc;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, addr);
        return 0;
    }

    return 0;
}

static void stm32h735_usart_write(void *opaque, hwaddr addr,
                                  uint64_t val64, unsigned int size)
{
    STM32H735UsartState *s = opaque;
    uint32_t value = val64;
    unsigned char ch;

    //printf("USART-Write 0x%" PRIx32 ", 0x%"HWADDR_PRIx"\n", value, addr);
    
    switch (addr) {
    case USART_ISR:
        if (value <= 0x3FF) {
            /* I/O being synchronous, TXE is always set. In addition, it may
               only be set by hardware, so keep it set here. */
            s->usart_isr = value | USART_ISR_TXE /*| USART_ISR_TEACK | USART_ISR_RTOF*/;
        } else {
            s->usart_isr &= value;
        }
        if (!(s->usart_isr & USART_ISR_RXNE)) {
            qemu_set_irq(s->irq, 0);
        }
        return;
    case USART_TDR:
        if (value < 0xF000) {
            ch = value;
            /* XXX this blocks entire thread. Rewrite to use
             * qemu_chr_fe_write and background I/O callbacks */
            qemu_chr_fe_write_all(&s->chr, &ch, 1);

            //HACK: Cant get char to write to stdout.
            putchar(ch);
            /* XXX I/O are currently synchronous, making it impossible for
               software to observe transient states where TXE or TC aren't
               set. Unlike TXE however, which is read-only, software may
               clear TC by writing 0 to the SR register, so set it again
               on each write. */
            s->usart_isr |= USART_ISR_TC | USART_ISR_TXE;
        }
        return;
    case USART_BRR:
        s->usart_brr = value;
        return;
    case USART_CR1:
        s->usart_cr1 = value & (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);
            if (s->usart_cr1 & USART_CR1_RXNEIE &&
                s->usart_isr & USART_ISR_RXNE) {
                qemu_set_irq(s->irq, 1);

            }      
        return;
    case USART_CR2:
        s->usart_cr2 = value;
        return;
    case USART_CR3:
        s->usart_cr3 = value;
        return;
    case USART_GTPR:
        s->usart_gtpr = value;
        return;
    case USART_RDR:
        s->usart_rdr = value;
        return;
    case USART_RTOR:
        s->usart_rtor = value;
        return;
    case USART_RQR:
        s->usart_rqr = value;
        return;
    case USART_ICR:
        s->usart_icr = value;
        return;
    case USART_PRESC:
        s->usart_presc = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%"HWADDR_PRIx"\n", __func__, addr);
    }
}

static const MemoryRegionOps stm32h735_usart_ops = {
    .read = stm32h735_usart_read,
    .write = stm32h735_usart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32h735_usart_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32H735UsartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32h735_usart_init(Object *obj)
{
    STM32H735UsartState *s = STM32H735_USART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &stm32h735_usart_ops, s,
                          TYPE_STM32H735_USART, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32h735_usart_realize(DeviceState *dev, Error **errp)
{
    STM32H735UsartState *s = STM32H735_USART(dev);

    qemu_chr_fe_set_handlers(&s->chr, stm32h735_usart_can_receive,
                             stm32h735_usart_receive, NULL, NULL,
                             s, NULL, true);
}

static void stm32h735_usart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32h735_usart_reset;
    device_class_set_props(dc, stm32h735_usart_properties);
    dc->realize = stm32h735_usart_realize;
}

static const TypeInfo stm32h735_usart_info = {
    .name          = TYPE_STM32H735_USART,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32H735UsartState),
    .instance_init = stm32h735_usart_init,
    .class_init    = stm32h735_usart_class_init,
};

static void stm32h735_usart_register_types(void)
{
    type_register_static(&stm32h735_usart_info);
}

type_init(stm32h735_usart_register_types)
