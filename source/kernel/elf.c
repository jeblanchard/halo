typedef unsigned int Elf32_Addr;
typedef unsigned short Elf32_Half;
typedef unsigned int Elf32_Off;
typedef int Elf32_Sword;
typedef unsigned int Elf32_Word;

#define EI_NIDENT 16

const Elf32_Half ET_NONE = 0;

const Elf32_Half ET_REL = 1;
const Elf32_Half ET_EXEC = 2;
const Elf32_Half ET_DYN = 3;
const Elf32_Half ET_CORE = 4;
const Elf32_Half ET_LOPROC = 0xff00;
const Elf32_Half ET_HIPROC = 0xffff;

const Elf32_Half EM_M32 = 1;
const Elf32_Half EM_SPARC = 2;
const Elf32_Half EM_386 = 3;
const Elf32_Half EM_68K = 4;
const Elf32_Half EM_88K = 5;
const Elf32_Half EM_860 = 7;
const Elf32_Half EM_MIPS = 8;
const Elf32_Half EM_MIPS_RS4_BE = 10;
const Elf32_Half RESERVED_0 = 11;
const Elf32_Half RESERVED_1 = 12;
const Elf32_Half RESERVED_2 = 13;
const Elf32_Half RESERVED_3 = 14;
const Elf32_Half RESERVED_4 = 15;
const Elf32_Half RESERVED_5 = 16;

typedef struct {
    unsigned char e_ident[EI_NIDENT];
    Elf32_Half e_type;
    Elf32_Half e_machine;
    Elf32_Word e_version;
    Elf32_Addr e_entry;
    Elf32_Off e_phoff;
    Elf32_Off e_shoff;
    Elf32_Word e_flags;
    Elf32_Half e_ehsize;
    Elf32_Half e_phentsize;
    Elf32_Half e_phnum;
    Elf32_Half e_shentsize;
    Elf32_Half e_shnum;
    Elf32_Half e_shstrndx;
} Elf32_Ehdr;

const Elf32_Word PT_NULL = 0;
const Elf32_Word PT_LOAD = 1;
const Elf32_Word PT_DYNAMIC = 2;
const Elf32_Word PT_INTERP = 3;
const Elf32_Word PT_NOTE = 4;
const Elf32_Word PT_SHLIB = 5;
const Elf32_Word PT_PHDR = 6;
const Elf32_Word PT_LOPROC = 0x70000000;
const Elf32_Word PT_HIPROC = 0x7fffffff;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;