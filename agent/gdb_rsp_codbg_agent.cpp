/*
 * gdb_rsp_codbg_agent.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#include "gdb_rsp_codbg_agent.h"
#include "codbg_agent.h"
#include "gdb_rsp_msg.h"
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <sys/types.h>
#include <stdio.h>

gdb_rsp_codbg_agent::gdb_rsp_codbg_agent(
		codbg_agent 	*agent,
		uint16_t		port) {
	// TODO Auto-generated constructor stub
	m_agent = agent;
	m_port = port;
	m_buf_max = 4096;
	m_buf = new int8_t[m_buf_max];
	m_buf_sz = 0;
	m_buf_idx = 0;
}

gdb_rsp_codbg_agent::~gdb_rsp_codbg_agent() {
	// TODO Auto-generated destructor stub
}

int gdb_rsp_codbg_agent::main()
{
	int		listen_sock, ret;
	struct sockaddr_in srv_addr;
#ifdef _WIN32
	WSADATA		wsaData;
	int iResult;

	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		fprintf(stdout, "WSAStartup failed; %d\n", iResult);
	}
#endif

	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	fprintf(stdout, "sock=%d\n", listen_sock);
	fflush(stdout);

	memset(&srv_addr, 0, sizeof(srv_addr));

	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	srv_addr.sin_port = htons(m_port);

	ret = bind(listen_sock, (struct sockaddr *)&srv_addr, sizeof(srv_addr));
	fprintf(stdout, "bind=%d\n", ret);
	fflush(stdout);

	ret = listen(listen_sock, 1);
	fprintf(stdout, "listen=%d\n", ret);
	fflush(stdout);


	m_fd = accept(listen_sock, 0, 0);

	fprintf(stdout, "data_sock=%d\n", m_fd);
	fflush(stdout);

	if (m_fd == -1) {
		return 1;
	}

	// Now, read messages
	gdb_rsp_msg *msg;
	while ((msg = read_msg())) {
		fprintf(stdout, "msg: %s\n", msg->msg().c_str());
		fflush(stdout);

		process_msg(*msg);

		delete msg;
	}

	return 0;
}

void gdb_rsp_codbg_agent::process_msg(gdb_rsp_msg &msg)
{
	int ch = msg.get_ch();

	fprintf(stdout, "ch=%c\n", ch);
	fflush(stdout);

	switch (ch) {

		case '?': {
			gdb_rsp_msg rsp;

			rsp.append("S");
			rsp.appendh(5);
			write_msg(rsp);
		} break;

		case 'c': {
			// Respond once we've moved forward
			m_agent->cont();

			gdb_rsp_msg rsp;
			rsp.append("S");
			rsp.appendh(5);
			write_msg(rsp);
		} break;

		case 'g': {
			gdb_rsp_msg rsp;

			for (uint32_t i=0; i<m_agent->n_regs(); i++) {
				rsp.append_h32(m_agent->read_reg(i));
			}

			write_msg(rsp);
		} break;

		//
		case 'H': {
			gdb_rsp_msg rsp;

			rsp.append("OK");
			write_msg(rsp);
		} break;

		case 'M': {
			uint32_t addr = msg.read_uint32();
			msg.get_ch(); // ,
			uint32_t sz = msg.read_uint32();
			msg.get_ch(); // :

			uint8_t *tmp = new uint8_t[sz];

			for (uint32_t i=0; i<sz; i++) {
				tmp[i] = msg.read_uint8();
			}

			m_agent->write(addr, tmp, sz);

			delete [] tmp;

			gdb_rsp_msg rsp;

			rsp.append("OK");
			write_msg(rsp);
		} break;

		// Read memory
		case 'm': {
			uint32_t addr = msg.read_uint32();
			msg.get_ch(); // ,
			uint32_t n_bytes = msg.read_uint32();
			uint8_t *tmp = new uint8_t[n_bytes];

			gdb_rsp_msg rsp;

			m_agent->read(addr, tmp, n_bytes);

			for (uint32_t i=0; i<n_bytes; i++) {
				rsp.appendh(tmp[i]);
			}

			write_msg(rsp);
			delete [] tmp;
		} break;

		case 'p': {
			uint32_t reg_idx = msg.read_uint32();
			gdb_rsp_msg rsp;
			rsp.append_h32(m_agent->read_reg(reg_idx));
			write_msg(rsp);
		} break;

		case 'q': {
			msg.unget_ch();
			string req = msg.read_tok();

			fprintf(stdout, "req=%s\n", req.c_str());

			if (req == "qSupported") {
				gdb_rsp_msg rsp;
				write_msg(rsp);
			} else {
				// Ignore message
				gdb_rsp_msg rsp;
				write_msg(rsp);
			}
		} break;

		case 's': {
			// No need to respond
			m_agent->stepi();

			fprintf(stdout, "post-step: 0x%08x\n", m_agent->read_reg(15));
			gdb_rsp_msg rsp;
			rsp.append("S");
			rsp.appendh(5);
			write_msg(rsp);
		} break;

		case 'Z': {
			uint8_t t = msg.read_uint8();
			msg.get_ch(); // ,
			uint32_t addr = msg.read_uint32();

			fprintf(stdout, "set breakpoint: 0x%08x\n", addr);
			m_agent->set_breakpoint(addr);

			gdb_rsp_msg rsp;

			rsp.append("OK");
			write_msg(rsp);
		} break;

		default: {
			// Signal that we don't know what to do
			gdb_rsp_msg rsp;
			write_msg(rsp);
		} break;
	}

}

gdb_rsp_msg *gdb_rsp_codbg_agent::read_msg()
{
	gdb_rsp_msg *msg = new gdb_rsp_msg();
	char chksum_h, chksum_l;
	char chksum[2];
	int ch;

	while ((ch = getch()) != -1 && ch != '$') {
		if (ch != '+') {
			fprintf(stdout, "unknown start char %c\n", ch);
			fflush(stdout);
		}
	}

	if (ch != -1) {
		while ((ch = getch()) != -1 && ch != '#' && ch != -1) {
//			fprintf(stdout, "Append %c\n", ch);
			msg->append(ch);
		}

		uint32_t i;
		for (i=0; i<2; i++) {
			if ((ch = getch()) == -1) {
				break;
			}
			chksum[i] = ch;
		}

		if (i != 2) {
			fprintf(stdout, "missing checksum\n");
			fflush(stdout);
			return 0;
		}

		msg->calc_chksum(chksum_h, chksum_l);
		if (chksum[0] == chksum_h && chksum[1] == chksum_l) {
			write_raw('+'); // Received message ok
		} else {
			write_raw('-'); // Checksum error
		}
	}

	if (msg->length()) {
		return msg;
	} else {
		delete msg;
		return 0;
	}
}

bool gdb_rsp_codbg_agent::write_raw(int8_t data)
{
	return write_raw(&data, 1);
}

bool gdb_rsp_codbg_agent::write_raw(int8_t *data, uint32_t len)
{
	send(m_fd, (char *)data, len, 0);
}

bool gdb_rsp_codbg_agent::write_msg(gdb_rsp_msg &msg)
{
	bool ret = true;
	char chksum_h, chksum_l;
	msg.calc_chksum(chksum_h, chksum_l);
	ret &= write_raw('$');
	ret &= write_raw(msg.get_data(), msg.length());
	ret &= write_raw('#');
	ret &= write_raw(chksum_h);
	ret &= write_raw(chksum_l);

	return ret;
}

int gdb_rsp_codbg_agent::getch()
{
	int ret = -1;

	if (m_buf_idx >= m_buf_sz) {
		m_buf_idx = 0;
		m_buf_sz = recv(m_fd, (char *)m_buf, m_buf_max, 0);
	}

	if (m_buf_idx < m_buf_sz) {
		ret = m_buf[m_buf_idx++];
	}

	return ret;
}

