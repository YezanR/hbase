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
"summary"		printf(" SUMMARY ");
"detailed"		printf(" DETAILED") ;
"version"					{ return VERSION; }
"whoami"        			{ return WHOAMI; }
"create"					{ return CREATE; }
"describe" 					{ return DESCRIBE; }
"disable" 					{ return DISABLE; }
"disable_all" 				{ return DISABLE_ALL ;}
"drop"	 					{ return DROP; }
"drop_all"					{ return DROP_ALL; }
"exists"	printf(" EXISTS");
"enable"					{ return ENABLE;}
"enable_all"				{ return ENABLE_ALL; }	
"is_enabled"				{ return IS_ENABLED; }
"is_disabled" 				{ return IS_DISABLED; }
"list"						{ return LIST; }
"show_filters"				{ return SHOW_FILTERS;}
"count"						{ return COUNT; }
"get"						{ return GET; }
"put"						{ return PUT; }
"INTERVAL"					{ printf(" INTERVAL"); }
"CATCH"						{ printf(" CATCH"); }
"COLUMN"					{ printf(" COLUMN"); }
"TIMESTAMP"					{ printf(" TIMESTAMP"); }
"TIMERANGE"					{ printf(" TIMERANGE"); }
"delete"					{ return DELETE; }
"deleteall"					{ return DELETE_ALL; }
";"							{ return SEMICOL; }
","							{ return COMMA; }
":"							{ return COLON; }
"."							{ return POINT; }
"*"							{ return STAR; }
"NAME"						{ return NAME; }
"VERSIONS"					{ return VERSIONS; }
"TTL"						{ printf(" TTL"); }
"BLOCKCACHE"				{ printf(" BLOCKCACHE"); }
"true"						{ return TRUE; }
"false"						{ return FALSE; }
"{"							{ return AOPEN; }
"}"							{ return ACLOSE; }
"["							{ return BOPEN; }
"]"							{ return BCLOSE; }
"BEGIN"						{ return BEG_IN; }
"exit"						{ return EXIT; }
"help"						{ return HELP; }
"=>"						{ return ARROW; }
{IDF}						{ return IDF; }
{INum}						{ return INUMBER; }
{DNum}						{ return DNUMBER; }
"\n"						{ if(FLEX_DEBUG) printf("ENTRER"); return EOC; }
" "+						{ }
"\t"+                       {}	
.							{ if (FLEX_DEBUG) printf("UNKOWN Token");}			
		
%% 
