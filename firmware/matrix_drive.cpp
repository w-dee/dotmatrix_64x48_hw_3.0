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

void led_init_gpio()
{
	// enable 26MHz output on GPIO0 for LED1642 PWCLK
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_CLK_OUT );

	// configure pins needed for communicating to the ICs
	pinMode(LED_COL_SER_GPIO, OUTPUT);
	pinMode(LED_COL_LATCH_GPIO, OUTPUT);
	pinMode(LED_HC595_LATCH_GPIO, OUTPUT);
	pinMode(LED_SER_CLOCK_GPIO, OUTPUT);
	digitalWrite(LED_COL_SER_GPIO, LOW);
	digitalWrite(LED_COL_LATCH_GPIO, LOW);
	digitalWrite(LED_HC595_LATCH_GPIO, LOW);
	digitalWrite(LED_SER_CLOCK_GPIO, LOW);

	// setup hardware SPI
	SPI.begin();
	SPI.setHwCs(false);
	SPI.setFrequency(25000000);
	SPI1U = SPIUMOSI | SPIUSSE | SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO
	SPI1C &= ~(SPICWBO | SPICRBO); // MSB first
#if 0
	// setup I2S clock output from GPIO15
	pinMode(15, FUNCTION_1); //I2SO_BCK (SCLK)

	I2S_CLK_ENABLE();
	I2SIC = 0x3F;
	I2SIE = 0;

	I2SC &= ~(I2SRST);
	I2SC |= I2SRST;
	I2SC &= ~(I2SRST);

	I2SFC &= ~(I2SDE | (I2STXFMM << I2STXFM) | (I2SRXFMM << I2SRXFM)); //Set RX/TX FIFO_MOD=0 and disable DMA (FIFO only)
	I2SCC &= ~((I2STXCMM << I2STXCM) | (I2SRXCMM << I2SRXCM)); //Set RX/TX CHAN_MOD=0

	// I2S clock freq is: 160000000.0 / 3 / 3 = 17777777.7777777... Hz
	uint32_t i2s_clock_div = 3 & I2SCDM;
	uint8_t i2s_bck_div = 3 & I2SBDM;

	//!trans master, !bits mod, rece slave mod, rece msb shift, right first, msb right
	I2SC &= ~(I2STSM | (I2SBMM << I2SBM) | (I2SBDM << I2SBD) | (I2SCDM << I2SCD));
	I2SC |= I2SRF | I2SMR | I2SRSM | I2SRMS | ((i2s_bck_div) << I2SBD) | ((i2s_clock_div) << I2SCD);

	// start I2S
	I2SC |= I2STXS; //Start transmission
#endif
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

#define led_col_ser_out(b) dW(LED_COL_SER_GPIO, (b))

#define led_col_latch_out(b) dW(LED_COL_LATCH_GPIO, (b))

#define led_hc595_latch() do { \
	dW(LED_HC595_LATCH_GPIO, HIGH); \
	GPO=GPO; \
	dW(LED_HC595_LATCH_GPIO, LOW); \
} while(0)


#define led_pulse_ser_clock() do { \
	dW(LED_SER_CLOCK_GPIO, HIGH); \
	dW(LED_SER_CLOCK_GPIO, LOW); \
} while(0)



/**
	Set LED1642 configuration register
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
void led_init_led1642()
{
	led_set_led1642_reg(7, 
		(1<<15) // 4096 brightness steps
		); // write to config reg
	led_set_led1642_reg(1,
		0xffff // all led on
		); // write to switch reg
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
 * Gamma curve function
 */
static constexpr uint32_t gamma_255_to_4095(int in)
{
  return bit_interleave((uint32_t) (pow((double)(in+20) / (255.0+20), 3.5) * 3800));  
}

#define G4(N) gamma_255_to_4095((N)), gamma_255_to_4095((N)+1), \
      gamma_255_to_4095((N)+2), gamma_255_to_4095((N)+3), 

#define G16(N) G4(N) G4((N)+4) G4((N)+8) G4((N)+12) 
#define G64(N) G16(N) G16((N)+16) G16((N)+32) G16((N)+48) 


/**
 * 8bit swap function
 */
 static constexpr uint32_t ICACHE_RAM_ATTR bit_8_swap(uint32_t v) {  return ((v & (0xff00ff00)) >> 8) | ((v & (0x00ff00ff)) << 8); }
/**
 * 16bit swap function
 */
static constexpr uint32_t ICACHE_RAM_ATTR bit_16_swap(uint32_t v) { return ((v & (0xffff0000)) >>16) | ((v & (0x0000ffff)) <<16); }

/**
 * byte reverse function
 */
static constexpr uint32_t ICACHE_RAM_ATTR byte_reverse(uint32_t v)
{
	return bit_16_swap(bit_8_swap(v));
}



