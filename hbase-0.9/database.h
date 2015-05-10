#ifndef DATABASE_H
#define DATABASE_H

#define 	DATABASE_DIRECTORY_NAME			"database/"
#define     MAX_NUMBER_OF_FILES_IN_DIR		200	

/*
	This header contains all functions necessary to interact with our file system (database):
	
		* creating directory for each table
		* Inside a specific table directory, creates a directory for each family column
		* Inside a specific family column directory,creates a file foreach column 
		* A column file will contain a simple map (row_name : value), later, we can add timestamp to each map...
		* Table schema will be stored separetly from data in table directory ( for each table directory, we will find a table schema file)


*/



#include "types.h"
#include "tabsym.h"

 

void 	 				add_table_to_database(Table* table);
boolean 	 				delete_table_from_database(Table* table);
// this function will update  table data in database when 'put' command is successfully executed 
//void 					update_table_in_database(Table* table, Command* cmd);
void 					update_table_in_database(char* table_name, Command* cmd);
// construct Table objet based on the schema file in database by passing the table name
Table*					get_table_from_database(char* table_name);

FamilyColumn*			get_family_column_from_database_by_location(char* fc_location);

FamilyColumn*			get_family_column_from_database(char* table_location, char* fc_name);

boolean					table_exists_in_database(char* table_name, char* table_location);

// return a list of table names in databse directory and modifies nb_tables to the size of this list
char** 					list_tables_by_name_in_database(int* nb_tables); 

char **  				list_columns_by_name_in_database(char* fc_location, int* nb_columns);

boolean 				add_row_in_database(char* column_location, Row* new_row);

boolean 				edit_row_in_database(char* column_location, Row* edit_row, ValType* new_val);
// this function will look first for the table in TABSYM and disable it
// then will disable it in the database 
// if table_location is NULL then the function will disable the table just in TABSYM
boolean					disable_table(char* table_location, int table_index);
boolean					enable_table(char* table_location, int table_index);


void 		alter_table(char* table_name);


#endif