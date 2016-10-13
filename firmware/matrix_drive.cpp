#include <Arduino.h>
#include "eagle_soc.h"

#define LED_COL_SER_GPIO 13
#define LED_COL_LATCH_GPIO 4
#define LED_HC595_LATCH_GPIO 15
#define LED_SER_CLOCK_GPIO 5

#define LED_MAX_ROW 24
#define LED_MAX_COL 64

// inline fast version of digitalWrite
#define dW(pin, val)  do { if(pin < 16){ \
    if(val) GPOS = (1 << pin); \
    else GPOC = (1 << pin); \
  } else if(pin == 16){ \
    GP16O = !!val; \
  } }while(0)



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
 * Gamma curve function
 */
static constexpr uint16_t gamma_255_to_4095(int in)
{
  return (uint16_t) (pow((double)(in+20) / (255.0+20), 3.5) * 3800);  
}

#define G4(N) gamma_255_to_4095((N)), gamma_255_to_4095((N)+1), \
      gamma_255_to_4095((N)+2), gamma_255_to_4095((N)+3), 

#define G16(N) G4(N) G4((N)+4) G4((N)+8) G4((N)+12) 
#define G64(N) G16(N) G16((N)+16) G16((N)+32) G16((N)+48) 



/**
	Select one row and enable driver for the row 
	@param n   Row number to activate(0 .. LED_MAX_ROW-1).
				Specify LED_MAX_ROW to unselect all rows.
*/
static void ICACHE_RAM_ATTR led_sel_row(int n)
{
	// shift-out row selection
	for(int i = 0; i < LED_MAX_ROW; i++)
	{
		led_col_ser_out(i == n);
		led_pulse_ser_clock();
	}

	// three HC595s are connected after four LED1642,
	// so we need to shift-out the data from LED1642 to HC595.
	for(int i = 0; i < (LED_MAX_COL/16) * 16; i++) // 16bit shift-register per one LED1642
	{
		led_pulse_ser_clock();
	}

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

static int interrupt_count;


/**
	Set LED1642 brightness for one row
	@param buf 256-step brightness buffer start(left-most is the start) 
 */
static void ICACHE_RAM_ATTR led_set_brightness()
{
	// TODO: write stride explanation here
	const uint8_t *buf = frame_buffer[current_row];

  static const uint16_t gamma_table[256] = {
    G64(0) G64(64) G64(128) G64(192)
  }; // this table must be accessible from interrupt routine;
   // do not place in FLASH !!

	// GPIO value cache
	uint32_t gpo = GPO;

	// for each leds in an LED1642
	for(int led = 15; led >= 0; --led)
	{
		const uint8_t *b = buf + led + ((LED_MAX_COL / 16)-1)*16;

		// for each LED1642
		for(int i = 0; i < LED_MAX_COL / 16; ++i, b-=16)
		{
			bool do_global_latch = i == (LED_MAX_COL / 16) -1  && led == 0;
			bool do_data_latch = i == (LED_MAX_COL / 16) -1  && led != 0;

			// when issuing global latch, blank current row now
			if(do_global_latch)
			{
				// once meke all LEDs blank
				led_sel_row(LED_MAX_ROW);
				PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_GPIO0 ); // disable 26MHz output ...
			}

			// convert 256-step brightness to 4096-step brightness
			uint32_t w = gamma_table[*b];

			// for each control bit
#define SET_BIT(N) do { \
			gpo &= ~  ( (1<<LED_COL_SER_GPIO) | (1<<LED_SER_CLOCK_GPIO)); \
			if(w & (1<<(N))) gpo |= (1<<LED_COL_SER_GPIO); \
			GPO = gpo; \
			gpo |= (1<<LED_SER_CLOCK_GPIO); \
			GPO = gpo; } while(0)

			SET_BIT(15);
			SET_BIT(14);
			SET_BIT(13);
			SET_BIT(12);
			SET_BIT(11);
			SET_BIT(10);
			SET_BIT(9);
			SET_BIT(8);
			SET_BIT(7);
			SET_BIT(6);
			SET_BIT(5);
			if(do_global_latch) {gpo |= (1<<LED_COL_LATCH_GPIO);  } // do global latch
			SET_BIT(4);
			SET_BIT(3);
			if(do_data_latch  ) {gpo |= (1<<LED_COL_LATCH_GPIO);  } // do data latch
			SET_BIT(2);
			SET_BIT(1);
			SET_BIT(0);

			gpo &= ~ ((1<<LED_COL_LATCH_GPIO) | (1<<LED_SER_CLOCK_GPIO));
			GPO = gpo;
		}
	}

	// enable 26MHz output on GPIO0 for LED1642 PWCLK
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO0_U, FUNC_CLK_OUT );

	// activate row driver
	led_sel_row(current_row);

	// step to next row
	++ current_row;
	if(current_row >= LED_MAX_ROW) current_row = 0;


}

/**
 * Timer interrupt handler
 */
static void ICACHE_RAM_ATTR timer_handler()
{
	++interrupt_count;

	// set brightness for one row
//	led_set_brightness();
}

/**
 * Initialize timer
 */
void led_init_timer()
{

	timer1_isr_init();
	timer1_attachInterrupt(timer_handler);
	const uint32_t interval = 50412;//80000000L / (80*LED_MAX_ROW)+555; // 80Hz refresh
	static_assert(interval >= 0 && interval <= 8388607, "Timer interval out of range");
	timer1_write(interval); 
	timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
}

// 507ms / 200count
// 443ms / 200count 
// 296ms / 200count
// 276ms
// 116ms
// 70ms
// 83ms
// 78

void test_led_sel_row()
{
	uint32_t start = millis();
	interrupt_count = 0;
	delay(1);
	uint32_t end = millis();
	Serial.printf("%d ms %d count\r\n", end - start, interrupt_count);
}

