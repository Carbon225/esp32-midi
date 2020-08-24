#ifndef COMPONENTS_ESP32_MIDI_INCLUDE_ESP32_MIDI_H_
#define COMPONENTS_ESP32_MIDI_INCLUDE_ESP32_MIDI_H_

#include "midi_fsm.h"

#include "driver/uart.h"
#include "driver/gpio.h"

class SerialMIDI
{
public:
	esp_err_t Install(uart_port_t uartNum, gpio_num_t gpioNum);
	esp_err_t RegisterCallback(midi_event_callback_t callback);

private:
	uart_port_t _uartNum = UART_NUM_MAX;
	MIDIFSM _fsm;

	static void isr_handler(void *args);
};

#endif /* COMPONENTS_ESP32_MIDI_INCLUDE_ESP32_MIDI_H_ */
