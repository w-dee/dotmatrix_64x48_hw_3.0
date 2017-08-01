#include <Arduino.h>
#include <SPI.h>
#include "eagle_soc.h"
#include <ESP8266WiFi.h>
#include "frame_buffer.h"
#include "matrix_drive.h"
#include "panic.h"
#include "settings.h"

#define LED_COL_SER_GPIO 13 // 13=MOSI
#define LED_COL_LATCH_GPIO 12 // 12=MISO, but in DIO mode, it acts as second line of MOSI
#define LED_HC595_LATCH_GPIO 15
#define LED_SER_CLOCK_GPIO 14 // 14=SCK

#define BUTTON_SENS_GPIO 16

#define LED_MAX_ROW 24
#define LED_MAX_COL 128


// inline fast version of digitalWrite
#define dW(pin, val)  do { if(pin < 16){ \
    if(val) GPOS = (1 << pin); \
    else GPOC = (1 << pin); \
  } else if(pin == 16){ \
    GP16O = !!val; \
  } }while(0)

#define led_hc595_latch_out(b) /* HC595 latch is not controlled by hardware anyway */ \
	dW(LED_HC595_LATCH_GPIO, (b))

extern "C" {
	void rom_i2c_writeReg_Mask(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
}


/**
 * Initialize GPIOs
 */
static void led_init_gpio()
{
	// configure pins needed for communicating to the ICs.
	// note some pins are later initialized to be
	// controlled by the hardware, not GPIO.
	pinMode(LED_COL_SER_GPIO, OUTPUT);
	pinMode(LED_COL_LATCH_GPIO, OUTPUT);
	pinMode(LED_HC595_LATCH_GPIO, OUTPUT);
	pinMode(LED_SER_CLOCK_GPIO, OUTPUT);
	pinMode(BUTTON_SENS_GPIO, INPUT);
	digitalWrite(LED_COL_SER_GPIO, LOW);
	digitalWrite(LED_COL_LATCH_GPIO, LOW);
	digitalWrite(LED_HC595_LATCH_GPIO, LOW);
	digitalWrite(LED_SER_CLOCK_GPIO, LOW);
}

/**
 * simple lfsr function
 */
static uint32_t led_lfsr(uint32_t lfsr)
{
	lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xd0000001u);
	return lfsr;
}

static void led_print_0_1(int v)
{
	if(v) Serial.print(F("1")); else Serial.print(F("0"));
}

/**
 * Set initial LED1642 configuration for POST
 */
static void led_post_set_led1642_config()
{
	constexpr uint16_t led_config = (1<<13); // enable SDO delay
	for(int i = 0; i < LED_MAX_COL / 16; ++i)
	{
		for(int bit = 15; bit >= 0; --bit)
		{
			// set bit
			dW(LED_COL_SER_GPIO, !!(led_config & (1<<bit)));

			// latch on
			if(i == (LED_MAX_COL / 16 - 1) &&
				bit == 7-1)
			{
				// latch (clock count while latch is active = 7, set CR function)
				dW(LED_COL_LATCH_GPIO, 1);
			}
			// clock
			dW(LED_SER_CLOCK_GPIO, 1);
			dW(LED_SER_CLOCK_GPIO, 0);
		}
		// latch off
		dW(LED_COL_LATCH_GPIO, 0);
	}
}

/**
 * POST check of serial chain
 */
static void led_post()
{
	delay(500);
	// this check uses GPIO, not SPI hardware.

	// set SDO delay; see also led_init_led1642() description
	for(int i = 0; i < (LED_MAX_COL / 16) * 2; ++i)
		led_post_set_led1642_config();

	// prepare for sending bits
	constexpr int num_bits = LED_MAX_COL + LED_MAX_ROW;
	uint32_t lfsr = 0xabcd0123;

	Serial.println(F("LED matrix driver: Checking serial data path ..."));

	Serial.print(F("sent    :"));
	// shift in test pattern into the shift register.
	for(int i = 0; i < num_bits; ++i)
	{
		// unfortunately (fortunately?) there is no need to
		// insert wait between these bit-banging, since
		// ESP8266's GPIO is very slow.
		dW(LED_COL_SER_GPIO, lfsr & 1);
		led_print_0_1(lfsr&1);
		dW(LED_SER_CLOCK_GPIO, 1);
		dW(LED_SER_CLOCK_GPIO, 0);
		lfsr = led_lfsr(lfsr);
	}
	Serial.print(F("\r\n"));

	// then, read these bits out
	pinMode(LED_COL_SER_GPIO, INPUT);
		// shift register's output shares its input pin.
	lfsr = 0xabcd0123;
	Serial.print(F("received:"));
	bool error = false;
	for(int i = 0; i < num_bits; ++i)
	{
		// sense the input pin;
		// we may need some delay here because
		// driving the input pin is very weak.
		delayMicroseconds(10);
		int r = digitalRead(LED_COL_SER_GPIO);
		led_print_0_1(r);
		if(r != (lfsr & 1))
		{
			// error found
			error = true;
		}
		
		dW(LED_SER_CLOCK_GPIO, 1);
		dW(LED_SER_CLOCK_GPIO, 0);
		lfsr = led_lfsr(lfsr);
	}
	Serial.print(F("\r\n"));
	pinMode(LED_COL_SER_GPIO, OUTPUT);

	if(error)
	{
		do_panic(2, F("Sent serial data does not return to the sender. Check each components are properly soldered."));
	}
	else
	{
		Serial.println(F("Serial data path check suceeded."));
	}
}

