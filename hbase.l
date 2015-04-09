%{
#include<stdio.h>
%}

%option noyywrap


IDF			[a-zA-Z][a-zA-Z0-9]*

%%
"status"	printf(" STATUS"); 
"alter_status"	printf(" ALTERSTATUS");
"'"		printf(" APS ");
"simple"	printf(" SIMPLE") ;
"summary"	printf(" SUMMARY ");
"detailed"	printf(" DETAILED") ;
"version"	printf(" VERSION");
"whoami"        printf(" WHOAMI");
"create"	printf(" CREATE");
"describe" 	printf(" DESCRIBE");
"disable" 	printf(" DISABLE");
"disable_all" 	printf(" DISABLEALL");
"drop"	 	printf(" DROP");
"drop_all"	printf(" DROPALL");
"exists"	printf(" EXISTS");
"enable"	printf(" ENABLE");
"enable_all"	printf(" ENABLEALL");
"is_enabled"	printf(" ISENABLED");
"is_disabled" 	printf(" ISDISABLED");
"list"		printf(" LIST");
"show_filters"	printf(" SHOWFILTERS");
"count"		printf(" COUNT");
"t.count"	printf(" TCOUNT");
"get"		printf(" GET");
"t.get"		printf(" TGET");
"put"		printf(" PUT");
"INTERVAL"	printf(" INTERVAL");
"CATCH"		printf(" CATCH");
"COLUMN"	printf(" COLUMN");
"TIMESTAMP"	printf(" TIMESTAMP");
"TIMERANGE"	printf(" TIMERANGE");
"delete"	printf(" DELETE");
"t.delete"	printf(" TDELETE");
"deleteall"	printf(" DELETEALL");
"t.deleteall"	printf(" TDELETEALL");
";"		printf("PV ");
","		printf(" VIRGULE");
"."		printf(" POINT");
"*"		printf(" ETOILE");
"NAME"		printf(" NAME");
"VERSIONS"	printf(" VERSIONS");
"TTL"		printf(" TTL");
"BLOCKCACHE"	printf(" BLOCKCACHE");
"true"		printf(" TRUEKEY");
"false"		printf(" FALSEKEY");
"{"		printf(" AO");
"}"		printf(" AF");
"["		printf(" ACO");
"]"		printf(" ACF");
"BEGIN"		printf(" _BEGIN");
"exit"		printf(" EXIT");
"help"		printf(" HELP");
"=>"		printf("FLECH");
{IDF}		printf("IDF");
{inum}		printf("INUM");
[\n]		printf("ENTRER");
[\t]	


%% 
