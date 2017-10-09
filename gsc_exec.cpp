#include "gsc_exec.hpp"

#if COMPILE_EXEC == 1

#include <pthread.h>

struct exec_outputline
{
	char content[COD2_MAX_STRINGLENGTH];
	exec_outputline *next;
};

struct exec_async_task
{
	exec_async_task *prev;
	exec_async_task *next;
	char command[COD2_MAX_STRINGLENGTH];
	int callback;
	bool done;
	bool save;
	bool error;
	bool hascallback;
	exec_outputline *output;
	unsigned int levelId;
};

exec_async_task *first_exec_async_task = NULL;

void gsc_exec()
{
	char *command;
	if(!stackGetParamString(0, &command))
	{
		stackError("gsc_exec() argument is undefined or has wrong type");
		stackPushUndefined();
		return;
	}
	Com_DPrintf("gsc_exec() executing: %s\n", command);
	FILE *fp;
	fp = popen(command, "r");
	if(fp == NULL)
	{
		stackPushUndefined();
		return;
	}
	char c;
	int curpos = 0;
	char content[COD2_MAX_STRINGLENGTH];
	stackPushArray();
	while((c = getc(fp)) != EOF)
	{
		if(c == '\n' || curpos == COD2_MAX_STRINGLENGTH - 1)
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
	if(fp == NULL)
	{
		task->error = true;
		return NULL;
	}
	if(task->save)
	{
		exec_outputline *output = new exec_outputline;
		task->output = output;
		output->next = NULL;
		char c;
		int curpos = 0;
		while((c = getc(fp)) != EOF)
		{
			if(c == '\n' || curpos == COD2_MAX_STRINGLENGTH - 1)
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
	{
		while(getc(fp) != EOF); //make thread wait for function to finish
	}
	pclose(fp);
	task->done = true;
	return NULL;
}

void gsc_exec_async_create()
{
	char *command;
	int callback;
	if(!stackGetParamString(0, &command))
	{
		stackError("gsc_exec_async_create() argument is undefined or has wrong type");
		stackPushUndefined();
		return;
	}
	Com_DPrintf("gsc_exec_async_create() executing: %s\n", command);
	exec_async_task *current = first_exec_async_task;
	while(current != NULL && current->next != NULL)
		current = current->next;
	exec_async_task *newtask = new exec_async_task;
	strncpy(newtask->command, command, COD2_MAX_STRINGLENGTH - 1);
	newtask->command[COD2_MAX_STRINGLENGTH - 1] = '\0';
	newtask->output = NULL;
	newtask->prev = current;
	newtask->next = NULL;
	if(!stackGetParamFunction(1, &callback))
		newtask->hascallback = false;
	else
	{
		newtask->callback = callback;
		newtask->hascallback = true;
	}
	newtask->done = false;
	newtask->save = true;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;

	if(current != NULL)
		current->next = newtask;
	else
		first_exec_async_task = newtask;

	pthread_t exec_doer;
	int error = pthread_create(&exec_doer, NULL, exec_async, newtask);
	if(error)
	{
		Com_DPrintf("error: %i\n", error);
		Com_DPrintf("Error detaching async handler thread\n");
		return;
	}
	pthread_detach(exec_doer);
	stackPushInt(1);
}

void gsc_exec_async_create_nosave()
{
	char *command;
	int callback;
	if(!stackGetParamString(0, &command))
	{
		stackError("gsc_exec_async_create_nosave() argument is undefined or has wrong type");
		stackPushUndefined();
		return;
	}
	Com_DPrintf("gsc_exec_async_create_nosave() executing: %s\n", command);
	exec_async_task *current = first_exec_async_task;
	while(current != NULL && current->next != NULL)
		current = current->next;
	exec_async_task *newtask = new exec_async_task;
	strncpy(newtask->command, command, COD2_MAX_STRINGLENGTH - 1);
	newtask->command[COD2_MAX_STRINGLENGTH - 1] = '\0';
	newtask->output = NULL;
	newtask->prev = current;
	newtask->next = NULL;
	if(!stackGetParamFunction(1, &callback))
		newtask->hascallback = false;
	else
	{
		newtask->callback = callback;
		newtask->hascallback = true;
	}
	newtask->done = false;
	newtask->save = false;
	newtask->error = false;
	newtask->levelId = scrVarPub.levelId;

	if(current != NULL)
		current->next = newtask;
	else
		first_exec_async_task = newtask;

	pthread_t exec_doer;
	int error = pthread_create(&exec_doer, NULL, exec_async, newtask);
	if(error)
	{
		Com_DPrintf("error: %i\n", error);
		Com_DPrintf("Error detaching async handler thread\n");
		return;
	}
	pthread_detach(exec_doer);
	stackPushInt(1);
}

void gsc_exec_async_checkdone()
{
	exec_async_task *current = first_exec_async_task;
	while(current != NULL)
	{
		exec_async_task *task = current;
		current = current->next;
		if(task->done)
		{
			//push to cod
			if(task->hascallback && (scrVarPub.levelId == task->levelId))
			{
				if(task->save && !task->error)
				{
					stackPushArray();
					exec_outputline *output = task->output;
					while(output != NULL)
					{
						exec_outputline *next = output->next;
						stackPushString(output->content);
						stackPushArrayLast();
						delete output;
						output = next;
					}
				}
				short ret = Scr_ExecThread(task->callback, (task->save && !task->error));
				Scr_FreeThread(ret);
			}
			//free task
			if(task->next != NULL)
				task->next->prev = task->prev;
			if(task->prev != NULL)
				task->prev->next = task->next;
			else
				first_exec_async_task = task->next;
			delete task;
		}
	}
}

#endif