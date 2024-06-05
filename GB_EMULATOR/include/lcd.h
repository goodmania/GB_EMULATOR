#pragma once

#include <common.h>

struct LcdContext {
    //registers...
    u8 _lcdC;
    u8 _lcdS;
    u8 _scrollY;
    u8 _scrollX;
    u8 _ly;
    u8 _lyCompare;
    u8 _dma;
    u8 _bgPalette;
    u8 _objPalette[2];
    u8 _winY;
    u8 _winX;

    //other data...
    u32 _bgColors[4];
    u32 _sp1Colors[4];
    u32 _sp2Colors[4];
};

enum LcdMode : u8 {
    MODE_HBLANK,
    MODE_VBLANK,
    MODE_OAM,
    MODE_XFER
};

enum StatSrc {
    SS_HBLANK = (1 << 3),
    SS_VBLANK = (1 << 4),
    SS_OAM = (1 << 5),
    SS_LYC = (1 << 6),
};

class Lcd {
public:
    Lcd();
    ~Lcd() = default;
    LcdContext* getContext() { return &_context; }
    void initialize();

    bool isBgwEable() { BIT(getContext()->_lcdC, 0); }
    bool isObjEable() { BIT(getContext()->_lcdC, 1); }
    u8 objHeight() { BIT(getContext()->_lcdC, 2) ? 16 : 8; }
    u16 bgMapArea() { BIT(getContext()->_lcdC, 3) ? 0x9C00 : 0x9800; }
    u16 bgwDataArea() { BIT(getContext()->_lcdC, 4) ? 0x8000 : 0x8800; }
    bool isWinEnable() { BIT(getContext()->_lcdC, 5); }
    u16 winMapArea() { BIT(getContext()->_lcdC, 6) ? 0x9C00 : 0x9800; }
    bool isLcdEnable() { BIT(getContext()->_lcdC, 7); }
    LcdMode getLcdMode() { (LcdMode)getContext()->_lcdS & 0b11; }
    void setLcdMode(LcdMode mode) { getContext()->_lcdS &= ~0b11; getContext()->_lcdS |= mode; }
    bool isLycEnable() { BIT(getContext()->_lcdS, 2); }
    void setLyc(bool b) { BIT_SET(getContext()->_lcdS, 2, b); }
    bool statInt(u8 src) {return getContext()->_lcdS & src;}

    u8 read(u16 address);
    void write(u16 address, u8 value);
private:
    
    void updatePalette(u8 paletteData, u8 palette);

    LcdContext _context;
    u64 _colorsDefault[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };

};


