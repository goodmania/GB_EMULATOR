#include "ram.h"
#include "memory"

Ram::Ram()
{
	memset(_wram, 0, sizeof(_wram));
	memset(_hram, 0, sizeof(_hram));
}

u8 Ram::wramRead(u16 address)
{
    address -= 0xC000;

    if (address >= 0x2000) {
        printf("INVALID WRAM ADDR %08X\n", address + 0xC000);
        exit(-1);
    }

    return _wram[address];
}

void Ram::wramWrite(u16 address, u8 value)
{
    address -= 0xC000;
    _wram[address] = value;
}

u8 Ram::hramRead(u16 address)
{
    address -= 0xFF80;
    return _hram[address];
}

void Ram::hramWrite(u16 address, u8 value)
{
    address -= 0xFF80;
    _hram[address] = value;
}
