#include "esp32_midi.h"
#include "midi_spec.h"
#include "error_check_return.h"

#include "esp_log.h"
static const char *TAG = "midi-parser";

esp_err_t SerialMIDI::Install(uart_port_t uartNum, gpio_num_t gpioNum)
{
	ESP_LOGD(TAG, "Installing MIDI driver");

	_uartNum = uartNum;

	uart_config_t uartConfig = {
			.baud_rate = MIDI_BAUD,
			.data_bits = UART_DATA_8_BITS,
			.parity = UART_PARITY_DISABLE,
			.stop_bits = UART_STOP_BITS_1,
			.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
			.rx_flow_ctrl_thresh = 0,
			.use_ref_tick = false
	};

	ESP_ERROR_CHECK_RETURN(uart_param_config(uartNum, &uartConfig));
	ESP_ERROR_CHECK_RETURN(uart_set_pin(uartNum, UART_PIN_NO_CHANGE, gpioNum, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
	ESP_ERROR_CHECK_RETURN(uart_isr_register(
			uartNum,
			isr_handler,
			this,
			ESP_INTR_FLAG_LEVEL1,
			NULL));

	uart_intr_config_t intrConfig = {
			.intr_enable_mask = UART_RXFIFO_TOUT_INT_ENA | UART_RXFIFO_FULL_INT_ENA,
			.rx_timeout_thresh = 1,
			.txfifo_empty_intr_thresh = 0,
			.rxfifo_full_thresh = 1
	};

	ESP_ERROR_CHECK_RETURN(uart_intr_config(uartNum, &intrConfig));

	ESP_ERROR_CHECK_RETURN(_fsm.Init());

	ESP_LOGI(TAG, "MIDI driver installed");

	return ESP_OK;
}

esp_err_t SerialMIDI::RegisterCallback(midi_event_callback_t callback)
{
	return _fsm.RegisterCallback(callback);
}

void SerialMIDI::isr_handler(void *args)
{
	SerialMIDI *obj = (SerialMIDI*) args;
	uart_dev_t *uart = &UART1;

	uint32_t rxfifo_cnt = uart->status.rxfifo_cnt;

	for (int i = 0; i < rxfifo_cnt; i++)
	{
		obj->_fsm.Feed(uart->fifo.rw_byte);
	}

	uart->int_clr.val = UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR;
}