static int led_pwm_current_div = 0;
static uint32_t led_pwm_current_pattern = 0;
static bool led_pwm_clock_running; //!< whether the LED PWM clock is running or not

#define SLOW_CLOCK_DIV 0x3f
#define SLOW_BCK_DIV 5

/**
 * Set SPI hardware clock division
 */
static void _set_i2s_div()
{
	uint32_t i2s_clock_div =
		(led_pwm_clock_running?led_pwm_current_div:SLOW_CLOCK_DIV) & I2SCDM;
	uint8_t i2s_bck_div = (led_pwm_clock_running?1:SLOW_BCK_DIV) & I2SBDM;

	//!trans master, !bits mod, rece slave mod, rece msb shift, right first, msb right
	I2SC &= ~(I2STSM | (I2SBMM << I2SBM) | (I2SBDM << I2SBD) | (I2SCDM << I2SCD));
	I2SC |= I2SRF | I2SMR | I2SRSM | I2SRMS | ((i2s_bck_div) << I2SBD) | ((i2s_clock_div) << I2SCD);

	// pattern
	I2STXF = (led_pwm_clock_running?led_pwm_current_pattern:0xaaaaaaaa);

}

/**
 * Set SPI hardware clock division
 */
static void set_i2s_div_pat(int div, uint32_t pat)
{
	led_pwm_current_div = div;
	led_pwm_current_pattern = pat;
	_set_i2s_div();
}

/**
 * Start LED PWM clock
 */
void led_start_pwm_clock()
{
	led_pwm_clock_running = true;
	_set_i2s_div();
}

/**
 * Stop LED PWM clock.
 * This function does not actually stop the clock;
 * Just slow down the clock to the rate which will not interfere with WiFi.
 * Because with LED1642's maximum possible brighness of 4095,
 * when internal clock counter being 4095, there is rarely possiblity of
 * showing blank if the brighness is maximum if the internal counter stops
 * at maximum value. Since internal clock counter is not readable,
 * I decided feeding the clock insteadof stopping it, but at very slow rate
 * with will not interfere with WiFi.
 */
void led_stop_pwm_clock()
{
	led_pwm_clock_running = false;
	_set_i2s_div();
}

/**
 * Initialize SPI hardware and clock generator for LED1642
 */
static void led_init_spi_and_ledclock()
{
	// setup hardware SPI
	SPI.begin();
	SPI.setHwCs(false);
	SPI.setFrequency((int)13333333);
	SPI1U = SPIUMOSI | SPIUSSE | SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO
	SPI1C &= ~(SPICWBO | SPICRBO); // MSB first

	// setup I2S data output from GPIO3
//	pinMode(15, FUNCTION_1); //I2SO_BCK (SCLK) // unused
	pinMode(3, FUNCTION_1); //I2SO_DATA (SDIN) // used for spread-spectrum clock output

	I2S_CLK_ENABLE();
	I2SIC = 0x3F;
	I2SIE = 0;

	I2SC &= ~(I2SRST);
	I2SC |= I2SRST;
	I2SC &= ~(I2SRST);

	I2SFC &= ~(I2SDE | (I2STXFMM << I2STXFM) | (I2SRXFMM << I2SRXFM)); //Set RX/TX FIFO_MOD=0 and disable DMA (FIFO only)
	I2SCC &= ~((I2STXCMM << I2STXCM) | (I2SRXCMM << I2SRXCM)); //Set RX/TX CHAN_MOD=0

	// I2S clock freq is: 160000000.0 / div / 1
	set_i2s_div_pat(4, 0xaaaaaaaa); // just for initial value

	// start I2S
	I2SC |= I2STXS; //Start transmission

}


