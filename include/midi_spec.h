#ifndef COMPONENTS_ESP32_MIDI_PRIVINCLUDE_MIDI_SPEC_H_
#define COMPONENTS_ESP32_MIDI_PRIVINCLUDE_MIDI_SPEC_H_

#include <cstdint>

#define MIDI_BAUD (31250) // (38400)

#define MIDI_SYSEX_START (0xF0)
#define MIDI_SYSEX_END (0xF7)

enum class midi_event_t
{
	Undefined = 0xFD,

	// realtime
	TimingClock = 0xF8,
	Start = 0xFA,
	Continue = 0xFB,
	Stop = 0xFC,
	ActiveSense = 0xFE,
	SystemReset = 0xFF,

	// status group 1
	ProgramChange = 0xC0,
	ChannelPressure = 0xD0,
	MidiTimeCode = 0xF1,
	SongSelect = 0xF3,

	// status group 2
	NoteOff = 0x80,
	NoteOn = 0x90,
	PolyPressure = 0xA0,
	ControlChange = 0xB0,
	PitchBend = 0xE0,
	SongPosition = 0xF2,
};

struct midi_message_t
{
	midi_event_t event;
	uint8_t *data;
};

#endif /* COMPONENTS_ESP32_MIDI_PRIVINCLUDE_MIDI_SPEC_H_ */
