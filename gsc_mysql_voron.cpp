#include "gsc_mysql_voron.hpp"

#if COMPILE_MYSQL_VORON == 1

#include <mysql/mysql.h>
#include <pthread.h>

#define INVALID_ENTITY -1
#define INVALID_STATE 0

enum
{
	INT_VALUE,
	FLOAT_VALUE,
	STRING_VALUE,
	VECTOR_VALUE,
	OBJECT_VALUE
};

struct async_mysql_task
{
	async_mysql_task *prev;
	async_mysql_task *next;
	int id;
	char query[COD2_MAX_STRINGLENGTH];
	int callback;
	bool done;
	bool complete;
	bool save;
	bool cleanup;
	MYSQL_RES *result;
	unsigned int levelId;
	bool hasargument;
	int valueType;
	int intValue;
	float floatValue;
	char stringValue[COD2_MAX_STRINGLENGTH];
	vec3_t vectorValue;
	unsigned int objectValue;
	int entityNum;
	int entityState;
};

MYSQL *async_mysql_connection = NULL;
async_mysql_task *first_async_mysql_task = NULL;
pthread_mutex_t lock_async_mysql;
int async_task_id = 0;

async_mysql_task *task_id_to_pointer(int id)
{
	async_mysql_task *current = first_async_mysql_task;

	while (current != NULL)
	{
		async_mysql_task *task = current;
		current = current->next;

		if (task->id == id)
			return task;
	}

	return NULL;
}

void *async_mysql_query_handler(void* dummy)
{
	while(1)
	{
		async_mysql_task *current = first_async_mysql_task;

		while (current != NULL)
		{
			async_mysql_task *task = current;
			current = current->next;

			if (!task->done)
			{
				int res = mysql_query(async_mysql_connection, task->query);

				if (res == 0)
					task->result = mysql_store_result(async_mysql_connection);
				else
				{
					task->complete = true;
					task->cleanup = true;
				}

				task->done = true;
			}

			if (task->cleanup)
			{
				pthread_mutex_lock(&lock_async_mysql);

				if (task->next != NULL)
					task->next->prev = task->prev;

				if (task->prev != NULL)
					task->prev->next = task->next;
				else
					first_async_mysql_task = task->next;

				if (task->result != NULL)
					mysql_free_result(task->result);

				delete task;

				pthread_mutex_unlock(&lock_async_mysql);
			}
		}

		usleep(10000);
	}

	return NULL;
}

