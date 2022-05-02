#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "hw/arm/stm32h735_soc.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "hw/misc/unimp.h"
#include "sysemu/sysemu.h"
#include "hw/boards.h"
#include "qemu/error-report.h"
#include "hw/sysbus.h"
#include "glib.h"


//Address for: USART1, USART2, USART3
static const uint32_t usart_addr[STM_NUM_USARTS] = { 0x40011000, 0x40004400, 0x40004800};

//position of USART
static const int usart_irq[STM_NUM_USARTS] = {37, 38, 39};

//pwr adress
static const uint32_t pwr_addr = 0x58024800;

//rcc adress
static const uint32_t rcc_addr = 0x58024400;

//flash interface adress
static const uint32_t flash_addr = 0x52002000;


static void stm32h735_soc_initfn(Object *obj)
{
    STM32H735State *s = STM32H735_SOC(obj);
    int i;

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    for (i = 0; i < STM_NUM_USARTS; i++) {
        object_initialize_child(obj, "usart[*]", &s->usart[i],
                                TYPE_STM32H735_USART);
    }

    object_initialize_child(obj, "pwr", &s->pwr, TYPE_STM32H735_PWR);
    object_initialize_child(obj, "rcc", &s->rcc, TYPE_STM32H735_RCC);

    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
    s->refclk = qdev_init_clock_in(DEVICE(s), "refclk", NULL, NULL, 0);
}

