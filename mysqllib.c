//Written by Roberto Rodrigues Filho, 2018
#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"
#include <mysql.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static CoreAPI *api;
static GlobalTypeLink *charArrayGT = NULL;

static char* getStringParam(VFrame *cframe, int pIndex) {
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, pIndex)) -> content;
	char *host = NULL;
	if (array != NULL) {
		host = malloc(array -> length + 1);
		memset(host, '\0', array -> length + 1);
		memcpy(host, array -> data, array -> length);
	} else { host = strdup(""); }
	return host;
}

static size_t getIntParam(VFrame *cframe, int pIndex) {
	size_t len = 0;
	copyHostInteger((unsigned char*) &len, getVariableContent(cframe, pIndex), sizeof(size_t));
	return len;
}

static void returnString(VFrame *f, char *value) {
	size_t asz = strlen(value);
	LiveArray *array = malloc(sizeof(LiveArray)+asz);
	memset(array, '\0', sizeof(LiveArray));
	array -> data = ((unsigned char*) array) + sizeof(LiveArray);
	memcpy(array -> data, value, asz);
	array -> length = strlen(value);
	array -> gtLink = charArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> refi.ocm = f -> blocking -> instance;
	array -> refi.refCount ++;
	array -> refi.type = array -> gtLink -> typeLink;
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
}

/*static void returnBoolean(VFrame *cframe, unsigned char res) {
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
}*/

static void returnInt(VFrame *cframe, size_t sz) {
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &sz, sizeof(size_t));
}

//int connect(char host[], int port, char user[], char pass[], char dbName[])
INSTRUCTION_DEF op_mysql_connect(VFrame *cframe) {
	MYSQL *con = NULL;
	char *host = getStringParam(cframe, 0);
	char *user = getStringParam(cframe, 2);
	char *pass = getStringParam(cframe, 3);
	char *db_name = getStringParam(cframe, 4);
	con = mysql_init(NULL);
	if (con == NULL) { api->throwException(cframe, "mysql_init() failed"); }
	if (con != NULL) {
		if (mysql_real_connect(con, host, user, pass, db_name, 0, NULL, 0) == NULL) {
			api->throwException(cframe, (char *) mysql_error(con));
			mysql_close(con);
			con = NULL;
		}
	}
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &con, sizeof(size_t));
	
	free(host);
	free(user);
	free(pass);
	free(db_name);
	
	return RETURN_OK;
}

//int executeQuery(int con, char query[])
INSTRUCTION_DEF op_mysql_execute_query(VFrame *cframe) {
	MYSQL *con;
	memcpy(&con, getVariableContent(cframe, 0), sizeof(size_t));
	
	char *query = getStringParam(cframe, 1);
	MYSQL_RES *res = NULL;
	if (mysql_query(con, query)) {
		api->throwException(cframe, (char *) mysql_error(con));
	} else { res = mysql_store_result(con); }
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(size_t));
	
	free(query);
	
	return RETURN_OK;
}

//int fetchRow(int result)
INSTRUCTION_DEF op_mysql_fetch_row(VFrame *cframe) {
	MYSQL_RES *result;
	memcpy(&result, getVariableContent(cframe, 0), sizeof(size_t));
	MYSQL_ROW row = mysql_fetch_row(result);
	size_t *res = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(res, &row, sizeof(size_t));
	return RETURN_OK;
}

//int numFields(int result)
INSTRUCTION_DEF op_mysql_num_fields(VFrame *cframe) {
	MYSQL_RES *result;
	memcpy(&result, getVariableContent(cframe, 0), sizeof(size_t));
	returnInt(cframe, mysql_num_fields(result));
	return RETURN_OK;
}

//char[] getField(int row, int field)
INSTRUCTION_DEF op_mysql_get_field(VFrame *cframe) {
	MYSQL_ROW row;
	memcpy(&row, getVariableContent(cframe, 0), sizeof(size_t));
	int index = getIntParam(cframe, 1);
	returnString(cframe, row[index] ? row[index] : "NULL");
	return RETURN_OK;
}

//char[] getFieldName(int result)
INSTRUCTION_DEF op_mysql_get_field_name(VFrame *cframe) {
	MYSQL_RES *result;
	memcpy(&result, getVariableContent(cframe, 0), sizeof(size_t));
	MYSQL_FIELD *field = mysql_fetch_field(result);
	returnString(cframe, field->name);
	return RETURN_OK;
}

//void close(int con, int result)
INSTRUCTION_DEF op_mysql_close(VFrame *cframe) {
	MYSQL *con;
	MYSQL_RES *result;
	memcpy(&con, getVariableContent(cframe, 0), sizeof(size_t));
	memcpy(&result, getVariableContent(cframe, 1), sizeof(size_t));
	mysql_free_result(result);
	mysql_close(con);
	return RETURN_OK;
}

Interface* load(CoreAPI *capi) {
	api = capi;
	setInterfaceFunction("connect", op_mysql_connect);
	setInterfaceFunction("executeQuery", op_mysql_execute_query);
	setInterfaceFunction("fetchRow", op_mysql_fetch_row);
	setInterfaceFunction("numFields", op_mysql_num_fields);
	setInterfaceFunction("getField", op_mysql_get_field);
	setInterfaceFunction("getFieldName", op_mysql_get_field_name);
	setInterfaceFunction("close", op_mysql_close);
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	return getPublicInterface();
}
 
void unload() {
	api -> decrementGTRefCount(charArrayGT);
}