void gsc_async_mysql_initialize()
{
	char *host, *user, *pass, *db;
	int port;

	if ( ! stackGetParams("ssssi", &host, &user, &pass, &db, &port))
	{
		stackError("gsc_async_mysql_initialize() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		MYSQL *my = mysql_init(NULL);
		my_bool reconnect = true;
		mysql_options(my, MYSQL_OPT_RECONNECT, &reconnect);

		if (!mysql_real_connect(my, host, user, pass, db, port, NULL, 0))
		{
			stackError("gsc_async_mysql_initialize() failed to initialize async mysql connection!");
			stackPushUndefined();
			return;
		}

		if (pthread_mutex_init(&lock_async_mysql, NULL) != 0)
		{
			stackError("gsc_async_mysql_initialize() mutex initialization failed!");
			stackPushUndefined();
			return;
		}

		pthread_t async_handler;

		if (pthread_create(&async_handler, NULL, async_mysql_query_handler, NULL) != 0)
		{
			stackError("gsc_mysql_async_initializer() error creating async mysql handler thread!");
			stackPushUndefined();
			return;
		}

		if (pthread_detach(async_handler) != 0)
		{
			stackError("gsc_mysql_async_initializer() error detaching async mysql handler thread!");
			stackPushUndefined();
			return;
		}

		async_mysql_connection = (MYSQL*)my;
	}
	else
		Com_DPrintf("gsc_async_mysql_initialize() async connection already initialized.\n");

	stackPushInt((int)async_mysql_connection);
}

void gsc_async_mysql_close()
{
	if (async_mysql_connection != NULL)
	{
		mysql_close(async_mysql_connection);
		stackPushInt(1);
	}
	else
		stackPushInt(0);
}

void gsc_async_mysql_create_query()
{
	char *query;

	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_async_mysql_create_query() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	pthread_mutex_lock(&lock_async_mysql);

	async_mysql_task *current = first_async_mysql_task;

	while (current != NULL && current->next != NULL)
		current = current->next;

	async_mysql_task *newtask = new async_mysql_task;

	newtask->id = async_task_id;

	if (async_task_id == 2147483647)
		async_task_id = 0;
	else
		async_task_id++;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	newtask->prev = current;
	newtask->next = NULL;

	int callback;

	if (!stackGetParamFunction(1, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->result = NULL;
	newtask->done = false;
	newtask->complete = false;
	newtask->save = true;
	newtask->cleanup = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = INVALID_ENTITY;
	newtask->entityState = INVALID_STATE;

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(2, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(2, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(2, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(2, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(2, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_mysql_task = newtask;

	pthread_mutex_unlock(&lock_async_mysql);

	stackPushInt(1);
}

void gsc_async_mysql_create_query_nosave()
{
	char *query;

	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_async_mysql_create_query_nosave() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	pthread_mutex_lock(&lock_async_mysql);

	async_mysql_task *current = first_async_mysql_task;

	while (current != NULL && current->next != NULL)
		current = current->next;

	async_mysql_task *newtask = new async_mysql_task;

	newtask->id = async_task_id;

	if (async_task_id == 2147483647)
		async_task_id = 0;
	else
		async_task_id++;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	newtask->prev = current;
	newtask->next = NULL;

	int callback;

	if (!stackGetParamFunction(1, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->result = NULL;
	newtask->done = false;
	newtask->complete = false;
	newtask->save = false;
	newtask->cleanup = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = INVALID_ENTITY;
	newtask->entityState = INVALID_STATE;

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(2, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(2, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(2, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(2, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(2, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_mysql_task = newtask;

	pthread_mutex_unlock(&lock_async_mysql);

	stackPushInt(1);
}

void gsc_async_mysql_create_entity_query(int entid)
{
	char *query;

	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_async_mysql_create_entity_query() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	pthread_mutex_lock(&lock_async_mysql);

	async_mysql_task *current = first_async_mysql_task;

	while (current != NULL && current->next != NULL)
		current = current->next;

	async_mysql_task *newtask = new async_mysql_task;

	newtask->id = async_task_id;

	if (async_task_id == 2147483647)
		async_task_id = 0;
	else
		async_task_id++;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	newtask->prev = current;
	newtask->next = NULL;

	int callback;

	if (!stackGetParamFunction(1, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->result = NULL;
	newtask->done = false;
	newtask->complete = false;
	newtask->save = true;
	newtask->cleanup = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = entid;
	newtask->entityState = *(int *)(G_ENTITY(newtask->entityNum) + 1);

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(2, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(2, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(2, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(2, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(2, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_mysql_task = newtask;

	pthread_mutex_unlock(&lock_async_mysql);

	stackPushInt(1);
}

void gsc_async_mysql_create_entity_query_nosave(int entid)
{
	char *query;

	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_async_mysql_create_entity_query_nosave() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	pthread_mutex_lock(&lock_async_mysql);

	async_mysql_task *current = first_async_mysql_task;

	while (current != NULL && current->next != NULL)
		current = current->next;

	async_mysql_task *newtask = new async_mysql_task;

	newtask->id = async_task_id;

	if (async_task_id == 2147483647)
		async_task_id = 0;
	else
		async_task_id++;

	strncpy(newtask->query, query, COD2_MAX_STRINGLENGTH - 1);
	newtask->query[COD2_MAX_STRINGLENGTH - 1] = '\0';

	newtask->prev = current;
	newtask->next = NULL;

	int callback;

	if (!stackGetParamFunction(1, &callback))
		newtask->callback = 0;
	else
		newtask->callback = callback;

	newtask->result = NULL;
	newtask->done = false;
	newtask->complete = false;
	newtask->save = false;
	newtask->cleanup = false;
	newtask->levelId = scrVarPub.levelId;
	newtask->hasargument = true;
	newtask->entityNum = entid;
	newtask->entityState = *(int *)(G_ENTITY(newtask->entityNum) + 1);

	int valueInt;
	float valueFloat;
	char *valueString;
	vec3_t valueVector;
	unsigned int valueObject;

	if (stackGetParamInt(2, &valueInt))
	{
		newtask->valueType = INT_VALUE;
		newtask->intValue = valueInt;
	}
	else if (stackGetParamFloat(2, &valueFloat))
	{
		newtask->valueType = FLOAT_VALUE;
		newtask->floatValue = valueFloat;
	}
	else if (stackGetParamString(2, &valueString))
	{
		newtask->valueType = STRING_VALUE;
		strcpy(newtask->stringValue, valueString);
	}
	else if (stackGetParamVector(2, valueVector))
	{
		newtask->valueType = VECTOR_VALUE;
		newtask->vectorValue[0] = valueVector[0];
		newtask->vectorValue[1] = valueVector[1];
		newtask->vectorValue[2] = valueVector[2];
	}
	else if (stackGetParamObject(2, &valueObject))
	{
		newtask->valueType = OBJECT_VALUE;
		newtask->objectValue = valueObject;
	}
	else
		newtask->hasargument = false;

	if (current != NULL)
		current->next = newtask;
	else
		first_async_mysql_task = newtask;

	pthread_mutex_unlock(&lock_async_mysql);

	stackPushInt(1);
}

void gsc_async_mysql_checkdone()
{
	async_mysql_task *current = first_async_mysql_task;

	while (current != NULL)
	{
		async_mysql_task *task = current;
		current = current->next;

		if (task->done && !task->complete)
		{
			task->complete = true;

			if (Scr_IsSystemActive() && task->save && task->callback && (scrVarPub.levelId == task->levelId))
			{
				if (task->entityNum != INVALID_ENTITY)
				{
					if (task->entityState != INVALID_STATE)
					{
						int state = *(int *)(G_ENTITY(task->entityNum) + 1);

						if (state != INVALID_STATE && state == task->entityState)
						{
							if (task->hasargument)
							{
								switch(task->valueType)
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

							stackPushInt(task->id);

							short ret = Scr_ExecEntThread(G_ENTITY(task->entityNum), task->callback, task->save + task->hasargument);
							Scr_FreeThread(ret);
						}
						else
							task->cleanup = true;
					}
					else
						task->cleanup = true;
				}
				else
				{
					if (task->hasargument)
					{
						switch(task->valueType)
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

						default:
							stackPushUndefined();
							break;
						}
					}

					stackPushInt(task->id);

					short ret = Scr_ExecThread(task->callback, task->save + task->hasargument);
					Scr_FreeThread(ret);
				}
			}
			else
				task->cleanup = true;
		}
	}
}

void gsc_async_mysql_errno()
{
	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_errno() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_errno(async_mysql_connection));
}

void gsc_async_mysql_error()
{
	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_error() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushString((char *)mysql_error(async_mysql_connection));
}

void gsc_async_mysql_affected_rows()
{
	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_affected_rows() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_affected_rows(async_mysql_connection));
}

void gsc_async_mysql_num_rows()
{
	int id;

	if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_async_mysql_num_rows() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_num_rows() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	async_mysql_task *target_task = task_id_to_pointer(id);

	if (target_task == NULL)
	{
		stackError("gsc_async_mysql_num_rows() target_task is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	if (target_task->result == NULL)
	{
		stackError("gsc_async_mysql_num_rows() result is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_num_rows(target_task->result));
}

void gsc_async_mysql_num_fields()
{
	int id;

	if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_async_mysql_num_fields() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_num_fields() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	async_mysql_task *target_task = task_id_to_pointer(id);

	if (target_task == NULL)
	{
		stackError("gsc_async_mysql_num_fields() target_task is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	if (target_task->result == NULL)
	{
		stackError("gsc_async_mysql_num_fields() result is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_num_fields(target_task->result));
}

void gsc_async_mysql_field_seek()
{
	int id;
	int offset;

	if ( ! stackGetParams("ii", &id, &offset))
	{
		stackError("gsc_async_mysql_field_seek() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_field_seek() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	async_mysql_task *target_task = task_id_to_pointer(id);

	if (target_task == NULL)
	{
		stackError("gsc_async_mysql_field_seek() target_task is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	if (target_task->result == NULL)
	{
		stackError("gsc_async_mysql_field_seek() result is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_field_seek(target_task->result, offset));
}

void gsc_async_mysql_fetch_field()
{
	int id;

	if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_async_mysql_fetch_field() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_fetch_field() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	async_mysql_task *target_task = task_id_to_pointer(id);

	if (target_task == NULL)
	{
		stackError("gsc_async_mysql_fetch_field() target_task is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	if (target_task->result == NULL)
	{
		stackError("gsc_async_mysql_fetch_field() result is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	MYSQL_FIELD *field = mysql_fetch_field(target_task->result);

	if (field == NULL)
	{
		stackPushUndefined();
		return;
	}

	char *ret = field->name;
	stackPushString(ret);
}

void gsc_async_mysql_fetch_row()
{
	int id;

	if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_async_mysql_fetch_row() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_fetch_row() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	async_mysql_task *target_task = task_id_to_pointer(id);

	if (target_task == NULL)
	{
		stackError("gsc_async_mysql_fetch_row() target_task is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	if (target_task->result == NULL)
	{
		stackError("gsc_async_mysql_fetch_row() result is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	MYSQL_ROW row = mysql_fetch_row(target_task->result);

	if (!row)
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	int numfields = mysql_num_fields(target_task->result);

	for (int i = 0; i < numfields; i++)
	{
		if (row[i] == NULL)
			stackPushUndefined();
		else
			stackPushString(row[i]);

		stackPushArrayLast();
	}
}

void gsc_async_mysql_free_task()
{
	int id;

	if ( ! stackGetParams("i", &id))
	{
		stackError("gsc_async_mysql_free_task() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_free_task() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	async_mysql_task *target_task = task_id_to_pointer(id);

	if (target_task == NULL)
	{
		stackError("gsc_async_mysql_free_task() target_task is a NULL-pointer");
		stackPushUndefined();
		return;
	}

	target_task->cleanup = true;
	stackPushInt(1);
}

void gsc_async_mysql_real_escape_string()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_async_mysql_real_escape_string() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (async_mysql_connection == NULL)
	{
		stackError("gsc_async_mysql_real_escape_string() async connection is not initialized!");
		stackPushUndefined();
		return;
	}

	char *to = (char *)malloc(strlen(str) * 2 + 1);
	mysql_real_escape_string(async_mysql_connection, to, str, strlen(str));
	stackPushString(to);
	free(to);
}

MYSQL *mysql_connection = NULL;

void gsc_mysql_initialize()
{
	char *host, *user, *pass, *db;
	int port;

	if ( ! stackGetParams("ssssi", &host, &user, &pass, &db, &port))
	{
		stackError("gsc_mysql_initialize() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		MYSQL *my = mysql_init(NULL);
		my_bool reconnect = true;
		mysql_options(my, MYSQL_OPT_RECONNECT, &reconnect);

		if (!mysql_real_connect(my, host, user, pass, db, port, NULL, 0))
		{
			stackError("gsc_mysql_initialize() failed to initialize synchronous mysql connection!");
			stackPushUndefined();
			return;
		}

		mysql_connection = (MYSQL*)my;
	}
	else
		Com_DPrintf("gsc_mysql_initialize() synchronous connection already initialized.\n");

	stackPushInt((int)mysql_connection);
}

void gsc_mysql_close()
{
	if (mysql_connection != NULL)
	{
		mysql_close(mysql_connection);
		stackPushInt(1);
	}
	else
		stackPushInt(0);
}

void gsc_mysql_query()
{
	char *query;

	if ( ! stackGetParams("s", &query))
	{
		stackError("gsc_mysql_query() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_query() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_query(mysql_connection, query));
}

void gsc_mysql_errno()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_errno() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_errno(mysql_connection));
}

void gsc_mysql_error()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_error() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushString((char *)mysql_error(mysql_connection));
}

void gsc_mysql_affected_rows()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_affected_rows() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	stackPushInt(mysql_affected_rows(mysql_connection));
}

void gsc_mysql_store_result()
{
	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_store_result() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	MYSQL_RES *result = mysql_store_result(mysql_connection);
	stackPushInt((int)result);
}

void gsc_mysql_num_rows()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_num_rows() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_num_rows() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	int ret = mysql_num_rows((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_num_fields()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_num_fields() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_num_fields() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	int ret = mysql_num_fields((MYSQL_RES *)result);
	stackPushInt(ret);
}

void gsc_mysql_field_seek()
{
	int result;
	int offset;

	if ( ! stackGetParams("ii", &result, &offset))
	{
		stackError("gsc_mysql_field_seek() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_field_seek() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	int ret = mysql_field_seek((MYSQL_RES *)result, offset);
	stackPushInt(ret);
}

void gsc_mysql_fetch_field()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_fetch_field() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_fetch_field() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	MYSQL_FIELD *field = mysql_fetch_field((MYSQL_RES *)result);

	if (field == NULL)
	{
		stackPushUndefined();
		return;
	}

	char *ret = field->name;
	stackPushString(ret);
}

void gsc_mysql_fetch_row()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_fetch_row() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_fetch_row() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	MYSQL_ROW row = mysql_fetch_row((MYSQL_RES *)result);

	if (!row)
	{
		stackPushUndefined();
		return;
	}

	stackPushArray();

	int numfields = mysql_num_fields((MYSQL_RES *)result);

	for (int i = 0; i < numfields; i++)
	{
		if (row[i] == NULL)
			stackPushUndefined();
		else
			stackPushString(row[i]);

		stackPushArrayLast();
	}
}

void gsc_mysql_free_result()
{
	int result;

	if ( ! stackGetParams("i", &result))
	{
		stackError("gsc_mysql_free_result() argument is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_free_result() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	mysql_free_result((MYSQL_RES *)result);
	stackPushInt(1);
}

void gsc_mysql_real_escape_string()
{
	char *str;

	if ( ! stackGetParams("s", &str))
	{
		stackError("gsc_mysql_real_escape_string() one or more arguments is undefined or has a wrong type");
		stackPushUndefined();
		return;
	}

	if (mysql_connection == NULL)
	{
		stackError("gsc_mysql_real_escape_string() synchronous connection is not initialized!");
		stackPushUndefined();
		return;
	}

	char *to = (char *)malloc(strlen(str) * 2 + 1);
	mysql_real_escape_string(mysql_connection, to, str, strlen(str));
	stackPushString(to);
	free(to);
}

#endif