/*
 * gdb_rsp_codbg_agent.h
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#ifndef GDB_RSP_CODBG_AGENT_H_
#define GDB_RSP_CODBG_AGENT_H_
#include <stdint.h>

class codbg_agent;
class gdb_rsp_msg;
class gdb_rsp_codbg_agent {

	public:
		gdb_rsp_codbg_agent(codbg_agent *agent, uint16_t port);

		virtual ~gdb_rsp_codbg_agent();

		int main();

		void process_msg(gdb_rsp_msg &msg);

	protected:

		gdb_rsp_msg *read_msg();

		bool write_raw(int8_t data);

		bool write_raw(int8_t *data, uint32_t len);

		bool write_msg(gdb_rsp_msg &msg);

		int getch();

	private:


		codbg_agent					*m_agent;
		uint16_t					m_port;
		int32_t						m_fd;
		int8_t						*m_buf;
		int32_t						m_buf_max;
		int32_t						m_buf_sz;
		int32_t						m_buf_idx;
};

#endif /* GDB_RSP_CODBG_AGENT_H_ */
