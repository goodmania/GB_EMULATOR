#include "gamepad.h"

void Gamepad::initialize()
{

}

bool Gamepad::buttonSel()
{
	return _context._buttonSel;
}

bool Gamepad::dirSel()
{
	return _context._dirSel;
}

void Gamepad::setSel(u8 value)
{
	_context._buttonSel = value & 0x20;
	_context._dirSel = value & 0x10;
}

u8 Gamepad::getOutput()
{
    u8 output = 0xCF;

    if (!buttonSel()) {
        if (getState()->_start) {
            output &= ~(1 << 3);
        }
        if (getState()->_select) {
            output &= ~(1 << 2);
        }
        if (getState()->_a) {
            output &= ~(1 << 0);
        }
        if (getState()->_b) {
            output &= ~(1 << 1);
        }
    }

    if (!dirSel()) {
        if (getState()->_left) {
            output &= ~(1 << 1);
        }
        if (getState()->_right) {
            output &= ~(1 << 0);
        }
        if (getState()->_up) {
            output &= ~(1 << 2);
        }
        if (getState()->_down) {
            output &= ~(1 << 3);
        }
    }

    return output;
}
