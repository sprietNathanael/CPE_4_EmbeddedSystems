#ifndef VOITURE_H
#define VOITURE_H

#include "core/system.h"
#include "core/systick.h"
#include "core/lpc_core.h"
#include "core/pio.h"
#include "lib/stdio.h"
#include "drivers/serial.h"
#include "drivers/gpio.h"
#include "extdrv/status_led.h"
#include "drivers/adc.h"
#include "drivers/timers.h"

/* Chose on of these depending on the signal you need:
 *  - inverted one (3), for use with a single transistor
 *  - non inverted (40 - 3), for use with two transistors (or none).
 */
#define DUTY_INVERTED 0
#if (DUTY_INVERTED == 1)
#define SERVO_MED_POS_DUTY_CYCLE  3
#else
#define SERVO_MED_POS_DUTY_CYCLE  (40 - 3)
#endif

int servo_config(uint8_t timer_num, uint8_t pwm_chan, uint8_t uart_num);
int set_servo(int givenAngle, int uart_num);

#endif