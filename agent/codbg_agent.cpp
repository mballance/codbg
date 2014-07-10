/*
 * codbg_agent.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#include "codbg_agent.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

codbg_agent::codbg_agent(uint32_t n_regs) {
	m_n_regs = n_regs;
	m_regs = new uint32_t[m_n_regs];
	memset(m_regs, 0, sizeof(uint32_t)*m_n_regs);
}

codbg_agent::~codbg_agent() {
	delete [] m_regs;
}

uint32_t codbg_agent::read_reg(uint32_t idx)
{
	if (idx < m_n_regs) {
		return m_regs[idx];
	} else {
		return 0xdeadbeef;
	}
}

void codbg_agent::write_reg(uint32_t idx, uint32_t val)
{
	if (idx < m_n_regs) {
		m_regs[idx] = val;
	}
}

void codbg_agent::set_breakpoint(uint32_t addr)
{
	m_bp.push_back(addr);
}

void codbg_agent::clr_breakpoint(uint32_t addr)
{
	// TODO:
}

void codbg_agent::read(uint64_t addr, uint8_t *data, uint32_t sz)
{
	m_memory.read(addr, data, sz);
}

void codbg_agent::write(uint64_t addr, uint8_t *data, uint32_t sz)
{
	m_memory.write(addr, data, sz);
}

void codbg_agent::write32(uint64_t addr, uint32_t data)
{
	uint8_t tmp[4];
	tmp[0] = data;
	tmp[1] = data >> 8;
	tmp[2] = data >> 16;
	tmp[3] = data >> 24;

	write(addr, tmp, 4);
}

//static const int	SHT_NULL			= 0;	/* sh_type */
static const int	SHT_PROGBITS		= 1;
static const int	SHT_SYMTAB			= 2;
#ifdef UNDEFINED
static const int	SHT_STRTAB			= 3;
static const int	SHT_RELA			= 4;
static const int	SHT_HASH			= 5;
static const int	SHT_DYNAMIC			= 6;
static const int	SHT_NOTE			= 7;
#endif
static const int	SHT_NOBITS			= 8;
#ifdef UNDEFINED
static const int	SHT_REL				= 9;
static const int	SHT_SHLIB			= 10;
static const int	SHT_DYNSYM			= 11;
static const int	SHT_UNKNOWN12		= 12;
static const int	SHT_UNKNOWN13		= 13;
static const int	SHT_INIT_ARRAY		= 14;
static const int	SHT_FINI_ARRAY		= 15;
static const int	SHT_PREINIT_ARRAY	= 16;
static const int	SHT_GROUP			= 17;
static const int	SHT_SYMTAB_SHNDX	= 18;
static const int	SHT_NUM				= 19;

static const int SHF_WRITE =          (1 << 0);     /* Writable */
#endif
static const int SHF_ALLOC =          (1 << 1);     /* Occupies memory during execution */
#ifdef UNDEFINED
static const int SHF_EXECINSTR =      (1 << 2);     /* Executable */
static const int SHF_MERGE     =      (1 << 4);     /* Might be merged */
static const int SHF_STRINGS   =      (1 << 5);     /* Contains nul-terminated strings */
static const int SHF_INFO_LINK =      (1 << 6);     /* `sh_info' contains SHT index */
static const int SHF_LINK_ORDER =     (1 << 7);     /* Preserve order after combining */
static const int SHF_OS_NONCONFORMING = (1 << 8);   /* Non-standard OS specific handling
                                       required */
static const int SHF_GROUP           =  (1 << 9);   /* Section is member of a group.  */
static const int SHF_TLS             =  (1 << 10);  /* Section hold thread-local data.  */
static const int SHF_MASKOS          =  0x0ff00000; /* OS-specific.  */
static const int SHF_MASKPROC        =  0xf0000000; /* Processor-specific */
static const int SHF_ORDERED         =  (1 << 30);  /* Special ordering requirement
                                       (Solaris).  */
static const int SHF_EXCLUDE         =  (1 << 31);  /* Section is excluded unless
                                       referenced or allocated (Solaris).
                                       referenced or allocated (Solaris).
                                       */
#endif


static const int EI_NIDENT  = 16;

typedef struct {
	uint8_t  	e_ident[EI_NIDENT]; /* bytes 0 to 15  */
	uint16_t 	e_e_type;           /* bytes 15 to 16 */
	uint16_t 	e_machine;          /* bytes 17 to 18 */
	uint32_t	e_version;          /* bytes 19 to 22 */
	uint32_t	e_entry;            /* bytes 23 to 26 */
	uint32_t	e_phoff;            /* bytes 27 to 30 */
	uint32_t	e_shoff;            /* bytes 31 to 34 */
	uint32_t	e_flags;            /* bytes 35 to 38 */
	uint16_t	e_ehsize;           /* bytes 39 to 40 */
	uint16_t 	e_phentsize;        /* bytes 41 to 42 */
	uint16_t 	e_phnum;            /* bytes 43 to 44 (2B to 2C) */
	uint16_t 	e_shentsize;        /* bytes 45 to 46 */
	uint16_t 	e_shnum;            /* bytes 47 to 48 */
	uint16_t 	e_shstrndx;         /* bytes 49 to 50 */
} ElfHeader;

