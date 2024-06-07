#include "dma.h"
#include "emu.h"
#include "lcd.h"

Lcd::Lcd(){
    initialize();
}

void Lcd::initialize()
{
    _context._lcdC = 0x91;
    _context._scrollX = 0;
    _context._scrollY = 0;
    _context._ly = 0;
    _context._lyCompare = 0;
    _context._bgPalette = 0xFC;
    _context._objPalette[0] = 0xFF;
    _context._objPalette[1] = 0xFF;
    _context._winY = 0;
    _context._winX = 0;

    for (int i = 0; i < 4; i++) {
        _context._bgColors[i] = static_cast<u32>(_colorsDefault[i]);
        _context._sp1Colors[i] = static_cast<u32>(_colorsDefault[i]);
        _context._sp2Colors[i] = static_cast<u32>(_colorsDefault[i]);
    }
}

u8 Lcd::read(u16 address)
{
    u8 offset = (address - 0xFF40);
    u8* p = (u8*)&_context;
    return p[offset];
}

void Lcd::write(u16 address, u8 value)
{
    u8 offset = (address - 0xFF40);
    u8* p = (u8*)&_context;
    p[offset] = value;

    if (offset == 6) {
        //0xFF46 = DMA
        EmuGet()->getDma()->start(value);
    }

    if (address == 0xFF47) {
        updatePalette(value, 0);
    }
    else if (address == 0xFF48) {
        updatePalette(value & 0b11111100, 1);
    }
    else if (address == 0xFF49) {
        updatePalette(value & 0b11111100, 2);
    }
}

void Lcd::updatePalette(u8 paletteData, u8 palette)
{
    u32* colors = _context._bgColors;

    switch (palette) {
    case 1:
        colors = _context._sp1Colors;
        break;
    case 2:
        colors = _context._sp2Colors;
        break;
    }

    colors[0] = static_cast<u32>(_colorsDefault[paletteData & 0b11]);
    colors[1] = static_cast<u32>(_colorsDefault[(paletteData >> 2) & 0b11]);
    colors[2] = static_cast<u32>(_colorsDefault[(paletteData >> 4) & 0b11]);
    colors[3] = static_cast<u32>(_colorsDefault[(paletteData >> 6) & 0b11]);
}
