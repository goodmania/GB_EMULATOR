#pragma once

#include <common.h>

class Ram {
public:
	Ram();
	~Ram() = default;

	u8 wramRead(u16 address);
	void wramWrite(u16 address, u8 value);

	u8 hramRead(u16 address);
	void hramWrite(u16 address, u8 value);
private:
	u8 _wram[0x2000]; //作業 RAM (WRAM)
	u8 _hram[0x80]; // FF80	FFFE 上位 RAM (HRAM)	スタック用の領域。
};