#pragma once

#include <common.h>

class Cart;

class Bus {
public:
	Bus(Cart& cart);
	~Bus() = default;
	u8 busRead(u16 address);
	void busWrite(u16 address, u8 value);
	u16 busRead16(u16 address);
	void busWrite16(u16 address, u16 value);
private:
	Cart* _cart;
};