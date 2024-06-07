#include "cart.h"
#include "cpu.h"
#include "emu.h"
#include "lcd.h"
#include "ppu.h"
#include "ui.h"

#include <memory>

Ppu::Ppu()
{
    initialize();
    initialize1();
}

Ppu::~Ppu()
{
    delete _context._videoBuffer;
    delete _lcd;
}

void Ppu::initialize()
{
    _context._currentFrame = 0;
    _context._lineTicks = 0;
    _context._videoBuffer = new u32[YRES * XRES];

    _context._pfc._lineX = 0;
    _context._pfc._pushedX = 0;
    _context._pfc._fetchX = 0;
    _context._pfc._pixelFifo._size = 0;
    _context._pfc._pixelFifo._head = _context._pfc._pixelFifo._tail = nullptr;
    _context._pfc._currentFetchState = FS_TILE;

    _context._lineSprites = 0;
    _context._fetchedEntryCount = 0;
    _context._windowLine = 0;

    memset(_context._oamRam, 0, sizeof(_context._oamRam));
    memset(_context._videoBuffer, 0, YRES * XRES * sizeof(u32));
}

void Ppu::initialize1()
{
    // todo : lcd ƒNƒ‰ƒX‚ðŒã‚ÅŽÀ‘•
    _lcd = new Lcd();
    _lcd->initialize();
    _lcd->setLcdMode(MODE_OAM);
}

void Ppu::tick()
{
    _context._lineTicks++;
    LcdMode mode = _lcd->getLcdMode();

    switch (mode) {
    case MODE_OAM:
        modeOam();
        break;
    case MODE_XFER:
        modeXfer();
        break;
    case MODE_VBLANK:
        modeVblank();
        break;
    case MODE_HBLANK:
        modeHblank();
        break;
    }
}

void Ppu::oamWrite(u16 address, u8 value)
{
    if (address >= 0xFE00) {
        address -= 0xFE00;
    }

    u8* p = (u8*)_context._oamRam;
    p[address] = value;
}

u8 Ppu::oamRead(u16 address)
{
    if (address >= 0xFE00) {
        address -= 0xFE00;
    }

    u8* p = (u8*)_context._oamRam;
    return p[address];
}

void Ppu::vramWrite(u16 address, u8 value)
{
    _context._vram[address - 0x8000] = value;
}

u8 Ppu::vramRead(u16 address)
{
    return _context._vram[address - 0x8000];
}

bool Ppu::windowVisible() {
    LcdContext* context = _lcd->getContext();
    return _lcd->isWinEnable()&& context->_winX >= 0 &&
        context->_winX <= 166 && context->_winY >= 0 &&
        context->_winY < YRES;
}

void Ppu::incrementLy() {
    LcdContext* lcdContext = _lcd->getContext();
    if (windowVisible() && lcdContext->_ly >= lcdContext->_winY &&
        lcdContext->_ly < lcdContext->_winY + YRES) {
        _context._windowLine++;
    }

    lcdContext->_ly++;

    if (lcdContext->_ly == lcdContext->_lyCompare) {
        _lcd->setLyc(1);
        Cpu* cpu = EmuGet()->getCpu();
        if (_lcd->statInt(SS_LYC)) {
            cpu->requestInterrupt(IT_LCD_STAT);
        }
    }
    else {
        _lcd->setLyc(0);
    }
}

void Ppu::loadLineSprites() {
    s32 curY = _lcd->getContext()->_ly;

    u8 spriteHeight = _lcd->objHeight();
    memset(_context._lineEntryArray, 0, sizeof(_context._lineEntryArray));

    for (s32 i = 0; i < 40; i++) {
        OamEntry e = _context._oamRam[i];

        if (!e._x) {
            //x = 0 means not visible...
            continue;
        }

        if (_context._lineSpriteCount >= 10) {
            //max 10 sprites per line...
            break;
        }

        if (e._y <= curY + 16 && e._y + spriteHeight > curY + 16) {
            //this sprite is on the current line.

            OamLineEntry* entry = &_context._lineEntryArray[_context._lineSpriteCount++];

            entry->_entry = e;
            entry->_next = NULL;

            if (!_context._lineSprites || _context._lineSprites->_entry._x > e._x) {
                entry->_next = _context._lineSprites;
                _context._lineSprites = entry;
                continue;
            }

            //do some sorting...

            OamLineEntry* le = _context._lineSprites;
            OamLineEntry* prev = le;

            while (le) {
                if (le->_entry._x > e._x) {
                    prev->_next = entry;
                    entry->_next = le;
                    break;
                }

                if (!le->_next) {
                    le->_next = entry;
                    break;
                }

                prev = le;
                le = le->_next;
            }
        }
    }
}

