/*
 * codbg_memory.h
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#ifndef CODBG_MEMORY_H_
#define CODBG_MEMORY_H_
#include <stdint.h>

class codbg_memory {

	struct mem_page {
		uint32_t			data[1024];
	};

	struct mem_nonleaf;

	typedef union {
		mem_nonleaf		*nonleaf;
		uint8_t			*page;
	} page_or_nonleaf_t;

	struct mem_nonleaf {
		page_or_nonleaf_t		pages[16];
	};


	public:
		codbg_memory();

		virtual ~codbg_memory();

		virtual void write(uint64_t addr, uint8_t *data, uint32_t sz);

		virtual void read(uint64_t addr, uint8_t *data, uint32_t sz);

	private:

		uint8_t *find_page(uint64_t addr, bool create=true);

	private:

		mem_nonleaf				m_root;

};

#endif /* CODBG_MEMORY_H_ */
