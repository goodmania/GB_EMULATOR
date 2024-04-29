#pragma once

#include <common.h>

class Emu {
public:

	struct EmuContext {
		bool _paused;
		bool _running;
		u64 _ticks;
	};

	s32 emuRun(s32 argc, char** argv);
private:

	EmuContext* emuGetContext;
};

