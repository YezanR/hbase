#ifndef DATABASE_H
#define DATABASE_H

#define 	DATABASE_DIRECTORY_NAME			"database/"
#define     MAX_NUMBER_OF_FILES_IN_DIR		200	

extern int DEBUG;

/*
	This header contains all functions necessary to interact with our file system (database):
	
		* creating directory for each table
		* Inside a specific table directory, creates a directory for each family column
		* Inside a specific family column directory,creates a file foreach column 
		* A column file will contain a simple map (row_name : value), later, we can add timestamp to each map...
		* Table schema will be stored separetly from data in table directory ( for each table directory, we will find a table schema file)


*/



#include "types.h"

void 	 				add_table_to_database(Table* table);
void 	 				delete_table_from_database(Table* table);
// this function will update  table data in database when 'put' command is successfully executed 
void 					update_table_in_database(Table* table, Command* cmd);

boolean					table_exists_in_database(char* table_name);
// return a list of table names in databse directory and modifies nb_tables to the size of this list
char** 					list_tables_by_name_in_database(int* nb_tables); 


#endif
