#include "gsc_exec.hpp"

#if COMPILE_EXEC == 1

#include <errno.h>
#include <pthread.h>

enum
{
	INT_VALUE,
	FLOAT_VALUE,
	STRING_VALUE,
	VECTOR_VALUE,
	OBJECT_VALUE
};

struct exec_outputline
{
	char content[MAX_STRINGLENGTH];
	exec_outputline *next;
};

struct exec_async_task
{
	exec_async_task *prev;
	exec_async_task *next;
	char command[MAX_STRINGLENGTH];
	int callback;
	bool done;
	bool save;
	bool error;
	exec_outputline *output;
	unsigned int levelId;
	bool hasargument;
	int valueType;
	int intValue;
	float floatValue;
	char stringValue[MAX_STRINGLENGTH];
	vec3_t vectorValue;
	unsigned int objectValue;
};

exec_async_task *first_exec_async_task = NULL;

void gsc_exec()
{
	char *command;

	if ( !stackGetParamString(0, &command) )
	{
		stackError("gsc_exec() argument is undefined or has wrong type");
		stackPushUndefined();
		return;
	}

	Com_DPrintf("gsc_exec() executing: %s\n", command);

	FILE *fp;

	fp = popen(command, "r");

	if ( fp == NULL )
	{
		Com_Printf("gsc_exec() popen failed: %s\n", strerror(errno));
		stackPushUndefined();
		return;
	}

	char c;
	int curpos = 0;
	char content[MAX_STRINGLENGTH];

	stackPushArray();

	while ( (c = getc(fp)) != EOF )
	{
		if ( c == '\n' || curpos == MAX_STRINGLENGTH - 1 )
		{
			content[curpos] = '\0';
			stackPushString(content);
			stackPushArrayLast();
			curpos = 0;
		}
		else
		{
			content[curpos] = c;
			curpos++;
		}
	}

	content[curpos] = '\0';

	stackPushString(content);
	stackPushArrayLast();

	pclose(fp);
}

void *exec_async(void *input_c)
{
	exec_async_task *task = (exec_async_task*)input_c;
	FILE *fp;

	fp = popen(task->command, "r");

	if ( fp == NULL )
	{	
		Com_Printf("exec_async() popen failed: %s\n", strerror(errno));
		task->error = true;
		return NULL;
	}

	if ( task->save )
	{
		exec_outputline *output = new exec_outputline;
		task->output = output;
		output->next = NULL;

		char c;
		int curpos = 0;

		while ( (c = getc(fp)) != EOF )
		{
			if ( c == '\n' || curpos == MAX_STRINGLENGTH - 1 )
			{
				output->content[curpos] = '\0';
				output->next = new exec_outputline;
				output = output->next;
				output->next = NULL;
				curpos = 0;
			}
			else
			{
				output->content[curpos] = c;
				curpos++;
			}
		}

		output->content[curpos] = '\0';
	}
	else
		while ( getc(fp) != EOF ); // Make thread wait for function to finish
	
	pclose(fp);
	task->done = true;
	return NULL;
}

void gsc_exec_async_create()
{
	char *command;
	int callback;
	
	if ( !stackGetParamString(0, &command) )
	{
		stackError("gsc_exec_async_create() argument is undefined or has wrong type");
		stackPushUndefined();
		return;
	}

	exec_async_task *current = first_exec_async_task;

	while ( current != NULL && current->next != NULL )
		current = current->next;

	exec_async_task *newtask = new exec_async_task;

	strncpy(newtask->command, command, MAX_STRINGLENGTH - 1);
	newtask->command[MAX_STRINGLENGTH - 1] = '\0';
	newtask->output = NULL;
	newtask->prev = current;
	newtask->next = NULL;

	if ( !stackGetParamFunction(1, &callback) )
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = true;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if ( stackGetParamInt(2, &valueInt) )
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if ( stackGetParamFloat(2, &valueFloat) )
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if ( stackGetParamString(2, &valueString) )
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if ( stackGetParamVector(2, valueVector) )
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if ( stackGetParamObject(2, &valueObject) )
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if ( current != NULL )
		current->next = newtask;
	else
		first_exec_async_task = newtask;

	Com_DPrintf("gsc_exec_async_create() executing: %s\n", command);
	
	pthread_t exec_doer;

	if ( pthread_create(&exec_doer, NULL, exec_async, newtask) != 0 )
	{
		stackError("gsc_exec_async_create() error creating exec async handler thread!");
		stackPushUndefined();
		return;
	}

	if ( pthread_detach(exec_doer) != 0 )
	{
		stackError("gsc_exec_async_create() error detaching exec async handler thread!");
		stackPushUndefined();
		return;
	}

	stackPushInt(1);
}

