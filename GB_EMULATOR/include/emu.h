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

	void initialize();
	void terminate();

	s32 emuRun(s32 argc, char** argv);
	EmuContext* getEmuContext();
	void emuCycles(s32 cpuCycles);

private:
	EmuContext* _emuContext;
	Cpu* _cpu;
	Cart* _cart;
};