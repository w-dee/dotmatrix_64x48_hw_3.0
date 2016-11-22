#include <Arduino.h>
#include "ir_control.h"


#define IR_SENS_GPIO 4




static constexpr size_t buffer_size = 1024; //!< buffer size
static uint8_t buffer_array[buffer_size]; //!< buffer array
static size_t buffer_index; //!< array read/write index

/**
 * reset index pointer
 */
static void buffer_reset() { buffer_index = 0; }

/**
 * push value. value is stored in encoded format.
 * returns whether the value is successfully stored.
 */
static bool ICACHE_RAM_ATTR buffer_push(uint16_t v)
{
	// encode format:
	// 0 ... 254 : store v
	// 255 ... 65535: store 255, store hi(v), low(v)
	if(v <= 254)
	{
		if(buffer_index >= buffer_size) return false;
		buffer_array[buffer_index++] = (uint8_t)v;
	}
	else
	{
		if(buffer_index + 2 >= buffer_size) return false;
		size_t bi = buffer_index;
		buffer_array[bi++] = (uint8_t)255;
		buffer_array[bi++] = (uint8_t)(v >> 8);
		buffer_array[bi++] = (uint8_t)(v & 0xff);
		buffer_index = bi;
	}
	return true;
}

/*
 * read value from current position.
 * returns -1 if there is no value left.
 */
static int32_t ICACHE_RAM_ATTR buffer_read()
{
	if(buffer_index >= buffer_size) return -1;
	uint8_t v = buffer_array[buffer_index ++];
	if(v == 255)
	{
		// two-byte encode
		if(buffer_index +1 >= buffer_size) return -1;
		size_t bi = buffer_index;
		uint8_t high = buffer_array[bi++];
		uint8_t low  = buffer_array[bi++];
		buffer_index = bi;
		return (high << 8) + low;
	}
	
	// 1-byte encode
	return v;
}



static ir_status_t ir_status; //!< the ir state

/**
 * the ir record status enum
 */
enum ir_record_status_t
{
	IRR_FIRST, //!< waiting for the first pulse
	IRR_POS, //!< recording positive
	IRR_NEG //!< recording negative
};

static ir_record_status_t ir_record_status; //!< the ir record status

static int ir_record_running_count; //!< running count for the record

/**
 * timer record handler
 */
static void ICACHE_RAM_ATTR timer_record_handler()
{
	bool b = !digitalRead(IR_SENS_GPIO); // negative logic
	ir_record_status_t irs = ir_record_status;
	if(irs == IRR_FIRST)
	{
		// waiting for first pulse
		if(b)
		{
			irs = IRR_POS;
			ir_record_running_count = 1;
		}
	}
	else
	{
		if(irs == IRR_POS && !b ||
			irs == IRR_NEG && b)
		{
			// pulse polarity changed
			if(irs == IRR_POS)
				irs = IRR_NEG;
			else
				irs = IRR_POS;

			if(!buffer_push(ir_record_running_count))
			{
				// buffer overflow
				ir_status = IR_FAILED;
			}
			ir_record_running_count = 1;
		}
		else
		{
			++ ir_record_running_count;
			if(ir_record_running_count > 10000) // timeout
			{
				if(!b)
				{
					if(!buffer_push(0)) // end mark
						ir_status = IR_FAILED; // buffer overflow
					else
						ir_status = IR_SUCCESS;
				}
				else
				{
					ir_status = IR_FAILED; // timeout
				}
			}
		}
	}
	ir_record_status = irs;
}


/**
 * reset GPIO2 and set low
 */
static void ICACHE_RAM_ATTR ir_stop_gpio_output()
{
	constexpr int pin = 2;
	GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
	GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
	GPES = (1 << pin); //Enable
}

/**
 * timer replay handler
 */
static void ICACHE_RAM_ATTR timer_replay_handler()
{
	ir_record_status_t irs = ir_record_status;
	if(--ir_record_running_count == 0)
	{
		// polarity change
		if(irs == IRR_POS)
			irs = IRR_NEG;
		else
			irs = IRR_POS;
		ir_record_running_count = buffer_read();
		if(ir_record_running_count == -1)
			irs = IRR_NEG, ir_status = IR_FAILED; // premature end
		if(ir_record_running_count == 0)
			irs = IRR_NEG, ir_status = IR_SUCCESS; // successful end
	}

	if(irs == IRR_POS)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_UART1_TXD_BK);
		USF(1) = 0x5b; // write 38kHz pulse
		USF(1) = 0x5b; // write 38kHz pulse
		// Although timer_replay_handler is called about once per
		// sending one character, we push the extra characters.
		// The important thing here is that the pulse is continuously sent,
		// there is no care about overflow.
	}
	else
	{
		// irs is not IRR_POS
		// force stop sending 0x5b
		ir_stop_gpio_output();
	}
	ir_record_status = irs;
}


/**
 * start timer
 */
static void start_timer()
{
	timer1_enable(TIM_DIV1, TIM_EDGE, TIM_LOOP);
}

/**
 * stop timer
 */
static void ICACHE_RAM_ATTR stop_timer()
{
	timer1_disable();
}

/**
 * timer handler for IR control
 */
static void ICACHE_RAM_ATTR timer_handler()
{
	switch(ir_status)
	{
	case IR_NONE:
		break;

	case IR_RECORD:
		timer_record_handler();
		break;

	case IR_REPLAY:
		timer_replay_handler();
		break;

	case IR_SUCCESS:
	case IR_FAILED:
		stop_timer();
		break;
	}
}


/**
 * Initialize timer
 */
static void ir_init_timer()
{
	timer1_isr_init();
	timer1_attachInterrupt(timer_handler);
	timer1_write(80000000/10000); // 10kHz interval
}



//---

/**
 * initialize IR controller facilities
 */
void ir_init()
{
	pinMode(2, OUTPUT);
	digitalWrite(2, LOW);
	Serial1.begin(113960, SERIAL_7N1, SERIAL_TX_ONLY, 2);
	U1C0 |= (1<<UCTXI); // invert TX
	ir_stop_gpio_output();


	/*
	note:
		sending continuously 0x5b makes following waveform:
		L (start bit) + 
		HHLHHLH (0x5b) +
		H (stop bit)
		 --> LHHLHHLHHLHHLHHLHHLHHLHHLHHLHHLHHLHH...
		inverting this makes it as:
		 --> HLLHLLHLLHLLHLLHLLHLLHLLHLLHLLHLLHLL...

		the baud rate is 113960.11, so the output frequency is:
		113960.11 / 3 = 37.987 kHz
	*/
	pinMode(IR_SENS_GPIO, INPUT);

	ir_init_timer();
}


/**
 * start recodring
 */
void ir_record()
{
	stop_timer();
	buffer_reset();
	ir_status = IR_RECORD;
	ir_record_status = IRR_FIRST;
	ir_record_running_count = 0;
	start_timer();
}

/**
 * start replaying
 */
void ir_replay()
{
	stop_timer();
	buffer_reset();
	ir_status = IR_REPLAY;
	ir_record_status = IRR_POS;
	ir_record_running_count = buffer_read();
	if(ir_record_running_count != -1)
		start_timer();
	else
		ir_status = IR_FAILED;
}

/**
 * get current status
 */
ir_status_t ir_get_status() { return ir_status; }

