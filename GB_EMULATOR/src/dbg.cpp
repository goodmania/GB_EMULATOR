#include "dbg.h"
#include "cart.h"
#include "emu.h"

void Dbg::update()
{
    Emu* emu = EmuGet();
    
    if (emu->getCart()->busRead(0xFF02) == 0x81) {
        char c = emu->getCart()->busRead(0xFF01);

        _dbgMsg[_msgSize++] = c;

        emu->getCart()->busWrite(0xFF02, 0);
    }
}

void Dbg::print()
{
    if (_dbgMsg[0]) {
        //printf("DBG: %s\n", dbg_msg);
    }
}