/**
 * Set SPI transaction length
 */
static void ICACHE_RAM_ATTR led_spi_set_length(int bits)
{
    const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    bits--;
    SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (0 << SPILMISO)));
}


static constexpr uint32_t bit_interleave8(uint32_t v)
{
	// 0b0000000000000000ABCDEFGHIJKLMNOP ->
	// 0b00000000ABCDEFGH00000000IJKLMNOP
	return (v  | v << 8) &
	   0b00000000111111110000000011111111u;
}

static constexpr uint32_t bit_interleave4(uint32_t v)
{
	// 0b00000000ABCDEFGH00000000IJKLMNOP ->
	// 0b0000ABCD0000EFGH0000IJKL0000MNOP
	return (v  | v << 4) &
	   0b00001111000011110000111100001111u;
}

static constexpr uint32_t bit_interleave2(uint32_t v)
{
	// 0b0000ABCD0000EFGH0000IJKL0000MNOP ->
	// 0b00AB00CD00EF00GH00IJ00KL00MN00OP
	return (v  | v << 2) &
	   0b00110011001100110011001100110011u;
}

static constexpr uint32_t bit_interleave1(uint32_t v)
{
	// 0b00AB00CD00EF00GH00IJ00KL00MN00OP
	// 0b0A0B0C0D0E0F0G0H0I0J0K0L0M0N0O0P
	return (v  | v << 1) &
	   0b01010101010101010101010101010101u;
}


/**
 * bit interleave function
 */
static constexpr uint32_t bit_interleave(uint16_t v)
{
	// Every bit in v goes to even bit of the result.
	// This is optimization for SPI DIO, which send every even bit
	// to the MOSI line, and every odd bit to the MISO line.
	return
		bit_interleave1(
		bit_interleave2(
		bit_interleave4(
		bit_interleave8(v))));
}

/**
 * 8bit swap function
 */
 static constexpr uint32_t ICACHE_RAM_ATTR bit_8_swap(uint32_t v) {
 	return ((v >> 8) & 0x00ff00ff) | ((v & 0x00ff00ff) << 8); }
/**
 * 16bit swap function
 */
static constexpr uint32_t ICACHE_RAM_ATTR bit_16_swap(uint32_t v) {
	return ((v >> 16) & 0x0000ffff) | ((v & 0x0000ffff) <<16); }

/**
 * byte reverse function
 */
static constexpr uint32_t ICACHE_RAM_ATTR byte_reverse(uint32_t v)
{
	return bit_16_swap(bit_8_swap(v));
}

/**
 * Gamma curve function
 */
static constexpr uint32_t gamma_255_to_4095(int in)
{
  return byte_reverse(
  	bit_interleave(
  	(uint32_t) (pow((double)(in+20) / (255.0+20), 3.5) * 3800)));  
}

#define G4(N) gamma_255_to_4095((N)), gamma_255_to_4095((N)+1), \
      gamma_255_to_4095((N)+2), gamma_255_to_4095((N)+3), 

#define G16(N) G4(N) G4((N)+4) G4((N)+8) G4((N)+12) 
#define G64(N) G16(N) G16((N)+16) G16((N)+32) G16((N)+48) 

/**
 * Gamma curve table
 */
static constexpr uint32_t gamma_table[256] = {
	G64(0) G64(64) G64(128) G64(192)
	}; // this table must be accessible from interrupt routine;
	// do not place in FLASH !!




/**
	Select one row and enable driver for the row 
	@param n   Row number to activate(0 .. LED_MAX_ROW-1).
				Specify -1 to unselect all rows.
*/
static void ICACHE_RAM_ATTR led_sel_row_prepare(int n)
{
	static constexpr uint32_t bit_sel_pattern[LED_MAX_ROW + 1] = {
		-1, // for -1
#define BP(N) (~byte_reverse(1U << (31-(N))))
#define BP8(N) \
	BP((N)+0),BP((N)+1),BP((N)+2),BP((N)+3), \
	BP((N)+4),BP((N)+5),BP((N)+6),BP((N)+7) 
		BP8(0),
		BP8(8),
		BP8(16) };

	// wait for SPI transaction
	while(SPI1CMD & SPIBUSY) /**/ ;

	// SPI setup
	SPI1U &= ~ SPIUFWDUAL;
	SPI1C &= ~ (SPICFASTRD | SPICDOUT); // not use DIO

	led_spi_set_length(LED_MAX_ROW + (LED_MAX_COL/16) * 16);

	// row selection
	volatile uint32_t * fifoPtr = &SPI1W0;

	// bit 31 = row0, 30 = row1 ... and so on
	*(fifoPtr++) = bit_sel_pattern[n + 1];

	// three HC595s are connected after four LED1642,
	// so we need to shift-out the dummy data from LED1642 to HC595.
	// here, we fill the fifo with zero, which will be later
	// switch off data for LED1642.
	constexpr int remain_len = ((LED_MAX_COL/16) * 16 - 1) / 32 + 1;
	for(int i = 0; i < remain_len; ++i) *(fifoPtr ++) = 0;

	// begin SPI transaction
	SPI1CMD |= SPIBUSY;
}
static void ICACHE_RAM_ATTR led_sel_row_blank()
{
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction

	// let HC595 latching the data
	led_hc595_latch_out(true);

	// restore SPI DIO mode
	SPI1U |= SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO

	// reset HC595 latching line
	led_hc595_latch_out(false);
}



