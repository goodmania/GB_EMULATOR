#pragma once

#include <common.h>

class Cart;
class Cpu;

class Emu {

	struct EmuContext {
		bool _paused;
		bool _running;
		u64 _ticks;
	};

public:
	Emu();
	~Emu();

	EmuContext* getEmuContext() { return _emuContext; }
	Cpu* getCpu() { return _cpu; }
	Timer* getTimer() { return _timer; }
	Cart* getCart() { return _cart; }

	s32 emuRun(s32 argc, char** argv);
	void emuCycles(s32 cpuCycles);

private:
	void initialize();
	void initialize1();
	void terminate();

	EmuContext* _emuContext;
	Cpu* _cpu;
	Cart* _cart;
	Timer* _timer;
};

Emu* EmuGet();