#include <Arduino.h>
#include <SPI.h>
#include "eagle_soc.h"
#include <ESP8266WiFi.h>

#define LED_COL_SER_GPIO 13 // 13=MOSI
#define LED_COL_LATCH_GPIO 12 // 12=MISO, but in DIO mode, it acts as second line of MOSI
#define LED_HC595_LATCH_GPIO 15
#define LED_SER_CLOCK_GPIO 14 // 14=SCK

#define BUTTON_SENS_GPIO 16

#define LED_MAX_ROW 24
#define LED_MAX_COL 64

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
 * Initialize SPI hardware and clock generator for LED1642
 */
static void led_init_spi_and_ledclock()
{
	// setup hardware SPI
	SPI.begin();
	SPI.setHwCs(false);
	SPI.setFrequency((int)11451419.19);
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

	// I2S clock freq is: 160000000.0 / 2 / 1 = 80MHz
	uint32_t i2s_clock_div = 2 & I2SCDM;
	uint8_t i2s_bck_div = 1 & I2SBDM;

	//!trans master, !bits mod, rece slave mod, rece msb shift, right first, msb right
	I2SC &= ~(I2STSM | (I2SBMM << I2SBM) | (I2SBDM << I2SBD) | (I2SCDM << I2SCD));
	I2SC |= I2SRF | I2SMR | I2SRSM | I2SRMS | ((i2s_bck_div) << I2SBD) | ((i2s_clock_div) << I2SCD);

	// write FIFO pattern
	// 8 rising edge in 32bit, and the clock is 80MHz, thus the output is 20MHz.
	// we add an intended jitter (spread spectrum) to the pattern, to reduce EMI.
	// the I2S hardware in ESP8266 will repeat this pattern.
	I2STXF = 
//			0b11100011010010100000100001110110; // ~~ -6db peak reduction compared to 0b11001100...
			0b11100110010010010000100001100100; // more L than above; for pull-uped output
//			0b11001100110011001100110011001100; // no SS pattern

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

uint32_t button_read; //!< read value of each button

static void ICACHE_RAM_ATTR button_read_gpio()
{
	// update button GPIO value
	static_assert(BUTTON_SENS_GPIO == 16, "Check the implementation");
	uint32_t value = button_read;
	uint32_t bit = (1U << current_row);
	value &= ~bit;
	if(GP16I & 0x01) value |= bit;
	button_read = value;
}

static constexpr int max_phase = 7; //!< phase count
	// current_phase
	// 0:             : row commit, set LED1642 control word
	// 1:             : read buttons, increment row, led (15 14 13 12)
	// 2:             : led (11 10  9  8)
	// 3:             : led ( 7  6  5  4)
	// 4:             : blank row
	// 5:             : row commit, led ( 3  2  1  0)
	// 6:             : set row

	// the interrupt timing of each phase is like this:
	// we try to minimize phase 5->6 and 6->0 duration, because
	// in these phase LED are off, long interrupt inverval
	//  lead to dimmed display. 
	// 0--1--------2---------3-----------4---5-----6---

	// each phase is distributed into small interrupts,
	// to minimize interrupt latency.

/**
 * timer counter value
 */
static constexpr int32_t timer_interval = 32768; //!< timer hsync interval in 80MHz cycle
static constexpr int32_t timer_duration_phase[max_phase] = //!< timer duration of each phase
	{
		3000,
		6368,
		8000,
		7400,
		2000,
		4000,
		2000
	};
static constexpr int phase_sum(int index) { return index == -1 ? 0 : timer_duration_phase[index] + phase_sum(index-1); }
static_assert(phase_sum(max_phase-1) == timer_interval, "timer_interval sum mismatch");

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

	constexpr uint32_t global_latch_pattern =
		byte_reverse(0b00000000000000000000001010101010);
	constexpr uint32_t data_latch_pattern   =
		byte_reverse(0b00000000000000000000000000101010);

	// fill fifo with buffer, with
	// converting gamma and adding latch pattern
#define WR(N, L) do { \
	uint32_t w = gamma_table[buf[start_led + (N)]]; w += L; \
	*(fifoPtr++) = w; } while(0)
#define PL(I,C,L) WR((I) +  ((LED_MAX_COL / 16)-1)*16 + (C) * -16, L )
	PL(3, 0, 0);
	PL(3, 1, 0);
	PL(3, 2, 0);
	PL(3, 3, data_latch_pattern);
	PL(2, 0, 0);
	PL(2, 1, 0);
	PL(2, 2, 0);
	PL(2, 3, data_latch_pattern);
	PL(1, 0, 0);
	PL(1, 1, 0);
	PL(1, 2, 0);
	PL(1, 3, data_latch_pattern);
	PL(0, 0, 0);
	PL(0, 1, 0);
	PL(0, 2, 0);
	PL(0, 3, start_led == 0 ? global_latch_pattern : data_latch_pattern);


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

	led_spi_set_length(32*4);

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
	led1642_configration_reg = byte_reverse(bit_interleave(1<<15)); // 4096 brightness steps
	led_set_led1642_reg(
		config_reg_pattern_from_num(7), 
		led1642_configration_reg
		); // write to config reg
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
	led_set_led1642_reg(
		config_reg_pattern_from_num(1),
		byte_reverse(bit_interleave(0xffff)) // all led on
		); // write to switch reg
	while(SPI1CMD & SPIBUSY) /**/ ; // wait for previous SPI transaction
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

		led_set_brightness_one_row(12);
		break;

	case 2:
		led_set_brightness_one_row(8);
		break;

	case 3:
		led_set_brightness_one_row(4);
		break;

	case 4:
		led_sel_row(-1);
		break;

	case 5:
		led_sel_row_commit();
		led_set_brightness_one_row(0);
		break;

	case 6:
		led_sel_row(current_row);

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

static int last_overrun_phase;

/**
 * Timer interrupt handler
 */
static void ICACHE_RAM_ATTR timer_handler()
{
	++interrupt_count;

	int phase = current_phase;
	uint32_t phase_duration = timer_duration_phase[phase];

	next_tick += phase_duration;
	timer0_write(next_tick);

	// set brightness for one row
	led_set_brightness();

	// interrupt delay overload check
	if((int32_t)(next_tick - interrupt_delay - ESP.getCycleCount()) < 0)
	{
		// timer next tick will be in past ...
		++ interrupt_overrun_count;
		interrupt_overrun = true;
		last_overrun_phase = phase;
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
	led_init_spi_and_ledclock();
	led_init_led1642();
	led_init_timer();
}




#define W 80
#define H 32

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




void test_led_sel_row()
{
/*
	while(current_phase != 1) led_set_brightness();
delay(100);
	led_set_brightness();
*/
/*
	step();

	for(int y = 0; y < 24; y++)
	{
		memcpy(frame_buffer[y], buffer[y] + 10, 64);
	}
	delay(10);
*/
	static uint32_t next = millis() + 1000;
	if(millis() >= next)
	{
		Serial.printf("%d %d %d %d %04x\r\n", interrupt_count, interrupt_overrun?1:0, last_overrun_phase, WiFi.RSSI(), button_read);

		interrupt_count = 0;
		interrupt_overrun = false;
		next =millis() + 1000;
	}
}



