#include "voiture.h"

static uint32_t servo_med_pos_cmd = 0;
static uint32_t servo_one_deg_step = 0;
static uint8_t timer = 0;
static uint8_t channel = 0;
static uint8_t blink_left_instruction = 0;
static uint8_t blink_left_state = 0;
static uint8_t blink_right_instruction = 0;
static uint8_t blink_right_state = 0;
static uint16_t refreshCounter = 0;


int servo_config(uint8_t timer_num, uint8_t pwm_chan, uint8_t uart_num)
{
	uint32_t servo_command_period = 0;
	struct lpc_timer_pwm_config timer_conf = {
		.nb_channels = 1,
		.period_chan = 3,
	};

	if (timer_num > LPC_TIMER_32B1) {
		uprintf(uart_num, "Bad timer number\n");
		return -1;
	}
	if (pwm_chan >= 3) {
		uprintf(uart_num, "Bad channel number\n");
		return -1;
	}
	timer = timer_num;
	channel = pwm_chan;
	timer_conf.outputs[0] = pwm_chan;

	/* compute the period and median position for the servo command */
	/* We want 20ms (50Hz), timer counts at main clock frequency */
	servo_command_period = get_main_clock() / 50;
	/* servo_command_period is 20ms, we need 1.5ms, which is 3/40. */
	servo_med_pos_cmd = ((servo_command_period / 40) * SERVO_MED_POS_DUTY_CYCLE);
	servo_one_deg_step = ((servo_command_period / 41) / 48);
	timer_conf.match_values[0] = servo_med_pos_cmd;
	timer_conf.period = servo_command_period;

	timer_on(timer, 0, NULL);
	timer_pwm_config(timer, &timer_conf);
	timer_start(timer);

	uprintf(uart_num, "Servos configured (T%d : C%d), Period : %d, med_pos : %d\n",
						timer, channel, servo_command_period, servo_med_pos_cmd);

	return 0;
}


int set_dir(int givenAngle, int uart_num)
{
	uint16_t val = 0, angle = givenAngle;
	uint32_t pos = servo_med_pos_cmd;

	if (angle > 180) {
		angle = 180;
	}

	/* And compute the new match value for the angle */
	if (angle >= 90) {
		pos += ((angle - 90) * servo_one_deg_step);

	} else {
		pos -= ((90 - angle) * servo_one_deg_step);
	}
	timer_set_match(timer, 1, pos);
	uprintf(uart_num, "Servo(%d): %d (%d)\n", 1, angle, pos);
	return val;
}

int set_speed_front(int givenSpeed, int uart_num)
{
	uint16_t val = 0, angle = givenSpeed;
	uint32_t pos = servo_med_pos_cmd;

	/* And compute the new match value for the angle */
	if (angle >= 90) {
		pos += ((angle - 90) * servo_one_deg_step);
	} else {
		pos -= ((90 - angle) * servo_one_deg_step);
	}
	timer_set_match(timer, 2, pos);
	uprintf(uart_num, "FRONT Servo(%d): %d (%d)\n", 2, angle, pos);
	return val;
}

int set_speed_back(int givenSpeed, int uart_num)
{
	uint16_t val = 0, angle = 90-givenSpeed;
	uint32_t pos = servo_med_pos_cmd;

	if (angle > 90) {
		angle = 90;
	}
	else if(angle < 0) {
		angle = 0;
	}

	/* And compute the new match value for the angle */
	if (angle >= 90) {
		pos += ((angle - 90) * servo_one_deg_step);
		switchOn_blink_right(1);
	} else {
		pos -= ((90 - angle) * servo_one_deg_step);
		switchOn_blink_left(1);
	}
	timer_set_match(timer, channel, pos);
	uprintf(uart_num, "Servo(%d): %d (%d)\n", channel, angle, pos);
	return val;
}


/* This mode reads values from ADC[0:2] every 150ms and uses the values to set the leds.
 * Pixels are updated when all pixel are set from ADC input.
 */
void mode_test(void)
{
	uint16_t red = 0, green = 0, blue = 0;
	red = 255;
	/* Set one pixel */
	ws2812_set_pixel(0, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
	blue = 255;
	ws2812_set_pixel(1, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
	ws2812_send_frame(2);
}

void switchOn_stop_light(uint8_t on, uint8_t uart)
{
	uprintf(uart, "switch on stop\n");
	uint16_t red = 0, green = 0, blue = 0;
	if(on)
	{
		red = 255;
	}
	else
	{
		red = 0;
	}
	ws2812_set_pixel(STOP_RIGHT, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
	ws2812_set_pixel(STOP_LEFT, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
}

void switchOn_blink_left(uint8_t on)
{
	uint16_t red = 0, green = 0, blue = 0;
	blink_left_state = on;
	if(on)
	{
		red = 204;
		green = 139;
	}
	else
	{
		red = 0;
		green = 0;
	}
	ws2812_set_pixel(BLINKER_LEFT_FRONT, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
	ws2812_set_pixel(BLINKER_LEFT_BACK, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
}

void switchOn_blink_right(uint8_t on)
{
	uint16_t red = 0, green = 0, blue = 0;
	blink_right_state = on;
	if(on)
	{
		red = 204;
		green = 139;
	}
	else
	{
		red = 0;
		green = 0;
	}
	ws2812_set_pixel(BLINKER_RIGHT_FRONT, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
	ws2812_set_pixel(BLINKER_RIGHT_BACK, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
}

void switchOn_lights(uint8_t on)
{
	uint16_t red = 0, green = 0, blue = 0;
	if(on)
	{
		red = 255;
		green = 239;
		blue = 89;
	}
	else
	{
		red = 0;
		green = 0;
		blue = 0;
	}
	ws2812_set_pixel(LIGHTS_LEFT, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
	ws2812_set_pixel(LIGHTS_RIGHT, (red & 0xFF), (green & 0xFF), (blue & 0xFF));
}

void blink_left(uint8_t on)
{
	blink_left_instruction = on;
}

void blink_right(uint8_t on)
{
	blink_right_instruction = on;
}

void refresh_blinkers()
{
	if(blink_left_state)
	{
		switchOn_blink_left(0);
	}
	else if(blink_left_instruction)
	{
		switchOn_blink_left(1);
	}

	if(blink_right_state)
	{
		switchOn_blink_right(0);
	}
	else if(blink_right_instruction)
	{
		switchOn_blink_right(1);
	}
}

void refresh_lights_global(uint8_t uart)
{
	refreshCounter++;
	if(refreshCounter >= MAX_REFRESH_BLINKERS)
	{
		refreshCounter = 0;
		refresh_blinkers();
	}
	ws2812_send_frame(0);
}

