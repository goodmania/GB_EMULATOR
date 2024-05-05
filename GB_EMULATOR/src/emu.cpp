#include <emu.h>
#include <cart.h>
#include <cpu.h>
#include <SDL.h>
#include <SDL_ttf.h>

void delay(u32 ms) 
{
    SDL_Delay(ms);
}

Emu::Emu()
{
    initialize();
}

Emu::~Emu()
{
    terminate();
}

void Emu::initialize()
{
    _emuContext = new EmuContext;
    _cpu = new Cpu;
    _cart = new Cart;

    if (_emuContext)
    {
        _emuContext->_running = true;
        _emuContext->_paused = false;
        _emuContext->_ticks = 0;
    }
}

void Emu::terminate()
{
    delete _emuContext;
    delete _cpu;
    delete _cart;
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