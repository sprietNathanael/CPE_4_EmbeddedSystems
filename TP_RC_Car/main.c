/****************************************************************************
 *   apps/base/servomotor/main.c
 *
 * Servo-motor example
 *
 * Copyright 2016 Nathael Pajani <nathael.pajani@ed3l.fr>
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************** */


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
#include "voiture.h"

#define MODULE_VERSION   0x04
#define MODULE_NAME "GPIO Demo Module"


#define SELECTED_FREQ  FREQ_SEL_48MHz


#define LPC_TIMER_PIN_CONFIG   (LPC_IO_MODE_PULL_UP | LPC_IO_DIGITAL | LPC_IO_DRIVE_HIGHCURENT)

/***************************************************************************** */
/* Pins configuration */
/* pins blocks are passed to set_pins() for pins configuration.
 * Unused pin blocks can be removed safely with the corresponding set_pins() call
 * All pins blocks may be safelly merged in a single block for single set_pins() call..
 */
const struct pio_config common_pins[] = {
	/* UART 0 */
	{ LPC_UART0_RX_PIO_0_1,  LPC_IO_DIGITAL },
	{ LPC_UART0_TX_PIO_0_2,  LPC_IO_DIGITAL },
	/* TIMER_32B0 */
	{ LPC_TIMER_32B0_M1_PIO_0_19, LPC_TIMER_PIN_CONFIG },
	{ LPC_TIMER_32B0_M2_PIO_0_20, LPC_TIMER_PIN_CONFIG },
	{ LPC_GPIO_0_21, (LPC_IO_MODE_PULL_UP | LPC_IO_DIGITAL) },
	ARRAY_LAST_PIO,
};

const struct pio_config adc_pins[] = {
	{ LPC_ADC_AD0_PIO_0_30, LPC_IO_ANALOG },
	{ LPC_ADC_AD1_PIO_0_31, LPC_IO_ANALOG },
	{ LPC_ADC_AD2_PIO_1_0,  LPC_IO_ANALOG },
	{ LPC_ADC_AD3_PIO_1_1,  LPC_IO_ANALOG },
	{ LPC_ADC_AD4_PIO_1_2,  LPC_IO_ANALOG },
	{ LPC_ADC_AD5_PIO_1_3,  LPC_IO_ANALOG },
	ARRAY_LAST_PIO,
};

const struct pio status_led_green = LPC_GPIO_1_4;
const struct pio status_led_red = LPC_GPIO_1_5;

const struct pio ws2812_data_out_pin = LPC_GPIO_0_21; /* Led control data pin */

/* ultrasensor pins */
const struct pio ultrasensor_echo = LPC_GPIO_0_6;
const struct pio ultrasensor_trigger = LPC_GPIO_0_5;

/***************************************************************************** */
void system_init()
{
	/* Stop the watchdog */
	startup_watchdog_disable(); /* Do it right now, before it gets a chance to break in */
	system_set_default_power_state();
	clock_config(SELECTED_FREQ);
	set_pins(common_pins);
	set_pins(adc_pins);
	gpio_on();
	status_led_config(&status_led_green, &status_led_red);
	/* System tick timer MUST be configured and running in order to use the sleeping
	 * functions */
	systick_timer_on(1); /* 1ms */
	systick_start();
}

/* Define our fault handler. This one is not mandatory, the dummy fault handler
 * will be used when it's not overridden here.
 * Note : The default one does a simple infinite loop. If the watchdog is deactivated
 * the system will hang.
 */
void fault_info(const char* name, uint32_t len)
{
	uprintf(UART0, name);
	while (1);
}

/***************************************************************************** */
/* Communication over USB */
uint8_t text_received = 0;
#define NB_CHARS  4
char inbuff[NB_CHARS + 1];
void data_rx(uint8_t c)
{
	static int idx = 0;
	if ((c != 0) && (c != '\n') && (c != '\r')) {
		inbuff[idx++] = c;
		if (idx >= NB_CHARS) {
			inbuff[NB_CHARS] = 0;
			idx = 0;
			text_received = 1;
		}
	} else {
		if (idx != 0) {
			inbuff[idx] = 0;
			text_received = 1;
		}
		idx = 0;
	}
}

void sleep(int value)
{
	volatile uint32_t delay = 0;
	while (delay++ < (value)) {}
}

