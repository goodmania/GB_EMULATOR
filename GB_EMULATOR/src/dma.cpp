#include "dma.h"
#include "emu.h"
#include "cart.h"
#include "ppu.h"

void Dma::start(u8 start)
{
    _context._active = true;
    _context._byte = 0;
    _context._startDelay = 2;
    _context._value = start;
}

void Dma::tick()
{
    if (!_context._active) {
        return;
    }

    if (_context._startDelay) {
        _context._startDelay--;
        return;
    }
    Ppu* ppu = EmuGet()->getPpu();
    Cart* cart = EmuGet()->getCart();
    ppu->oamWrite(_context._byte, cart->busRead((_context._value * 0x100) + _context._byte));

    _context._byte++;
    _context._active = _context._byte < 0xA0;
}

bool Dma::transferring()
{
    return _context._active;
}
