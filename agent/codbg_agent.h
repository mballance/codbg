/*
 * codbg_agent.h
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#ifndef CODBG_AGENT_H_
#define CODBG_AGENT_H_
#include <stdint.h>
#include "codbg_agent_if.h"
#include "codbg_memory.h"
#include <vector>

using std::vector;

class codbg_target_if;
class codbg_agent {

	public:

		codbg_agent(
				uint32_t			n_reg
				);

		virtual ~codbg_agent();

//		bool init(codbg_target_if *target_if);

		inline uint32_t n_regs() const { return m_n_regs; }

		virtual uint32_t read_reg(uint32_t idx);

		virtual void write_reg(uint32_t idx, uint32_t val);

		virtual void stepi() = 0;

		virtual void cont() = 0;

		virtual void set_breakpoint(uint32_t addr);

		virtual void clr_breakpoint(uint32_t addr);

		virtual void read(uint64_t addr, uint8_t *data, uint32_t sz);

		virtual void write(uint64_t addr, uint8_t *data, uint32_t sz);

		virtual void write32(uint64_t addr, uint32_t data);

		virtual bool load_exe(const char *file);

	protected:

		virtual bool is_breakpoint(uint32_t addr);

	private:
		uint32_t				m_n_regs;
		uint32_t				*m_regs;

		codbg_memory			m_memory;

		vector<uint32_t>		m_bp;

};

#endif /* CODBG_AGENT_H_ */