/**
	Select one row and enable driver for the row 
	@param n   Row number to activate(0 .. LED_MAX_ROW-1).
				Specify -1 to unselect all rows.
*/
static void ICACHE_RAM_ATTR led_sel_row(int n)
{
	// wait for SPI transaction
	while(SPI1CMD & SPIBUSY) /**/ ;


	SPI1U &= ~ SPIUFWDUAL;
	SPI1C &= ~ (SPICFASTRD | SPICDOUT); // not use DIO

	led_spi_set_length(LED_MAX_ROW + (LED_MAX_COL/16) * 16);

	// row selection
	volatile uint32_t * fifoPtr = &SPI1W0;

	// bit 31 = row0, 30 = row1 ... and so on
	if(n ==-1)
		*(fifoPtr++) = 0;
	else
		*(fifoPtr++) = byte_reverse(1U<< (31-n));

	// three HC595s are connected after four LED1642,
	// so we need to shift-out the dummy data from LED1642 to HC595.
	// dummy data can be any pattern.
	constexpr int remain_len = ((LED_MAX_COL/16) * 16) / 32 + 1;
	for(int i = 0; i < remain_len; ++i) *(fifoPtr ++) = 0;

	// begin SPI transaction
	SPI1CMD |= SPIBUSY;
}

static void ICACHE_RAM_ATTR led_sel_row_commit()
{

	// wait for SPI transaction
	while(SPI1CMD & SPIBUSY) /**/ ;

	SPI1U |= SPIUFWDUAL;
	SPI1C |= SPICFASTRD | SPICDOUT; // use DIO

	// let HC595 latching the data
	led_hc595_latch();
}


/**
 * The frame buffer
 */
static unsigned char frame_buffer[LED_MAX_ROW][LED_MAX_COL] = {
	#include "op.inc"
};
static int current_row; // current scanning row
static int current_phase; // current phase; 4 stage phase.
	// current_phase
	// 0              : led (15 14 13 12)
	// 1:             : led (11 10  9  8)
	// 2:             : led ( 7  6  5  4)
	// 3:             : blank row, led ( 3  2  1  0), set row

static int interrupt_count;


static void ICACHE_RAM_ATTR led_set_brightness_one_row(int start_led)
{
	// TODO: write stride explanation here
	static_assert(LED_MAX_COL == 64, "sorry at this point LED_MAX_COL is not flexible");
	const uint8_t *buf = frame_buffer[current_row];

	static const uint32_t gamma_table[256] = {
	G64(0) G64(64) G64(128) G64(192)
	}; // this table must be accessible from interrupt routine;
	// do not place in FLASH !!


	volatile uint32_t * fifoPtr = &SPI1W0;

	while(SPI1CMD & SPIBUSY) /**/ ;
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
			uint32_t w = gamma_table[*b];

			// add latch pattern
			constexpr uint32_t global_latch_pattern = 0b00000000000000000000001010101010;
			constexpr uint32_t data_latch_pattern   = 0b00000000000000000000000000101010;
			if(do_global_latch) w |= global_latch_pattern;
			if(do_data_latch)   w |= data_latch_pattern;

			// push to fifo
			*(fifoPtr++) = byte_reverse(w);
			// note:
			// every even bit of fifo goes to COLSER line (MOSI),
			// and every odd bit of fifo goes to COLLATCH line (MISO).
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
		led_set_brightness_one_row(12);
		break;
	case 1:
		led_set_brightness_one_row(8);
		break;
	case 2:
		led_set_brightness_one_row(4);
		led_sel_row(-1);
		break;
	case 3:
		led_sel_row_commit();

		led_set_brightness_one_row(0);

		led_sel_row(current_row);
		// step to next row
		++ current_row;
		if(current_row >= LED_MAX_ROW) current_row = 0;

		led_sel_row_commit();
		break;
	}

	// step to next phase
	++current_phase;
	if(current_phase == 4) current_phase = 0;
}



/**
 * timer counter value
 */
static constexpr int32_t timer_interval = 9216; // 80000000.0/9216 = 8680.55555... Hz (90.4Hz vsync)
static uint32_t next_tick;

/**
 * Timer interrupt handler
 */
static void ICACHE_RAM_ATTR timer_handler()
{
	++interrupt_count;

	next_tick += timer_interval;
	timer0_write(next_tick);

	// set brightness for one row
	led_set_brightness();
}

/**
 * Initialize timer
 */
void led_init_timer()
{
/*
	timer1_isr_init();
	timer1_attachInterrupt(timer_handler);
	static_assert(timer_interval  >= 0 && timer_interval  <= 8388607, "Timer interval out of range");
	timer1_write(timer_interval); 
	timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
*/

	timer0_isr_init();
	timer0_attachInterrupt(timer_handler);
	timer0_write(next_tick = ESP.getCycleCount() + timer_interval);

}

void test_led_sel_row()
{

	static uint32_t next = millis() + 1000;
	if(millis() >= next)
	{
		Serial.printf("%d \r\n", interrupt_count);
		interrupt_count = 0;
		next =millis() + 1000;
	}

}

