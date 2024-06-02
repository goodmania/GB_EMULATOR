#pragma once

#include <common.h>

struct GamepadState{
    bool _start;
    bool _select;
    bool _a;
    bool _b;
    bool _up;
    bool _down;
    bool _left;
    bool _right;
};

struct GamepadContext {
    bool _buttonSel;
    bool _dirSel;
    GamepadState _controller;
};

class Gamepad {
public:
    void initialize();
    bool buttonSel();
    bool dirSel();
    void setSel(u8 value);

    GamepadContext* getContext() { return &_context; }
    GamepadState* getState() { return &_context._controller; }
    u8 getOutput();
private:
    GamepadContext _context;
};