static void ICACHE_RAM_ATTR led_sel_row_show()
{
}


static int current_row; //!< current scanning row
static int current_phase; //!< current phase; 6 stage phase.

uint32_t button_read; //!< read value of each button

static void ICACHE_RAM_ATTR button_read_gpio()
{
	// update button GPIO value
	static_assert(BUTTON_SENS_GPIO == 16, "Check the implementation");
	uint32_t value = button_read;
	uint32_t bit = (1U << current_row);
	value &= ~bit;
	if(!(GP16I & 0x01)) value |= bit;
	button_read = value;
}

static constexpr int max_phase = 12; //!< phase count
	// current_phase
	// 0:             : set LED1642 control word
	// 1:             : read buttons, increment row, led ([0])
	// 2:             : led ([2])
	// 3:             : led ([4])
	// 4:             : led ([6])
	// 5:             : led ([8])
	// 6:             : led ([10])
	// 7:             : led ([12])
	// 8:             : all led off(LED1642)
	// 9:             : blank prepare (send next row data to the HC595)
	// 10:            : blank row(HC595), led ([14])
	// 11:            : show row(HC595), all led on(LED1642)

	// each phase is distributed into small interrupts,
	// to minimize interrupt latency.

/**
 * timer counter value
 */
struct timer_interval_values_t
{
	int32_t freq_div; //!< frequency division value
	uint32_t clock_pattern; //!< bit pattern of the clock
	int32_t timer_interval;//!< timer hsync interval in 80MHz cycle
	int32_t timer_duration_phase[max_phase];//!< timer duration of each phase
};
static constexpr timer_interval_values_t timer_interval_values[LED_NUM_INTERVAL_MODE] =
{
	{
		3,
		0xaaaaaaaa,
		36864,
		{
			3000,
			3400,
			3464,
			3000,
			3000,
			3000,
			3000,
			3000,
			3000,
			3000,
			3500,
			2500,

		}
	},
	{
		5,
		0xaaaaaaaa,
		40960,
		{
			3300,
			3860,
			3300,
			3300,
			3400,
			3400,
			3400,
			3400,
			3400,
			3400,
			3400,
			3400,
		}
	},
	{
		6,
		0xaaaaaaaa,
		49152,
		{
			4100	,
			4000	,
			4152	,
			4100	,
			4100	,
			4100	,
			4100	,
			4100	,
			4100	,
			4100	,
			4100	,
			4100	,
		}
	},
};
static int current_interval_index = 0;


static constexpr int phase_sum(const timer_interval_values_t values, int index) { return index == -1 ? 0 : values.timer_duration_phase[index] + phase_sum(values, index-1); }
static_assert(phase_sum(timer_interval_values[0], max_phase-1) == timer_interval_values[0].timer_interval, "timer_interval[0] sum mismatch");
static_assert(phase_sum(timer_interval_values[1], max_phase-1) == timer_interval_values[1].timer_interval, "timer_interval[1] sum mismatch");
static_assert(phase_sum(timer_interval_values[2], max_phase-1) == timer_interval_values[2].timer_interval, "timer_interval[2] sum mismatch");

static inline uint32_t ICACHE_RAM_ATTR led_tbl_gamma(uint8_t x) { return gamma_table[x]; }
static inline uint32_t ICACHE_RAM_ATTR led_tbl_bw(uint8_t x)
{
	// black & white (2 level) display
	return x >= 0x80 ? byte_reverse(0b01010101010101010101010101010101) : 0;
}


/**
 * set one line brightness
 */
