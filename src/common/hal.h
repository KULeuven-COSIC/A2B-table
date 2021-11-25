#ifndef HAL_H
#define HAL_H

#include <stdint.h>
#include <stdio.h>

void hal_setup(void);
void hal_send_str(const char *in);
uint64_t hal_get_time(void);
void printcycles(const char *s, uint64_t c);

#define TRIGGER_OSCI GPIO1
#define TRIGGER_PINPOINT GPIO4

#define trigger_osci_high() (gpio_set(GPIOA, TRIGGER_OSCI))
#define trigger_osci_low() (gpio_clear(GPIOA, TRIGGER_OSCI))
#define trigger_pinpoint_high() (gpio_set(GPIOA, TRIGGER_PINPOINT))
#define trigger_pinpoint_low() (gpio_clear(GPIOA, TRIGGER_PINPOINT))

#endif
