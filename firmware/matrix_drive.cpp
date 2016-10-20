#include <Arduino.h>
#include <SPI.h>
#include "eagle_soc.h"

#define LED_COL_SER_GPIO 13 // 13=MOSI
#define LED_COL_LATCH_GPIO 12 // 12=MISO, but in DIO mode, it acts as second line of MOSI
#define LED_HC595_LATCH_GPIO 15
#define LED_SER_CLOCK_GPIO 14 // 14=SCK

#define LED_MAX_ROW 24
#define LED_MAX_COL 64

// inline fast version of digitalWrite
#define dW(pin, val)  do { if(pin < 16){ \
    if(val) GPOS = (1 << pin); \
    else GPOC = (1 << pin); \
  } else if(pin == 16){ \
    GP16O = !!val; \
  } }while(0)

extern "C" {
	void rom_i2c_writeReg_Mask(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
}


#define led_col_ser_out(b) dW(LED_COL_SER_GPIO, (b)) // for non-SPI mode

#define led_col_latch_out(b) dW(LED_COL_LATCH_GPIO, (b)) // for non-SPI mode

#define led_pulse_ser_clock() /* for non-SPI mode */ do { \
	dW(LED_SER_CLOCK_GPIO, HIGH); \
	dW(LED_SER_CLOCK_GPIO, LOW); \
} while(0)

#define led_hc595_latch_out(b) /* HC595 latch is not controlled by hardware anyway */ \
	dW(LED_HC595_LATCH_GPIO, (b))


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
	digitalWrite(LED_COL_SER_GPIO, LOW);
	digitalWrite(LED_COL_LATCH_GPIO, LOW);
	digitalWrite(LED_HC595_LATCH_GPIO, LOW);
	digitalWrite(LED_SER_CLOCK_GPIO, LOW);
}

/**
	Set LED1642 configuration register.
	Note this function uses GPIO (not SPI hardware) to initialize
	LED1642, call this function before calling led_init_spi_and_ledclock()
 */
static void led_set_led1642_reg(int reg_no, uint16_t w)
{
	// for each LED1642
	for(int i = 0; i < LED_MAX_COL / 16; i++)
	{
		// for each control bit
		for(uint32_t bit = (1<<15); bit; bit >>= 1)
		{
			led_col_ser_out(bit & w);

			// setting LED1642 config reg needs seven clock pulse
			// while the latch signal is asserted.
			// other register may use defferent pulse count.
			if(i == (LED_MAX_COL / 16) -1 /* last LED1642 */)
			{
				if(bit == (1<<(reg_no-1)))
					led_col_latch_out(true);
			}

			led_pulse_ser_clock();
		}

		led_col_latch_out(false);
	}
}

/**
	Initialize LED1642
 */
static void led_init_led1642()
{
	led_set_led1642_reg(7, 
		(1<<15) // 4096 brightness steps
		); // write to config reg
	led_set_led1642_reg(1,
		0xffff // all led on
		); // write to switch reg
}


/**
 * Initialize SPI hardware and clock generator for LED1642
 */
static void led_init_spi_and_ledclock()
{
	// enable 26MHz output on GPIO0 for LED1642 PWCLK
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_CLK_OUT );

	// setup hardware SPI
	SPI.begin();
	SPI.setHwCs(false);
	SPI.setFrequency(10000000);
	SPI1U = SPIUMOSI | SPIUSSE | SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO
	SPI1C &= ~(SPICWBO | SPICRBO); // MSB first

	// setup I2S clock output from GPIO15
//	pinMode(15, FUNCTION_1); //I2SO_BCK (SCLK)
	pinMode(3, FUNCTION_1); //I2SO_DATA (SDIN)

	I2S_CLK_ENABLE();
	I2SIC = 0x3F;
	I2SIE = 0;

	I2SC &= ~(I2SRST);
	I2SC |= I2SRST;
	I2SC &= ~(I2SRST);

	I2SFC &= ~(I2SDE | (I2STXFMM << I2STXFM) | (I2SRXFMM << I2SRXFM)); //Set RX/TX FIFO_MOD=0 and disable DMA (FIFO only)
	I2SCC &= ~((I2STXCMM << I2STXCM) | (I2SRXCMM << I2SRXCM)); //Set RX/TX CHAN_MOD=0

	// I2S clock freq is: 160000000.0 / 2 / 1 = 80MHz
	uint32_t i2s_clock_div = 2 & I2SCDM;
	uint8_t i2s_bck_div = 1 & I2SBDM;

	//!trans master, !bits mod, rece slave mod, rece msb shift, right first, msb right
	I2SC &= ~(I2STSM | (I2SBMM << I2SBM) | (I2SBDM << I2SBD) | (I2SCDM << I2SCD));
	I2SC |= I2SRF | I2SMR | I2SRSM | I2SRMS | ((i2s_bck_div) << I2SBD) | ((i2s_clock_div) << I2SCD);

	// write FIFO pattern
	// 8 rising edge in 32bit, and the clock is 80MHz, thus the output is 20MHz.
	// we add an intended jitter to the pattern, to reduce EMI (spread spectrum).
	// the I2S hardware in ESP8266 will repeat this pattern.
	I2STXF = 
