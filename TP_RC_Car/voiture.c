#include "voiture.h"

static uint32_t servo_med_pos_cmd = 0;
static uint32_t servo_one_deg_step = 0;
static uint8_t timer = 0;
static uint8_t channel = 0;
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


int set_servo(int givenAngle, int uart_num)
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
	timer_set_match(timer, channel, pos);
	uprintf(uart_num, "Servo(%d): %d (%d)\n", channel, angle, pos);
	return val;
}