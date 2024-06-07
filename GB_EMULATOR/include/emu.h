#pragma once

#include <common.h>

class Cart;
class Cpu;
class Dma;
class Gamepad;
class Io;
class Ppu;
class Ram;
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
	Gamepad* getGamepad() { return _gamepad; }
	Io* getIo() { return _io; }
	Ppu* getPpu() { return _ppu; }
	Ram* getRam() { return _ram; }
	Timer* getTimer() { return _timer; }
	Ui* getUi() { return _ui; }
	
	s32 emuRun(s32 argc, char** argv);
	void emuCycles(s32 cpuCycles);
	void* cpuRun(void* p);

private:
	void initialize();
	void initialize1();
	void terminate();

	EmuContext* _emuContext;

	Cart* _cart;
	Cpu* _cpu;
	Dma* _dma;
	Gamepad* _gamepad;
	Io* _io;
	Ppu* _ppu;
	Ram* _ram;
	Timer* _timer;	
	Ui* _ui;
};

Emu* EmuGet();