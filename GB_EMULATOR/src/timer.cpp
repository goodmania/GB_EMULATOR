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

void Timer::write(u16 address, u8 value) {
    switch (address) {
    case 0xFF04:
        //DIV
        _context._div = 0;
        break;

    case 0xFF05:
        //TIMA
        _context._tima = value;
        break;

    case 0xFF06:
        //TMA
        _context._tma = value;
        break;

    case 0xFF07:
        //TAC
        _context._tac = value;
        break;
    }
}

u8 Timer::read(u16 address)
{
    switch (address) {
    case 0xFF04:
        return _context._div >> 8;
    case 0xFF05:
        return _context._tima;
    case 0xFF06:
        return _context._tma;
    case 0xFF07:
        return _context._tac;
    }
}
