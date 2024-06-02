#pragma once

#include <common.h>

class Cart;
class Cpu;
class Dma;
class Gamepad;
class Ppu;
class Timer;
class Ui;

class Emu {

	struct EmuContext {
		bool _paused;
		bool _running;
		bool _die;
		u64 _ticks;
	};

public:
	Emu();
	~Emu();

	EmuContext* getEmuContext() { return _emuContext; }
	Cpu* getCpu() { return _cpu; }
	Cart* getCart() { return _cart; }
	Dma* getDma() { return _dma; }
	Ppu* getPpu() { return _ppu; }
	Timer* getTimer() { return _timer; }
	Ui* getUi() { return _ui; }
	Gamepad* getGamepad() { return _gamepad; }
	
	s32 emuRun(s32 argc, char** argv);
	void emuCycles(s32 cpuCycles);

private:
	void initialize();
	void initialize1();
	void terminate();

	EmuContext* _emuContext;

	Cart* _cart;
	Cpu* _cpu;
	Dma* _dma;
	Ppu* _ppu;
	Timer* _timer;	
	Ui* _ui;
	Gamepad* _gamepad;
};

Emu* EmuGet();