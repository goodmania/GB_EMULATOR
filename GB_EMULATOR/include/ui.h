#pragma once

#include <common.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_Surface;

static const s32 SCREEN_WIDTH = 1024;
static const s32 SCREEN_HEIGHT = 768;

class Ui {
public:
	Ui();
	~Ui() = default;

	void initialize();
	void handleEvents();
	void update();
	void delay(u32 ms);
	u32 getTicks();
	void displayTile(SDL_Surface* surface, u16 startLocation, u16 tileNum, s32 x, s32 y);
	void onKey(bool down, u32 keyCode);
private:
	void updateDbgWindow();

	SDL_Window* _sdlWindow = nullptr;
	SDL_Renderer* _sdlRenderer = nullptr;
	SDL_Texture* _sdlTexture = nullptr;
	SDL_Surface* _screen = nullptr;

	SDL_Window* _sdlDebugWindow = nullptr;
	SDL_Renderer* _sdlDebugRenderer = nullptr;
	SDL_Texture* _sdlDebugTexture = nullptr;
	SDL_Surface* _debugScreen = nullptr;

	const s32 _scale = 4;
	s64 _tileColors[4] = { 0xFFFFFFFF, 0xFFAAAAAA, 0xFF555555, 0xFF000000 };
};

