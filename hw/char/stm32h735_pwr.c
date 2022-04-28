#include "qemu/osdep.h"
#include "hw/char/stm32h735_pwr.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"


static void stm32h735_pwr_reset(DeviceState *dev)
{
    STM32H735PwrState *s = STM32H735_PWR(dev);

    s->pwr_cr1 = 0xF000C000;
    s->pwr_csr1 = 0x00004000;
    s->pwr_cr2= 0x00000000;
    s->pwr_cr3 =  0x00000046;
    s->pwr_cpucr =  0x00000000;
    s->pwr_d3cr = 0x00006000; //0x00004000
    //s->pwr_wkupcr = 0x00000000;
    //s->pwr_wkupfr = 0x00000000;
    //s->pwr_wkuppr = 0x00000000;

    qemu_set_irq(s->irq, 0);
}

static uint64_t stm32h735_pwr_read(void *opaque, hwaddr addr,  unsigned int size)
{

    STM32H735PwrState *s = opaque;

    switch (addr) {
    case PWR_CR1:
        return s->pwr_cr1;
    case PWR_CSR1:
        return s->pwr_csr1;
    case PWR_CR2:
        return s->pwr_cr2;
    case PWR_CR3:
        return s->pwr_cr3;
    case PWR_CPUCR:
        return s->pwr_cpucr;
    case PWR_D3CR:
        return s->pwr_d3cr;
    case PWR_WKUPCR:
        return s->pwr_wkupcr;
    case PWR_WKUPFR:
        return s->pwr_wkupfr;
    case PWR_WKUPEPR:
        return s->pwr_wkupepr;
    default:
        return 0;
    }

    return 0;
}

static void stm32h735_pwr_write(void *opaque, hwaddr addr, uint64_t val64,  unsigned int size)
{

    STM32H735PwrState *s = opaque;
    uint64_t value = val64;

    switch (addr) {
    case PWR_CR1:
        s->pwr_cr1 = value;
        return; 
    case PWR_CSR1:
        s->pwr_csr1 = value;
        return; 
    case PWR_CR2:
        s->pwr_cr2 = value;
        return;
    case PWR_CR3:
        s->pwr_cr3 = PWR_CR3_BYPASS;
        return; 
    case PWR_CPUCR:
        s->pwr_cpucr = value;
        return; 
    case PWR_D3CR:
        s->pwr_d3cr = PWR_D3CR_VOSRDY;
        //s->pwr_d3cr = PWR_D3CR_VOS;
        return;
    /*    
    case PWR_WKUPCR:
        return s->pwr_wkupcr;
    case PWR_WKUPFR:
        return s->pwr_wkupfr;
    case PWR_WKUPEPR:
        return s->pwr_wkuppr;
    */
    default:
        return;
    }

}

static const MemoryRegionOps stm32h735_pwr_ops = {
    .read = stm32h735_pwr_read,
    .write = stm32h735_pwr_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32h735_pwr_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32H735PwrState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32h735_pwr_init(Object *obj)
{
    STM32H735PwrState *s = STM32H735_PWR(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &stm32h735_pwr_ops, s,
                          TYPE_STM32H735_PWR, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

/*
static void stm32h735_pwr_realize(DeviceState *dev, Error **errp)
{
    STM32H735PwrState *s = STM32H735_PWR(dev);

    qemu_chr_fe_set_handlers(&s->chr, stm32h735_usart_can_receive,
                             stm32h735_usart_receive, NULL, NULL,
                             s, NULL, true);
}
*/

static void stm32h735_pwr_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32h735_pwr_reset;
    device_class_set_props(dc, stm32h735_pwr_properties);
    //dc->realize = stm32h735_pwr_realize;
}

static const TypeInfo stm32h735_pwr_info = {
    .name          = TYPE_STM32H735_PWR,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32H735PwrState),
    .instance_init = stm32h735_pwr_init,
    .class_init    = stm32h735_pwr_class_init,
};

static void stm32h735_pwr_register_types(void)
{
    type_register_static(&stm32h735_pwr_info);
}

type_init(stm32h735_pwr_register_types)
