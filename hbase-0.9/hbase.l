%{
#include	<stdio.h>
#include	"hbase.h"
%}

%option noyywrap

IDF			  [a-zA-Z][a-zA-Z0-9]*
INum		  [0-9]+
DNum		  [0-9]+"."[0-9]+

%%
"status"					{ return STATUS; }
"alter_status"				{ return ALTER_STATUS; }
"'"							{ return APOST; }
"simple"					{ return SIMPLE ; }
"version"					{ return VERSION; }
"whoami"        			{ return WHOAMI; }
"create"					{ return CREATE; }
"describe" 					{ return DESCRIBE; }
"disable" 					{ return DISABLE; }
"disable_all" 				{ return DISABLE_ALL ;}
"drop"	 					{ return DROP; }
"scan"	 					{ return SCAN; }
"drop_all"					{ return DROP_ALL; }
"exists"					{ return EXISTS; }
"enable"					{ return ENABLE;}
"enable_all"				{ return ENABLE_ALL; }	
"is_enabled"				{ return IS_ENABLED; }
"is_disabled" 				{ return IS_DISABLED; }
"list"						{ return _LIST; }
"show_filters"				{ return SHOW_FILTERS;}
"count"						{ return COUNT; }
"get"						{ return GET; }
"put"						{ return PUT; }
"delete"					{ return DELETE; }
"deleteall"					{ return DELETE_ALL; }
";"							{ return SEMICOL; }
","							{ return COMMA; }
":"							{ return COLON; }
"."							{ return POINT; }
"*"							{ return STAR; }
"NAME"						{ return NAME; }
"VERSIONS"					{ return VERSIONS; }
"true"						{ return TRUE; }
"truncate"                  { return TRUNCATE;}
"false"						{ return FALSE; }
"{"							{ return AOPEN; }
"}"							{ return ACLOSE; }
"["							{ return BOPEN; }
"]"							{ return BCLOSE; }
"BEGIN"						{ return BEG_IN; }
"exit"						{ return EXIT; }
"help"						{ return _HELP; }
"=>"						{ return ARROW; }
{IDF}						{ return IDF; }
{INum}						{ return INUMBER; }
{DNum}						{ return DNUMBER; }
"\n"						{ if(FLEX_DEBUG) printf("ENTRER"); return EOC; }
" "+						{ }
"\t"+                       {}	
.							{ return UNKNOWN_TOKEN;}			
		
%% 