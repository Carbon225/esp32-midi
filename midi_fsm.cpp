#include "esp32_midi.h"
#include "midi_spec.h"

#include <cstring>

#include "esp_log.h"
static const char *TAG = "midi-fsm";

static const uint8_t StatusGr1Bytes[] = {
		0xC0, // Program change
		0xD0, // Channel pressure
		0xF1, // MIDI time code
		0xF3, // Song select
};

static const uint8_t StatusGr2Bytes[] = {
		0x80, // Note off
		0x90, // Note on
		0xA0, // Poly pressure
		0xB0, // Control change
		0xE0, // Pitch bend
		0xF2, // Song position
};

esp_err_t MIDIFSM::Init()
{
	_eventQueue = xQueueCreate(32, sizeof(midi_message_t));
	xTaskCreate(queue_task, "fms_queue", 4096, this, 5, NULL);
	_fsmState = fsm_state_t::FirstByte;
	return ESP_OK;
}

esp_err_t MIDIFSM::Feed(uint8_t byte)
{
	if (IsRealtime(byte))
	{
		HandleRealtime(byte);
		return ESP_OK;
	}
	else if (IsStatus(byte))
	{
		_fsmBuf[0] = byte;
		_bufPos = 1;
		DispatchStatus();
		return ESP_OK;;
	}

	switch (_fsmState)
	{
	case fsm_state_t::Reset:
		ESP_EARLY_LOGW(TAG, "FSM in reset state");
		break;

	case fsm_state_t::FirstByte:
		break;

	case fsm_state_t::StatusGr1_ReceiveByte:
		_fsmBuf[1] = byte;
		HandleMessage(1);
		break;

	case fsm_state_t::StatusGr2_FirstByte:
		_fsmBuf[1] = byte;
		_fsmState = fsm_state_t::StatusGr2_SecondByte;
		break;

	case fsm_state_t::StatusGr2_SecondByte:
		_fsmBuf[2] = byte;
		HandleMessage(2);
		_fsmState = fsm_state_t::StatusGr2_FirstByte;
		break;

	case fsm_state_t::SysEx_ReceiveByte:
		if (byte == MIDI_SYSEX_END)
		{
			HandleSysEx();
		}
		else
		{
			if (_bufPos >= sizeof(_fsmBuf))
			{
				ESP_EARLY_LOGW(TAG, "FSM buffer full");
				return ESP_ERR_NO_MEM;
			}
			_fsmBuf[_bufPos++] = byte;
		}

		break;

	default:
		break;
	}

	return ESP_OK;
}

esp_err_t MIDIFSM::DispatchStatus()
{
	uint8_t statusByte = _fsmBuf[0];
	if (!IsSystem(statusByte))
	{
		statusByte &= 0xF0;
	}

	if (IsStatusGr1(statusByte))
	{
		_fsmState = fsm_state_t::StatusGr1_ReceiveByte;
	}
	else if (IsStatusGr2(statusByte))
	{
		_fsmState = fsm_state_t::StatusGr2_FirstByte;
	}
	else if (_fsmBuf[0] == MIDI_SYSEX_START)
	{
		_fsmState = fsm_state_t::SysEx_ReceiveByte;
	}
	else
	{
		ESP_EARLY_LOGW(TAG, "Unknown status code 0x%x", _fsmBuf[0]);
		return ESP_ERR_INVALID_ARG;
	}

	return ESP_OK;
}

esp_err_t MIDIFSM::HandleMessage(int length)
{
	uint8_t *data = new uint8_t[length];
	memcpy(data, _fsmBuf + 1, length);

	midi_message_t msg = {
			midi_event_t::Undefined,
			data
	};

	if (IsSystem(_fsmBuf[0]))
	{
		msg.event = (midi_event_t) _fsmBuf[0];
	}
	else
	{
		msg.event = (midi_event_t) (_fsmBuf[0] & 0xF0);
	}

	BaseType_t woken = pdFALSE;
	xQueueSendFromISR(_eventQueue, &msg, &woken);
	if (woken == pdTRUE)
	{
		portYIELD_FROM_ISR();
	}

	return ESP_OK;
}

esp_err_t MIDIFSM::HandleSysEx()
{
	ESP_EARLY_LOGW(TAG, "SysEx not implemented");
	return ESP_OK;
}

esp_err_t MIDIFSM::HandleRealtime(uint8_t byte)
{
	midi_message_t msg = {
			(midi_event_t) byte,
			NULL
	};

	BaseType_t woken = pdFALSE;
	xQueueSendFromISR(_eventQueue, &msg, &woken);
	if (woken == pdTRUE)
	{
		portYIELD_FROM_ISR();
	}

	return ESP_OK;
}

bool MIDIFSM::IsStatus(uint8_t byte)
{
	return byte & 0b10000000;
}

bool MIDIFSM::IsSystem(uint8_t byte)
{
	return (byte & 0xF0) == 0xF0;
}

bool MIDIFSM::IsStatusGr1(uint8_t byte)
{
	for (int i = 0; i < sizeof(StatusGr1Bytes); i++)
		if (StatusGr1Bytes[i] == byte)
			return true;

	return false;
}

bool MIDIFSM::IsStatusGr2(uint8_t byte)
{
	for (int i = 0; i < sizeof(StatusGr2Bytes); i++)
		if (StatusGr2Bytes[i] == byte)
			return true;

	return false;
}

bool MIDIFSM::IsRealtime(uint8_t byte)
{
	return (byte & 0b11111000) == 0b11111000;
}

void MIDIFSM::queue_task(void *args)
{
	MIDIFSM *obj = (MIDIFSM*) args;
	while (true)
	{
		midi_message_t msg;
		if (xQueueReceive(obj->_eventQueue, &msg, portMAX_DELAY) == pdTRUE)
		{
			if (obj->_eventCb)
				obj->_eventCb(msg);

			delete [] msg.data;
		}
	}
}

esp_err_t MIDIFSM::RegisterCallback(midi_event_callback_t callback)
{
	if (!callback)
		return ESP_ERR_INVALID_ARG;

	_eventCb = callback;

	return ESP_OK;
}




