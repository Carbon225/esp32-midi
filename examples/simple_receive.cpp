#include "main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"
static const char *TAG = "main";

#define MIDI_UART UART_NUM_1
#define MIDI_RX_GPIO GPIO_NUM_21

void HandleMidiMessage(midi_message_t msg)
{
	switch (msg.event)
	{
	// 0 data bytes
	case midi_event_t::TimingClock:
		ESP_LOGI(TAG, "Timing clock");
		break;

	case midi_event_t::Undefined:
		ESP_LOGI(TAG, "Undefined");
		break;

	case midi_event_t::Start:
		ESP_LOGI(TAG, "Start");
		break;

	case midi_event_t::Continue:
		ESP_LOGI(TAG, "Continue");
		break;

	case midi_event_t::Stop:
		ESP_LOGI(TAG, "Stop");
		break;

	case midi_event_t::ActiveSense:
		ESP_LOGI(TAG, "Active sense");
		break;

	case midi_event_t::SystemReset:
		ESP_LOGI(TAG, "System reset");
		break;

	// 1 data byte
	case midi_event_t::ProgramChange:
		ESP_LOGI(TAG, "Program change");
		break;

	case midi_event_t::ChannelPressure:
		ESP_LOGI(TAG, "Channel pressure");
		break;

	case midi_event_t::MidiTimeCode:
		ESP_LOGI(TAG, "Midi time code");
		break;

	case midi_event_t::SongSelect:
		ESP_LOGI(TAG, "System reset");
		break;

	// 2 data bytes
	case midi_event_t::NoteOff:
		ESP_LOGI(TAG, "Note off");
		break;

	case midi_event_t::NoteOn:
		ESP_LOGI(TAG, "Note on %d %d", msg.data[0], msg.data[1]);
		break;

	case midi_event_t::PolyPressure:
		ESP_LOGI(TAG, "Poly pressure");
		break;

	case midi_event_t::ControlChange:
		ESP_LOGI(TAG, "Control change %d %d", msg.data[0], msg.data[1]);
		break;

	case midi_event_t::PitchBend:
		ESP_LOGI(TAG, "Pitch bend");
		break;

	case midi_event_t::SongPosition:
		ESP_LOGI(TAG, "Song position");
		break;
	}
}

extern "C" void app_main()
{
	ESP_ERROR_CHECK(midi.RegisterCallback(HandleMidiMessage));
	ESP_ERROR_CHECK(midi.Install(MIDI_UART, MIDI_RX_GPIO));
}