void gsc_exec_async_create_nosave()
{
	char *command;
	int callback;

	if ( !stackGetParamString(0, &command) )
	{
		stackError("gsc_exec_async_create_nosave() argument is undefined or has wrong type");
		stackPushUndefined();
		return;
	}

	exec_async_task *current = first_exec_async_task;

	while ( current != NULL && current->next != NULL )
		current = current->next;

	exec_async_task *newtask = new exec_async_task;

	strncpy(newtask->command, command, MAX_STRINGLENGTH - 1);
	newtask->command[MAX_STRINGLENGTH - 1] = '\0';
	newtask->output = NULL;
	newtask->prev = current;
	newtask->next = NULL;

	if ( !stackGetParamFunction(1, &callback) )
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->done = false;
	newtask->save = false;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if ( stackGetParamInt(2, &valueInt) )
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if ( stackGetParamFloat(2, &valueFloat) )
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if ( stackGetParamString(2, &valueString) )
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if ( stackGetParamVector(2, valueVector) )
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if ( stackGetParamObject(2, &valueObject) )
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if ( current != NULL )
		current->next = newtask;
	else
		first_exec_async_task = newtask;

	Com_DPrintf("gsc_exec_async_create_nosave() executing: %s\n", command);
	
	pthread_t exec_doer;

	if ( pthread_create(&exec_doer, NULL, exec_async, newtask) != 0 )
	{
		stackError("gsc_exec_async_create_nosave() error creating exec async handler thread!");
		stackPushUndefined();
		return;
	}

	if ( pthread_detach(exec_doer) != 0 )
	{
		stackError("gsc_exec_async_create_nosave() error detaching exec async handler thread!");
		stackPushUndefined();
		return;
	}

	stackPushInt(1);
}

void gsc_exec_async_checkdone()
{
	exec_async_task *current = first_exec_async_task;

	while ( current != NULL )
	{
		exec_async_task *task = current;
		current = current->next;
		if ( task->done )
		{
			if ( Scr_IsSystemActive() && task->save && task->callback && !task->error && scrVarPub.levelId == task->levelId )
			{
				if ( task->hasargument )
				{
					switch ( task->valueType )
					{
					case INT_VALUE:
						stackPushInt(task->intValue);
						break;

					case FLOAT_VALUE:
						stackPushFloat(task->floatValue);
						break;

					case STRING_VALUE:
						stackPushString(task->stringValue);
						break;

					case VECTOR_VALUE:
						stackPushVector(task->vectorValue);
						break;

					case OBJECT_VALUE:
						stackPushObject(task->objectValue);
						break;

					default:
						stackPushUndefined();
						break;
					}
				}

				stackPushArray();
				exec_outputline *output = task->output;
				bool hasoutput = false;

				while ( output != NULL )
				{
					exec_outputline *next = output->next;
					stackPushString(output->content);
					stackPushArrayLast();
					delete output;
					output = next;
					hasoutput = true;
				}

				if ( !hasoutput )
				{
					stackPushUndefined();
					stackPushArrayLast();
				}
				
				short ret = Scr_ExecThread(task->callback, task->save + task->hasargument);
				Scr_FreeThread(ret);
			}

			// Free task
			if ( task->next != NULL )
				task->next->prev = task->prev;

			if ( task->prev != NULL )
				task->prev->next = task->next;
			else
				first_exec_async_task = task->next;

			delete task;
		}
	}
}

#endif