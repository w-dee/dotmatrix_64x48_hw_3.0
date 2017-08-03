#include <Arduino.h>
#include "sensors.h"
#include "bme280.h"
#include "pendulum.h"

BME280 bme280;

static bool sensors_get_flag = false;
bme280_result_t bme280_result;


static void sensors_bme280_get()
{
	if(!bme280.available()) return;
	double temperature, humidity, pressure;
	uint8_t measuring, im_update;
	bme280.getData(&temperature, &humidity, &pressure);

	bme280_result.temp_10 = temperature * 10;
	bme280_result.humidity = humidity;
	bme280_result.pressure = pressure;
}


void sensors_init()
{
	bme280.begin();
	bme280.setMode(BME280_MODE_NORMAL, BME280_TSB_1000MS, BME280_OSRS_x1, BME280_OSRS_x1, BME280_OSRS_x1, BME280_FILTER_OFF);
	sensors_bme280_get();
}


void sensors_raise_flag() { sensors_get_flag = true; }
static pendulum_t sensors_pendulum(&sensors_raise_flag, 10000);

void sensors_check()
{
	if(sensors_get_flag)
	{
		sensors_get_flag = false;
		sensors_bme280_get();
	}
}