static void ICACHE_RAM_ATTR led_set_brightness_one_row(int start_led, bool do_global_latch = false)
{
	// TODO: write stride explanation here
	/*
		led order:
		  0   2   4   6   8  10  12  14  ...
		  1   3   5   7   9  11  13  15  ...

		VRAM order:
	0:	  0   1   2   3   4   5   6   7 ...
	1:	  0   1   2   3   4   5   6   7 ...

		16 LEDs can be set at most per one call to this function

		first (on VRAM address)
		offset row0 start_led + 0  +   : 0,  1*16,  2*16,  3*16,  4*16,  5*16,  6*16,  7*16(LATCH),
		offset row1 start_led + 0  +   : 0,  1*16,  2*16,  3*16,  4*16,  5*16,  6*16,  7*16(LATCH),
		
			
	*/ 



//	static_assert(LED_MAX_COL == 64, "sorry at this point LED_MAX_COL is not flexible");
	const uint8_t *buf = get_current_frame_buffer().array()[current_row*2];
	const uint8_t *buf2 = get_current_frame_buffer().array()[current_row*2+1];

	volatile uint32_t * fifoPtr = &SPI1W0;

	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
	led_spi_set_length(32*16);

	constexpr uint32_t global_latch_pattern =
		byte_reverse(0b00000000000000000000001010101010);
	constexpr uint32_t data_latch_pattern   =
		byte_reverse(0b00000000000000000000000000101010);

	// fill fifo with buffer, with
	// converting gamma and adding latch pattern
#define WR(N, L, TBL) do { \
	uint32_t w = TBL(*(N)); w += L; \
	*(fifoPtr++) = w; } while(0)
#define PL(TBL, I,C,L) WR((I)+start_led+(C), (L), TBL)
#define REG(TBL) do { \
	PL(TBL, 0*8,buf,  0); \
	PL(TBL, 1*8,buf,  0); \
	PL(TBL, 2*8,buf,  0); \
	PL(TBL, 3*8,buf,  0); \
	PL(TBL, 4*8,buf,  0); \
	PL(TBL, 5*8,buf,  0); \
	PL(TBL, 6*8,buf,  0); \
	PL(TBL, 7*8,buf,  data_latch_pattern); \
	PL(TBL, 0*8,buf2, 0); \
	PL(TBL, 1*8,buf2, 0); \
	PL(TBL, 2*8,buf2, 0); \
	PL(TBL, 3*8,buf2, 0); \
	PL(TBL, 4*8,buf2, 0); \
	PL(TBL, 5*8,buf2, 0); \
	PL(TBL, 6*8,buf2, 0); \
	PL(TBL, 7*8,buf2, do_global_latch ? global_latch_pattern : data_latch_pattern); \
	} while(0)

	if(led_pwm_clock_running)
		REG(led_tbl_gamma);
	else
		REG(led_tbl_bw);


	// begin SPI transaction
	SPI1CMD |= SPIBUSY;
}

/**
 Returns configuration pattern from register number. this return value can be used
 in led_set_led1642_reg().
 */
static constexpr uint32_t ICACHE_RAM_ATTR config_reg_pattern_from_num(int n)
{
	// n = 1 ->  0b0010
	// n = 2 ->  0b1010 ...
	// n = 3 ->  0b101010 ...
	return byte_reverse(((1U << n*2) - 1) & 0xaaaaaaaau);
}


/**
	Set LED1642 configuration register.
	Note this function uses SPI hardware to set the register.
	call this function after calling led_init_spi_and_ledclock()
	@param	 latch_pattern   latch pattern. this must be in form of eg.
		byte_reverse(0b000000001010101010)
		 when register number = 5. Every one must be in odd digit in the pettern
		and the population count of '1' is the register number.
	@param	register_pattern  register value which must be already converted by 
		byte_reverse(bit_interleave()).
 */
static void ICACHE_RAM_ATTR led_set_led1642_reg(uint32_t latch_pattern, uint32_t register_pattern)
{
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction


	SPI1U |= SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO

	volatile uint32_t * fifoPtr = &SPI1W0;

	led_spi_set_length(32*(LED_MAX_COL / 16));

	// for each LED1642
	for(int i = 0; i < LED_MAX_COL / 16; ++i)
	{
		bool do_latch = i == (LED_MAX_COL / 16) -1;
		uint32_t w = register_pattern;

		if(do_latch)   w |= latch_pattern;

		// push to fifo
		*(fifoPtr++) = w;
	}

	// begin SPI transaction
	SPI1CMD |= SPIBUSY;
}

/**
 LED1642 configuration register word (must be already bit-interleaved and byte-swapped by byte_reverse(bit_interleave()) )
 */
static uint32_t led1642_configration_reg;

/**
 * whether led1642_configration_reg has changed or not
 */
static bool led1642_configration_reg_changed;


