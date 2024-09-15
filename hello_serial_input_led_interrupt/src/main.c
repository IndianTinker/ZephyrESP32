#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SWITCH_PIN 23
#define LED_PIN 25

static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static struct gpio_callback switch_cb_data;


void print_callback_info(const struct gpio_callback *cb)
{
    printk("Callback Information:\n");
    printk("  Handler function address: %p\n", cb->handler);
    printk("  Pin mask: 0x%08x\n", cb->pin_mask);
}

void switch_pressed_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int switch_state = gpio_pin_get(gpio_dev, SWITCH_PIN);
    printk("Switch state changed: %s\n", switch_state ? "OFF" : "ON");
    
    gpio_pin_set(gpio_dev, LED_PIN, !switch_state);
}

void main(void)
{
    int ret;

    if (!device_is_ready(gpio_dev)) {
        printk("Error: GPIO device is not ready\n");
        return;
    }

    ret = gpio_pin_configure(gpio_dev, SWITCH_PIN, GPIO_INPUT | GPIO_PULL_UP);
    if (ret != 0) {
        printk("Error: Failed to configure switch GPIO pin %d\n", ret);
        return;
    }

    ret = gpio_pin_configure(gpio_dev, LED_PIN, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        printk("Error: Failed to configure LED GPIO pin %d\n", ret);
        return;
    }

    ret = gpio_pin_interrupt_configure(gpio_dev, SWITCH_PIN, GPIO_INT_EDGE_BOTH);
    if (ret != 0) {
        printk("Error: Failed to configure interrupt %d\n", ret);
        return;
    }

    gpio_init_callback(&switch_cb_data, switch_pressed_callback, BIT(SWITCH_PIN));
    print_callback_info(&switch_cb_data);  // Print callback info

    gpio_add_callback(gpio_dev, &switch_cb_data);

    printk("Switch monitoring started. Press the switch to toggle LED.\n");

    while (1) {
        k_sleep(K_FOREVER);
    }
}