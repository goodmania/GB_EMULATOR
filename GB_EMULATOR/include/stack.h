#pragma once

#include <common.h>

class Stack {
public:
	void stack_push(u8 data);
	void stack_push16(u16 data);
	u8 stack_pop();
	u16 stack_pop16();

private:

};


