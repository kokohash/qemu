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

/* Main SYSCLK frequency in Hz (16MHz) */
#define SYSCLK_FRQ 16000000ULL

static void stm32h735_init(MachineState *machine)
{
    DeviceState *dev;
    Clock *sysclk;

    /* This clock doesn't need migration because it is fixed-frequency */
    sysclk = clock_new(OBJECT(machine), "SYSCLK");
    clock_set_hz(sysclk, SYSCLK_FRQ);

    dev = qdev_new(TYPE_STM32H735_SOC);
    qdev_prop_set_string(dev, "cpu-type", ARM_CPU_TYPE_NAME("cortex-m7"));
    qdev_connect_clock_in(dev, "sysclk", sysclk);
    sysbus_realize_and_unref(SYS_BUS_DEVICE(dev), &error_fatal);

    armv7m_load_kernel(ARM_CPU(first_cpu),
                       machine->kernel_filename,
                       FLASH_SIZE);
}

static void stm32h735_machine_init(MachineClass *mc)
{
    mc->desc = "ST STM32H735 (Cortex-M7)";
    mc->init = stm32h735_init;
    mc->ignore_memory_transaction_failures = true;
}

DEFINE_MACHINE("stm32h735", stm32h735_machine_init)