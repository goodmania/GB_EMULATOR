#pragma once

#include <common.h>

class Dbg {
	void update();
	void print();
private:
	char _dbgMsg[1024];
	int _msgSize;
};