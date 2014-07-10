/*
 * gdb_rsp_msg.h
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#ifndef GDB_RSP_MSG_H_
#define GDB_RSP_MSG_H_
#include <stdint.h>
#include <string>

using std::string;

class gdb_rsp_msg {
	public:

		gdb_rsp_msg();

		virtual ~gdb_rsp_msg();

		void append(int8_t data);
		void appendh(uint8_t data);
		void append_h32(uint32_t data);
		void append(const char *str);

		void calc_chksum(char &high, char &low) const;

		inline uint32_t length() const { return m_data_idx; }
		inline int8_t *get_data() const { return m_data; }

		const string msg() const;

		const string read_tok();

		uint32_t read_uint32();

		uint8_t read_uint8();

		int get_ch();

		void unget_ch();

	private:

		void ensure_space(uint32_t req);

		static char itoh(uint8_t data);
		static uint8_t htoi(char c);

	private:

		int8_t			*m_data;
		uint32_t		 m_data_idx;
		uint32_t		 m_data_max;
		uint32_t		 m_read_idx;
};

#endif /* GDB_RSP_MSG_H_ */
