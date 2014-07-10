/*
 * arm_codbg_trace_server.cpp
 *
 *  Created on: Jul 6, 2014
 *      Author: ballance
 */
#include "gdb_rsp_codbg_agent.h"
#include "tracefile_codbg_agent.h"
#include <stdlib.h>

/**
 * .elf .trc
 */
int main(int argc, char **argv) {
	const char *tracefile = 0;
	const char *exefile = 0;

	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
		} else {
			if (!exefile) {
				exefile = argv[i];
			} else if (!tracefile) {
				tracefile = argv[i];
			} else {
				fprintf(stdout, "Error: Unknown argument %s\n", argv[i]);
				exit(1);
			}
		}
	}

	if (!exefile) {
		fprintf(stdout, "Error: no exefile specified\n");
		exit(1);
	}
	if (!tracefile) {
		fprintf(stdout, "Error: no tracefile specified\n");
		exit(1);
	}

	tracefile_codbg_agent *trace_agent = new tracefile_codbg_agent(26, 15);
	trace_agent->init(tracefile);
	trace_agent->load_exe(exefile);
	gdb_rsp_codbg_agent *agent = new gdb_rsp_codbg_agent(trace_agent, 4321);

	agent->main();

}