/**
	Initialize LED1642
 */
static void led_init_led1642()
{
	// See also: led_post() also sets initial setting for LED1642 only for POST
	led1642_configration_reg =
		byte_reverse(bit_interleave(
			(1<<11) | (1<<12) | // Output turn-on/off time: on:180ns, off:150ns
			(1<<15) | (1<<13) | 63)); // 4096 brightness steps, SDO delay

	// Here we should repeat setting configuration register several times
	// at least number of LED1642.
	// We should enable SDO delay because the timing is too tight to
	// transfer data on the serial chain if no SDO delay is applied.
	// Because SDO delay would be set properly only if the previous
	// LED1642 on the chain has delay on the data,
	// so we must set SDO delay one by one from the first LED1642 on the chain
	// to make all LED1642s have proper configuration.

	for(int i = 0; i < (LED_MAX_COL / 16)*2; ++i)
	{
		led_set_led1642_reg(
			config_reg_pattern_from_num(7), 
			led1642_configration_reg
			); // write to config reg
		while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
	}
	led_set_led1642_reg(
		config_reg_pattern_from_num(1),
		byte_reverse(bit_interleave(0xffff)) // all led on
		); // write to switch reg
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
}


static void ICACHE_RAM_ATTR led_sel_led1642_all_blank()
{
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
	led_set_led1642_reg(
		config_reg_pattern_from_num(1),
		byte_reverse(bit_interleave(0x0000)) // all led off
		); // write to switch reg
}

static void ICACHE_RAM_ATTR led_sel_led1642_all_show()
{
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
	led_set_led1642_reg(
		config_reg_pattern_from_num(1),
		byte_reverse(bit_interleave(0xffff)) // all led on
		); // write to switch reg
}


/**
	Set LED1642 brightness timer handler

 */
static void ICACHE_RAM_ATTR led_set_brightness()
{

	switch(current_phase)
	{
	case 0:
		// set configuration register
		if(led1642_configration_reg_changed)
		{
			led_set_led1642_reg(config_reg_pattern_from_num(7),
				led1642_configration_reg);
			led1642_configration_reg_changed = false;
		}
		break;

	case 1:
		// read button gpio
		button_read_gpio();
		// step to next row
		++ current_row;
		if(current_row >= LED_MAX_ROW) current_row = 0;

		led_set_brightness_one_row(0);
		break;

	case 2:
		led_set_brightness_one_row(1);
		break;

	case 3:
		led_set_brightness_one_row(2);
		break;

	case 4:
		led_set_brightness_one_row(3);
		break;

	case 5:
		led_set_brightness_one_row(4);
		break;

	case 6:
		led_set_brightness_one_row(5);
		break;

	case 7:
		led_set_brightness_one_row(6);
		break;

	case 8:
		led_sel_led1642_all_blank();
		break;

	case 9:
		led_sel_row_prepare(current_row);
		break;

	case 10:
		led_sel_row_blank();
		led_set_brightness_one_row(7, true);
		break;

	case 11:
		led_sel_row_show();
		led_sel_led1642_all_show();
		break;
	}

	// step to next phase
	++current_phase;
	if(current_phase == max_phase) current_phase = 0;
}


/**
 * next interrupt tick
 */
static uint32_t next_tick = 0;


/**
 * interrupt counter
 */
static uint32_t interrupt_count = 0;

/**
 * interrupt overrun counter
 */
static uint32_t interrupt_overrun_count = 0;

/**
 * interrupt allowable delay
 * ;should be below all timer_interval_values[].timer_interval
 */
static constexpr uint32_t interrupt_arrowable_delay = 15000;

/**
 * expected interrupt delay from end of the interrupt to next interrupt 
 */
static constexpr uint32_t interrupt_delay = 200;


/**
 * last overruned phase
 */
static int last_overrun_phase = 0;


/**
 * Timer interrupt handler
 */
