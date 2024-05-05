#pragma once

#include <common.h>

class Cart {
public:

	struct RomHeader {
		u8 _entry[4];
		u8 _logo[0x30]; //  0x30 = 40

		char _title[16];
		u16 _newLicCode;
		u8 _sgbFlag;
		u8 _type;
		u8 _romSize;
		u8 _ramSize;
		u8 _destCode;
		u8 _licCode;
		u8 _version;
		u8 _checkSum;
		u16 _globalCheckSum;
	};

	void initialize();
	bool cartLoad(char* cart);
	
private:
	const char* cartLicName();
	const char* cartTypeName();
};