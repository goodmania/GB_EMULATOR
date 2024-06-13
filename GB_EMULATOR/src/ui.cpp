#include "bus.h"
#include "cart.h"
#include "emu.h"
#include "gamepad.h"
#include "ppu.h"
#include "ui.h"

#include <SDL.h>
#include <SDL_ttf.h>

Ui::Ui() {
    
}

void Ui::initialize()
{
    SDL_Init(SDL_INIT_VIDEO);
    printf("SDL INIT\n");
    TTF_Init();
    printf("TTF INIT\n");

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &_sdlWindow, &_sdlRenderer);

    _screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
        0x00FF0000,
        0x0000FF00,
        0x000000FF,
        0xFF000000);
    _sdlTexture = SDL_CreateTexture(_sdlRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH, SCREEN_HEIGHT);

    SDL_CreateWindowAndRenderer(
        16 * 8 * _scale,
        32 * 8 * _scale,
        0,
        &_sdlDebugWindow,
        &_sdlDebugRenderer
    );

    _debugScreen = SDL_CreateRGBSurface(
        0,
        (16 * 8 * _scale) + (16 * _scale),
        (32 * 8 * _scale) + (64 * _scale),
        32,
        0x00FF0000,
        0x0000FF00,
        0x000000FF,
        0xFF000000
    );

    _sdlDebugTexture = SDL_CreateTexture(
        _sdlDebugRenderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        (16 * 8 * _scale) + (16 * _scale),
        (32 * 8 * _scale) + (64 * _scale)
    );

    s32 x, y;
    SDL_GetWindowPosition(_sdlWindow, &x, &y);
    SDL_SetWindowPosition(_sdlDebugWindow, x + SCREEN_WIDTH + 10, y);

    onKey(false, SDLK_z);
    onKey(false, SDLK_x);
    onKey(false, SDLK_RETURN);
    onKey(false, SDLK_TAB);
    onKey(false, SDLK_UP);
    onKey(false, SDLK_DOWN);
    onKey(false, SDLK_LEFT);
    onKey(false, SDLK_RIGHT);
}

void Ui::handleEvents()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) > 0)
    {
        if (e.type == SDL_KEYUP) {
            onKey(false, e.key.keysym.sym);
        }

        if (e.type == SDL_KEYDOWN) {
            onKey(true, e.key.keysym.sym);
        }

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_CLOSE) {
            EmuGet()->getEmuContext()->_die = true;
        }
    }
}

void Ui::onKey(bool down, u32 keyCode) {
    GamepadState* state = EmuGet()->getGamepad()->getState();
    switch (keyCode) {
    case SDLK_z: state->_b = down; break;
    case SDLK_x: state->_a = down; break;
    case SDLK_RETURN: state->_start = down; break;
    case SDLK_TAB: state->_select = down; break;
    case SDLK_UP: state->_up = down; break;
    case SDLK_DOWN: state->_down = down; break;
    case SDLK_LEFT: state->_left = down; break;
    case SDLK_RIGHT: state->_right = down; break;
    }
}

void Ui::update()
{
    SDL_Rect rc;
    rc.x = rc.y = 0;
    rc.w = rc.h = 2048;
    
    u32* video_buffer = EmuGet()->getPpu()->getContext()->_videoBuffer;

    for ( s32 lineNum = 0; lineNum < YRES; lineNum++) {
        for (s32 x = 0; x < XRES; x++) {
            rc.x = x * _scale;
            rc.y = lineNum * _scale;
            rc.w = _scale;
            rc.h = _scale;

            SDL_FillRect(_screen, &rc, video_buffer[x + (lineNum * XRES)]);
        }
    }

    SDL_UpdateTexture(_sdlTexture, nullptr, _screen->pixels, _screen->pitch);
    SDL_RenderClear(_sdlRenderer);
    SDL_RenderCopy(_sdlRenderer, _sdlTexture, nullptr, nullptr);
    SDL_RenderPresent(_sdlRenderer);

    updateDbgWindow();
}

void Ui::delay(u32 ms)
{
    SDL_Delay(ms);
}

u32 Ui::getTicks()
{
    return SDL_GetTicks();
}

void Ui::displayTile(SDL_Surface* surface, u16 startLocation, u16 tileNum, s32 x, s32 y)
{
    SDL_Rect rc;
    Cart* cart = EmuGet()->getCart();

    for (s32 tileY = 0; tileY < 16; tileY += 2) {
        u8 b1 = cart->busRead(startLocation + (tileNum * 16) + tileY);
        u8 b2 = cart->busRead(startLocation + (tileNum * 16) + tileY + 1);

        for (s32 bit = 7; bit >= 0; bit--) {
            u8 hi = !!(b1 & (1 << bit)) << 1;
            u8 lo = !!(b2 & (1 << bit));

            u8 color = hi | lo;

            rc.x = x + ((7 - bit) * _scale);
            rc.y = y + (tileY / 2 * _scale);
            rc.w = _scale;
            rc.h = _scale;

            SDL_FillRect(surface, &rc, (Uint32)_tileColors[color]);
        }
    }
}

void Ui::updateDbgWindow()
{
    int xDraw = 0;
    int yDraw = 0;
    int tileNum = 0;

    SDL_Rect rc;
    rc.x = 0;
    rc.y = 0;
    rc.w = _debugScreen->w;
    rc.h = _debugScreen->h;
    SDL_FillRect(_debugScreen, &rc, 0xFF111111);

    u16 addr = 0x8000;

    //384 tiles, 24 x 16
    for (int y = 0; y < 24; y++) {
        for (int x = 0; x < 16; x++) {
            displayTile(_debugScreen, addr, tileNum, xDraw + (x * _scale), yDraw + (y * _scale));
            xDraw += (8 * _scale);
            tileNum++;
        }

        yDraw += (8 * _scale);
        xDraw = 0;
    }

    SDL_UpdateTexture(_sdlDebugTexture, NULL, _debugScreen->pixels, _debugScreen->pitch);
    SDL_RenderClear(_sdlDebugRenderer);
    SDL_RenderCopy(_sdlDebugRenderer, _sdlDebugTexture, NULL, NULL);
    SDL_RenderPresent(_sdlDebugRenderer);
}
