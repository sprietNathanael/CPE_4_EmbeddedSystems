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
#include "extdrv/ws2812.h"

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

#define BLINKER_LEFT_FRONT 0
#define LIGHTS_LEFT 1
#define LIGHTS_RIGHT 2
#define BLINKER_RIGHT_FRONT 3
#define BLINKER_RIGHT_BACK 4
#define STOP_RIGHT 5
#define STOP_LEFT 6
#define BLINKER_LEFT_BACK 7

#define LED_NUMBER 8
#define MAX_REFRESH_BLINKERS 50

int servo_config(uint8_t timer_num, uint8_t pwm_chan, uint8_t uart_num);
int set_dir(int givenAngle, int uart_num);
int set_speed_front(int givenAngle, int uart_num);
int set_speed_back(int givenSpeed, int uart_num);
void mode_test(void);
void switchOn_stop_light(uint8_t on, uint8_t uart);
void switchOn_blink_left(uint8_t on);
void switchOn_blink_right(uint8_t on);
void switchOn_lights(uint8_t on);
void blink_left(uint8_t on);
void blink_right(uint8_t on);
void refresh_lights_global(uint8_t uart);
void refresh_blinkers();

#endif