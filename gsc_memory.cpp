#include "gsc_memory.hpp"
#include <vector>

#if COMPILE_MEMORY == 1



/* ->CPP */

struct S_POINTER
{
	int address;
	char type;
};
std::vector<S_POINTER> Pointers;

struct binarybuffer
{
	int address;
	int pos;
	std::vector<char *> *strings;
};

/*
S_POINTER get_pointer_byADDR(int address)
{
	size_t size = Pointers.size();
	for (int i = 0; i < size; i++)
		if (Pointers[i].address == address)
			return Pointers[i];
}
*/

int get_ipointer_byADDR(int address)
{
	size_t size = Pointers.size();
	for (size_t i = 0; i < size; i++)
		if (Pointers[i].address == address)
			return i;
		
	return -1;
}

void gsc_memory_pointer_new()
{
	char pointerType;

	if ( ! stackGetParams("s", &pointerType))
	{
		stackError("gsc_memory_pointer_new() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	
	S_POINTER Pointer;
			
	switch(pointerType)
	{
		case 's':
			Pointer.address = (int)new char;
			break;
			
		case 'i':
			Pointer.address = (int)new int;
			break;
			
		default:
			stackPushUndefined();
			return;
	}
	
	Pointer.type = pointerType;
	Pointers.push_back(Pointer);
	
	stackPushInt(Pointer.address);
}

void gsc_memory_pointer_free()
{
	int Address;

	if ( ! stackGetParams("i", &Address))
	{
		stackError("gsc_memory_pointer_free() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	delete (int *)Address;
	stackPushInt(0);
}

void gsc_memory_pointer_set() /* DATA */
{
	int Address;
	int Value;

	if ( ! stackGetParams("ii", &Address, &Value))
	{
		stackError("gsc_memory_pointer_set() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	
	int indexPointer;
	if ((indexPointer = get_ipointer_byADDR(Address)) != -1)
	{
		S_POINTER Pointer = Pointers[indexPointer];
		switch (Pointer.type)
		{
			case 's':
				*(char *)Pointer.address = (char)Value;
				stackPushBool(qtrue);
				return;
				
			case 'i':
				*(int *)Pointer.address = Value;
				stackPushBool(qtrue);
				return;
		}
	}
	
	stackPushBool(qfalse);
}

/*
void gsc_memory_pointer_get()
{
	int Address;
	int Value;

	if ( ! stackGetParams("ii", &Address, &Value))
	{
		stackError("gsc_memory_pointer_get() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	
	int indexPointer;
	if ((indexPointer = get_ipointer_byADDR(Address)) != -1)
	{
		S_POINTER Pointer = Pointers[indexPointer];
		switch (Pointer.type)
		{
			case 's':
				stackPushString(*(char *)Pointer.address);
				return;
				
			case 'i':
				stackPushInt(*(int *)Pointer.address);
				return;
		}
	}
	
	stackPushUndefined();
}
*/

/* <-CPP */



void gsc_memory_malloc()
{
	int bytes;

	if ( ! stackGetParams("i", &bytes))
	{
		stackError("gsc_memory_malloc() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	
	stackPushInt((int) malloc(bytes));
}

void gsc_memory_free()
{
	int memory;

	if ( ! stackGetParams("i", &memory))
	{
		stackError("gsc_memory_free() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	free((void*)memory);
	stackPushInt(0);
}

void gsc_memory_int_get()
{
	int memory;

	if ( ! stackGetParams("i", &memory))
	{
		stackError("gsc_memory_int_get() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	stackPushInt(*(int*)memory);
}

void gsc_memory_int_set()
{
	int memory, value;

	if ( ! stackGetParams("ii", &memory, &value))
	{
		stackError("gsc_memory_int_set() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	*(int*)memory = value;
	stackPushInt(1);
}

void gsc_memory_memset()
{
	int memory, value, bytes;

	if ( ! stackGetParams("iii", &memory, &value, &bytes))
	{
		stackError("gsc_memory_memset() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	memset((void*)memory, value, bytes);
	stackPushInt(1);
}

void gsc_binarybuffer_new()
{
	int address;
	if ( ! stackGetParams("i", &address))
	{
		stackError("gsc_binarybuffer_new() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	struct binarybuffer *bb = (struct binarybuffer *)malloc(sizeof(struct binarybuffer));
	bb->address = address;
	bb->pos = 0;
	bb->strings = new std::vector<char *>();
	stackPushInt((int)bb);
}

void gsc_binarybuffer_free()
{
	struct binarybuffer *bb;
	if ( ! stackGetParams("i", &bb))
	{
		stackError("gsc_binarybuffer_free() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	for (std::vector<char *>::const_iterator i = bb->strings->begin(); i != bb->strings->end(); i++)
		free(*i);
	delete bb->strings;
	free(bb);
	stackPushInt(1);
}

void gsc_binarybuffer_seek()
{
	struct binarybuffer *bb;
	int pos;
	if ( ! stackGetParams("ii", &bb, &pos))
	{
		stackError("gsc_binarybuffer_seek() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	bb->pos = pos;
	stackPushInt(1);
}

void gsc_binarybuffer_write()
{
	struct binarybuffer *bb;
	char *type;
	if ( ! stackGetParams("is", &bb, &type))
	{
		stackError("gsc_binarybuffer_write() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	switch (type[0])
	{
	case 'i':
	{
		int tmp_int;
		stackGetParamInt(2, &tmp_int);
		*(int *)(bb->address + bb->pos) = tmp_int;
		bb->pos += 4;
		break;
	}
	case 'f':
	{
		float tmp_float;
		stackGetParamFloat(2, &tmp_float);
		*(float *)(bb->address + bb->pos) = tmp_float;
		bb->pos += 4;
		break;
	}
	case 'd':
	{
		float tmp_float;
		stackGetParamFloat(2, &tmp_float);
		*(double *)(bb->address + bb->pos) = (double)tmp_float;
		bb->pos += 8;
		break;
	}
	case 's':
	{
		char *tmp_str;
		stackGetParamString(2, &tmp_str);
		char *copy = (char *)malloc(strlen(tmp_str) + 1);
		strcpy(copy, tmp_str);
		bb->strings->push_back(copy);
		*(char **)(bb->address + bb->pos) = copy;
		bb->pos += 4;
		break;
	}
	case 'c':
	{
		char *tmp_str;
		stackGetParamString(2, &tmp_str);
		*(char *)(bb->address + bb->pos) = tmp_str[0];
		bb->pos += 1;
		break;
	}
	case 'v':
	{
		float tmp_vector[3];
		stackGetParamVector(2, tmp_vector);
		*(float *)(bb->address + bb->pos + 0) = tmp_vector[0];
		*(float *)(bb->address + bb->pos + 4) = tmp_vector[1];
		*(float *)(bb->address + bb->pos + 8) = tmp_vector[2];
		bb->pos += 12;
		break;
	}
	}
	stackPushInt(1);
}

void gsc_binarybuffer_read()
{
	struct binarybuffer *bb;
	char *type;
	if ( ! stackGetParams("is", &bb, &type))
	{
		stackError("gsc_binarybuffer_read() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}
	switch (type[0])
	{
	case 'i':
	{
		int tmp_int;
		tmp_int = *(int *)(bb->address + bb->pos);
		bb->pos += 4;
		stackPushInt(tmp_int);
		return;
	}
	case 'f':
	{
		float tmp_float;
		tmp_float = *(float *)(bb->address + bb->pos);
		bb->pos += 4;
		stackPushFloat(tmp_float);
		return;
	}
	case 'd':
	{
		float tmp_float;
		tmp_float = (float)*(double *)(bb->address + bb->pos);
		bb->pos += 8;
		stackPushFloat(tmp_float);
		return;
	}
	case 's':
	{
		char *tmp_str;
		tmp_str = *(char **)(bb->address + bb->pos);
		bb->pos += 4;
		stackPushString(tmp_str);
		return;
	}
	case 'c':
	{
		char tmp_str[2];
		tmp_str[0] = *(char *)(bb->address + bb->pos);
		tmp_str[1] = '\0';
		bb->pos += 1;
		stackPushString(tmp_str);
		return;
	}
	case 'v':
	{
		float *tmp_vector;
		tmp_vector = (float *)(bb->address + bb->pos + 0);
		bb->pos += 12;
		stackPushVector(tmp_vector);
		return;
	}
	}
	stackPushUndefined();
}
#endif
