#include "dllmain.h"

HMODULE module = NULL;

static void Thread()
{
	HMODULE samp = GetModuleHandleA("samp.dll");

	if (samp)
	{
		std::string language;

		std::fstream stream;

		stream.open("samp-lang.ini", std::fstream::in);

		if (!stream) // File doesn't exist? Let's create it
		{
			stream.open("samp-lang.ini", std::fstream::out | std::fstream::trunc);

			stream << "English";

			language = "English"; // This is on purpose, as it will try to open the "English" language file and free the library since it doesn't exist.
		}
		else
		{
			stream >> language;
		}

		stream.close();

		stream.clear();

		stream.open("samp-lang\\" + language + ".ini", std::fstream::in);

		if(stream)
		{
			Pattern pattern(GetCurrentProcess(), samp);

			static std::string strings[73];
			DWORD offsets[73], OldProtect = NULL;

			offsets[0] = pattern.FindPattern("\x50\x8D\x4C\x24\x0C\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x8D\x44\x24\x14", "xxxxxx????xx????xxxx") + 0x6; // \r\nGame Version: %s\r\n
			offsets[1] = pattern.FindPattern("\x8B\x49\x04\x8B\x89\x00\x00\x00\x00\x51\x8B\x4C\x24\x08\x51\x52\x68\x00\x00\x00\x00\x50", "xxxxx????xxxxxxxx????x") + 0x11; // Warning(%s): Exception 0x%X at 0x%X
			offsets[2] = pattern.FindPattern("\x51\x8B\x0D\x00\x00\x00\x00\x52\x51\x68\x00\x00\x00\x00\xEB\x15", "xxx????xxx????xx") + 0xA; // Warning(opcode 0x%X): Exception 0x%X at 0x%X
			offsets[3] = pattern.FindPattern("\x89\x0A\x8B\x0D\x00\x00\x00\x00\x89\x42\x04\x81\xEF\x00\x00\x00\x00\x68\x00\x00\x00\x00\x03\xF0\x33\xED", "xxxx????xxxxx????x????xxxx") + 0x12; // Client Network Stats
			offsets[4] = pattern.FindPattern("\x8B\x4C\x24\x3C\x8B\x54\x24\x50\xC1\xE9\x0A\x51\xC1\xEA\x0A\x52\x68", "xxxxxxxxxxxxxxxxx") + 0x11; // Process Mem: %uKB Working Set: %uKB\n
			offsets[5] = pattern.FindPattern("\x8B\x0D\x00\x00\x00\x00\x51\xC1\xE8\x14\x50\xC1\xEE\x14\x56\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x83\xC4\x14", "xx????xxxxxxxxxx????x????x????xxx") + 0x10; // Streaming Mem: %uMB Total: %uMB System: %uMB\n
			offsets[6] = pattern.FindPattern("\xE8\x00\x00\x00\x00\x85\xC0\x8B\x4C\x24\x1C\x75\x0D\xE8\x00\x00\x00\x00\x50\x68\x00\x00\x00\x00\xEB\x0B\xE8\x00\x00\x00\x00\x50\x68", "x????xxxxxxxxx????xx????xxx????xx") + 0x21; // InCar Send Rate: %u\n
			offsets[7] = pattern.FindPattern("\x8B\x4C\x24\x18\x85\xC9\x74\x66\xE8\x00\x00\x00\x00\x85\xC0\x8B\x4C\x24\x1C\x75\x0D\xE8\x00\x00\x00\x00\x50\x68\x00\x00\x00\x00\xEB\x0B", "xxxxxxxxx????xxxxxxxxx????xx????xx") + 0x1C; // OnFoot Send Rate: %u\n
			offsets[8] = pattern.FindPattern("\x8B\x54\x24\x3C\x50\x52\x55\x53\x8D\x84\x24\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x8D\x84\x24\x00\x00\x00\x00\x83\xC4\x28", "xxxxxxxxxxx????x????xx????xxx????xxx") + 0x10; // Vehicles: %u\nVehicle Models: %u\nVehicle Models Loaded: %u\nRWObjects: %u\nPedSlotsUsed: %u\nObjectSlotsUsed: %u\nShotCompMode: %u\nFreeIDEs: %u\n
			offsets[9] = pattern.FindPattern("\x8B\x15\x00\x00\x00\x00\x8B\x82\x00\x00\x00\x00\x8B\x48\x18\x8B\x41\x22\x8B\x10\x83\xEC\x10\xDD\x5C\x24\x08\x89\x44\x24\x2C\x89\x54\x24\x28\xDD\x1C\x24", "xx????xx????xxxxxxxxxxxxxxxxxxxxxxxxxx") + 0x27; // Download Rate: %.2f KB/s\nUpload Rate: %.2f KB/s\n
			offsets[10] = pattern.FindPattern("\x5E\x59\xC3\xA1\x00\x00\x00\x00\x85\xC0\x74\x0E\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\x5E\x59\xC3", "xxxx????xxxxx????xx????xxxxxx") + 0xD; // pagesize [10-20] (lines)
			offsets[11] = pattern.FindPattern("\x5E\x83\xC4\x04\xE9\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x85\xC0\x74\x0E\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\x5E\x59\xC3", "xxxxx????x????xxxxx????xx????xxxxxx") + 0x13; // Valid fontsize: -3 to 5
			offsets[12] = pattern.FindPattern("\x6A\x01\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x85\xC0\x74\x0E\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\xC3", "xxx????x????x????xxxxx????xx????xxxx") + 0x16; // Audio messages: Off
			offsets[13] = pattern.FindPattern("\x85\xC0\x74\x32\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\xC3\x6A\x01", "xxxxx????xx????xxxxxx") + 0x5; // Audio messages: On
			offsets[14] = pattern.FindPattern("\xA1\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x50\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x0C\xC3", "x????xx????xx????xx????xxxx") + 0xD; // Memory: %u
			offsets[15] = pattern.FindPattern("\x83\xC4\x0C\x5E\x59\xC3\x8B\x0D\x00\x00\x00\x00\x56\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x0C\x5E\x59\xC3", "xxxxxxxx????xx????xx????xxxxxx") + 0xE; // -> Frame Limiter: valid amounts are 20-90
			offsets[16] = pattern.FindPattern("\x74\x0D\x6A\x00\x56\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x56\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x0C\x5E\x59\xC3", "xxxxxx????x????x????xx????xx????xxxxxx") + 0x16; // -> Frame Limiter: %u
			offsets[17] = pattern.FindPattern("\x85\xC9\x74\x0E\x6A\x00\x6A\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x08", "xxxxxxxxx????x????xx????x????xx????xxx") + 0x19; // -> Head movements enabled
			offsets[18] = pattern.FindPattern("\xA1\x00\x00\x00\x00\x85\xC0\x74\x6D\x8B\x48\x65\x85\xC9\x74\x33\xC7\x40\x00\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x0E\x6A\x00\x6A\x01", "x????xxxxxxxxxxxxx?????xx????xxxxxxxx") + 0x35; // -> Head movements disabled
			offsets[19] = pattern.FindPattern("\x83\xEC\x20\xDD\x5C\x24\x18\x8B\xCE\xD9\x40\x38\xDD\x5C\x24\x10\xD9\x40\x34\xDD\x5C\x24\x08\xD9\x40\x30\xDD\x1C\x24\xE8\x00\x00\x00\x00\x50", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????x") + 0x3B; // -> OnFoot position saved
			offsets[20] = pattern.FindPattern("\xDD\x5C\x24\x18\xD9\x40\x38\xDD\x5C\x24\x10\xD9\x40\x34\xDD\x5C\x24\x08\xD9\x40\x30\x0F\xB7\x46\x22\xDD\x1C\x24\x50\x68\x00\x00\x00\x00\x57", "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx????x") + 0x35; // -> InCar position saved
			offsets[21] = pattern.FindPattern("\x8D\x4C\x24\x1C\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x8B\xF8\x83\xC4\x14\x85\xFF\x75\x1B\x8B\x15\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????xx????xxxxxxxxxxx????x????") + 0x1F; // I can't open the savepositions.txt file for append.
			offsets[22] = pattern.FindPattern("\xDD\x5C\x24\x18\xD9\x40\x38\xDD\x5C\x24\x10\xD9\x40\x34\xDD\x5C\x24\x08\xD9\x40\x30\xDD\x1C\x24\x68\x00\x00\x00\x00\x56\xE8\x00\x00\x00\x00\x56", "xxxxxxxxxxxxxxxxxxxxxxxxx????xx????x") + 0x30; // -> OnFoot pos saved
			offsets[23] = pattern.FindPattern("\x8D\x54\x24\x28\x68\x00\x00\x00\x00\x52\xE8\x00\x00\x00\x00\x8B\xF0\x83\xC4\x20\x85\xF6\x75\x19\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxx????xx????xxxxxxxxxx????x????") + 0x1E; // I can't open the rawvehicles.txt file for append.
			offsets[24] = pattern.FindPattern("\x0F\xB7\x46\x22\xDD\x1C\x24\x50\x68\x00\x00\x00\x00\x57\xE8\x00\x00\x00\x00\x57\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00\x51", "xxxxxxxxx????xx????xx????xx????x????x") + 0x20; // -> InCar pos saved
			offsets[25] = pattern.FindPattern("\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x8B\x4C\x24\x08\x8B\x15\x00\x00\x00\x00\x51\x68\x00\x00\x00\x00\x52\xE8\x00\x00\x00\x00\x83\xC4\x18\xC3", "x????x????xxxxxx????xx????xx????xxxx") + 0x16; // Current Interior: %u
			offsets[26] = pattern.FindPattern("\x8B\x0D\x00\x00\x00\x00\x8B\x41\x28\x85\xC0\x75\x1F\x6A\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x6A\x00", "xx????xxxxxxxxxx????x????x????x????xx") + 0x1F; // SA-MP Keys:
			offsets[27] = pattern.FindPattern("\x8B\x0D\x00\x00\x00\x00\x8B\x41\x28\x85\xC0\x75\x1F\x6A\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\x6A\x00", "xx????xxxxxxxxxx????x????x????x????xx") + 0x1A; // {BB9999}F1\t{BBBBBB}Display this help dialog\n{BB9999}Tab\t{BBBBBB}Display the scoreboard\n{BB9999}F4\t{BBBBBB}Allows you to change class next time you respawn\n{BB9999}F5\t{BBBBBB}Show bandwidth statistics\n{BB9999}F7\t{BBBBBB}Toggle the chat box\n{BB9999}F8\t{BBBBBB}Take a screenshot\n{BB9999}F9\t{BBBBBB}Toggle the deathwindow\n{BB9999}T/F6\t{BBBBBB}Allows you to enter a chat message\n{BB9999}G\t{BBBBBB}Enter vehicle as passenger\n
			offsets[28] = pattern.FindPattern("\xDD\x1C\x24\xE8\x00\x00\x00\x00\x0F\xB7\x57\x04\x50\x52\x8D\x44\x24\x64\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00", "xxxx????xxxxxxxxxxx????xx????xx????") + 0x13; // [id: %d, type: %d]\nDistance: %.2fm
			offsets[29] = pattern.FindPattern("\x68\x00\x00\x00\x00\x89\x5A\x08\x6A\x00\x89\x42\x0C\xE8\x00\x00\x00\x00\xD9\x46\x28\xD8\x4E\x14\xE8\x00\x00\x00\x00\x8B\x54\x24\x2C\x6A\x01\x68\x00\x00\x00\x00", "x????xxxxxxxxx????xxxxxxx????xxxxxxx????") + 0x1; // score
			offsets[30] = pattern.FindPattern("\x03\xF8\x03\xD8\x83\xEC\x10\x8B\xC4\x89\x38\x89\x68\x04\x89\x58\x08\x68\x00\x00\x00\x00\x89\x48\x0C\x8B\x0D\x00\x00\x00\x00\x6A\x00\xE8\x00\x00\x00\x00", "xxxxxxxxxxxxxxxxxx????xxxxx????xxx????") + 0x12; // name
			offsets[31] = pattern.FindPattern("\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\xD9\x46\x08\xD8\x05\x00\x00\x00\x00\x83\xC4\x0C\xE8\x00\x00\x00\x00\xD9\x46\x0C\xD8\x05\x00\x00\x00\x00", "x????xx????xxxxx????xxxx????xxxxx????") + 0x1; // Players: %d
			offsets[32] = pattern.FindPattern("\x50\xE8\x00\x00\x00\x00\x83\xC4\x0C\xEB\x14\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x08\x8B\x44\x24\x44", "xx????xxxxxxx????x????xx????xxxxxxx") + 0x12; // Unable to save screenshot.
			offsets[33] = pattern.FindPattern("\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x0C\xEB\x14\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x08", "x????xx????xxxxxxx????x????xx????xxx") + 0x1; // Screenshot Taken - sa-mp-%03i.png
			offsets[34] = pattern.FindPattern("\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x5C\x6A\x00\x6A\x01\x68\x00\x00\x00\x00\x8D\x8C\x24\x00\x00\x00\x00\x51", "x????xx????xxxxxxxx????xxx????x") + 0x1; // [id: %d, type: %d subtype: %d Health: %.1f preloaded: %u]\nDistance: %.2fm\nPassengerSeats: %u\ncPos: %.3f,%.3f,%.3f\nsPos: %.3f,%.3f,%.3f
			offsets[35] = pattern.FindPattern("\xA1\x00\x00\x00\x00\x85\xC0\x74\x0F\x57\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x0C\x5F\x5E\x33\xC0", "x????xxxxxx????xx????xxxxxxx") + 0xB; // Warning: Model %u wouldn't load in time!
			offsets[36] = pattern.FindPattern("\x66\x8B\x35\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x85\xC0\x74\x12\x0F\xB7\xCE\x51\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x0C", "xxx????x????xxxxxxxxx????xx????xxx") + 0x15; // Removed player %u due to error.
			offsets[37] = pattern.FindPattern("\xBE\x00\x00\x00\x00\x8B\xFD\xF3\xA5\x66\xA5\xA4\xC6\x85\x00\x00\x00\x00\x00\x8A\x85\x00\x00\x00\x00\x84\xC0\x5B\x74\x30\x8B\xC5\x8D\x50\x01", "x????xxxxxxxxx?????xx????xxxxxxxxxx") + 0x1; // Error: unmatched tilde
			offsets[38] = pattern.FindPattern("\xBE\x00\x00\x00\x00\x8B\xFD\xF3\xA5\xC6\x85\x00\x00\x00\x00\x00\x83\xBD\x00\x00\x00\x00\x00\x75\x26\x8B\x85\x00\x00\x00\x00\x83\xF8\xFF", "x????xxxxxx?????xx?????xxxx????xxx") + 0x1; // Error: can't use k-codes in long string
			offsets[39] = pattern.FindPattern("\x85\xC0\x75\x13\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\x8B\x0D\x00\x00\x00\x00\x89\x0D\x00\x00\x00\x00", "xxxxx????x????xx????xxxxx????xx????") + 0xA; // No texture!
			offsets[40] = pattern.FindPattern("\x8B\x15\x00\x00\x00\x00\xA1\x00\x00\x00\x00\x51\x8B\x0D\x00\x00\x00\x00\x52\x50\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x1C\x5E\x5B\xC3", "xx????x????xxx????xxx????xx????xxxxxx") + 0x15; // Width %u Height: %u Depth: %u
			offsets[41] = pattern.FindPattern("\x8B\x80\x00\x00\x00\x00\x3B\xC3\x89\x45\x4C\x75\x25\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x08\x89\x5D\x40", "xx????xxxxxxxxx????x????xx????xxxxxx") + 0x14; // Warning: bad train carriages
			offsets[42] = pattern.FindPattern("\x89\x45\x4C\x89\x45\x40\x89\x4D\x44\x75\x19\xA1\x00\x00\x00\x00\x57\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x0C\xE9\x00\x00\x00\x00", "xxxxxxxxxxxx????xx????xx????xxxx????") + 0x12; // Warning: couldn't create vehicle type: %u
			offsets[43] = pattern.FindPattern("\x8B\x8C\x87\x00\x00\x00\x00\x85\xC9\x74\x21\x50\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x33\xC9\x66\x8B\x0E\x83\xC4\x0C", "xxx????xxxxxx????x????xx????xxxxxxxx") + 0x12; // Warning: vehicle %u was not deleted
			offsets[44] = pattern.FindPattern("\xEB\x05\x83\xF8\x64\x75\x17\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\x5E\xC2\x04\x00", "xxxxxxxx????x????xx????xxxxxxx") + 0xD; // All vehicle waiting slots are consumed!
			offsets[45] = pattern.FindPattern("\x66\x3D\x20\x03\x72\x1E\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x4F\x0F\xB7\xD0\x52\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x0C\xEB\x3B", "xxxxxxxx????xxxxxxxxx????xx????xxxxx") + 0x15; // Warning: ignoring large TextDraw size=%u
			offsets[46] = pattern.FindPattern("\xD9\x1C\x24\x50\x51\x8B\xCE\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x6A\x05\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "xxxxxxxx????xx????xxx????x????x????") + 0x1A; // ~r~Car Jacked~w~!
			offsets[47] = pattern.FindPattern("\x8B\x0D\x00\x00\x00\x00\x85\xC9\x74\x0F\x50\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x0C\x5F\x33\xC0\x5E\xC2\x24\x00", "xx????xxxxxx????xx????xxxxxxxxxx") + 0xC; // Warning: bad object model ID (%u)
			offsets[48] = pattern.FindPattern("\xC7\x44\x24\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x0F\xBE\xC8\x60\x9C\x8D\x4C\x24\x30\x88\x14\x24\x88\x1C\x24\x89\x4C\x24\x20", "xxx?????x????xxxxxxxxxxxxxxxxxxx") + 0x4; // Warning: Couldn't spawn player(%u)
			offsets[49] = pattern.FindPattern("\xE9\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x89\x4C\x24\x08\x60\xC7\x44\x24\x00\x00\x00\x00\x00\x9C\xE9\x00\x00\x00\x00", "x????x????xxxxxxxx?????xx????") + 0x13; // Connected to {B9C9BF}%.64s
			offsets[50] = pattern.FindPattern("\x68\x00\x00\x00\x00\xC7\x04\x24\x00\x00\x00\x00\x68\x00\x00\x00\x00\x66\x89\x04\x24\x9C\x8D\x64\x24\x08\xE9\x00\x00\x00\x00", "x????xxx????x????xxxxxxxxxx????") + 0x8; // CONNECTION REJECTED: Incorrect Version.
			offsets[51] = pattern.FindPattern("\xC7\x44\x24\x00\x00\x00\x00\x00\x9C\xE9\x00\x00\x00\x00\x9C\x9C\x87\x74\x24\x08\x9C\x68\x00\x00\x00\x00\xFF\x34\x24\x9C\xFF\x74\x24\x18\xC2\x1C\x00", "xxx?????xx????xxxxxxxx????xxxxxxxxxxx") + 0x4; // CONNECTION REJECTED: Unacceptable NickName
			offsets[52] = pattern.FindPattern("\x60\x60\x9C\xC7\x44\x24\x00\x00\x00\x00\x00\xE9\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xC7\x05\x00\x00\x00\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00", "xxxxxx?????x????x????xx????????x????") + 0x7; // Please choose another nick between and 3-20 characters
			offsets[53] = pattern.FindPattern("\x60\xC7\x44\x24\x00\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x60\x9C\xC7\x44\x24\x00\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x9C\x60\xE9\x00\x00\x00\x00", "xxxx?????x????xxxxx?????x????xxx????") + 0x5; // Please use only a-z, A-Z, 0-9
			offsets[54] = pattern.FindPattern("\xC7\x44\x24\x00\x00\x00\x00\x00\x60\x68\x00\x00\x00\x00\xE9\x00\x00\x00\x00\xC6\x44\x24\x00\x00\x89\x74\x24\x30\x68\x00\x00\x00\x00\xC6\x04\x24\x74\x66\x89\x7C\x24\x00", "xxx?????xx????x????xxx??xxxxx????xxxxxxxx?") + 0x4; // Use /quit to exit or press ESC and select Quit Game
			offsets[55] = pattern.FindPattern("\xC2\x24\x00\x9C\xC7\x44\x24\x00\x00\x00\x00\x00\x9C\x66\x89\x2C\x24\x9C\x89\x44\x24\x2C\x68\x00\x00\x00\x00\xFF\x74\x24\x04\x66\x89\x34\x24", "xxxxxxx?????xxxxxxxxxxx????xxxxxxxx") + 0x8; // CONNECTION REJECTED: Bad mod version.
			offsets[56] = pattern.FindPattern("\xE8\x00\x00\x00\x00\x68\x00\x00\x00\x00\x54\x60\x8D\x64\x24\x28\x0F\x85\x00\x00\x00\x00\x60\x60\xC7\x44\x24\x00\x00\x00\x00\x00\xFF\x34\x24\xE9\x00\x00\x00\x00", "x????x????xxxxxxxx????xxxxx?????xxxx????") + 0x1C; // CONNECTION REJECTED: Unable to allocate a player slot.
			offsets[57] = pattern.FindPattern("\x60\xC7\x44\x24\x00\x00\x00\x00\x00\x9C\x9C\x89\x6C\x24\x04\x89\x44\x24\x28\x9C\x52\x8D\x64\x24\x30\xE8\x00\x00\x00\x00\x80\xFF\x93\xD0\xCA\x83\xC4\x08", "xxxx?????xxxxxxxxxxxxxxxxx????xxxxxxxx") + 0x5; // Connected. Joining the game...
			offsets[58] = pattern.FindPattern("\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x08\x8B\xCF\xE8\x00\x00\x00\x00\x8B\x97\x00\x00\x00\x00\x8B\x42\x18\x85\xC0\x74\x33\x56\x33\xF6", "x????xx????xxxxxx????xx????xxxxxxxxxx") + 0x1; // Lost connection to the server. Reconnecting..
			offsets[59] = pattern.FindPattern("\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x53\xE8\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x68\x00\x00\x00\x00\x52\xE8\x00\x00\x00\x00\x83\xC4\x08\x5F\x5E\x5D\x5B", "x????xx????xx????xx????x????xx????xxxxxxx") + 0x18; // The server is restarting..
			offsets[60] = pattern.FindPattern("\x75\x34\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x83\xC4\x08\x85\xC9\x74\x0E\x6A\x00\x6A\x00", "xxx????x????xx????xx????xxxxxxxxxxx") + 0x8; // NameTag Player Status: ON
			offsets[61] = pattern.FindPattern("\x8B\x0D\x00\x00\x00\x00\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x83\xC4\x08\x85\xC9\x74\x0E\x6A\x00\x6A\x01", "xx????x????xx????xx????xxxxxxxxxxx") + 0x7; // NameTag Player Status: OFF
			offsets[62] = pattern.FindPattern("\x6A\x00\xFF\x50\x08\xEB\x24\x68\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x08\x89\x9E\x00\x00\x00\x00\xEB\x08", "xxxxxxxx????xx????xx????xxxxx????xx") + 0x8; // The server didn't respond. Retrying..
			offsets[63] = pattern.FindPattern("\x8B\x15\x00\x00\x00\x00\x68\x00\x00\x00\x00\x52\xE8\x00\x00\x00\x00\x8B\x8E\x00\x00\x00\x00\x8B\x01\x83\xC4\x08\x6A\x00\x6A\x00\xFF\x50\x08", "xx????x????xx????xx????xxxxxxxxxxxx") + 0x7; // Wrong server password.
			offsets[64] = pattern.FindPattern("\xA1\x00\x00\x00\x00\x56\x68\x00\x00\x00\x00\x50\x8B\xF1\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x83\xC4\x08\x85\xC9\x74\x07\x6A\x01\xE8\x00\x00\x00\x00", "x????xx????xxxx????xx????xxxxxxxxxx????") + 0x7; // Server closed the connection.
			offsets[65] = pattern.FindPattern("\xEB\x79\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x83\xC4\x08\xEB\x64\x68\x00\x00\x00\x00\xEB\x3E\x57\x8B\xCE", "xxx????x????xx????xxxxxx????xxxxx") + 0x18; // The server is full. Retrying...
			offsets[66] = pattern.FindPattern("\x57\x8B\xCE\xE8\x00\x00\x00\x00\xE9\x00\x00\x00\x00\x8B\x15\x00\x00\x00\x00\x68\x00\x00\x00\x00\x52\xE8\x00\x00\x00\x00\x83\xC4\x08\xEB\x79", "xxxx????x????xx????x????xx????xxxxx") + 0x14; // You are banned from this server.
			offsets[67] = pattern.FindPattern("\x8D\x56\x20\x68\x00\x00\x00\x00\x9C\x9C\x89\x54\x24\x44\x9C\x8D\x64\x24\x48\x0F\x86\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxx????xxxxxxxxxxxxx????x????") + 0x1A; // Connecting to %s:%d...
			offsets[68] = pattern.FindPattern("\x87\x4C\x24\x28\x66\xF7\xD1\xFF\x34\x24\xE9\x00\x00\x00\x00\x56\xC7\x04\x24\x00\x00\x00\x00\xE8\x00\x00\x00\x00\xFF\x70\x60", "xxxxxxxxxxx????xxxx????x????xxx") + 0x13; // {FFFFFF}SA-MP {B9C9BF}What.Ever.Version {FFFFFF}Started
			offsets[69] = pattern.FindPattern("\x68\x00\x00\x00\x00\x52\xE8\x00\x00\x00\x00\x83\xC4\x08\x39\x6E\x10\x0F\x84\x00\x00\x00\x00\x8B\x0E\xE8\x00\x00\x00\x00\x3C\x37\x0F\x84\x00\x00\x00\x00", "x????xx????xxxxxxxx????xxx????xxxx????") + 0x1; // Returning to class selection after next death
			offsets[70] = pattern.FindPattern("\x85\xC0\x74\x22\x8B\x0E\x6A\x00\x6A\x00\xE8\x00\x00\x00\x00\x8B\x0D\x00\x00\x00\x00\x6A\x05\x68\x00\x00\x00\x00\x68\x00\x00\x00\x00", "xxxxxxxxxxx????xx????xxx????x????") + 0x1D; // // Stay within the ~r~world boundries
			offsets[71] = pattern.FindPattern("\x8B\xF9\x8B\x4C\x87\x04\x85\xC9\x74\x21\x50\xA1\x00\x00\x00\x00\x68\x00\x00\x00\x00\x50\xE8\x00\x00\x00\x00\x33\xC9\x66\x8B\x0E\x83\xC4\x0C\x51\x8B\xCF", "xxxxxxxxxxxx????x????xx????xxxxxxxxxxx") + 0x11; // Warning: actor %u was not deleted
			offsets[72] = pattern.FindPattern("\x8B\x4E\x04\x8B\x1E\x52\x03\x46\x08\x03\xC1\x8B\x4D\x0C\x03\xC3\x50\x68\x00\x00\x00\x00\x51\xE8\x00\x00\x00\x00\x83\xC4\x40\x5F", "xxxxxxxxxxxxxxxxxx????xx????xxxx") + 0x12; // Messages in Send buffer: %u\nMessages sent: %u\nBytes sent: %u\nAcks sent: %u\nAcks in send buffer: %u\nMessages waiting for ack: %u\nMessages resent: %u\nBytes resent: %u\nPacketloss: %.1f%%\nMessages received: %u\nBytes received: %u\nAcks received: %u\nDuplicate acks received: %u\n

			size_t y = NULL;

			for (short i = 0; i < sizeof(strings) / sizeof(strings[0]); i++)
			{
				std::getline(stream, strings[i]);

				if (!strings[i].empty())
				{
					while(y != strings[i].length()) // Yes, deal with it
					{
						y++;

						if (strings[i][y] == '\\' && (y + 1) < strings[i].length())
						{
							switch (strings[i][y + 1]) // Yes, replace the two with one char
							{
								case 'n':
								{
									strings[i].replace(y, 2, "\n"); 
									break;
								}
								case 'r':
								{
									strings[i].replace(y, 2, "\r");
									break;
								}
								case 't':
								{
									strings[i].replace(y, 2, "\t");
									break;
								}
							}					
						}
					}

					y = NULL;

					VirtualProtect(reinterpret_cast<void*>(offsets[i]), 4, PAGE_EXECUTE_READWRITE, &OldProtect);

					*reinterpret_cast<DWORD*>(offsets[i]) = reinterpret_cast<DWORD>(&strings[i][0]);
				}
			}

			stream.close();
		}
		else
		{
			FreeLibraryAndExitThread(module, 0);
		}
	}
	else
	{
		FreeLibraryAndExitThread(module, 0);
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		module = hModule;
		DisableThreadLibraryCalls(hModule);
		return CreateThread(0, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Thread), 0, 0, 0) > 0;
	}
	return 1;
}