//			0b11100011010010100000100001110110; // -9.261996db compared to 0b11001100...
			0b11100110010010010000100001100100; 

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


/**
 * bit interleave function
 */
static constexpr uint32_t bit_interleave(uint16_t v)
{
	// Every bit in v goes to even bit of the result.
	// This is optimization for SPI DIO, which send every even bit
	// to the MOSI line, and every odd bit to the MISO line.
	return
		((uint32_t)(v & 0b0000000000000001) <<  0) |
		((uint32_t)(v & 0b0000000000000010) <<  1) |
		((uint32_t)(v & 0b0000000000000100) <<  2) |
		((uint32_t)(v & 0b0000000000001000) <<  3) |
		((uint32_t)(v & 0b0000000000010000) <<  4) |
		((uint32_t)(v & 0b0000000000100000) <<  5) |
		((uint32_t)(v & 0b0000000001000000) <<  6) |
		((uint32_t)(v & 0b0000000010000000) <<  7) |
		((uint32_t)(v & 0b0000000100000000) <<  8) |
		((uint32_t)(v & 0b0000001000000000) <<  9) |
		((uint32_t)(v & 0b0000010000000000) << 10) |
		((uint32_t)(v & 0b0000100000000000) << 11) |
		((uint32_t)(v & 0b0001000000000000) << 12) |
		((uint32_t)(v & 0b0010000000000000) << 13) |
		((uint32_t)(v & 0b0100000000000000) << 14) |
		((uint32_t)(v & 0b1000000000000000) << 15) |
		0;
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
static void ICACHE_RAM_ATTR led_sel_row(int n)
{
	static constexpr uint32_t bit_sel_pattern[LED_MAX_ROW + 1] = {
		0, // for -1
#define BP(N) byte_reverse(1U << (31-(N)))
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
	// dummy data can be any pattern.
	constexpr int remain_len = ((LED_MAX_COL/16) * 16 - 1) / 32 + 1;
	for(int i = 0; i < remain_len; ++i) *(fifoPtr ++) = 0;

	// begin SPI transaction
	SPI1CMD |= SPIBUSY;
}

static void ICACHE_RAM_ATTR led_sel_row_commit()
{

	// wait for SPI transaction
	while(SPI1CMD & SPIBUSY) /**/ ;

	// let HC595 latching the data
	led_hc595_latch_out(true);

	// restore SPI DIO mode
	SPI1U |= SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO

	// reset HC595 latching line
	led_hc595_latch_out(false);

}


/**
 * The frame buffer
 */
static unsigned char frame_buffer[LED_MAX_ROW][LED_MAX_COL] = {
	#include "op.inc"
};

static int current_row; //!< current scanning row
static int current_phase; //!< current phase; 6 stage phase.

static constexpr int max_phase = 6; //!< phase count
	// current_phase
	// 0:             : row commit, led (15 14 13 12)
	// 1:             : led (11 10  9  8)
	// 2:             : led ( 7  6  5  4)
	// 3:             : blank row
	// 4:             : row commit, led ( 3  2  1  0)
	// 5:             : set row

	// the interrupt timing of each phase is like this:
	// we try to minimize phase 3, 4, 5 duration, because
	// in these phase LED are off, thus leads to dimmed display. 
	// 0----------1---------2------------3---4-----5---

	// each phase is distributed into small interrupts,
	// to minimize interrupt latency.

/**
 * timer counter value
 */
static constexpr int32_t timer_interval = 32768; //!< timer hsync interval in 80MHz cycle
static constexpr int32_t timer_duration_phase[max_phase] = //!< timer duration of each phase
	{ 
		8368,
		8000,
		8400,
		2000,
		4000,
		2000
	};
static_assert(
	timer_duration_phase[0] + timer_duration_phase[1] +
	timer_duration_phase[2] + timer_duration_phase[3] +
	timer_duration_phase[4] + timer_duration_phase[5]  == timer_interval, "timer_interval sum mismatch");

static uint32_t next_tick;

/**
 * set one line brightness
 */
static void ICACHE_RAM_ATTR led_set_brightness_one_row(int start_led)
{
	// TODO: write stride explanation here
	static_assert(LED_MAX_COL == 64, "sorry at this point LED_MAX_COL is not flexible");
	const uint8_t *buf = frame_buffer[current_row];



	volatile uint32_t * fifoPtr = &SPI1W0;

	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
	led_spi_set_length(32*16);

	for(int led_i = 3 ; led_i >= 0; --led_i)
	{
		int current_led = start_led + led_i;

		const uint8_t *b = buf + current_led + ((LED_MAX_COL / 16)-1)*16;

		// for each LED1642
		for(int i = 0; i < LED_MAX_COL / 16; ++i, b-=16)
		{
			bool do_global_latch = i == (LED_MAX_COL / 16) -1  && current_led == 0;
			bool do_data_latch   = i == (LED_MAX_COL / 16) -1  && current_led != 0;

			// convert 256-step brightness to 4096-step brightness
			uint32_t w = gamma_table[*b]; // note this table is already byte-reversed

			// add latch pattern
			constexpr uint32_t global_latch_pattern =
				byte_reverse(0b00000000000000000000001010101010);
			constexpr uint32_t data_latch_pattern   =
				byte_reverse(0b00000000000000000000000000101010);
			if(do_global_latch) w |= global_latch_pattern;
			if(do_data_latch)   w |= data_latch_pattern;

			// push to fifo
			*(fifoPtr++) = w;
			// note:
			// every even bit of fifo goes to COLSER line (MOSI),
			// and every odd bit of fifo goes to COLLATCH line (MISO).
			// data written to fifo must be in big-endian because the hardware
			// always pull a byte from the first byte (not word) of the fifo.
		}
	}


	// begin SPI transaction
	SPI1CMD |= SPIBUSY;


}

/**
	Set LED1642 brightness timer handler

 */
static void ICACHE_RAM_ATTR led_set_brightness()
{
	switch(current_phase)
	{
	case 0:
		led_sel_row_commit();
		led_set_brightness_one_row(12);
		break;

	case 1:
		led_set_brightness_one_row(8);
		break;

	case 2:
		led_set_brightness_one_row(4);
		break;

	case 3:
		led_sel_row(-1);
		break;

	case 4:
		led_sel_row_commit();
		led_set_brightness_one_row(0);
		break;

	case 5:
		led_sel_row(current_row);
		// step to next row
		++ current_row;
		if(current_row >= LED_MAX_ROW) current_row = 0;

		break;
	}

	// step to next phase
	++current_phase;
	if(current_phase == max_phase) current_phase = 0;
}


/**
 * interrupt counter
 */
static uint32_t interrupt_count;

/**
 * interrupt overrun counter
 */
static uint32_t interrupt_overrun_count;

/**
 * whether overrun has occured
 */
static bool interrupt_overrun;

/**
 * expected interrupt delay from the end of the handler to the next interrupt enable point
 */
static constexpr uint32_t interrupt_delay = 50;

/**
 * maximum interrupt overrun count allowed before resetting the tick
 */
static constexpr uint32_t max_overrun_count = 20;


/**
 * Timer interrupt handler
 */
static void ICACHE_RAM_ATTR timer_handler()
{
	++interrupt_count;

	uint32_t phase_duration = timer_duration_phase[current_phase];

	next_tick += phase_duration;
	timer0_write(next_tick);

	// set brightness for one row
	led_set_brightness();

	if((int32_t)(next_tick - interrupt_delay - ESP.getCycleCount()) < 0)
	{
		// timer next tick will be in past ...
		++ interrupt_overrun_count;
		interrupt_overrun = true;
		uint32_t temp_next_tick = ESP.getCycleCount() +interrupt_delay; // adjust timing
		timer0_write(temp_next_tick);
		if(interrupt_overrun_count >= max_overrun_count)
		{
			// reset next_tick because the interrupt is too heavy, so 
			// it seems no way to recover timer delay
			next_tick = temp_next_tick;
		}
	}
	else
	{
		// reset overrun count
		interrupt_overrun_count = 0;
	}
}

/**
 * Initialize timer
 */
static void led_init_timer()
{

	timer0_isr_init();
	timer0_attachInterrupt(timer_handler);
	timer0_write(next_tick = ESP.getCycleCount() + timer_interval);

}

/**
 * Initialize LED matrix driver
 */
void led_init()
{
	led_init_gpio();
	led_init_led1642();
	led_init_spi_and_ledclock();
	led_init_timer();
}

void test_led_sel_row()
{
/*
	while(current_phase != 1) led_set_brightness();
delay(100);
	led_set_brightness();
*/

	static uint32_t next = millis() + 1000;
	if(millis() >= next)
	{
		Serial.printf("%d %d\r\n", interrupt_count, interrupt_overrun?1:0);
		interrupt_count = 0;
		interrupt_overrun = false;
		next =millis() + 1000;
	}

}



