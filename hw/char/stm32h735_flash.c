#include "qemu/osdep.h"
#include "hw/char/stm32h735_flash.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"


static void stm32h735_flash_reset(DeviceState *dev)
{
    STM32H735FlashRegState *s = STM32H735_FLASH_REG(dev);

    s->flash_acr = 0x00000037;
    s->flash_keyr = 0x00000000;
    s->flash_optkeyr = 0x00000000;
    s->flash_cr = 0x00000031;
    s->flash_sr = 0x00000000;
    s->flash_ccr = 0x00000000;
    s->flash_optcr = 0x00000001;
    //s->flash_optsr_cur = ;
    //s->flash_optsr_prg =;
    s->flash_optccr = 0x00000000;
    //s->flash_prar_cur =;
    //s->flash_prar_prg =;
    //s->flash_scar_cur =;
    //s->flash_scar_prg =;
    s->flash_wpsn_cur = 0x00000000;
    s->flash_wpsn_prg = 0x00000000;
    //s->flash_boot_cur =;
    //s->flash_boot_prg =;
    s->flash_crccr =  0x001C0000;
    s->flash_crcsaddr = 0x00000000;
    s->flash_crceaddr = 0x00000000;
    s->flash_crcdatar = 0x00000000;
    s->flash_ecc_far = 0x00000000;
    //s->flash_optsr2_cur = ;
    //s->flash_optsr2_prg =;

    qemu_set_irq(s->irq, 0);
}

static uint64_t stm32h735_flash_read(void *opaque, hwaddr addr,  unsigned int size)
{

    STM32H735FlashRegState *s = opaque;

    switch (addr) {
    case FLASH_ACR:
        return s->flash_acr;
    case FLASH_KEYR:
        return s->flash_keyr;
    case FLASH_OPTKEYR:
        return s->flash_optkeyr;
    case FLASH_CR:
        return s->flash_cr;
    case FLASH_SR:
        return s->flash_sr;
    case FLASH_CCR:
        return s->flash_ccr;
    case FLASH_OPTCR:
        return s->flash_optcr;
    case FLASH_OPTCCR:
        return s->flash_optccr;
    case FLASH_WPSN_CUR:
        return s->flash_wpsn_cur;
    case FLASH_WPSN_PRG:
        return s->flash_wpsn_prg;
    case FLASH_CRCCR:
        return s->flash_crccr;
    case FLASH_CRCEADDR:
        return s->flash_crceaddr;
    case FLASH_CRCSADDR:
        return s->flash_crcsaddr;
    case FLASH_CRCDATAR:
        return s->flash_crcdatar;
    case FLASH_ECC_FAR:
        return s->flash_ecc_far;
    default:
        return 0;
    }

    return 0;
}

static void stm32h735_flash_write(void *opaque, hwaddr addr, uint64_t val64,  unsigned int size)
{
    STM32H735FlashRegState *s = opaque;
    //uint64_t value = val64;

    switch (addr) {
    case FLASH_ACR:
        s->flash_acr = FLASH_ACR_LATENCY;
        return;
    default:
        return;
    }

}

static const MemoryRegionOps stm32h735_flash_ops = {
    .read = stm32h735_flash_read,
    .write = stm32h735_flash_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32h735_flash_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32H735FlashRegState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32h735_flash_init(Object *obj)
{
    STM32H735FlashRegState *s = STM32H735_FLASH_REG(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &stm32h735_flash_ops, s,
                          TYPE_STM32H735_FLASH_REG, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32h735_flash_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32h735_flash_reset;
    device_class_set_props(dc, stm32h735_flash_properties);
    //dc->realize = stm32h735_flash_realize;
}

static const TypeInfo stm32h735_flash_info = {
    .name          = TYPE_STM32H735_FLASH_REG,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32H735FlashRegState),
    .instance_init = stm32h735_flash_init,
    .class_init    = stm32h735_flash_class_init,
};

static void stm32h735_flash_register_types(void)
{
    type_register_static(&stm32h735_flash_info);
}

type_init(stm32h735_flash_register_types)
