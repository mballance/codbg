/*
 * gdb_rsp_msg.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */

#include "gdb_rsp_msg.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>

gdb_rsp_msg::gdb_rsp_msg() {
	// TODO Auto-generated constructor stub
	m_data = 0;
	m_data_idx = 0;
	m_data_max = 0;
	m_read_idx = 0;
}

gdb_rsp_msg::~gdb_rsp_msg() {
	// TODO Auto-generated destructor stub
	if (m_data) {
		delete [] m_data;
	}
}

void gdb_rsp_msg::append(int8_t data)
{
	// Ensure space
	ensure_space(1);
	m_data[m_data_idx++] = data;
}

void gdb_rsp_msg::append(const char *str)
{
	while (*str) {
		append(*str);
		str++;
	}
}

void gdb_rsp_msg::appendh(uint8_t data)
{
	append(itoh((data >> 4) & 0xF));
	append(itoh(data & 0xF));
}

void gdb_rsp_msg::append_h32(uint32_t data)
{
	for (uint32_t i=0; i<4; i++) {
		appendh((data >> 8*i) & 0xFF);
	}
	/*
	for (int32_t i=0; i<8; i++) {
		fprintf(stdout, "Append: %c\n", itoh((data >> 4*i) & 0xf));
		append(itoh((data >> 4*i) & 0xf));
	}
	 */
}

void gdb_rsp_msg::calc_chksum(char &high, char &low) const
{
	uint8_t chksum = 0;

	for (uint32_t i=0; i<m_data_idx; i++) {
		chksum += m_data[i];
	}

	high = itoh(((chksum >> 4) & 0xF));
	low = itoh((chksum & 0xF));
}

const string gdb_rsp_msg::msg() const
{
	string ret;
	for (uint32_t i=0; i<m_data_idx; i++) {
		ret.push_back(m_data[i]);
	}

	return ret;
}

int gdb_rsp_msg::get_ch()
{
	if (m_read_idx < m_data_idx) {
		return m_data[m_read_idx++];
	} else {
		return -1;
	}
}

void gdb_rsp_msg::unget_ch()
{
	if (m_read_idx > 0) {
		m_read_idx--;
	}
}

const string gdb_rsp_msg::read_tok()
{
	string ret;
	uint32_t start = m_read_idx;

	while (m_read_idx < m_data_idx) {
		int ch = m_data[m_read_idx];
		if (ch == ':' || ch == '+') {
			break;
		}
		m_read_idx++;
	}

	for (uint32_t i=start; i<m_read_idx; i++) {
		ret.push_back(m_data[i]);
	}

	return ret;
}

// Read up to a 32-bit integer
uint32_t gdb_rsp_msg::read_uint32()
{
	uint32_t ret = 0;

	for (uint32_t i=0; i<8 && m_read_idx < m_data_idx; i++) {
		char c = m_data[m_read_idx];

		if ((c >= '0' && c <= '9') ||
				(c >= 'A' && c <= 'F') ||
				(c >= 'a' && c <= 'f')) {
			ret <<= 4;
			ret |= htoi(c);
			m_read_idx++;
		} else {
			break;
		}
	}

	return ret;
}

// Read up to a 8-bit integer
uint8_t gdb_rsp_msg::read_uint8()
{
	uint8_t ret = 0;

	for (uint32_t i=0; i<2 && m_read_idx < m_data_idx; i++) {
		char c = m_data[m_read_idx];

		if ((c >= '0' && c <= '9') ||
				(c >= 'A' && c <= 'F') ||
				(c >= 'a' && c <= 'f')) {
			ret <<= 4;
			ret |= htoi(c);
			m_read_idx++;
		} else {
			break;
		}
	}

	return ret;
}

void gdb_rsp_msg::ensure_space(uint32_t req)
{
	if (req+m_data_idx >= m_data_max) {
		// resize the bugger
		int8_t *tmp = m_data;
		uint32_t n_pages = (m_data_max / 4096) + ((req-1) / 4096) + 1;
		m_data = new int8_t[4096*n_pages];

		if (tmp) {
			memcpy(m_data, tmp, m_data_idx);
			delete [] tmp;
		}
		m_data_max = (4096 * n_pages);
	}
}

char gdb_rsp_msg::itoh(uint8_t data)
{
	data &= 0xF;

	if (data < 10) {
		return '0' + data;
	} else {
		return 'a' + (data-10);
	}
}

uint8_t gdb_rsp_msg::htoi(char c)
{
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'A' && c <= 'F') {
		return 10 + (c - 'A');
	} else if (c >= 'a' && c <= 'f') {
		return 10 + (c - 'a');
	} else {
		return 0;
	}
}
