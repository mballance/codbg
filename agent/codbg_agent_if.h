/*
 * codbg_agent_if.h
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#ifndef CODBG_AGENT_IF_H_
#define CODBG_AGENT_IF_H_
#include <stdint.h>

class codbg_agent_if {

	public:

		virtual ~codbg_agent_if() {}

		virtual void reg_change(uint32_t idx, uint64_t val) = 0;

		virtual void mem_change32(uint64_t addr, uint32_t val) = 0;



};




#endif /* CODBG_AGENT_IF_H_ */
