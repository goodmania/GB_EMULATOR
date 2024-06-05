#include "cart.h"
#include "cpu.h"
#include "dma.h"
#include "emu.h"
#include "io.h"
#include "ppu.h"
#include "ram.h"

#include <map>

namespace{

static const char* ROM_TYPES[] = {
    "ROM ONLY",
    "MBC1",
    "MBC1+RAM",
    "MBC1+RAM+BATTERY",
    "0x04 ???",
    "MBC2",
    "MBC2+BATTERY",
    "0x07 ???",
    "ROM+RAM 1",
    "ROM+RAM+BATTERY 1",
    "0x0A ???",
    "MMM01",
    "MMM01+RAM",
    "MMM01+RAM+BATTERY",
    "0x0E ???",
    "MBC3+TIMER+BATTERY",
    "MBC3+TIMER+RAM+BATTERY 2",
    "MBC3",
    "MBC3+RAM 2",
    "MBC3+RAM+BATTERY 2",
    "0x14 ???",
    "0x15 ???",
    "0x16 ???",
    "0x17 ???",
    "0x18 ???",
    "MBC5",
    "MBC5+RAM",
    "MBC5+RAM+BATTERY",
    "MBC5+RUMBLE",
    "MBC5+RUMBLE+RAM",
    "MBC5+RUMBLE+RAM+BATTERY",
    "0x1F ???",
    "MBC6",
    "0x21 ???",
    "MBC7+SENSOR+RUMBLE+RAM+BATTERY",
};

static std::map<u32, const char*> LIC_CODE = {
    {0x00, "None"},
    {0x01, "Nintendo R&D1"},
    {0x08, "Capcom"},
    {0x13, "Electronic Arts"},
    {0x18, "Hudson Soft" },
    {0x19, "b-ai"},
    {0x20, "kss"},
    {0x22, "pow"},
    {0x24, "PCM Complete"},
    {0x25, "san-x" },
    {0x28, "Kemco Japan"},
    {0x29, "seta"},
    {0x30, "Viacom"},
    {0x31, "Nintendo"},
    {0x32, "Bandai"},
    {0x33, "Ocean/Acclaim"},
    {0x34, "Konami"},
    {0x35, "Hector"},
    {0x37, "Taito"},
    {0x38, "Hudson"},
    {0x39, "Banpresto"},
    {0x41, "Ubi Soft"},
    {0x42, "Atlus" },
    {0x44, "Malibu"},
    {0x46, "angel"},
    {0x47, "Bullet-Proof"},
    {0x49, "irem"},
    {0x50, "Absolute"},
    {0x51, "Acclaim"},
    {0x52, "Activision"},
    {0x53, "American sammy"},
    {0x54, "Konami"},
    {0x55, "Hi tech entertainment"},
    {0x56, "LJN"},
    {0x57, "Matchbox"},
    {0x58, "Mattel"},
    {0x59, "Milton Bradley"},
    {0x60, "Titus"},
    {0x61, "Virgin"},
    {0x64, "LucasArts"},
    {0x67, "Ocean"},
    {0x69, "Electronic Arts"},
    {0x70, "Infogrames"},
    {0x71, "Interplay"},
    {0x72, "Broderbund"},
    {0x73, "sculptured"},
    {0x75, "sci"},
    {0x78, "THQ"},
    {0x79, "Accolade"},
    {0x80, "misawa"},
    {0x83, "lozc"},
    {0x86, "Tokuma Shoten Intermedia"},
    {0x87, "Tsukuda Original"},
    {0x91, "Chunsoft"},
    {0x92, "Video system"},
    {0x93, "Ocean/Acclaim"},
    {0x95, "Varie"},
    {0x96, "Yonezawa/sfpal"},
    {0x97, "Kaneko"},
    {0x99, "Pack in soft"},
    {0xA4, "Konami (Yu-Gi-Oh!)"},
};
} // namespace

void Cart::initialize()
{

}

