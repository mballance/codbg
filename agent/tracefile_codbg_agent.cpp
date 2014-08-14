/*
 * tracefile_codbg_agent.cpp
 *
 *  Created on: Jul 8, 2014
 *      Author: ballance
 */

#include "tracefile_codbg_agent.h"
#include <string.h>
#include <stdlib.h>

tracefile_codbg_agent::tracefile_codbg_agent(
		uint32_t n_regs,
		uint32_t pc_idx) : codbg_agent(n_regs) {
	m_fd = 0;
	m_pc_idx = pc_idx;
}

tracefile_codbg_agent::~tracefile_codbg_agent() {
	// TODO Auto-generated destructor stub
}

bool tracefile_codbg_agent::init(const char *file)
{
	m_fd = fopen(file, "r");

	if (!m_fd) {
		return false;
	}

	// Process records until we either reach the end of the file
	// or reach the first execute
	string rec;
	while (true) {
		uint32_t start_off;
		rec = next_record(start_off);

		if (rec == "") {
			break;
		}

		process_record(rec);
		size_t colon_idx = rec.find(':');
		if (colon_idx != string::npos) {
			if (rec.substr(colon_idx+2, 8) == "EXECUTE:") {
//				fseek(m_fd, start_off, 0);
				break;
			}
		}

	}

	return (rec != "");
}

void tracefile_codbg_agent::stepi()
{
	m_exec = false;
	string rec;
	uint32_t start_off;

	while ((rec = next_record(start_off)) != "") {
		process_record(rec);
		if (m_exec) {
			break;
		}
	}
}

void tracefile_codbg_agent::cont()
{
	m_hit_breakpoint = false;
	string rec;
	uint32_t start_off;

	fprintf(stdout, "--> cont: 0x%08x\n", read_reg(15));

	while ((rec = next_record(start_off)) != "") {
		process_record(rec);
		if (m_hit_breakpoint) {
			break;
		}
	}
	fprintf(stdout, "<-- cont: 0x%08x hit_breakpoint=%d\n",
			read_reg(15), m_hit_breakpoint);
}

string tracefile_codbg_agent::next_record(uint32_t &start_off)
{
	char line[256];
	start_off = ftell(m_fd);

	if (!fgets(line, sizeof(line), m_fd)) {
		return "";
	} else {
		line[strlen(line)-1] = 0;
		return string(line);
		/*
		string ret = string(line);
		ret.resize(ret.size()-1);
		return ret;
		 */
	}
}

void tracefile_codbg_agent::process_record(const string &rec)
{
	size_t colon_idx = rec.find(':');
	if (colon_idx != string::npos) {
		if (rec.substr(colon_idx+2, 8) == "EXECUTE:") {
			string sub = rec.substr(colon_idx+2+9);
			const char *sub_s = sub.c_str();
			uint32_t addr = strtoul(sub_s, 0, 0);
//			fprintf(stdout, "addr=0x%08x\n", addr);
			execute(addr);
		} else if (rec.substr(colon_idx+2, 11) == "REG_CHANGE:") {
			string sub = rec.substr(colon_idx+2+12);
			const char *sub_s = sub.c_str();
			char *n;

			uint32_t reg = strtoul(sub_s, &n, 0);
			n++; // skip whitespace
			uint32_t val = strtoul(n, 0, 0);

			if (reg == 14) {
				val &= 0x0FFFFFFF;
			}

			write_reg(reg, val);
		} else if (rec.substr(colon_idx+2, 11) == "MEM_ACCESS:") {
			uint32_t idx = colon_idx+2+12;
			bool is_write = false;

			if (rec.at(idx) == 'R') {
				idx += 5;
			} else {
				idx += 6;
				is_write = true;
			}

			string sub = rec.substr(idx);
			const char *sub_s = sub.c_str();
			char *tmp;

			uint32_t addr = strtoul(sub_s, &tmp, 0);
			tmp++;
			uint32_t data = strtoul(tmp, 0, 0);

			write32(addr, data);
			/*
			fprintf(stdout, "%s 0x%08x 0x%08x\n",
					(is_write)?"WRITE":"READ", addr, data);
			 */
		}
	}
}

void tracefile_codbg_agent::execute(uint32_t addr)
{
	m_exec = true;

	if (is_breakpoint(addr)) {
		m_hit_breakpoint = true;
	}

	write_reg(m_pc_idx, addr);
}

