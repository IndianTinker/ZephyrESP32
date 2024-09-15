#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/ring_buffer.h>

#define LED_PIN 25
#define UART_NODE DT_NODELABEL(uart0)
#define RING_BUF_SIZE 64

static const struct device *uart_dev = DEVICE_DT_GET(UART_NODE);
static const struct device *gpio_dev = DEVICE_DT_GET(DT_NODELABEL(gpio0));

// Ring buffer for storing received data
RING_BUF_DECLARE(uart_ringbuf, RING_BUF_SIZE);

// Function to process received data
static void process_uart_data(uint8_t data)
{
    if (data == '1') {
        gpio_pin_set(gpio_dev, LED_PIN, 1);
        printk("LED ON\n");
    } else if (data == '0') {
        gpio_pin_set(gpio_dev, LED_PIN, 0);
        printk("LED OFF\n");
    }
}

// UART interrupt callback
static void uart_cb(const struct device *dev, void *user_data)
{
    uint8_t received_byte;
    if (!uart_irq_update(dev)) {
        return;
    }

    while (uart_irq_rx_ready(dev)) {
        uart_fifo_read(dev, &received_byte, 1);
        ring_buf_put(&uart_ringbuf, &received_byte, 1);
    }
}

void main(void)
{
    uint8_t data;
    
    if (!device_is_ready(uart_dev) || !device_is_ready(gpio_dev)) {
        printk("Devices not ready\n");
        return;
    }

    // Configure LED
    gpio_pin_configure(gpio_dev, LED_PIN, GPIO_OUTPUT_INACTIVE);

    // Configure UART
    uart_irq_callback_user_data_set(uart_dev, uart_cb, NULL);
    uart_irq_rx_enable(uart_dev);

    printk("LED Control: Send '1' to turn ON, '0' to turn OFF\n");

    while (1) {
        // Process any data in the ring buffer
        uint32_t data;
        while (ring_buf_get(&uart_ringbuf, &data, 1) > 0) {
            process_uart_data(data);
        }
        k_msleep(10);  // Small delay to prevent busy-waiting
    }
}