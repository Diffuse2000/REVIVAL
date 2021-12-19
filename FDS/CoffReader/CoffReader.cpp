#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include <string_view>

using std::string;
using std::vector;

#define IMAGE_FILE_MACHINE_I386              0x014c  // Intel 386.
#define IMAGE_FILE_32BIT_MACHINE             0x0100  // 32 bit word machine.

#define IMAGE_FILE_RELOCS_STRIPPED           0x0001  // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE          0x0002  // File is executable  (i.e. no unresolved external references).
#define IMAGE_FILE_DLL                       0x2000  // File is a DLL.

#define IMAGE_SYM_UNDEFINED					(uint16_t)0          // Symbol is undefined or is common.
#define IMAGE_SYM_CLASS_EXTERNAL            0x0002


struct CoffHeader {
	unsigned short 	machine;
	unsigned short 	number_of_sections;
	long 			timestamp;
	long 			symbol_table_ptr;
	long 			number_of_symbols;
	unsigned short 	size_of_opt_header;
	unsigned short 	flags;
};

static_assert(sizeof(CoffHeader) == 20, "Size of CoffHeader != 20");


#pragma pack (push, 2)
struct Symbol {
	union {
		uint8_t    short_name[8];
		struct {
			uint32_t   zeros;     // if 0, use LongName
			uint32_t   offset;      // offset into string table
		} name;
	};
	uint32_t	value;
	uint16_t	section_number;
	uint16_t    type;
	uint8_t     storage_class;
	uint8_t     number_of_aux_symbols;
};

static_assert(sizeof(Symbol) == 18, "Size of Symbol != 18");


struct SectionHeader {
	uint8_t    name[8];
	uint32_t   virtual_size;
	uint32_t   virtual_address;
	uint32_t   size_of_raw_data;
	uint32_t   pointer_to_raw_data;
	uint32_t   pointer_to_relocations;
	uint32_t   pointer_to_linenumbers;
	uint16_t   number_of_relocations;
	uint16_t   number_of_linenumbers;
	uint32_t   characteristics;
};

static_assert(sizeof(SectionHeader) == 40, "Size of SectionHeader != 40");

#define IMAGE_REL_I386_DIR32            0x0006  // Direct 32-bit reference to the symbols virtual address

struct Relocation {
	uint32_t   virtual_address;
	uint32_t   symbol_table_index;
	uint16_t   type;
};
static_assert(sizeof(Relocation) == 10, "Size of Relocation != 10");
#pragma pack(pop)


typedef int(__stdcall* VPFunc)(
	void* lpAddress,		// address of region of committed pages
	unsigned long dwSize,			// size of the region
	unsigned long flNewProtect,	// desired access protection
	unsigned long* lpflOldProtect // address of variable to get old protection
	);



class CoffReader {
	vector<uint8_t>									buffer;
	const CoffHeader*								header = nullptr;
	const SectionHeader*							section_table = nullptr;
	const Symbol*									symbol_table = nullptr;
	const uint8_t*									string_table = nullptr;
	const SectionHeader*							data = nullptr;
	const SectionHeader*							text = nullptr;
	uint32_t										data_idx = 0;
	uint32_t										text_idx = 0;
	const Relocation*								text_relocations = nullptr;
	std::map<std::string, uintptr_t, std::less<>>	global_symbols; // std::less, no less..
	uint32_t										entry_point = 0;

	std::string_view GetName(const Symbol& symbol) {
		if (symbol.name.zeros == 0) {
			return std::string_view((const char *)&string_table[symbol.name.offset]);
		} else {
			auto result = std::string_view((const char*)&symbol.short_name, 8);
			while (result.size() > 0 && result.back() == '\0') {
				result.remove_suffix(1);
			}
			return result;
		}
	}

	void Read(std::string file) {
		FILE* f = fopen(file.c_str(), "rb");
		if (!f) {
			throw std::runtime_error("Can't open file: " + file);
		}

		if (0 != fseek(f, 0, SEEK_END)) {
			throw std::runtime_error("file is not opened for random access: " + file);
		}

		const int size = ftell(f);

		if (size > 1024 * 1024) {
			throw std::runtime_error("file is too big: " + file);
		}

		buffer.resize(size);
		if (0 != fseek(f, 0, SEEK_SET)) {
			throw std::runtime_error("file is not opened for random access: " + file);
		}

		fread(&buffer[0], size, 1, f);
	}

	const void* AcquireBytes(size_t &offset, size_t size) {
		if (offset > buffer.size() || size > buffer.size() || offset + size > buffer.size()) {
			throw std::runtime_error("data offset/size mismatch: " + std::to_string(offset) + "/" + std::to_string(size));
		}
		auto ptr = offset;
		offset += size;
		return &buffer[ptr];
	}

