#pragma once

#include <common.h>

struct TimerContext {
	u16 _div; // Divider Register. This register is incremented at rate of 16384Hz (~16779Hz on SGB). Writing any value to this register resets it to 00h.
	u8 _tima; // Timer Counter. This timer is incremented at the clock frequency specified by the TAC register ($FF07)
	u8 _tma; // Timer Modulo. When TIMA overflows, it is reset to the value in this register and an interrupt is requested.
	u8 _tac; // Timer Control. TMA が $FF に設定されている場合、TAC で選択されたクロック周波数で割り込みが要求されます

	/*
	* TAC Register
	* Bit  2   - Timer Enable
	* 	 * Bits 1-0 - Input Clock Select
	* 	 *            00: CPU Clock / 1024 (DMG, CGB:   4096 Hz, SGB:   ~4194 Hz)
	* 	 *            01: CPU Clock / 16   (DMG, CGB: 262144 Hz, SGB: ~268400 Hz)
	* 	 *            10: CPU Clock / 64   (DMG, CGB:  65536 Hz, SGB:  ~67110 Hz)
	* 	 *            11: CPU Clock / 256  (DMG, CGB:  16384 Hz, SGB:  ~16780 Hz)
	* 	 */
};

class Timer {
public:
	Timer();
	~Timer() = default;

	void initialize();
	void tick();
	void write(u16 address, u8 value);
	u8 read(u16 address);

	TimerContext& getTimerContext() { return _context; }

private:
	TimerContext _context;
};