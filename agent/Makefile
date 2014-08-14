
EXEEXT=.exe

all : arm_codbg_trace_server$(EXEEXT)

MEM_MODEL_SRC = \
	codbg_memory.cpp

CODBG_AGENT_SRC = \
	codbg_agent.cpp

GDB_RSP_CODBG_AGENT_SRC = \
	gdb_rsp_codbg_agent.cpp \
	gdb_rsp_msg.cpp
	

ARM_CODBG_TRACE_SERVER_SRC = \
	$(CODBG_AGENT_SRC) \
	$(GDB_RSP_CODBG_AGENT_SRC) \
	$(MEM_MODEL_SRC) \
	tracefile_codbg_agent.cpp \
	arm_codbg_trace_server.cpp \
	
	
ARM_CODBG_TRACE_SERVER_OBJS = $(ARM_CODBG_TRACE_SERVER_SRC:.cpp=.o)	

arm_codbg_trace_server$(EXEEXT) : $(ARM_CODBG_TRACE_SERVER_OBJS)
	$(CXX) -o $@ $^ -lwsock32