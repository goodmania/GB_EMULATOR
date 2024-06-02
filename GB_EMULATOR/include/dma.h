#pragma once

#include <common.h>

struct DmaContext{
    bool _active;
    u8 _byte;
    u8 _value;
    u8 _startDelay;
};

class Dma {
public:
    void start(u8 start);
    void tick();
    bool transferring();
private:
    DmaContext _context;
};


