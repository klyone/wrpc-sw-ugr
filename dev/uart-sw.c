/*
 * This work is part of the White Rabbit project
 *
 * Copyright (C) 2013 CERN (www.cern.ch)
 * Author: Alessandro Rubini <rubini@gnudd.com>
 *
 * Released according to the GNU GPL, version 2 or any later version.
 */
#include <wrc.h>
#include <uart-sw.h>

static struct wrc_uart_sw __attribute__((aligned(16)))  uart_sw_dev = {
	.magic = UART_SW_MAGIC,
	.wsize = CONFIG_UART_SW_WSIZE,
	.rsize = CONFIG_UART_SW_RSIZE,
};

static uint16_t nreturned;

void uart_init(void)
{
	/* zero fields, as we may be reloaded */
	uart_sw_dev.nwritten = uart_sw_dev.nread = 0;
}

void uart_write_byte(int b)
{
	int index;

	if (b == '\n')
		uart_write_byte('\r');

	index = uart_sw_dev.nwritten % CONFIG_UART_SW_WSIZE;
	uart_sw_dev.wbuffer[index] = b;
	uart_sw_dev.nwritten++;

	/* fake a real uart, so user-space can poll not-too-fast */
	usleep(1000 * 1000 / 11520);
}

int uart_write_string(const char *s)
{
	const char *t = s;
	while (*s)
		uart_write_byte(*(s++));
	return s - t;
}

int puts(const char *s) __attribute__((alias("uart_write_string")));


int uart_read_byte()
{
	int index;

	if (nreturned == uart_sw_dev.nread) /* nread == written by host */
		return -1;

	index = (nreturned++) % CONFIG_UART_SW_RSIZE;
	return uart_sw_dev.rbuffer[index];
}
