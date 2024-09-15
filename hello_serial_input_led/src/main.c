#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SWITCH_PIN 23
#define SLEEP_TIME_MS 100

static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

void main(void)
{
    int ret;

    if (!device_is_ready(gpio_dev))
    {
        printk("Error: GPIO device is not ready\n");
        return;
    }

    ret = gpio_pin_configure(gpio_dev, SWITCH_PIN, GPIO_INPUT | GPIO_PULL_UP);
    if (ret != 0)
    {
        printk("Error: Failed to configure switch GPIO pin %d\n", ret);
        return;
    }

    ret = gpio_pin_configure(gpio_dev, 25, GPIO_OUTPUT_ACTIVE);
    if (ret != 0)
    {
        printk("Error: Failed to configure led GPIO pin %d\n", ret);
        return;
    }

    printk("Switch monitoring started. Press the switch to see changes.\n");

    while (1)
    {
        int switch_state = gpio_pin_get(gpio_dev, SWITCH_PIN);
        printk("Switch state: %s\n", switch_state ? "OFF" : "ON");
        ret = gpio_pin_set_raw(gpio_dev, 25, !switch_state);
        if (ret != 0)
        {
            return;
        }
        k_msleep(SLEEP_TIME_MS);
    }
}