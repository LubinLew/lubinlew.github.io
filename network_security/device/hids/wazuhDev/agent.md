# Agent

编译 Agent

```bash
## 编译 Agnet 选项
make TARGET=agent     # to build the unix agent
make TARGET=winagent  # to build the windows agent

## Agent 依赖 external(各种库依赖如json/zlib等), 编译 ${BUILD_AGENT}
agent: external
	${MAKE} ${BUILD_AGENT}

## ${BUILD_AGENT} 由多个程序组成
BUILD_AGENT+=navsec-agentd
BUILD_AGENT+=agent-auth
BUILD_AGENT+=navsec-agent-auth
BUILD_AGENT+=navsec-logcollector
BUILD_AGENT+=navsec-syscheckd
BUILD_AGENT+=navsec-execd
BUILD_AGENT+=manage_agents
BUILD_AGENT+=navsec-modulesd
```

### navsec-agentd


```makefile

client_agent_c := $(wildcard client-agent/*.c)
client_agent_o := $(client_agent_c:.c=.o)

# 编译 src/client-agent下*.c 文件的规则
client-agent/%.o: client-agent/%.c
	${OSSEC_CC} ${OSSEC_CFLAGS} -I./client-agent -DARGV0=\"navsec-agentd\" -c $^ -o $@

## navsec-agentd 依赖 src/client-agent/*.o 等
navsec-agentd: ${client_agent_o} monitord/rotate_log.o monitord/compress_log.o
	${OSSEC_CCBIN} ${OSSEC_LDFLAGS} $^ ${OSSEC_LIBS} -o $@
```
