/*
 * tracefile_codbg_agent.h
 *
 *  Created on: Jul 8, 2014
 *      Author: ballance
 */

#ifndef TRACEFILE_CODBG_AGENT_H_
#define TRACEFILE_CODBG_AGENT_H_
#include "codbg_agent.h"
#include <stdio.h>
#include <string>

using std::string;

class tracefile_codbg_agent : public codbg_agent {

	public:

		tracefile_codbg_agent(
				uint32_t n_regs,
				uint32_t pc_idx);

		virtual ~tracefile_codbg_agent();

		bool init(const char *file);

		virtual void stepi();

		virtual void cont();

	protected:

		string next_record(uint32_t &start_off);

		void process_record(const string &rec);

		void execute(uint32_t addr);

	private:

		FILE				*m_fd;
		bool				m_exec;
		bool				m_hit_breakpoint;
		uint32_t			m_pc_idx;

};

#endif /* TRACEFILE_CODBG_AGENT_H_ */