void Ppu::modeOam()
{
    if (_context._lineTicks >= 80) {
        _lcd->setLcdMode(MODE_XFER);

        _context._pfc._currentFetchState = FS_TILE;
        _context._pfc._lineX = 0;
        _context._pfc._fetchX = 0;
        _context._pfc._pushedX = 0;
        _context._pfc._fifoX = 0;
    }

    if (_context._lineTicks == 1) {
        //read oam on the first tick only...
        _context._lineSprites = 0;
        _context._lineSpriteCount = 0;

        loadLineSprites();
    }
}

void Ppu::modeXfer()
{
    pipelineProcess();

    if (_context._pfc._pushedX >= XRES) {
        pipelineFifoReset();
        _lcd->setLcdMode(MODE_HBLANK);
        
        if (_lcd->statInt(SS_HBLANK)) {
            EmuGet()->getCpu()->requestInterrupt(IT_LCD_STAT);
        }
    }
}

void Ppu::modeVblank()
{
    LcdContext* lcdContext = _lcd->getContext();
    if (_context._lineTicks >= TICKS_PER_LINE) {
        incrementLy();

        if (lcdContext->_ly >= LINES_PER_FRAME) {
            _lcd->setLcdMode(MODE_OAM);
            lcdContext->_ly = 0;
            _context._windowLine = 0;
        }

        _context._lineTicks = 0;
    }
}

void Ppu::modeHblank()
{
    static u32 targetFrameTime = 1000 / 60;
    static s64 prevFrameTime = 0;
    static s64 startTimer = 0;
    static s64 frameCount = 0;
    LcdContext* lcdContext = _lcd->getContext();

    if (_context._lineTicks >= TICKS_PER_LINE) {
        incrementLy();

        if (lcdContext->_ly >= YRES) {
            _lcd->setLcdMode(MODE_VBLANK);
            EmuGet()->getCpu()->requestInterrupt(IT_VBLANK);
            
            if (_lcd->statInt(SS_VBLANK)) {
                EmuGet()->getCpu()->requestInterrupt(IT_LCD_STAT);
            }

            _context._currentFrame++;

            //calc FPS...
            u32 end = EmuGet()->getUi()->getTicks();
            u32 frameTime = end - prevFrameTime;

            if (frameTime < targetFrameTime) {
                delay((targetFrameTime - frameTime));
            }

            if (end - startTimer >= 1000) {
                u32 fps = static_cast<u32>(frameCount);
                startTimer = end;
                frameCount = 0;

                printf("FPS: %d\n", fps);
                
                if (EmuGet()->getCart()->needSave()) {
                    EmuGet()->getCart()->batterySave();
                }
            }

            frameCount++;
            prevFrameTime = EmuGet()->getUi()->getTicks();
        }
        else {
            _lcd->setLcdMode(MODE_OAM);
        }
        _context._lineTicks = 0;
    }
}

void Ppu::pixelFifoPush(u32 value)
{
    // pixelFifoPop ‚Å delete
    FifoEntry* next = new FifoEntry;
    next->_next = NULL;
    next->_value = value;

    if (!_context._pfc._pixelFifo._head) {
        //first entry...
        _context._pfc._pixelFifo._head = _context._pfc._pixelFifo._tail = next;
    }
    else {
        _context._pfc._pixelFifo._tail->_next = next;
        _context._pfc._pixelFifo._tail = next;
    }

    _context._pfc._pixelFifo._size++;
}

