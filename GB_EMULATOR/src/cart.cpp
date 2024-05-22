#include <cart.h>
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

bool Cart::cartLoad(char* cart)
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
    //for now just ROM ONLY type supported...

    return _context._romData[address];
}

void Cart::write(u16 address, u8 value)
{
    //for now, ROM ONLY...
    NO_IMPL
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