static void stm32h735_soc_realize(DeviceState *dev_soc, Error **errp)
{
    STM32H735State *s = STM32H735_SOC(dev_soc);
    DeviceState *dev, *armv7m;
    SysBusDevice *busdev;
    int i;

    MemoryRegion *system_memory = get_system_memory();


    /*
     * We use s->refclk internally and only define it with qdev_init_clock_in()
     * so it is correctly parented and not leaked on an init/deinit; it is not
     * intended as an externally exposed clock.
     */
    if (clock_has_source(s->refclk)) {
        error_setg(errp, "refclk clock must not be wired up by the board code");
        return;
    }

    if (!clock_has_source(s->sysclk)) {
        error_setg(errp, "sysclk clock must be wired up by the board code");
        return;
    }

    /* The refclk always runs at frequency HCLK / 8 */
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);

    /*
     * Init flash region
     * Flash starts at 0x08000000 and then is aliased to boot memory at 0x0
     */
    memory_region_init_rom(&s->flash, OBJECT(dev_soc), "STM32H735.flash",
                           FLASH_SIZE, &error_fatal);
    memory_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "STM32H735.flash.alias", &s->flash, 0, FLASH_SIZE);
    memory_region_add_subregion(system_memory, FLASH_BASE_ADDRESS, &s->flash);
    memory_region_add_subregion(system_memory, 0, &s->flash_alias);

    // sram init
    memory_region_init_ram(&s->ram_exec, NULL, "STM32H735.ram_exec", RAM_EXEC_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, RAM_EXEC_ADRESS, &s->ram_exec);

    memory_region_init_ram(&s->dtcmram, NULL, "STM32H735.dtcmram", DTCMRAM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, DTCMRAM_ADRESS, &s->dtcmram);

    #if USE_ITCMRAM 
    memory_region_init_ram(&s->itcmram, NULL, "STM32H735.itcmram", ITCMRAM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, ITCMRAM_ADRESS, &s->itcmram);
    #endif

    memory_region_init_ram(&s->ram_d2, NULL, "STM32H735.ram_d2", RAM_D2_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, RAM_D2_ADRESS, &s->ram_d2);

    memory_region_init_ram(&s->ram_d3, NULL, "STM32H735.ram_d3", RAM_D3_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, RAM_D3_ADRESS, &s->ram_d3);

    /* Init ARMv7m */
    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq", 61);
    qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->refclk);
    object_property_set_link(OBJECT(&s->armv7m), "memory",
                             OBJECT(get_system_memory()), &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp)) {
        return;
    }

    /* Attach UART (uses USART registers) and USART controllers */
    for (i = 0; i < STM_NUM_USARTS; i++) {
        dev = DEVICE(&(s->usart[i]));
        qdev_prop_set_chr(dev, "chardev", serial_hd(i));
        if (!sysbus_realize(SYS_BUS_DEVICE(&s->usart[i]), errp)) {
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, usart_addr[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, usart_irq[i]));
    }

    //pwr init
    dev = DEVICE(&(s->pwr));
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->pwr), errp)) 
    {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, pwr_addr);

    //rcc init
    dev = DEVICE(&(s->rcc));
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->rcc), errp)) 
    {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, rcc_addr);

    //flash interface init
    dev = DEVICE(&(s->flash_reg));
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->flash_reg), errp)) 
    {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, flash_addr);

    //First page :)
    create_unimplemented_device("HSEM", 0x58026400, 0x400);
    create_unimplemented_device("ADC3", 0x58026000, 0x400);
    //create_unimplemented_device("DMAMUX2", 0x58025800, 0x400);
    //create_unimplemented_device("BDMA", 0x58025400, 0x400);
    create_unimplemented_device("CRC", 0x58024C00, 0x400);
    //create_unimplemented_device("PWR", 0x58024800, 0x400);
    //create_unimplemented_device("RCC", 0x58024400, 0x400);
    create_unimplemented_device("GPIOK", 0x58022800, 0x400);
    create_unimplemented_device("GPIOJ", 0x58022400, 0x400);
    create_unimplemented_device("GPIOH", 0x58021C00, 0x400);
    create_unimplemented_device("GPIOG", 0x58021800, 0x400);
    create_unimplemented_device("GPIOF", 0x58021400, 0x400);
    create_unimplemented_device("GPIOE", 0x58021000, 0x400);
    create_unimplemented_device("GPIOD", 0x58020C00, 0x400);
    create_unimplemented_device("GPIOC", 0x58020800, 0x400);
    create_unimplemented_device("GPIOB", 0x58020400, 0x400);
    create_unimplemented_device("GPIOA", 0x58020000, 0x400);

    //Secound page :)
    //create_unimplemented_device("DTS", 0x58006800, 0x400);
    //create_unimplemented_device("SAI4", 0x58005400, 0x400);
    create_unimplemented_device("IWDG", 0x58004800, 0x400);
    create_unimplemented_device("RTC & BKP registers", 0x58004000, 0x400);
    //create_unimplemented_device("COMP1 - COMP2", 0x58003800, 0x400);
    //create_unimplemented_device("LPTIM5", 0x58003000, 0x400);
    //create_unimplemented_device("LPTIM4", 0x58002C00, 0x400);
    //create_unimplemented_device("LPTIM3", 0x58002800, 0x400);
    //create_unimplemented_device("LPTIM2", 0x58002400, 0x400);
    //create_unimplemented_device("I2C4", 0x58001C00, 0x400);
    //create_unimplemented_device("SPI/I2S6", 0x58001400, 0x400);
    //create_unimplemented_device("LPUART1", 0x58000C00, 0x400);
    //create_unimplemented_device("SYSCFG", 0x58000400, 0x400);
    create_unimplemented_device("EXTI", 0x58000000, 0x400);

    //Third page
    //create_unimplemented_device("OTFDEC2", 0x5200BC00, 0x400);
    //create_unimplemented_device("OTFDEC1", 0x5200B800, 0x400);
    //create_unimplemented_device("OCTOSPI I/O", 0x5200B400, 0x400);
    //create_unimplemented_device("Delay Block OCTOSPI2", 0x5200B000, 0x400);
    //create_unimplemented_device("OCTOSPI2", 0x5200A000, 0x400);
    //create_unimplemented_device("Delay Block SDMMC1", 0x52008000, 0x400);
    //create_unimplemented_device("SDMMC1", 0x52009000, 0x400);
    //create_unimplemented_device("Delay Block OCTOSPI1 ", 0x52006000, 0x400);
    //create_unimplemented_device("OCTOSPI1 control registers", 0x52005000, 0x400);
    create_unimplemented_device("FMC control registers", 0x52004000, 0x400);
    //create_unimplemented_device("Flash interface registers", 0x52002000, 0x400);
    //create_unimplemented_device("Chrom-Art (DMA2D)", 0x52001000, 0x400);
    //create_unimplemented_device("MDMA", 0x52000000, 0x400);
    //create_unimplemented_device("GPV", 0x51000000, 0x400);
    create_unimplemented_device("WWDG", 0x50003000, 0x400);
    //create_unimplemented_device("LTDC", 0x50001000, 0x400);
    //create_unimplemented_device("CORDIC", 0x48024400, 0x400);
    //create_unimplemented_device("FMAC", 0x48024000, 0x400);
    //create_unimplemented_device("RAMECC D2 domain", 0x48023000, 0x400);
    //create_unimplemented_device("Delay Block SDMMC2", 0x48022400, 0x400);
    //create_unimplemented_device("RNG", 0x48021800, 0x400);
    //create_unimplemented_device("HASH", 0x48021400, 0x400);
    //create_unimplemented_device("CRYPTO", 0x48021000, 0x400);
    //create_unimplemented_device("PSSI", 0x48020400, 0x400);
    //create_unimplemented_device("DCMI", 0x48020000, 0x400);

    //Fourth page
    //create_unimplemented_device("USB1 OTG_HS", 0x40040000, 0x400);
    //create_unimplemented_device("ETHERNET MAC", 0x40028000, 0x400);
    create_unimplemented_device("ADC1 - ADC2", 0x40022000, 0x400);
    //create_unimplemented_device("DMAMUX1", 0x40020800, 0x400);
    //create_unimplemented_device("DMA2", 0x40020400, 0x400);
    create_unimplemented_device("DMA1", 0x40020000, 0x400);
    //create_unimplemented_device("DFSDM1", 0x40017800, 0x400);
    //create_unimplemented_device("SAI1", 0x40015800, 0x400);
    //create_unimplemented_device("SPI5", 0x40015000, 0x400);
    create_unimplemented_device("TIM17", 0x40014800, 0x400);
    create_unimplemented_device("TIM16", 0x40014400, 0x400);
    create_unimplemented_device("TIM15", 0x40014000, 0x400);
    //create_unimplemented_device("SPI4", 0x40013400, 0x400);
    create_unimplemented_device("SPI1 / I2S1", 0x40013000, 0x400);
    //create_unimplemented_device("USART10", 0x40011C00, 0x400);
    //create_unimplemented_device("UART9", 0x40011800, 0x400);
    //create_unimplemented_device("USART6", 0x40011400, 0x400);
    //create_unimplemented_device("USART1", 0x40011000, 0x400);
    create_unimplemented_device("TIM8", 0x40010400, 0x400);
    create_unimplemented_device("TIM1", 0x40010000, 0x400);

    //Fifth page
    //create_unimplemented_device("TIM24", 0x4000E400, 0x400);
    //create_unimplemented_device("TIM23", 0x4000E000, 0x400);
    //create_unimplemented_device("FDCAN3", 0x4000D400, 0x400);
    //create_unimplemented_device("CAN Message RAM", 0x4000AC00, 0x400);
    //create_unimplemented_device("CAN CCU", 0x4000A800, 0x400);
    //create_unimplemented_device("FDCAN2", 0x4000A400, 0x400);
    //create_unimplemented_device("FDCAN1", 0x4000A000, 0x400);
    //create_unimplemented_device("MDIOS", 0x40009400, 0x400);
    //create_unimplemented_device("OPAMP", 0x40009000, 0x400);
    //create_unimplemented_device("SWPMI", 0x40008800, 0x400);
    //create_unimplemented_device("CRS", 0x40008400, 0x400);
    //create_unimplemented_device("UART8", 0x40007C00, 0x400);
    //create_unimplemented_device("UART7", 0x40007800, 0x400);
    create_unimplemented_device("DAC1/2", 0x40007400, 0x400);
    create_unimplemented_device("HDMI-CEC", 0x40006C00, 0x400);
    //create_unimplemented_device("I2C5", 0x40006400, 0x400);
    //create_unimplemented_device("I2C3", 0x40005C00, 0x400);
    create_unimplemented_device("I2C2", 0x40005800, 0x400);
    create_unimplemented_device("I2C1", 0x40005400, 0x400);
    //create_unimplemented_device("UART5", 0x40005000, 0x400);
    //create_unimplemented_device("UART4", 0x40004C00, 0x400);
    //create_unimplemented_device("USART3", 0x40004800, 0x400);
    //create_unimplemented_device("USART2", 0x40004400, 0x400);
    //create_unimplemented_device("SPDIFRX1", 0x40004000, 0x400);
    //create_unimplemented_device("SPI3 / I2S3 ", 0x40003C00, 0x400);
    create_unimplemented_device("SPI2 / I2S2", 0x40003800, 0x400);
    //create_unimplemented_device("LPTIM1", 0x40002400, 0x400);
    create_unimplemented_device("TIM14", 0x40002000, 0x400);
    create_unimplemented_device("TIM13", 0x40001C00, 0x400);
    create_unimplemented_device("TIM12", 0x40001800, 0x400);

    //Sixth page
    create_unimplemented_device("TIM7", 0x40001400, 0x400);
    create_unimplemented_device("TIM6", 0x40001000, 0x400);
    create_unimplemented_device("TIM5", 0x40000C00, 0x400);
    create_unimplemented_device("TIM4", 0x40000800, 0x400);
    create_unimplemented_device("TIM3", 0x40000400, 0x400);
    create_unimplemented_device("TIM2", 0x40000000, 0x400);
    

    //create_unimplemented_device("DUMMY", 0xFFFFF000, 0xFFF);
    
}

static Property stm32h735_soc_properties[] = {
    DEFINE_PROP_STRING("cpu-type", STM32H735State, cpu_type),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32h735_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = stm32h735_soc_realize;
    device_class_set_props(dc, stm32h735_soc_properties);
}

static const TypeInfo stm32h735_soc_info = {
    .name          = TYPE_STM32H735_SOC,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32H735State),
    .instance_init = stm32h735_soc_initfn,
    .class_init    = stm32h735_soc_class_init,
};

static void stm32h735_soc_types(void)
{
    type_register_static(&stm32h735_soc_info);
}

type_init(stm32h735_soc_types)