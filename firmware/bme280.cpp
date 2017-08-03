#include "bme280.h"
#include "Arduino.h"
#include "Wire.h"


 /*
 * BME280 access class from 
 * http://garretlab.web.fc2.com/arduino/lab/temperature_humidity_and_pressure_sensor/index.html
 * Modified by W.Dee to check BME280 available
 */

// TODO .... where is the original implementation copyright !?

BME280::BME280() {
 is_available = false;
}
 
void BME280::begin() {
//  Wire.begin();
}
 
bool BME280::setMode(uint8_t mode, uint8_t t_sb, uint8_t osrs_h, uint8_t osrs_t, uint8_t osrs_p, uint8_t filter) {
  uint8_t config, ctrl_meas, ctrl_hum;
 
  config = (t_sb << 5) | (osrs_p << 2); // spi3w_en = 0;
  ctrl_meas = (osrs_t << 5) | (osrs_p << 2) | mode;
  ctrl_hum = osrs_h;
 
  if(0 != writeRegister(BME280_CTRL_HUM, ctrl_hum)) { is_available = false; return false; }
  if(0 != writeRegister(BME280_CTRL_MEAS, ctrl_meas)) { is_available = false; return false; }
  if(0 != writeRegister(BME280_CONFIG, config)) { is_available = false; return false; }
 
  if(!readTrimmingParameter()) { is_available = false; return false; }
  is_available = true;
  return true;
}
 
bool BME280::getData(double *temperature, double *humidity, double *pressure) {
  uint8_t data[8];
  uint32_t adc_T, adc_H, adc_P;
 
  if(0 == readRegister(BME280_PRESS_MSB, data, 8)) return false;
  
  adc_P = (data[0] << 12) | (data[1] << 4) | (data[2] >> 4);
  adc_T = (data[3] << 12) | (data[4] << 4) | (data[5] >> 4);
  adc_H = (data[6] << 8) | data[7];
 
  *temperature = compensate_T(adc_T) / 100.0;
  *pressure = compensate_P(adc_P) / 256.0 / 100.0;
  *humidity = compensate_H(adc_H) / 1024.0;

  return true;
}
 
void BME280::getStatus(uint8_t *measuring, uint8_t *im_update) {
  uint8_t status;
 
  readRegister(BME280_STATUS, &status, 1);
  *measuring = (status & 0x08) ? 1 : 0;
  *im_update = (status & 0x01) ? 1 : 0;
}
 
uint8_t BME280::isMeasuring() {
  uint8_t status;
 
  readRegister(BME280_STATUS, &status, 1);
  return (status & 0x08) ? 1 : 0;
}
 
uint8_t BME280::isUpdating() {
  uint8_t status;
 
  readRegister(BME280_STATUS, &status, 1);
  return (status & 0x01) ? 1 : 0;
}
 
uint8_t BME280::readId() {
  uint8_t data;
 
  readRegister(BME280_ID, &data, 1);
  return data;
}
 
bool BME280::readTrimmingParameter() {
  uint8_t data[32];
  bool success = true;
 
  success = success && (0 != readRegister(BME280_CALIB00, &(data[0]), 24)); /* 0x88-0x9f */
  success = success && (0 != readRegister(BME280_CALIB25, &(data[24]), 1)); /* 0xa1 */
  success = success && (0 != readRegister(BME280_CALIB26, &(data[25]), 7)); /* 0xe1-0xe7 */

  if(success)
  { 
	  dig_T1 = data[0] | (data[1] << 8);
	  dig_T2 = data[2] | (data[3] << 8);
	  dig_T3 = data[4] | (data[5] << 8);
	 
	  dig_P1 = data[6] | (data[7] << 8);
	  dig_P2 = data[8] | (data[9] << 8);
	  dig_P3 = data[10] | (data[11] << 8);
	  dig_P4 = data[12] | (data[13] << 8);
	  dig_P5 = data[14] | (data[15] << 8);
	  dig_P6 = data[16] | (data[17] << 8);
	  dig_P7 = data[18] | (data[19] << 8);
	  dig_P8 = data[20] | (data[21] << 8);
	  dig_P9 = data[22] | (data[23] << 8);
	 
	  dig_H1 = data[24];
	  dig_H2 = data[25] | (data[26] << 8);
	  dig_H3 = data[27];
	  dig_H4 = (data[28] << 4) | (data[29] & 0x0f);
	  dig_H5 = ((data[29] >> 4) & 0x0f) | (data[30] << 4);
	  dig_H6 = data[31];
  }

  return success;
}
 
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t BME280::compensate_T(int32_t adc_T) {
  int32_t var1, var2, T;
 
  var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}
 
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8 fractional bits).
// Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t BME280::compensate_P(int32_t adc_P) {
  int64_t var1, var2, p;
  var1 = ((int64_t)t_fine) - 128000;
  var2 = var1 * var1 * (int64_t)dig_P6;
  var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
  var2 = var2 + (((int64_t)dig_P4) << 35);
  var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
  var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
  if (var1 == 0) {
    return 0; // avoid exception caused by division by zero
  }
  p = 1048576 - adc_P;
  p = (((p << 31) - var2) * 3125) / var1;
  var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
  var2 = (((int64_t)dig_P8) * p) >> 19;
  p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);
  return (uint32_t)p;
}
 
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional bits).
// Output value of “47445” represents 47445/1024 = 46.333 %RH
uint32_t BME280::compensate_H(int32_t adc_H) {
  int32_t v_x1_u32r;
 
  v_x1_u32r = (t_fine - ((int32_t)76800));
  v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                 ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) * (((v_x1_u32r *
                     ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
                     ((int32_t)dig_H2) + 8192) >> 14));
  v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
  v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
  v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
  return (uint32_t)(v_x1_u32r >> 12);
}
 
uint8_t BME280::writeRegister(uint8_t address, uint8_t data) {
  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(address);
  Wire.write(data);
  return Wire.endTransmission();
}
 
uint8_t BME280::readRegister(uint8_t address, uint8_t data[], uint8_t numberOfData) {
  uint8_t numberOfDataRead;
 
  Wire.beginTransmission(BME280_ADDRESS);
  Wire.write(address);
  if(0 != Wire.endTransmission()) return 0;
 
  Wire.requestFrom((uint8_t)BME280_ADDRESS, numberOfData);
  uint32_t start = millis();
  while (Wire.available() < numberOfData) {
    if((int32_t)(millis() - start) >= 5)
    	return 0; // timeout
    optimistic_yield(1);
  }
 
  for (numberOfDataRead = 0; numberOfDataRead < numberOfData; numberOfDataRead++) {
    data[numberOfDataRead] = Wire.read();
  }
 
  return (numberOfDataRead);
}