u32 Ppu::pixelFifoPop()
{
    if (_context._pfc._pixelFifo._size <= 0) {
        fprintf(stderr, "ERR IN PIXEL FIFO!\n");
        exit(-8);
    }

    FifoEntry* popped = _context._pfc._pixelFifo._head;
    _context._pfc._pixelFifo._head = popped->_next;
    _context._pfc._pixelFifo._size--;

    u32 val = popped->_value;

    // pixelFifoPush ‚Å new
    delete(popped);
    return val;
}

u32 Ppu::fetchSpritePixels(s32 bit, u32 color, u8 bgColor)
{
    LcdContext* lcdContext = _lcd->getContext();

    for (int i = 0; i < _context._fetchedEntryCount; i++) {
        s32 spX = (_context._fetchedEntries[i]._x - 8) + ((lcdContext->_scrollX % 8));

        if (spX + 8 < _context._pfc._fifoX) {
            //past pixel point already...
            continue;
        }

        s32 offset = _context._pfc._fifoX - spX;

        if (offset < 0 || offset > 7) {
            //out of bounds..
            continue;
        }

        bit = (7 - offset);

        if (_context._fetchedEntries[i]._fXFlip) {
            bit = offset;
        }

        u8 hi = !!(_context._pfc._fetchEntryData[i * 2] & (1 << bit));
        u8 lo = !!(_context._pfc._fetchEntryData[(i * 2) + 1] & (1 << bit)) << 1;

        bool bgPriority = _context._fetchedEntries[i]._fBgp;

        if (!(hi | lo)) {
            //transparent
            continue;
        }

        if (!bgPriority || bgColor == 0) {
            color = (_context._fetchedEntries[i]._fPn) ?
                lcdContext->_sp2Colors[hi | lo] : lcdContext->_sp1Colors[hi | lo];

            if (hi | lo) {
                break;
            }
        }
    }

    return color;
}

bool Ppu::pipelineFifoAdd()
{
    if (_context._pfc._pixelFifo._size > 8) {
        //fifo is full!
        return false;
    }

    LcdContext* lcdContext = _lcd->getContext();
    s32 x = _context._pfc._fetchX - (8 - (lcdContext->_scrollX % 8));

    for (s32 i = 0; i < 8; i++) {
        s32 bit = 7 - i;
        u8 hi = !!(_context._pfc._bgwFetchData[1] & (1 << bit));
        u8 lo = !!(_context._pfc._bgwFetchData[2] & (1 << bit)) << 1;
        u32 color = lcdContext->_bgColors[hi | lo];

        
        if (!_lcd->isBgwEable()) {
            color = lcdContext->_bgColors[0];
        }

        if (_lcd->isObjEable()) {
            color = fetchSpritePixels(bit, color, hi | lo);
        }

        if (x >= 0) {
            pixelFifoPush(color);
            _context._pfc._fifoX++;
        }
    }

    return true;
}

void Ppu::pipelineLoadSpriteTile()
{
    OamLineEntry* le = _context._lineSprites;
    LcdContext* lcdContext = _lcd->getContext();

    while (le) {
        s32 spX = (le->_entry._x - 8) + (lcdContext->_scrollX % 8);

        if ((spX >= _context._pfc._fetchX && spX < _context._pfc._fetchX + 8) ||
            ((spX + 8) >= _context._pfc._fetchX && (spX + 8) < _context._pfc._fetchX + 8)) {
            //need to add entry
            _context._fetchedEntries[_context._fetchedEntryCount++] = le->_entry;
        }

        le = le->_next;

        if (!le || _context._fetchedEntryCount >= 3) {
            //max checking 3 sprites on pixels
            break;
        }
    }
}

void Ppu::pipelineLoadSpriteData(u8 offset)
{
    LcdContext* lcdContext = _lcd->getContext();
    int curY = lcdContext->_ly;
    u8 spriteHeight = _lcd->objHeight();

    for (int i = 0; i < _context._fetchedEntryCount; i++) {
        u8 ty = ((curY + 16) - _context._fetchedEntries[i]._y) * 2;

        if (_context._fetchedEntries[i]._fYFlip) {
            //flipped upside down...
            ty = ((spriteHeight * 2) - 2) - ty;
        }

        u8 tileIndex = _context._fetchedEntries[i]._tile;

        if (spriteHeight == 16) {
            tileIndex &= ~(1); //remove last bit...
        }

        _context._pfc._fetchEntryData[(i * 2) + offset] = 
            EmuGet()->getCart()->busRead(0x8000 + (tileIndex * 16) + ty + offset);
    }
}

