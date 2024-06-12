#include "io.h"
#include "dma.h"
#include "emu.h"
#include "ppu.h"
#include "lcd.h"
#include "timer.h"
#include "cpu.h"
#include "gamepad.h"

u8 Io::read(u16 address)
{
    Emu* emu = EmuGet();
    Cpu* cpu = emu->getCpu();
    Dma* dma = emu->getDma();
    Gamepad* gamepad = emu->getGamepad();
    Lcd* lcd = emu->getPpu()->getLcd();
    Timer* timer = emu->getTimer();

    if (address == 0xFF00) {
        return gamepad->getOutput(); 
    } 

    if (address == 0xFF01) {
        return _serialData[0]; 
    }

    if (address == 0xFF02) {
        return _serialData[1];
    }

    if (BETWEEN(address, 0xFF04, 0xFF07)) {
        return timer->read(address);
    }

    if (address == 0xFF0F) {
        return cpu->getIntFlags();
    }

    if (BETWEEN(address, 0xFF10, 0xFF3F)) {
        //ignore sound
        return 0;
    }

    if (BETWEEN(address, 0xFF40, 0xFF4B)) {
        return lcd->read(address);
    }

    printf("UNSUPPORTED bus_read(%04X)\n", address);
    return 0;
}

void Io::write(u16 address, u8 value)
{
    Emu* emu = EmuGet();
    Cpu* cpu = emu->getCpu();
    Dma* dma = emu->getDma();
    Gamepad* gamepad = emu->getGamepad();
    Lcd* lcd = emu->getPpu()->getLcd();
    Timer* timer = emu->getTimer();

    if (address == 0xFF00) {
        gamepad->setSel(value);
        return;
    }

    if (address == 0xFF01) {
        _serialData[0] = value;
        return;
    }

    if (address == 0xFF02) {
        _serialData[1] = value;
        return;
    }

    if (BETWEEN(address, 0xFF04, 0xFF07)) {
        timer->write(address, value);
        return;
    }

    if (address == 0xFF0F) {
        cpu->setIntFlags(value);
        return;
    }

    if (BETWEEN(address, 0xFF10, 0xFF3F)) {
        //ignore sound
        return;
    }

    if (BETWEEN(address, 0xFF40, 0xFF4B)) {
        lcd->write(address, value);
        return;
    }

    printf("UNSUPPORTED bus_write(%04X)\n", address);
}
