#include <basic.h>
#include <ng/fs.h>
#include <ng/irq.h>
#include <ng/panic.h>
#include <ng/thread.h>
#include <ng/tty.h>
#include <ng/x86/cpu.h>
#include <ng/x86/pic.h>
#include <ng/x86/portio.h>
#include <ng/x86/uart.h>
#include <stdio.h>

#define UART_DATA 0
#define UART_INTERRUPT_ENABLE 1
#define UART_BAUD_LOW 0
#define UART_BAUD_HIGH 1
#define UART_FIFO_CTRL 2
#define UART_LINE_CTRL 3
#define UART_MODEM_CTRL 4
#define UART_LINE_STATUS 5
#define UART_MODEM_STATUS 6

static bool is_transmit_empty(port com) {
    return (inb(com + UART_LINE_STATUS) & 0x20) != 0;
}

static bool is_data_available(port com) {
    return (inb(com + UART_LINE_STATUS) & 0x01) != 0;
}

static void wait_for_transmit_empty(port com) {
    while (!is_transmit_empty(com))
        ;
}

static void wait_for_data_available(port com) {
    while (!is_data_available(com))
        ;
}

void x86_uart_write_byte(port p, const char b) {
    wait_for_transmit_empty(p);
    outb(p + UART_DATA, b);
}

void x86_uart_write(port p, const char *buf, size_t len) {
    for (size_t i = 0; i < len; i++) x86_uart_write_byte(p, buf[i]);
}

char x86_uart_read_byte(port p) {
    wait_for_data_available(p);
    return inb(p + UART_DATA);
}

void x86_uart_enable_interrupt(port com) {
    // For now, I only support interrupt on data available
    outb(com + UART_INTERRUPT_ENABLE, 0x9);
}

void x86_uart_disable_interrupt(port com) {
    outb(com + UART_INTERRUPT_ENABLE, 0x0);
}

void x86_uart_irq_handler(interrupt_frame *r, void *serial_port) {
    port_addr_t port = (port_addr_t)(intptr_t)serial_port;
    char f = x86_uart_read_byte(port);

    switch (port) {
        case 0x3f8:
            write_to_serial_tty(&dev_serial, f);
            break;
        case 0x2f8:
            write_to_serial_tty(&dev_serial2, f);
            break;
            // case COM3:
            //         write_to_serial_tty(&serial_tty3, f);
            //         break;
            // case COM4:
            //         write_to_serial_tty(&serial_tty4, f);
            //         break;
    }
}

static void x86_uart_setup(port_addr_t p) {
    // TODO: cleanup with registers above
    outb(p + 1, 0x00);
    outb(p + 3, 0x80);
    outb(p + 0, 0x03);
    outb(p + 1, 0x00);
    outb(p + 3, 0x03);
    outb(p + 2, 0xC7);
    outb(p + 4, 0x0B);
    x86_uart_enable_interrupt(p);
}

void x86_uart_init() {
    x86_uart_setup(0x3f8);
    x86_uart_setup(0x2f8);

    irq_install(4, x86_uart_irq_handler, (void *)0x3f8);
    irq_install(3, x86_uart_irq_handler, (void *)0x2F8);

    // irq_install(4, x86_uart_irq_handler, (void *)0x3E8);
    // irq_install(3, x86_uart_irq_handler, (void *)0x2E8);
}
