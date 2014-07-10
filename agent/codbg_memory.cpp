/*
 * codbg_memory.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#include "codbg_memory.h"
#include <memory.h>
#include <stdio.h>

codbg_memory::codbg_memory() {
	// TODO Auto-generated constructor stub

	memset(&m_root, 0, sizeof(mem_nonleaf));
}

codbg_memory::~codbg_memory() {
	// TODO Auto-generated destructor stub
}

void codbg_memory::write(uint64_t addr, uint8_t *data, uint32_t sz)
{
	for (uint32_t i=0; i<sz; i++) {
		uint8_t *page = find_page(addr, true);
		uint32_t page_off = (addr & 0xFFF);

		page[page_off] = data[i];

		addr++;
	}
}

void codbg_memory::read(uint64_t addr, uint8_t *data, uint32_t sz)
{
	for (uint32_t i=0; i<sz; i++) {
		uint8_t *page = find_page(addr, true);
		uint32_t page_off = (addr & 0xFFF);

		data[i] = page[page_off];

		addr++;
	}
}

uint8_t *codbg_memory::find_page(uint64_t addr, bool create)
{
	mem_nonleaf *nl = &m_root;
	uint8_t		*page = 0;

	// Address:
	// 63:0
	// 63:12
	// 63:48 47:40 39:32 31:24 23:16 15:8 7:0
	// 63:60 59:56 55:52 51:48 47:40 39:32 31:24 23:16 15:8 7:0
	//
//	fprintf(stdout, "find_page: 0x%08llx\n", addr);
	for (uint32_t i=15; i>=3; i--) {
		uint32_t off = ((addr >> 4*i) & 0xF);
//		fprintf(stdout, "i=%d off=%d\n", i, off);
		if (!nl->pages[off].nonleaf) {
			if (create) {
				if (i > 4) {
					fprintf(stdout, "Create nonleaf %d %d for 0x%08llx\n", i, off, addr);
					nl->pages[off].nonleaf = new mem_nonleaf();
				} else {
					fprintf(stdout, "Create leaf %d %d for 0x%08llx\n", i, off, addr);
					nl->pages[off].page = new uint8_t[4096];
				}
			} else {
				break;
			}
		}

		if (i > 4) {
			nl = nl->pages[off].nonleaf;
		} else {
			page = nl->pages[off].page;
		}
	}

//	fprintf(stdout, "return page %p\n", page);

	return page;
}