bool Cart::load(char* cart)
{
    snprintf(_context._filename, sizeof(_context._filename), "%s", cart);
    FILE* fp = nullptr;
    fopen_s(&fp, cart, "r");

    if (!fp)
    {
        printf("Failed to open: %s\n", cart);
        return false;
    }

    printf("Opened: %s\n", _context._filename);

    fseek(fp, 0, SEEK_END); // move file pointer to the end
    _context._romSize = ftell(fp); // get the current position of the file pointer
    rewind(fp); // move the file pointer back to the beginning

    _context._romData = new u8[_context._romSize]; // todo : delete
    fread(_context._romData, _context._romSize, 1, fp);
    fclose(fp);

    _context._header = (RomHeader*)(_context._romData + 0x100);
    _context._header->_title[15] = 0;

    printf("Cartridge Loaded:\n");
    printf("\t Title    : %s\n", _context._header->_title);
    printf("\t Type     : %2.2X (%s)\n", _context._header->_type, cartTypeName());
    printf("\t ROM Size : %d KB\n", 32 << _context._header->_romSize);
    printf("\t RAM Size : %2.2X\n", _context._header->_ramSize);
    printf("\t LIC Code : %2.2X (%s)\n", _context._header->_licCode, cartLicName());
    printf("\t ROM Vers : %2.2X\n", _context._header->_version);

    u16 x = 0;
    for (u16 i = 0x0134; i <= 0x014C; i++) {
        x = x - _context._romData[i] - 1;
    }

    printf("\t Checksum : %2.2X (%s)\n", _context._header->_checkSum, (x & 0xFF) ? "PASSED" : "FAILED");

    return true;
}

u8 Cart::read(u16 address)
{
    if (!mbc1() || address < 0x4000) {
        return _context._romData[address];
    }

    if ((address & 0xE000) == 0xA000) {
        if (!_context._ramEnabled) {
            return 0xFF;
        }

        if (!_context._ramBank) {
            return 0xFF;
        }

        return _context._ramBank[address - 0xA000];
    }

    return _context._romBankX[address - 0x4000];
}

void Cart::write(u16 address, u8 value)
{
    if (!mbc1()) {
        return;
    }

    if (address < 0x2000) {
        _context._ramEnabled = ((value & 0xF) == 0xA);
    }

    if ((address & 0xE000) == 0x2000) {
        //rom bank number
        if (value == 0) {
            value = 1;
        }

        value &= 0b11111;

        _context._romBankValue = value;
        _context._romBankX = _context._romData + (0x4000 * _context._romBankValue);
    }

    if ((address & 0xE000) == 0x4000) {
        //ram bank number
        _context._ramBankValue = value & 0b11;

        if (_context._ramBanking) {
            if (needSave()) {
                batterySave();
            }

            _context._ramBank = _context._ramBanks[_context._ramBankValue];
        }
    }

    if ((address & 0xE000) == 0x6000) {
        //banking mode select
        _context._bankingMode = value & 1;

        _context._ramBanking = _context._bankingMode;

        if (_context._ramBanking) {
            if (needSave()) {
                batterySave();
            }

            _context._ramBank = _context._ramBanks[_context._ramBankValue];
        }
    }

    if ((address & 0xE000) == 0xA000) {
        if (!_context._ramEnabled) {
            return;
        }

        if (!_context._ramBank) {
            return;
        }

        _context._ramBank[address - 0xA000] = value;

        if (_context._battery) {
            _context._needSave = true;
        }
    }
}

bool Cart::needSave()
{
    return _context._needSave;
}

bool Cart::mbc1() {
    return BETWEEN(_context._header->_type, 1, 3);
}

bool Cart::battery() {
    //mbc1 only for now...
    return _context._header->_type == 3;
}

void Cart::batteryLoad()
{
    if (!_context._ramBank) {
        return;
    }

    char fn[1048];
    sprintf(fn, "%s.battery", _context._filename);
    FILE* fp = fopen(fn, "rb");

    if (!fp) {
        fprintf(stderr, "FAILED TO OPEN: %s\n", fn);
        return;
    }

    fread(_context._ramBank, 0x2000, 1, fp);
    fclose(fp);
}

void Cart::batterySave()
{
    if (!_context._ramBank) {
        return;
    }

    char fn[1048];
    sprintf(fn, "%s.battery", _context._filename);
    FILE* fp = fopen(fn, "wb");

    if (!fp) {
        fprintf(stderr, "FAILED TO OPEN: %s\n", fn);
        return;
    }

    fwrite(_context._ramBank, 0x2000, 1, fp);
    fclose(fp);
}

