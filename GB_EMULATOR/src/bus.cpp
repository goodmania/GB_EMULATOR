
#include <bus.h>
#include <cart.h>

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

Bus::Bus(Cart& cart)
{
    _cart = &cart;
    assert(_cart != nullptr);
}

u8 Bus::busRead(u16 address)
{
    if (address < 0x8000) {
        //ROM Data
        return _cart->read(address);
    }
#if 0
    else if (address < 0xA000) {
        //Char/Map Data
        return ppuVramRead(address);
    }
    else if (address < 0xC000) {
        //Cartridge RAM
        return cartRead(address);
    }
    else if (address < 0xE000) {
        //WRAM (Working RAM)
        return wramRead(address);
    }
    else if (address < 0xFE00) {
        //reserved echo ram...
        return 0;
    }
    else if (address < 0xFEA0) {
        //OAM
        if (dmaTransferring()) {
            return 0xFF;
        }

        return ppuOamRead(address);
    }
    else if (address < 0xFF00) {
        //reserved unusable...
        return 0;
    }
    else if (address < 0xFF80) {
        //IO Registers...
        return ioRead(address);
    }
    else if (address == 0xFFFF) {
        //CPU ENABLE REGISTER...
        return cpuGetIeRegister();
    }
    
    return hram_read(address);
#endif
    NO_IMPL
}

void Bus::busWrite(u16 address, u8 value)
{
    if (address < 0x8000) {
        //ROM Data
        _cart->write(address, value);
        return;
    }
    NO_IMPL
}

u16 Bus::busRead16(u16 address)
{
    u16 lo = busRead(address);
    u16 hi = busRead(address + 1);

    return lo | (hi << 8);
}

void Bus::busWrite16(u16 address, u16 value)
{
    busWrite(address + 1, (value >> 8) & 0xFF);
    busWrite(address, value & 0xFF);
}
