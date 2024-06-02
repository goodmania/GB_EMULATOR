#pragma once

#include <common.h>

static const int LINES_PER_FRAME = 154;
static const int TICKS_PER_LINE = 456;
static const int YRES = 144;
static const int XRES = 160;

class Lcd;

enum FetchState {
    FS_TILE,
    FS_DATA0,
    FS_DATA1,
    FS_IDLE,
    FS_PUSH
};

struct FifoEntry {
    struct FifoEntry* _next;
    u32 _value; //32 bit color value.
};

struct Fifo{
    FifoEntry* _head;
    FifoEntry* _tail;
    u32 _size;
};

struct PixelFifoContext {
    FetchState _currentFetchState;
    Fifo _pixelFifo;
    u8 _lineX;
    u8 _pushedX;
    u8 _fetchX;
    u8 _bgwFetchData[3];
    u8 _fetchEntryData[6]; //oam data..
    u8 _mapY;
    u8 _mapX;
    u8 _tileY;
    u8 _fifoX;
};

struct OamEntry{
    u8 _y;
    u8 _x;
    u8 _tile;

    u8 _fCgbPn : 3;
    u8 _fCgbVramBank : 1;
    u8 _fPn : 1;
    u8 _fXFlip : 1;
    u8 _fYFlip : 1;
    u8 _fBgp : 1;

};

/*
 Bit7   BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
 Bit6   Y flip          (0=Normal, 1=Vertically mirrored)
 Bit5   X flip          (0=Normal, 1=Horizontally mirrored)
 Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
 Bit3   Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
 Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
 */

struct OamLineEntry {
    OamEntry _entry;
    OamLineEntry* _next;
};

struct PpuContext {
    OamEntry _oamRam[40];
    u8 _vram[0x2000];

    PixelFifoContext _pfc;

    u8 _lineSpriteCount; //0 to 10 sprites.
    OamLineEntry* _lineSprites; //linked list of current sprites on line.
    OamLineEntry _lineEntryArray[10]; //memory to use for list.

    u8 _fetchedEntryCount;
    OamEntry _fetchedEntries[3]; //entries fetched during pipeline.
    u8 _windowLine;

    u32 _currentFrame;
    u32 _lineTicks;
    u32* _videoBuffer;
} ;

class Ppu {
public:
    Ppu();
    ~Ppu();

    void oamWrite(u16 address, u8 value);
    u8 oamRead(u16 address);

    void vramWrite(u16 address, u8 value);
    u8 vramRead(u16 address);

    PpuContext* getContext() { return &_context; }
    void tick();

private:
    void initialize();
    void initialize1();
    bool windowVisible();
    void incrementLy();
    void loadLineSprites();

    void modeOam();
    void modeXfer();
    void modeVblank();
    void modeHblank();

    // pipeline
    void pixelFifoPush(u32 value);
    u32 pixelFifoPop();
    u32 fetchSpritePixels(s32 bit, u32 color, u8 bgColor);
    bool pipelineFifoAdd();
    void pipelineLoadSpriteTile();
    void pipelineLoadSpriteData(u8 offset);
    void pipelineLoadWindowTile();
    void pipelineFetch();
    void pipelinePushPixel();
    void pipelineProcess();
    void pipelineFifoReset();

    PpuContext _context;
    Lcd* _lcd;
};