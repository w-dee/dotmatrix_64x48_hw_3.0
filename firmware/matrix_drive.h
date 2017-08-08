#ifndef MATRIX_DRIVE_H
#define MATRIX_DRIVE_H

#define LED_NUM_INTERVAL_MODE 3
enum led_interval_mode_t {
	LIM_AUTO,
	LIM_MODE0,
	LIM_MODE1,
	LIM_MODE2,
	LIM_PWM_OFF
};
void led_disable_i2s_output();
void led_pre_init();
void led_init();
void led_write_settings();
extern uint32_t button_read;
void led_start_pwm_clock();
void led_stop_pwm_clock();
void led_set_interval_mode(led_interval_mode_t mode);
led_interval_mode_t led_get_interval_mode();
void led_set_interval_mode_from_channel(uint8_t ch);
#endif

