#ifndef MATRIX_DRIVE_H
#define MATRIX_DRIVE_H

void led_init();
void led_set_interval_mode(int mode);
#define LED_NUM_INTERVAL_MODE 3
extern uint32_t button_read;
void led_start_pwm_clock();
void led_stop_pwm_clock();
#endif

