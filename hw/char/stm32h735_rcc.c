#include "qemu/osdep.h"
#include "hw/char/stm32h735_rcc.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"


static void stm32h735_rcc_reset(DeviceState *dev)
{
    STM32H735RccState *s = STM32H735_RCC(dev);

    s->rcc_cr = 0x00000001;
    s->rcc_hsicfgr = 0x40000;
    s->rcc_crrcr = 0x00000;
    s->rcc_csicfgr = 0x20000;
    s->rcc_cfgr =  0x00000000;
    s->rcc_d1cfgr = 0x00000000;
    s->rcc_d2cfgr = 0x00000000;
    s->rcc_d3cfgr = 0x00000000;
    s->rcc_pllckselr = 0x02020200;
    s->rcc_pllcfgr = 0x01FF0000;
    s->rcc_pll1divr = 0x01010280;
    s->rcc_pll1fracr = 0x00000000;
    s->rcc_pll2divr = 0x01010280;
    s->rcc_pll2fracr = 0x00000000;
    s->rcc_pll3divr = 0x01010280;
    s->rcc_pll3fracr = 0x00000000;
    s->rcc_d2ccip1r = 0x00000000;
    s->rcc_d2ccip2r = 0x00000000;
    s->rcc_d3ccipr = 0x00000000;
    s->rcc_cier = 0x00000000;
    s->rcc_cifr = 0x00000000;
    s->rcc_cicr = 0x00000000;
    s->rcc_bdcr = 0x00000000;
    s->rcc_csr = 0x00000000;
    s->rcc_ahb3rstr = 0x00000000;
    s->rcc_ahb1rstr = 0x00000000;
    s->rcc_ahb2rstr = 0x00000000;
    s->rcc_ahb4rstr = 0x00000000;
    s->rcc_apb3rstr = 0x00000000;
    s->rcc_apb1lrstr = 0x00000000;
    s->rcc_apb1hrstr = 0x00000000;
    s->rcc_apb2rstr = 0x00000000;
    s->rcc_apb4rstr = 0x00000000;
    s->rcc_gcr = 0x00000000;
    s->rcc_d3amr = 0x00000000;
    s->rcc_rsr = 0x00FA0000;
    s->rcc_ahb3enr = 0x00000000;
    s->rcc_ahb1enr = 0x00000000;
    s->rcc_ahb2enr = 0x00000000;
    s->rcc_ahb4enr = 0x00000000;
    s->rcc_apb3enr = 0x00000000;
    s->rcc_apb1lenr = 0x00000000;
    s->rcc_apb1henr = 0x00000000 ;
    s->rcc_apb2enr = 0x00000000;
    s->rcc_apb4enr = 0x00010000;
    s->rcc_ahb3lpenr = 0xF0E95111;
    s->rcc_ahb1lpenr = 0x06038023;
    s->rcc_ahb2lpenr = 0x60030271;
    s->rcc_ahb4lpenr = 0x312806FF;
    s->rcc_apb3lpenr = 0x00000048;
    s->rcc_apb1llpenr = 0xEAFFC3FF;
    s->rcc_apb1hlpenr = 0x03000136;
    s->rcc_apb2lpenr = 0x405730F3;
    s->rcc_apb4lpenr = 0x0421DEAA;


    qemu_set_irq(s->irq, 0);
}

static uint64_t stm32h735_rcc_read(void *opaque, hwaddr addr,  unsigned int size)
{

    STM32H735RccState *s = opaque;

    printf("RCC-Read 0x%"HWADDR_PRIx"\n", addr);

    switch (addr) {
    case RCC_CR:
        return s->rcc_cr;
    case RCC_HSICFGR:
        return s->rcc_hsicfgr;
    case RCC_CIER:
        return s->rcc_cier;
    case RCC_CIFR:
        return s->rcc_cifr;
    case RCC_CICR:
        return s->rcc_cicr;
    case RCC_BDCR:
        return s->rcc_bdcr;
    case RCC_CFGR:
        return s->rcc_cfgr;
    case RCC_D1CFGR:
        return s->rcc_d1cfgr;
    case RCC_PLLCKSELR:
        return s->rcc_pllckselr;
    default:
        return 0;
    }

    return 0;
}

static void stm32h735_rcc_write(void *opaque, hwaddr addr, uint64_t val64,  unsigned int size)
{
    
    STM32H735RccState *s = opaque;
    uint64_t value = val64;

    printf("RCC-Write 0x%" PRIx64 ", 0x%"HWADDR_PRIx"\n", value, addr);

    s->rcc_cfgr |= RCC_CFGR_SWS;

    switch (addr) {
    case RCC_CR:
        s->rcc_cr = RCC_CR_HSERDY | RCC_CR_PLL1RDY | RCC_CR_HSIRDY | RCC_CR_HSION | RCC_CR_HSEON | RCC_CR_HSIDIVF | RCC_CR_HSIKERON;
        return;
    case RCC_HSICFGR:
        s->rcc_hsicfgr = value;
        return;
    case RCC_CIER:
        s->rcc_cier = RCC_CIER_LSERDYIE;
        return;
    case RCC_CIFR:
        s->rcc_cifr = RCC_CIFR_LSERDYF;
        return;
    case RCC_CICR:
        s->rcc_cicr = RCC_CICR_PLL1RDYC;
        return;
    case RCC_BDCR:
        s->rcc_bdcr = RCC_BDCR_LSERDY;
        return;
    case RCC_CFGR:
        s->rcc_cfgr |= RCC_CFGR_SW | RCC_CFGR_MCO1;
        return;
    case RCC_D1CFGR:
        s->rcc_d1cfgr = RCC_D1CFGR_D1PPRE | RCC_D1CFGR_D1CPRE | RCC_D1CFGR_HPRE;
        return;
    case RCC_PLLCKSELR:
        s->rcc_pllckselr = RCC_PLLCKSELR_PLLSRC;
    default:
        return;
    }

}

static const MemoryRegionOps stm32h735_rcc_ops = {
    .read = stm32h735_rcc_read,
    .write = stm32h735_rcc_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32h735_rcc_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32H735RccState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32h735_rcc_init(Object *obj)
{
    STM32H735RccState *s = STM32H735_RCC(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &stm32h735_rcc_ops, s,
                          TYPE_STM32H735_RCC, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32h735_rcc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32h735_rcc_reset;
    device_class_set_props(dc, stm32h735_rcc_properties);
    //dc->realize = stm32h735_pwr_realize;
}

static const TypeInfo stm32h735_rcc_info = {
    .name          = TYPE_STM32H735_RCC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32H735RccState),
    .instance_init = stm32h735_rcc_init,
    .class_init    = stm32h735_rcc_class_init,
};

static void stm32h735_rcc_register_types(void)
{
    type_register_static(&stm32h735_rcc_info);
}

type_init(stm32h735_rcc_register_types)
