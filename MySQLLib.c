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

//int connect(char host[], int port, char user[], char pass[], char dbName[])
INSTRUCTION_DEF op_mysql_connect(FrameData *cframe) {
	MYSQL *con = NULL;
	
	char *host = x_getParam_char_array(api, cframe, 0);
	char *user = x_getParam_char_array(api, cframe, 2);
	char *pass = x_getParam_char_array(api, cframe, 3);
	char *db_name = x_getParam_char_array(api, cframe, 4);
	con = mysql_init(NULL);
	if (con == NULL) { api->throwException(cframe, "mysql_init() failed"); }
	if (con != NULL) {
		if (mysql_real_connect(con, host, user, pass, db_name, 0, NULL, 0) == NULL) {
			api->throwException(cframe, (char *) mysql_error(con));
			mysql_close(con);
			con = NULL;
		}
	}
	api -> returnRaw(cframe, (unsigned char*) &con, sizeof(size_t));
	
	free(host);
	free(user);
	free(pass);
	free(db_name);
	
	return RETURN_OK;
}

//int executeQuery(int con, char query[])
INSTRUCTION_DEF op_mysql_execute_query(FrameData *cframe) {
	MYSQL *con;
	memcpy(&con, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	char *query = x_getParam_char_array(api, cframe, 1);
	MYSQL_RES *res = NULL;
	if (mysql_query(con, query)) {
		api->throwException(cframe, (char *) mysql_error(con));
	} else { res = mysql_store_result(con); }
	
	api -> returnRaw(cframe, (unsigned char*) &res, sizeof(size_t));
	
	free(query);
	
	return RETURN_OK;
}

//int fetchRow(int result)
INSTRUCTION_DEF op_mysql_fetch_row(FrameData *cframe) {
	MYSQL_RES *result;
	memcpy(&result, api -> getParamRaw(cframe, 0), sizeof(size_t));
	MYSQL_ROW row = mysql_fetch_row(result);
	
	api -> returnRaw(cframe, (unsigned char*) &row, sizeof(size_t));
	
	return RETURN_OK;
}

//int numFields(int result)
INSTRUCTION_DEF op_mysql_num_fields(FrameData *cframe) {
	MYSQL_RES *result;
	memcpy(&result, api -> getParamRaw(cframe, 0), sizeof(size_t));
	api -> returnInt(cframe, mysql_num_fields(result));
	return RETURN_OK;
}

//char[] getField(int row, int field)
INSTRUCTION_DEF op_mysql_get_field(FrameData *cframe) {
	MYSQL_ROW row;
	memcpy(&row, api -> getParamRaw(cframe, 0), sizeof(size_t));
	int index = api -> getParamInt(cframe, 1);
	
	char* dat = row[index] ? row[index] : "NULL";
	DanaEl* array = api -> makeArray(charArrayGT, strlen(dat));
	memcpy(api -> getArrayContent(array), dat, strlen(dat));
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
}

//char[] getFieldName(int result)
INSTRUCTION_DEF op_mysql_get_field_name(FrameData *cframe) {
	MYSQL_RES *result;
	memcpy(&result, api -> getParamRaw(cframe, 0), sizeof(size_t));
	MYSQL_FIELD *field = mysql_fetch_field(result);
	
	char* dat = field->name;
	DanaEl* array = api -> makeArray(charArrayGT, strlen(dat));
	memcpy(api -> getArrayContent(array), dat, strlen(dat));
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
}

//void close(int con, int result)
INSTRUCTION_DEF op_mysql_close(FrameData *cframe) {
	MYSQL *con;
	MYSQL_RES *result;
	memcpy(&con, api -> getParamRaw(cframe, 0), sizeof(size_t));
	memcpy(&result, api -> getParamRaw(cframe, 1), sizeof(size_t));
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
