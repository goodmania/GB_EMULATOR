#include "timer.h"

void Timer::initialize()
{
    _context._div = 0xAC00;
}

void Timer::tick()
{
    u16 prevDiv = _context._div;

    _context._div++;

    bool timerUpdate = false;

    switch (_context._tac & (0b11)) {
    case 0b00:
        timerUpdate = (prevDiv & (1 << 9)) && (!(_context._div & (1 << 9)));
        break;
    case 0b01:
        timerUpdate = (prevDiv & (1 << 3)) && (!(_context._div & (1 << 3)));
        break;
    case 0b10:
        timerUpdate = (prevDiv & (1 << 5)) && (!(_context._div & (1 << 5)));
        break;
    case 0b11:
        timerUpdate = (prevDiv & (1 << 7)) && (!(_context._div & (1 << 7)));
        break;
    }

    if (timerUpdate && _context._tac & (1 << 2)) {
        _context._tima++;

        if (_context._tima == 0xFF) {
            _context._tima = _context._tma;

            cpu_request_interrupt(IT_TIMER);
        }
    }
}

void Timer::write(u16 address, u8 value)
{
}

u8 Timer::read(u16 address)
{
	return u8();
}
