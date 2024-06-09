#pragma once

#include <common.h>

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

struct CartContext {
	char _filename[1024];
	u32 _romSize;
	u8* _romData;
	RomHeader* _header;

	//mbc1 related data
	bool _ramEnabled;
	bool _ramBanking;

	u8* _romBankX;
	u8 _bankingMode;

	u8 _romBankValue;
	u8 _ramBankValue;

	u8* _ramBank; //current selected ram bank
	u8* _ramBanks[16]; //all ram banks

	//for battery
	bool _battery; //has battery
	bool _needSave; //should save battery backup.
};

class Cart {
public:
	Cart() = default;
	~Cart();
	void initialize();
	bool load(char* cart);
	u8 read(u16 address);
	void write(u16 address, u8 value);

	bool mbc1();
	bool battery();
	void setupBanking();

	bool needSave();
	void batteryLoad();
	void batterySave();

	u8 busRead(u16 address);
	void busWrite(u16 address, u8 value);
	u16 busRead16(u16 address);
	void busWrite16(u16 address, u16 value);

private:
	const char* cartLicName();
	const char* cartTypeName();

	CartContext _context;
};