	void Parse() {
		size_t offset = 0;
		header = (const CoffHeader *)AcquireBytes(offset, sizeof(CoffHeader));
		if (header->machine != IMAGE_FILE_MACHINE_I386) {
			throw std::runtime_error("Unknown obj file machine type: " + std::to_string(header->machine));
		}

		if (header->size_of_opt_header != 0 || 
			header->flags & IMAGE_FILE_RELOCS_STRIPPED  ||
			header->flags & IMAGE_FILE_EXECUTABLE_IMAGE ||
			header->flags & IMAGE_FILE_DLL) {
			throw std::runtime_error("unsupported file format");
		}

		section_table = (const SectionHeader *)AcquireBytes(offset, sizeof(SectionHeader) * header->number_of_sections);

		// we assume only one .data segment, and one .text segment, no .bss
		for (int ii = 0; ii < header->number_of_sections; ++ii) {
			if (!memcmp(section_table[ii].name, ".data", 5)) {
				data = &section_table[ii];
				data_idx = ii;
			} else if (!memcmp(section_table[ii].name, ".text", 5)) {
				text = &section_table[ii];
				text_idx = ii;
			}
		}

		if (!data || !text) {
			throw std::runtime_error("Couldn't locate data and text sections");
		}

		offset = header->symbol_table_ptr;
		symbol_table = (const Symbol*)AcquireBytes(offset, sizeof(Symbol) * header->number_of_symbols);

		const uint32_t *string_table_size = (const uint32_t*)AcquireBytes(offset, sizeof(uint32_t));

		offset -= (sizeof(uint32_t));
		string_table = (const uint8_t *)AcquireBytes(offset, *string_table_size);

		// we can safely ignore data relocations in our case - we only use it to determine where the code starts 
		// in order to enable self-modifying code, which becomes redaunadant in the case we are loading the code ourselves...
		//offset = data->pointer_to_relocations;
		//const Relocation* data_relocation = (const Relocation*)AcquireBytes(offset, sizeof(Relocation) * data->number_of_relocations);

		offset = text->pointer_to_relocations;
		text_relocations = (const Relocation*)AcquireBytes(offset, sizeof(Relocation) * text->number_of_relocations);

		for (int ii = 0; ii < header->number_of_symbols; ++ii) {
			const Symbol& symbol = symbol_table[ii];
			std::string_view name = GetName(symbol);
			if (name.starts_with("_IX_") && name.ends_with("_AsmFiller")) {
				entry_point = symbol.value;
			}
		}

		if (entry_point == 0) {
			throw std::runtime_error("Couldn't locate entrypoint");
		}
			 

		//for (int ii = 0; ii < text->number_of_relocations; ++ii) {
		//	const Symbol* symbol = &symbol_table[text_relocation[ii].symbol_table_index];
		//	int banana = 1;
		//}
	}

	void Relocate(void* data, void* text) {
		for (int ii = 0; ii < section_table[text_idx].number_of_relocations; ++ii) {
			const auto &relocation = text_relocations[ii];
			const auto& symbol = symbol_table[relocation.symbol_table_index];

			if (relocation.type != IMAGE_REL_I386_DIR32) {
				throw std::runtime_error("unsupported relocation type");
			}

			const void* relocation_ptr = nullptr;
			if (symbol.section_number - 1 == text_idx) {
				relocation_ptr = (void *)((uint8_t *)text - (uint8_t *)this->text->virtual_address + symbol.value);
			} else if (symbol.section_number - 1 == data_idx) {
				relocation_ptr = (void*)((uint8_t*)data - (uint8_t*)this->data->virtual_address + symbol.value);
			} else if (symbol.section_number == IMAGE_SYM_UNDEFINED && symbol.storage_class == IMAGE_SYM_CLASS_EXTERNAL) {
				auto name = GetName(symbol);
				auto iter = global_symbols.find(name);
				if (iter == global_symbols.end()) {
					throw std::runtime_error("unknown external symbol");
				}
				relocation_ptr = (void *)iter->second;
			} else {
				throw std::runtime_error("unknown section number");
			}

			*(uint32_t*)(((uint32_t) text) + (uint32_t)relocation.virtual_address - (uint32_t)this->text->virtual_address) = (uint32_t)relocation_ptr;
		}
	}

public:
	CoffReader(std::string file, std::map<string, uintptr_t, std::less<>> global_symbols) : global_symbols(global_symbols) {
		Read(file);
		Parse();
	}

	// We assume 16 byte alignment
	// We assume the code will not be freed
	void* MapCode(VPFunc VirtualProtectFunc, uint32_t Protection) {
		auto data_size = section_table[data_idx].size_of_raw_data;
		auto code_size = section_table[text_idx].size_of_raw_data;
		auto code_start = (data_size + 15) & ~15; // align to 16
		auto buffer = _aligned_malloc(code_start + code_size, 16);

		memcpy(buffer, &data[section_table[data_idx].pointer_to_raw_data], data_size);
		memcpy((uint8_t *)buffer + code_start, &data[section_table[text_idx].pointer_to_raw_data], code_size);

		Relocate(buffer, (uint8_t*)buffer + code_start);
		unsigned long	OldProtection;
		VirtualProtectFunc((uint8_t*)buffer + code_start, code_size, Protection, &OldProtection);


		return (uint8_t*)buffer + code_start + entry_point;
	}
};

//int __stdcall Dummy(
//	void* lpAddress,		// address of region of committed pages
//	unsigned long dwSize,			// size of the region
//	unsigned long flNewProtect,	// desired access protection
//	unsigned long* lpflOldProtect // address of variable to get old protection
//) {
//	return 0;
//}


//extern "C" float g_zscale;
//extern "C" float g_zscale256;
//extern "C" long VESA_BPSL;
//extern "C" long PageSize;
//extern "C" long XRes;
//
//void TestCoff() {
//	std::map<string, uintptr_t, std::less<>> global_symbols { 
//		{"_g_zscale",		(uintptr_t)&g_zscale},
//		{"_g_zscale256",	(uintptr_t)&g_zscale256},
//		{"_VESA_BPSL",		(uintptr_t)&VESA_BPSL},
//		{"_PageSize",		(uintptr_t)&PageSize},
//		{"_XRes",			(uintptr_t)&XRes},	 
//	};
//
//	CoffReader coff("C:\\Projects\\REVIVAL\\FDS\\IXTGZSAM.obj", global_symbols);
//	coff.MapCode(Dummy, 0);
//
//	int banana = 1;
//}