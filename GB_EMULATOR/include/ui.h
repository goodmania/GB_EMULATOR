#pragma once

#include <common.h>

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;
class SDL_Surface;

static const s32 SCREEN_WIDTH = 1024;
static const s32 SCREEN_HEIGHT = 768;

class Ui {
public:
	void initialize();
	void handleEvents();
	void update();
	void delay(u32 ms);
	u32 getTicks();
	void displayTile(SDL_Surface* surface, u16 startLocation, u16 tileNum, s32 x, s32 y);
	void onKey(bool down, u32 keyCode);
private:
	void updateDbgWindow();

	SDL_Window* _sdlWindow;
	SDL_Renderer* _sdlRenderer;
	SDL_Texture* _sdlTexture;
	SDL_Surface* _screen;

	SDL_Window* _sdlDebugWindow;
	SDL_Renderer* _sdlDebugRenderer;
	SDL_Texture* _sdlDebugTexture;
	SDL_Surface* _debugScreen;

	const s32 _scale = 4;
	s64 _tileColors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };
};

