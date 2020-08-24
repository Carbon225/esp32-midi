#ifndef COMPONENTS_ESP32_MIDI_INCLUDE_MIDI_FSM_H_
#define COMPONENTS_ESP32_MIDI_INCLUDE_MIDI_FSM_H_

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "midi_spec.h"

enum class fsm_state_t
{
	Reset,
	FirstByte,

	StatusGr1_ReceiveByte,

	StatusGr2_FirstByte,
	StatusGr2_SecondByte,

	SysEx_ReceiveByte
};

typedef void (*midi_event_callback_t)(midi_message_t);

class MIDIFSM
{
public:
	esp_err_t Init();
	esp_err_t Feed(uint8_t byte);
	esp_err_t RegisterCallback(midi_event_callback_t callback);

	static bool IsSystem(uint8_t byte);
	static bool IsStatus(uint8_t byte);
	static bool IsStatusGr1(uint8_t byte);
	static bool IsStatusGr2(uint8_t byte);
	static bool IsRealtime(uint8_t byte);

private:
	int _bufPos = 0;
	uint8_t _fsmBuf[64];
	fsm_state_t _fsmState = fsm_state_t::Reset;
	QueueHandle_t _eventQueue;
	midi_event_callback_t _eventCb = NULL;

	static void queue_task(void *args);

	esp_err_t DispatchStatus();
	esp_err_t HandleMessage(int length);
	esp_err_t HandleSysEx();
	esp_err_t HandleRealtime(uint8_t byte);
};

#endif /* COMPONENTS_ESP32_MIDI_INCLUDE_MIDI_FSM_H_ */