static void ICACHE_RAM_ATTR timer_handler()
{
	static bool in_handler;
	if(in_handler) return;
	in_handler = true;

	++interrupt_count;
	uint32_t current_tick = ESP.getCycleCount();

	/*
		           nt        nt+iad
		============*----------+

		            ^ct just in time
		                  ^ct arrowable
		                       ^ct  arrowable
		                           ^ct not arrowable
	*/

	if((SPI1CMD & SPIBUSY) ||
		(int32_t)(next_tick + interrupt_arrowable_delay - current_tick) < 0)
	{
		// SPI transaction in progress or
		// interrupt too late;
		// round up to next cycle
		++ interrupt_overrun_count;
		last_overrun_phase = current_phase;
		uint32_t tick_add = timer_interval_values[current_interval_index].timer_interval;
#if F_CPU == 160000000
		tick_add *= 2;
#endif

		next_tick += tick_add;
		while((int32_t)(next_tick + interrupt_arrowable_delay - current_tick) < 0)
			next_tick += tick_add;
		timer0_write(next_tick);
	}
	else
	{
		int phase = current_phase;
		uint32_t phase_duration =
			timer_interval_values[current_interval_index].timer_duration_phase[phase];

#if F_CPU == 160000000
		phase_duration *= 2;
#endif
		next_tick += phase_duration;

		// call LED SPI function
		led_set_brightness();

		// check next tick;
		current_tick = ESP.getCycleCount();
		if((next_tick + interrupt_delay - current_tick) < 0)
		{
			// next interrupt point will be already in past
			last_overrun_phase = current_phase;
			uint32_t tick_add = timer_interval_values[current_interval_index].timer_interval;
#if F_CPU == 160000000
			tick_add *= 2;
#endif
			while((int32_t)(next_tick + interrupt_delay - current_tick) < 0)
				next_tick += tick_add;
		}
		timer0_write(next_tick);
	}


	in_handler = false;
}

/**
 * Initialize timer
 */
static void led_init_timer()
{
	next_tick = ESP.getCycleCount() + 65536*8;/* any values over one vsync interval */
	timer0_isr_init();
	timer0_write(next_tick);
	timer0_attachInterrupt(timer_handler);
	timer0_write(next_tick);
}


static uint8_t wifi_current_ch = 0;
static led_interval_mode_t current_interval_mode = LIM_AUTO;

/**
 * Initialize LED matrix driver
 */
void led_init()
{
	for(int i = 0; i < 48; i++)
		for(int j = 0; j < 64; j++)
		get_current_frame_buffer().array()[i][j] = 0;

	led_init_gpio();
	led_post();
	led_init_spi_and_ledclock();
	led_init_led1642();
	led_init_timer();
	led_start_pwm_clock();

	delay(3000);

	// restore led interval mode
	bool res = settings_write(F("led_interval_mode"), F("0"), SETTINGS_NO_OVERWRITE);
	String strval;
	res = settings_read(F("led_interval_mode"), strval);
	led_set_interval_mode( (led_interval_mode_t) strval.toInt());
}

void led_write_settings()
{
	settings_write(F("led_interval_mode"), String((long)current_interval_mode, /*base = */10));
}

/**
 * LED uninitialization
 */
void led_uninit()
{
}


/**
 * set led pwm clock mode from wifi channel
 */
static led_interval_mode_t led_interval_mode_from_channel(uint8_t ch)
{
	// this table is from empirical test
	// mode 0 = 26.666...MHz
	// mode 1 = 16.000   MHz
	// mode 2 = 13.333...MHz
	switch(ch)
	{
		case	1	: return	LIM_MODE0	; //	
		case	2	: return	LIM_MODE0	; //
		case	3	: return	LIM_MODE2	; //
		case	4	: return	LIM_MODE2	; //
		case	5	: return	LIM_MODE1	; //	
		case	6	: return	LIM_MODE1	; //	
		case	7	: return	LIM_MODE0	; //	
		case	8	: return	LIM_MODE2	; //	
		case	9	: return	LIM_MODE2	; //	
		case	10	: return	LIM_MODE2	; //	
		case	11	: return	LIM_MODE1	; //	or 0
		case	12	: return	LIM_MODE0	; //	
		case	13	: return	LIM_MODE2	; //	
	}

	return LIM_MODE0; // fallback ... this should be most robust against EMI
}

static void set_i2s_div_from_current_interval_index()
{
	set_i2s_div_pat(timer_interval_values[current_interval_index].freq_div,
		timer_interval_values[current_interval_index].clock_pattern);
}

static void _led_set_interval_mode()
{
	switch(current_interval_mode)
	{
	case LIM_AUTO:
		current_interval_index =
			(int)led_interval_mode_from_channel(wifi_current_ch) - (int)LIM_MODE0;
		set_i2s_div_from_current_interval_index();
		led_start_pwm_clock();
		break;

	case LIM_MODE0:
	case LIM_MODE1:
	case LIM_MODE2:
		current_interval_index = (int)current_interval_mode - (int)LIM_MODE0;
		set_i2s_div_from_current_interval_index();
		led_start_pwm_clock();
		break;

	case LIM_PWM_OFF:
		current_interval_index = 0;
		led_stop_pwm_clock();
		break;
	}
}

