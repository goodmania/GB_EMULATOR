#pragma once

#include <common.h>

struct TimerContext {
	u16 _div;
	u8 _tima;
	u8 _tma;
	u8 _tac;
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