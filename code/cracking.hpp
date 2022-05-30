#ifndef _CRACKING_HPP_
#define _CRACKING_HPP_

#include "gsc.hpp"

void cracking_hook_function(int from, int to);
void cracking_hook_call(int from, int to);

int cracking_write_hex(int address, char *hex);
int singleHexToNumber(char hexchar);
int hexToBuffer(char *hex, char *buffer, int bufferLen);

class cHook
{
public:
	int from;
	int to;
	unsigned char oldCode[5];
	cHook(int from, int to);
	void hook();
	void unhook();
};

#endif