const char* Cart::cartLicName()
{
    if (_context._header->_newLicCode <= 0xA4) {
        return LIC_CODE[_context._header->_licCode];
    }
    return "UNKNOWN";
}

const char* Cart::cartTypeName()
{
    if (_context._header->_type <= 0x22) {
        return ROM_TYPES[_context._header->_type];
    }
    return "UNKNOWN";
}

// Bus read / write

// 0x0000 - 0x3FFF : ROM Bank 0
// 0x4000 - 0x7FFF : ROM Bank 1 - Switchable
// 0x8000 - 0x97FF : CHR RAM
// 0x9800 - 0x9BFF : BG Map 1
// 0x9C00 - 0x9FFF : BG Map 2
// 0xA000 - 0xBFFF : Cartridge RAM
// 0xC000 - 0xCFFF : RAM Bank 0
// 0xD000 - 0xDFFF : RAM Bank 1-7 - switchable - Color only
// 0xE000 - 0xFDFF : Reserved - Echo RAM
// 0xFE00 - 0xFE9F : Object Attribute Memory
// 0xFEA0 - 0xFEFF : Reserved - Unusable
// 0xFF00 - 0xFF7F : I/O Registers
// 0xFF80 - 0xFFFE : Zero Page

u8 Cart::busRead(u16 address)
{
    Emu* emu = EmuGet();
    Cpu* cpu = emu->getCpu();
    Dma* dma = emu->getDma();
    Io* io = emu->getIo();
    Ppu* ppu = emu->getPpu();
    Ram* ram = emu->getRam();

    if (address < 0x8000) {
        //ROM Data
        return read(address);
    }
    else if (address < 0xA000) {
        //Char/Map Data
        return ppu->vramRead(address);
    }
    else if (address < 0xC000) {
        //Cartridge RAM
        return read(address);
    }
    else if (address < 0xE000) {
        //WRAM (Working RAM)
        return ram->wramRead(address);
    }
    else if (address < 0xFE00) {
        //reserved echo ram...
        return 0;
    }
    else if (address < 0xFEA0) {
        //OAM
        if (dma->transferring()) {
            return 0xFF;
        }

        return ppu->oamRead(address);
    }
    else if (address < 0xFF00) {
        //reserved unusable...
        return 0;
    }
    else if (address < 0xFF80) {
        //IO Registers...
        return io->read(address);
    }
    else if (address == 0xFFFF) {
        //CPU ENABLE REGISTER...
        return cpu->getIeRegister();
    }

    return ram->hramRead(address);

}

void Cart::busWrite(u16 address, u8 value)
{
    Emu* emu = EmuGet();
    Cpu* cpu = emu->getCpu();
    Dma* dma = emu->getDma();
    Io* io = emu->getIo();
    Ppu* ppu = emu->getPpu();
    Ram* ram = emu->getRam();

    if (address < 0x8000) {
        //ROM Data
        write(address, value);
    }
    else if (address < 0xA000) {
        //Char/Map Data
        ppu->vramWrite(address, value);
    }
    else if (address < 0xC000) {
        //EXT-RAM
        write(address, value);
    }
    else if (address < 0xE000) {
        //WRAM
        ram->wramWrite(address, value);
    }
    else if (address < 0xFE00) {
        //reserved echo ram
    }
    else if (address < 0xFEA0) {
        //OAM
        if (dma->transferring()) {
            return;
        }

        ppu->oamWrite(address, value);
    }
    else if (address < 0xFF00) {
        //unusable reserved
    }
    else if (address < 0xFF80) {
        //IO Registers...
        io->write(address, value);
    }
    else if (address == 0xFFFF) {
        //CPU SET ENABLE REGISTERs

        cpu->setIeRegister(value);
    }
    else {
        ram->hramWrite(address, value);
    }
}

u16 Cart::busRead16(u16 address)
{
    u16 lo = busRead(address);
    u16 hi = busRead(address + 1);

    return lo | (hi << 8);
}

void Cart::busWrite16(u16 address, u16 value)
{
    busWrite(address + 1, (value >> 8) & 0xFF);
    busWrite(address, value & 0xFF);
}