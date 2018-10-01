#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

void trapHandler(unsigned int oldArea);
void tlbHandler();
void pgmtrapHandler();
void sysbkHandler();

#endif