/* Program Headers */
typedef struct {
	uint32_t p_type;     /* entry type */
	uint32_t p_offset;   /* file offset */
	uint32_t p_vaddr;    /* virtual address */
	uint32_t p_paddr;    /* physical address */
	uint32_t p_filesz;   /* file size */
	uint32_t p_memsz;    /* memory size */
	uint32_t p_flags;    /* entry flags */
	uint32_t p_align;    /* memory/file alignment */
} Elf32_Phdr;


/* Section Headers */
typedef struct {
	uint32_t sh_name;        /* section name - index into string table */
	uint32_t sh_type;        /* SHT_... */
	uint32_t sh_flags;       /* SHF_... */
	uint32_t sh_addr;        /* virtual address */
	uint32_t sh_offset;      /* file offset */
	uint32_t sh_size;        /* section size */
	uint32_t sh_link;        /* misc info */
	uint32_t sh_info;        /* misc info */
	uint32_t sh_addralign;   /* memory alignment */
	uint32_t sh_entsize;     /* entry size if table */
} Elf32_Shdr;

typedef struct {
    uint32_t	st_name;
    uint32_t	st_value;
    uint32_t	st_size;
    uint8_t		st_info;
    uint8_t		st_other;
    uint16_t	st_shndx;
} Elf32_Sym;

bool codbg_agent::load_exe(const char *filename)
{
	FILE *fp;
	ElfHeader hdr;
	Elf32_Phdr phdr;
	Elf32_Shdr shdr;

	fp = fopen(filename, "r");

	if (!fp) {
		fprintf(stdout, "Error: Failed to open %s\n", filename);
		return false;
	}

	fread(&hdr, sizeof(ElfHeader), 1, fp);

	if (hdr.e_ident[1] != 'E' ||
			hdr.e_ident[2] != 'L' ||
			hdr.e_ident[3] != 'F') {
		fprintf(stdout, "Error: Invalid ELF file\n");
		return false;
	}

	if (hdr.e_machine != 40) {
		fprintf(stdout, "Error: Invalid ELF machine\n");
		return false;
	}

	for (int i=0; i<hdr.e_shnum; i++) {
		fseek(fp, hdr.e_shoff+hdr.e_shentsize*i, 0);

		fread(&shdr, sizeof(Elf32_Shdr), 1, fp);

		if (shdr.sh_type == SHT_PROGBITS && shdr.sh_size != 0 &&
			(shdr.sh_flags & SHF_ALLOC) != 0) {
			// read in data
			uint8_t *tmp = new uint8_t[shdr.sh_size];
			fprintf(stdout, "Loading %d bytes @ 0x%08x..0x%08x\n",
					(int)shdr.sh_size, (int)shdr.sh_addr, (int)(shdr.sh_addr+shdr.sh_size));

			fseek(fp, shdr.sh_offset, 0);
			fread(tmp, shdr.sh_size, 1, fp);

			// Copy data to memory
			write(shdr.sh_addr, tmp, shdr.sh_size);
			/*
			for (int j=0; j<shdr.sh_size; j+=4) {
				data = tmp[j];
				data |= (tmp[j+1] << 8);
				data |= (tmp[j+2] << 16);
				data |= (tmp[j+3] << 24);

				m_mem_if->write32(shdr.sh_addr+j, data);

				if (j+4 >= shdr.sh_size) {
					fprintf(stdout, "Load last addr=0x%08x\n", shdr.sh_addr+j);
				}
			}
			 */

			delete [] tmp;
		} else if (shdr.sh_type == SHT_SYMTAB) {
			// First, read the header for the string-tab
			Elf32_Shdr str_shdr;
			fseek(fp, hdr.e_shoff+hdr.e_shentsize*shdr.sh_link, 0);
			fread(&str_shdr, sizeof(Elf32_Shdr), 1, fp);

			char *str_tmp = new char[str_shdr.sh_size];
			fseek(fp, str_shdr.sh_offset, 0);
			fread(str_tmp, str_shdr.sh_size, 1, fp);

			for (int i=0; i<shdr.sh_size; i+=sizeof(Elf32_Sym)) {
				Elf32_Sym sym;

				fseek(fp, (shdr.sh_offset+i), 0);
				fread(&sym, sizeof(Elf32_Sym), 1, fp);

				if (sym.st_info == 0) { // function/global
//					string name = &str_tmp[sym.st_name];
//					m_symtab[name] = sym.st_value;
				}
			}

			delete [] str_tmp;
		}

		if (shdr.sh_type == SHT_NOBITS && shdr.sh_size != 0) {
			fprintf(stdout, "Fill %0d bytes @ 0x%08x..0x%08x\n",
					shdr.sh_size, shdr.sh_addr, (shdr.sh_addr+shdr.sh_size));
			for (int j=0; j<shdr.sh_size; j+=4) {
				write32(shdr.sh_addr+j, 0);

				if (j+4 >= shdr.sh_size) {
					fprintf(stdout, "Fill last addr=0x%08x\n", (uint32_t)(shdr.sh_addr+j));
				}
			}
		}
	}

	fclose(fp);
}

bool codbg_agent::is_breakpoint(uint32_t addr)
{
	for (uint32_t i=0; i<m_bp.size(); i++) {
		if (m_bp.at(i) == addr) {
			return true;
		}
	}

	return false;
}
