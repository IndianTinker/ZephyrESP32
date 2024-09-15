#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define SWITCH_PIN 23
#define LED_PIN 25
#define DEBOUNCE_DELAY_MS 50  // 50ms debounce delay

static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));
static struct gpio_callback switch_cb_data;

static bool led_state = false;  // Track LED state
static int64_t last_time = 0;   // For debounce

void switch_pressed_callback(const struct device *dev, struct gpio_callback *cb, uint32_t pins)
{
    int64_t current_time = k_uptime_get();
    
    // Simple debounce: ignore interrupts that occur too soon after the previous one
    if ((current_time - last_time) < DEBOUNCE_DELAY_MS) {
        return;
    }
    
    last_time = current_time;

    // Toggle LED state
    led_state = !led_state;
    gpio_pin_set(gpio_dev, LED_PIN, led_state);
    
    printk("Switch pressed. LED is now %s\n", led_state ? "ON" : "OFF");
}

void main(void)
{
    int ret;

    if (!device_is_ready(gpio_dev)) {
        printk("Error: GPIO device is not ready\n");
        return;
    }

    // Configure switch pin (input with pull-up)
    ret = gpio_pin_configure(gpio_dev, SWITCH_PIN, GPIO_INPUT | GPIO_PULL_UP);
    if (ret != 0) {
        printk("Error: Failed to configure switch GPIO pin %d\n", ret);
        return;
    }

    // Configure LED pin (output, initially off)
    ret = gpio_pin_configure(gpio_dev, LED_PIN, GPIO_OUTPUT_INACTIVE);
    if (ret != 0) {
        printk("Error: Failed to configure LED GPIO pin %d\n", ret);
        return;
    }

    // Configure interrupt for switch (falling edge only)
    ret = gpio_pin_interrupt_configure(gpio_dev, SWITCH_PIN, GPIO_INT_EDGE_FALLING);
    if (ret != 0) {
        printk("Error: Failed to configure interrupt %d\n", ret);
        return;
    }

    gpio_init_callback(&switch_cb_data, switch_pressed_callback, BIT(SWITCH_PIN));
    gpio_add_callback(gpio_dev, &switch_cb_data);

    printk("Switch monitoring started. Press the switch to toggle LED.\n");

    while (1) {
        k_sleep(K_FOREVER);
    }
}