int atoi(char* chr)
{
	int res = 0;
	int len = strlen(chr);
	int i = 0;
	for (i = 0; i < len; i++)
	{
		if (chr[i] >= '0' && chr[i] <= '9') {
			res *= 10;
			res += (chr[i] - '0');
		} else {
			res = -1;
			break;
		}
	}
	return res;
}

#define ANGLE_OPCODE 'A'
#define STOP_OPCODE 'S'
#define LIGHT_OPCODE 'W'
#define BLINK_RIGHT_OPCODE 'R'
#define BLINK_LEFT_OPCODE 'L'
#define SPEED_OPCODE 'F'
#define BACK_OPCODE 'B'

void computeReceivedFrame()
{
	uprintf(UART0, "Frame Received\n");
	char opCode = inbuff[0];
	int data = atoi(inbuff+1);
	switch(opCode)
	{
		case ANGLE_OPCODE:
			set_dir(data,0);
			break;
		case STOP_OPCODE:
			switchOn_stop_light(data, 0);
			break;
		case LIGHT_OPCODE:
			switchOn_lights(data);
			break;
		case BLINK_RIGHT_OPCODE:
			blink_right(data);
			break;
		case BLINK_LEFT_OPCODE:
			blink_left(data);
			break;
		case SPEED_OPCODE:
			set_speed(data,0);
			break;
		}

}

/*==================Ultrasonic sensor=====================*/

/* Note that clock cycles counter wraps every 89 seconds with system clock running at 48 MHz */
static volatile uint32_t pulse_start = 0;  /* Clock cycles counter upon echo start */
static volatile uint32_t pulse_end = 0;    /* Clock cycles counter upon echo end */
static volatile uint32_t pulse_duration = 0;
void pulse_feedback(uint32_t gpio) {
	static uint32_t pulse_state = 0;
	if (pulse_state == 0) {
		pulse_start = systick_get_clock_cycles();
		pulse_state = 1;
	} else {
		pulse_end = systick_get_clock_cycles();
		if (pulse_end > pulse_start) {
			pulse_duration = (pulse_end - pulse_start);
		} else {
			pulse_duration = (0xFFFFFFFF - pulse_start);
			pulse_duration += pulse_end;
		}
		pulse_state = 0;
	}
}

/* Delay between measures should be at least 50ms  */
#define DELAY 50

/***************************************************************************** */
int main(void)
{
	uint32_t next_time = 0;
	uint32_t delay = 0;

	system_init();
	uart_on(UART0, 115200, data_rx);
	next_time = systick_get_tick_count();
	servo_config(LPC_TIMER_32B0, 1, 2, 0);
	/* Callback on pulse start and end */
	set_gpio_callback(pulse_feedback, &ultrasensor_echo, EDGES_BOTH);

	uprintf(UART0, "Ultrasonic distance sensor using GPIO %d.%d\n", ultrasensor_echo.port, ultrasensor_echo.pin);

	/* Led strip configuration */
	ws2812_config(&ws2812_data_out_pin);
	//isb();
	msleep(5);
 	/*switchOn_stop_light(1);
 	blink_right(1);
 	blink_left(1);
 	switchOn_lights(1);
 	refresh_lights_global();*/

	while (1) {

		//================== Serial Reception ==========================
		if (text_received != 0) {
			status_led(red_toggle);
			computeReceivedFrame();
			text_received = 0;
		}
 		refresh_lights_global(0);

		
 		//================= Ultrasonic distance measurment ===============
 		
		uint32_t distance = 0;

		// Initiate distance mesurement
		gpio_dir_out(ultrasensor_trigger);
		gpio_clear(ultrasensor_trigger);
		usleep(10);
		gpio_set(ultrasensor_trigger);
		usleep(10);
		gpio_clear(ultrasensor_trigger);
		pulse_duration = 0;

		//Wait for value to be available
		while (pulse_duration == 0) {
			msleep(1);
		}
		//Convert pulse width in us to distance in mm 
		distance = ((pulse_duration * 10) / (get_main_clock() / (1000*1000)));
		distance = distance / 29;
		if(distance < 100)
		{
			set_speed(90,0);
			switchOn_stop_light(1, 0);
		}
		// Send value on serial 
		// uprintf(UART0, "dist: %dmm\n", distance);


		// And wait at least 50ms between loops 
		delay = next_time - systick_get_tick_count();
		
		//------
		
 		/*if (delay > DELAY) {
			delay = DELAY;
		}
		msleep(delay);
		next_time += DELAY;*/
		msleep(50);
	}
	return 0;
}
