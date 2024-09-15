#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/printk.h>

#define LED_PIN 25
#define UART_NODE DT_NODELABEL(uart0)

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);
static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

void main(void)
{
    int ret;
    unsigned char rx_buf[1];

    if (!device_is_ready(uart_dev)) {
        printk("UART device not ready\n");
        return;
    }

    if (!device_is_ready(gpio_dev)) {
        printk("GPIO device not ready\n");
        return;
    }

    ret = gpio_pin_configure(gpio_dev, LED_PIN, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        printk("Error configuring GPIO pin\n");
        return;
    }

    printk("LED Control: Send '1' to turn ON, '0' to turn OFF\n");

    while (1) {
        if (uart_poll_in(uart_dev, rx_buf) == 0) {
            if (rx_buf[0] == '1') {
                gpio_pin_set(gpio_dev, LED_PIN, 1);
                printk("LED ON\n");
            } else if (rx_buf[0] == '0') {
                gpio_pin_set(gpio_dev, LED_PIN, 0);
                printk("LED OFF\n");
            }
        }
        k_msleep(10);  // Small delay to prevent busy-waiting
    }
}