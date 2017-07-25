#include <Arduino.h>

#include "ir_control.h"
#include "Esp.h"


/**
 * Display panic state.
 * Blinks the Status LED which indicates the panic code,
 * and send message on Serial line.
 * this function is intended to be used as
 * a pre-LED matrix initialization error indication.
 */
void __attribute__((noreturn)) do_panic(int code, const String & message)
{
	// configure GPIO
	ir_stop(); // status LED shares the pin with IR output

	constexpr int pin = 2;

	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);

	// interrupts are still enabled ...

	// infinite loop
	for(;;)
	{
		Serial.printf_P(PSTR("PANIC: "));
		Serial.println(message.c_str());
		for(int i = 0; i < code; i++)
		{
			digitalWrite(pin, HIGH);
			delay(250);
			digitalWrite(pin, LOW);
			delay(250);
		}
		delay(2000);
	}
}

