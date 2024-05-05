#pragma once

#include <common.h>

class Bus {
public:
	u8 busRead(u16 address);
	void busWrite(u16 address, u8 value);
};

