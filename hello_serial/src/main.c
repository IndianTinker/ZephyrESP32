#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/sys/printk.h>

void main(void)
{
    int count = 1;

    while (true) {
        printk("%d\n", count);
        count++;
        k_msleep(1000);
    }
}