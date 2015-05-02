#ifndef _ANALYSER_HABSE_H
#define _ANALYSER_HABSE_H

/*
	list_command 		--> command 	list_command_aux
	list_command_aux 	--> list_command | epsilon
	command 			--> create | put | drop


*/

extern int yylex(void);
extern char* yytext;
extern int yylineno;

static int DEBUG = 1;
static int FLEX_DEBUG = 0;

#include "types.h"

typedef enum
{
	ARROW, APOST, AOPEN, ACLOSE, BOPEN, BCLOSE, COMMA, COLON, POINT, SEMICOL, STAR, IDF, INUMBER, DNUMBER, ALPHANUM, TRUE, FALSE, CREATE, ALTER, DESCRIBE,
	DISABLE, DISABLE_ALL, DROP, DROP_ALL, ENABLE, ENABLE_ALL, STATUS, SCAN, COUNT, GET, PUT, DELETE, DELETE_ALL, HELP, ALTER_STATUS, WHOAMI, SIMPLE,
	VERSION, IS_ENABLED, IS_DISABLED, LIST, SHOW_FILTERS, EOC, NAME, VERSIONS, FILTER, METHOD, BEG_IN, EXIT
}typetoken;

boolean _prog();
boolean _list_command();
boolean _list_command_aux();
boolean _command();
boolean _aux();
boolean _aux_aux();
boolean _map();
boolean _expor();
boolean _exporaux();
boolean _expand();
boolean _expandaux();
boolean _key();

boolean _create();
boolean _put();
boolean _drop();
boolean _alter();
boolean _scan();
boolean _table_name();
boolean _create_arg();
boolean _create_argaux();
boolean _create_argaux_aux();
boolean _create_aux();
boolean _create_map();
boolean _list_map_aux();
boolean _map_aux();
boolean _list_map();
boolean _value();
boolean _family_aux();
boolean _f_aux();
boolean _family_column();
boolean _family_column_put();
boolean _row_name();
boolean _TS();
boolean _col_aux();
boolean _map_next();
boolean _create_arg_aux();
boolean _enable();
boolean _disable();
boolean _is_enabled();
boolean _is_disabled();
boolean _describe();
boolean _alter_aux();
boolean _map_alter();

#endif