void led_set_interval_mode(led_interval_mode_t mode)
{
	current_interval_mode = mode;
	_led_set_interval_mode();
}

led_interval_mode_t led_get_interval_mode()
{
	return current_interval_mode;
}


void led_set_interval_mode_from_channel(uint8_t ch)
{
	wifi_current_ch = ch;
	_led_set_interval_mode();
}


#if 0
#define W 160
#define H 60

static unsigned char buffer[H][W] = { {0} };

static int sv[W] = {0};
static int ssv[W] = {0};

static void step()
{
	for(int x = 0; x < W; x++)
	{
		sv[x] += ssv[x];
		if(sv[x] < 0) sv[x] = 0, ssv[x] +=4;
		if(sv[x] > 255) sv[x] = 255, ssv[x] -=4;
		ssv[x] += rand() %10-5;
		buffer[H-1][x] = sv[x];
	}


	for(int y = 0; y < H-1; y++)
	{
		for(int x = 1; x < W-1; x++)
		{
			if(y < H-3 && buffer[y+3][x] > 50)
			{
				buffer[y][x] = 
					(
					buffer[y+2][x-1] + 
					buffer[y+3][x  ]*6 + 
					buffer[y+2][x+1] +

					buffer[y+1][x-1] + 
					buffer[y+2][x  ]*6 + 
					buffer[y+1][x+1] 
					
					)  / 16; 
			}
			else
			{
				buffer[y][x] = 
					(
					buffer[y+1][x-1] + 
					buffer[y+1][x  ]*2 + 
					buffer[y+1][x+1] )  / 4; 
			}
		}
	}
}

#endif
/*
unsigned char PROGMEM op[][64] = {
#include "op.inc"
};
*/
void test_led_sel_row()
{
/*
	for(int y = 0; y < 48; y++)
	{
		for(int x = 0; x < 64; x++)
		{
			get_current_frame_buffer().array()[y][x] = pgm_read_byte(&(op[y][x]));
		}
	}
*/
//
/*
static int count = 0;
if(count == 0)
{

	for(int y = 24; y < 48; y++)
	{
		for(int x = 0; x < 64; x++)
		{
			get_current_frame_buffer().array()[y][x] = x*4;
		}
	}
	++count;
}
*/
/*
while(1)
{

	while(current_phase != 1) led_set_brightness();

	led_set_brightness();
*/
/*
step();
	for(int y = 0; y < 48; y++)
	{
		memcpy(get_current_frame_buffer().array()[y], buffer[y] + 10, 128);
	}
delay(10);
*/

/*
if(current_row == 0)
{
	static int nf = 0;
	++nf;
	if((nf & 3) == 3)
	{
	step();
	}
}

*/
/*
{
	static int mode;
	static uint32_t next = millis() + 20000;
	if(millis() >= next)
	{
		++mode;
		if(mode == LED_NUM_INTERVAL_MODE) mode = 0;

		led_set_interval_mode(mode);
		Serial.printf("mode:%d\r\n", mode);
		next = millis() + 20000;

	}
}
*/

	// check timer0 call is near the future
	uint32_t tick = ESP.getCycleCount();
	if(int32_t(next_tick - tick) < 0 ||
		 int32_t(next_tick - tick) >= 65536)
	{
		// here should be never called but
		// at this point I don't know why this situation occurs...
	 	next_tick = tick + timer_interval_values[current_interval_index].timer_interval;
	 	timer0_write(next_tick);
	 	Serial.printf_P(PSTR("====== Tick adjusted ====== next_tick:%u tick:%u diff:%d = "), next_tick, tick,
	 		next_tick - tick);
	 	if(int32_t(next_tick - tick) < 0)
	 		Serial.printf_P(PSTR("next tick is in past.\r\n"));
	 	else
	 		Serial.printf_P(PSTR("next tick is in far future.\r\n"));
	}

/*
	{
		uint32_t tick = ESP.getCycleCount();
		Serial.printf("%u %u %d  \r", next_tick, tick, next_tick - tick);
	}
*/
{
	static uint32_t next = millis() + 1000;
	if(millis() >= next)
	{
		Serial.printf("int_cnt:%d ovrn:%d last_ovrn_p:%d rssi=%d chan=%d mode=%d %04x\r\n",interrupt_count, (int)interrupt_overrun_count, last_overrun_phase, WiFi.RSSI(), WiFi.channel(), current_interval_index, button_read);

		interrupt_count = 0;
		interrupt_overrun_count = 0;
		next =millis() + 1000;
		int n = analogRead(0);
		Serial.printf("ambient=%d phy_mode=%d\r\n", n, WiFi.getPhyMode());


	}
}

}