void Ppu::pipelineLoadWindowTile()
{
    LcdContext* lcdContext = _lcd->getContext();
    if (!windowVisible()) {
        return;
    }

    u8 window_y = lcdContext->_winY;

    if (_context._pfc._fetchX + 7 >= lcdContext->_winX &&
        _context._pfc._fetchX + 7 < lcdContext->_winX + YRES + 14) {
        if (lcdContext->_ly >= window_y && lcdContext->_ly < window_y + XRES) {
            u8 wTileY = _context._windowLine / 8;

            _context._pfc._bgwFetchData[0] = EmuGet()->getCart()->busRead(
                _lcd->winMapArea() +
                ((_context._pfc._fetchX + 7 - lcdContext->_winX) / 8) +
                (wTileY * 32));

            if (_lcd->bgwDataArea() == 0x8800) {
                _context._pfc._bgwFetchData[0] += 128;
            }
        }
    }
}

void Ppu::pipelineFetch()
{
    switch (_context._pfc._currentFetchState) {
    case FS_TILE: {
        _context._fetchedEntryCount = 0;
        
        if (_lcd->isBgwEable()) {
            _context._pfc._bgwFetchData[0] = EmuGet()->getCart()->busRead(
                _lcd->bgMapArea() +
                (_context._pfc._mapX / 8) +
                (((_context._pfc._mapY / 8)) * 32));
            
            if (_lcd->bgwDataArea() == 0x8800) {
                _context._pfc._bgwFetchData[0] += 128;
            }

            pipelineLoadWindowTile();
        }
        
        if (_lcd->isObjEable() && _context._lineSprites) {
            pipelineLoadSpriteTile();
        }

        _context._pfc._currentFetchState = FS_DATA0;
        _context._pfc._fetchX += 8;
    } break;

    case FS_DATA0: {
        _context._pfc._bgwFetchData[1] = EmuGet()->getCart()->busRead(
            _lcd->bgwDataArea() +
            (_context._pfc._bgwFetchData[0] * 16) +
            _context._pfc._tileY);

        pipelineLoadSpriteData(0);

        _context._pfc._currentFetchState = FS_DATA1;
    } break;

    case FS_DATA1: {
        _context._pfc._bgwFetchData[2] = EmuGet()->getCart()->busRead(_lcd->bgwDataArea() +
            (_context._pfc._bgwFetchData[0] * 16) +
            _context._pfc._tileY + 1);

        pipelineLoadSpriteData(1);

        _context._pfc._currentFetchState = FS_IDLE;

    } break;

    case FS_IDLE: {
        _context._pfc._currentFetchState = FS_PUSH;
    } break;

    case FS_PUSH: {
        if (pipelineFifoAdd()) {
            _context._pfc._currentFetchState = FS_TILE;
        }

    } break;

    }
}

void Ppu::pipelinePushPixel()
{
    LcdContext* lcdContext = _lcd->getContext();
    if (_context._pfc._pixelFifo._size > 8) {
        u32 pixelData = pixelFifoPop();

        if (_context._pfc._lineX >= (lcdContext->_scrollX % 8)) {
            _context._videoBuffer[_context._pfc._pushedX +
                (lcdContext->_ly * XRES)] = pixelData;

            _context._pfc._pushedX++;
        }

        _context._pfc._lineX++;
    }
}

void Ppu::pipelineProcess()
{
    LcdContext* lcdContext = _lcd->getContext();
    _context._pfc._mapY = (lcdContext->_ly + lcdContext->_scrollY);
    _context._pfc._mapX = (_context._pfc._fetchX + lcdContext->_scrollX);
    _context._pfc._tileY = ((lcdContext->_ly + lcdContext->_scrollY) % 8) * 2;

    if (!(_context._lineTicks & 1)) {
        pipelineFetch();
    }

    pipelinePushPixel();
}

void Ppu::pipelineFifoReset()
{
    while (_context._pfc._pixelFifo._size) {
        pixelFifoPop();
    }

    _context._pfc._pixelFifo._head = 0;
}


