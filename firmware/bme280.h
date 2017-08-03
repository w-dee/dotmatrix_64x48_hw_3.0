#ifndef BME280_H
#define BME280_H

/*
 * BME280 access class header from 
 * http://garretlab.web.fc2.com/arduino/lab/temperature_humidity_and_pressure_sensor/index.html
 */

 
#include <stdint.h>
 
#define BME280_ADDRESS 0x76
 
#define BME280_ID        0xd0
#define BME280_CTRL_HUM  0xf2
#define BME280_STATUS    0xf3
#define BME280_CTRL_MEAS 0xf4
#define BME280_CONFIG    0xf5
#define BME280_CALIB00   0x88
#define BME280_CALIB25   0xa1
#define BME280_CALIB26   0xe1
#define BME280_PRESS_MSB 0xf7
 
// Oversampling rate
#define BME280_OSRS_NONE 0x00
#define BME280_OSRS_x1   0x01
#define BME280_OSRS_x2   0x02
#define BME280_OSRS_x4   0x03
#define BME280_OSRS_x8   0x04
#define BME280_OSRS_x16  0x05
 
#define BME280_MODE_SLEEP   0x00
#define BME280_MODE_FORCED  0x01
#define BME280_MODE_NORMAL 0x03
 
#define BME280_TSB_0P5MS  0x00
#define BME280_TSB_62P5MS 0x01
#define BME280_TSB_125MS  0x02
#define BME280_TSB_250MS  0x03
#define BME280_TSB_500MS  0x04
#define BME280_TSB_1000MS 0x05
#define BME280_TSB_10MS   0x06
#define BME280_TSB_20MS   0x07
 
#define BME280_FILTER_OFF      0x00
#define BME280_FILTER_COEF_2   0x01
#define BME280_FILTER_COEF_4   0x02
#define BME280_FILTER_COEF_8   0x03
#define BME280_FILTER_COEF_16  0x04
 
class BME280 {
  public:
    BME280();
    void begin();
    bool setMode(
      uint8_t mode, // sensor mode
      uint8_t t_sb, // inactive duration
      uint8_t osrs_h, // oversampling of humidity data
      uint8_t osrs_t, // oversampling of temperature data
      uint8_t osrs_p, // oversampling of pressure data
      uint8_t filter // time constant of the IIR filter
    ); //!< returns whether settings BME280 is success or not (mostly fails if BME280 is not connected)
    bool getData(double *temperature, double *humidity, double *pressure);
    void getStatus(uint8_t *measuring, uint8_t *im_update);
    uint8_t isMeasuring();
    uint8_t isUpdating();
    uint8_t readId();
    bool available() const { return is_available; }
  private:
  	bool is_available;
    /* Trimming parameters dig_ */
    uint16_t dig_T1;
    int16_t dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t dig_H1, dig_H3;
    int16_t dig_H2, dig_H4, dig_H5;
    int8_t dig_H6;
    int32_t t_fine;
 
    bool readTrimmingParameter();
    uint8_t writeRegister(uint8_t address, uint8_t data);
    uint8_t readRegister(uint8_t address, uint8_t data[], uint8_t numberOfData);
 
    int32_t compensate_T(int32_t adc_T);
    uint32_t compensate_P(int32_t adc_P);
    uint32_t compensate_H(int32_t adc_H);
};
 
#endif /* BME280_H */

