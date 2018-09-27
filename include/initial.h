#ifndef INITIAL_H
#define INITIAL_H

#define DEV_REAL_ADDR(addr)	(addr + (DEV_IL_START * DEV_REGBLOCK_SIZE))
#define LINENO(addr)		((DEV_REAL_ADDR(addr) - DEV_REG_START) / DEV_REGBLOCK_SIZE)
#define DEVICENO(addr)		(((DEV_REAL_ADDR(addr) - DEV_REG_START) % DEV_REGBLOCK_SIZE) / DEV_REG_SIZE)
#define TERMTYPE(addr)		((((DEV_REAL_ADDR(addr) - DEV_REG_START) % DEV_REGBLOCK_SIZE) % DEV_REG_SIZE) / (2 * WS))
#define TERMNO(addr)		(DEVICENO(addr) + (TERMTYPE(addr) * DEV_PER_INT))

typedef unsigned int memaddr;
typedef unsigned int cpu_t;

extern void test();
void newArea(memaddr address, void handler());

#endif
