#include "emu.h"

#include "cart.h"
#include "cpu.h"
#include "dma.h"
#include "gamepad.h"
#include "ppu.h"
#include "timer.h"
#include "ui.h"

#include <SDL.h>
#include <SDL_ttf.h>

namespace {
    Emu* g_emu;
} // namespace

void delay(u32 ms) 
{
    SDL_Delay(ms);
}

Emu::Emu()
{
    initialize();
    initialize1();
}

Emu::~Emu()
{
    terminate();
}

void Emu::initialize()
{
    _emuContext = new EmuContext;
    _timer = new Timer;
    _cart = new Cart;
    _cpu = new Cpu;
    _ppu = new Ppu;
    _dma = new Dma;
    _ui = new Ui;
    _gamePad = new Gamepad;

    if (_emuContext)
    {
        _emuContext->_running = true;
        _emuContext->_paused = false;
        _emuContext->_ticks = 0;
    }
}

void Emu::initialize1()
{
    g_emu = this;
}

void Emu::terminate()
{
    delete _emuContext;
    delete _timer;
    delete _cpu;
    delete _cart;
    delete _dma;
    delete _ui;
    delete _gamePad;
}

Emu::EmuContext* Emu::getEmuContext()
{
    return _emuContext;
}

s32 Emu::emuRun(s32 argc, char** argv) {

    if (argc < 2) {
        printf("Usage: emu <rom_file>\n");
        return -1;
    }

    if (!_cart->cartLoad(argv[1])) {
        printf("Failed to load ROM file: %s\n", argv[1]);
        return -2;
    }

    printf("Cart loaded..\n");

    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");
    TTF_Init();
    printf("TTF INIT\n");

    _cpu->initialize();

    while (_emuContext->_running) {
        if (_emuContext->_paused) {
            delay(10);
            continue;
        }

        if (!_cpu->step()) {
            printf("CPU Stopped\n");
            return -3;
        }

        _emuContext->_ticks++;
    }

    return 0;
}

void Emu::emuCycles(s32 cpuCycles) {
    for (s32 i = 0; i < cpuCycles; i++) {
        for (s32 n = 0; n < 4; n++) {
            _emuContext->_ticks++;
            _timer->tick();
            _ppu->tick();
        }
        _dma->tick();
    }
}

Emu* EmuGet()
{
    if (g_emu)
    {
        return g_emu;
    }
    return nullptr;
}