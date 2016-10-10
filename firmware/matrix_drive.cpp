#include <Arduino.h>

#define LED_COL_SER_GPIO 16
#define LED_COL_LATCH_GPIO 4
#define LED_HC595_LATCH_GPIO 15
#define LED_SER_CLOCK_GPIO 5

#define LED_MAX_ROW 24
#define LED_MAX_COL 64

void led_init_gpio()
{
	// enable 26MHz output on GPIO0 for LES1642 PWCLK
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

static inline void ICACHE_RAM_ATTR led_col_ser_out(bool b)
{
	digitalWrite(LED_COL_SER_GPIO, b?HIGH:LOW);
}
static inline void ICACHE_RAM_ATTR led_col_latch_out(bool b)
{
	digitalWrite(LED_COL_LATCH_GPIO, b?HIGH:LOW);
}
static inline void ICACHE_RAM_ATTR led_hc595_latch()
{
	digitalWrite(LED_HC595_LATCH_GPIO, HIGH);
	// do we need some wait here ?
	digitalWrite(LED_HC595_LATCH_GPIO, LOW);
}
static inline void ICACHE_RAM_ATTR led_pulse_ser_clock()
{
	digitalWrite(LED_SER_CLOCK_GPIO, HIGH);
	// do we need some wait here ?
	digitalWrite(LED_SER_CLOCK_GPIO, LOW);
}

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
	Set LED1642 brightness for one row
	@param buf 256-step brightness buffer start(left-most is the start) 
 */
static void ICACHE_RAM_ATTR led_set_brightness(const uint8_t * buf)
{
	// TODO: write stride explanation here


  static const uint16_t gamma_table[256] = {
    G64(0) G64(64) G64(128) G64(192)
  };


	// for each leds in an LED1642
	for(int led = 15; led >= 0; --led)
	{
		const uint8_t *b = buf + led + ((LED_MAX_COL / 16)-1)*16;
		// for each LED1642
		for(int i = 0; i < LED_MAX_COL / 16; ++i, b-=16)
		{
			// convert 256-step brightness to 4096-step brightness
			uint16_t w = gamma_table[*b];

			// for each control bit
			for(uint32_t bit = (1<<15); bit; bit >>= 1)
			{
				led_col_ser_out(bit & w);

				// setting LED1642 data latch needs 3 clock pulses
				// while the latch signal is asserted, except for last global latch
				// which needs 5 clock pulses.
				if(i == (LED_MAX_COL / 16) -1 /* last LED1642 */)
				{
					if(led == 0)
					{	
						// last latch; do global latch
						if(bit == (1<<4))
							led_col_latch_out(true);
					}
					else
					{
						// not last latch; do data latch
						if(bit == (1<<2))
							led_col_latch_out(true);
					}
				}

				led_pulse_ser_clock();
			}
			led_col_latch_out(false);
		}
	}
}


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

void test_led_sel_row()
{

	led_sel_row(LED_MAX_ROW);

	unsigned char buf[64];
	for(int i = 0; i < 64; i++)
		buf[i] = (i<<2) + (i>>4); // ABCDEF -> ABCDEF AB
	led_set_brightness(buf);

	static int n;
	led_sel_row(n);
	n++;
	if(n == LED_MAX_ROW) n =0;
	delay(1);
}

