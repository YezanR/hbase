#ifndef TABSYM_H
#define TABSYM_H

#define MAX_TABLE_NAME_LENGTH 			20
#define MAX_COMMAND_NAME_LENGTH 		20
#define MAX_FAMILY_COLUMN_NUMBER 		20
#define MAX_COLUMN_NUMBER 				100
#define MAX_FAMILY_COLUMN_NAME_LENGTH 	20
#define MAX_TABSYM_LENGTH				100
#define MAX_ROW_NUMBER 					200
#define MAX_STRING_VAL_LENGTH 			30
#define MAX_COLUMN_NAME_LENGTH 			20
#define MAX_ROW_NAME_LENGTH 			20
#define MAX_TABLES_NUMBER				200
#define MAX_VALUE_LENGTH				50

#define DEFAULT_COLUMN_NAME 			"default_column"


#include  "types.h"




static  	Table* 		TABSYM[MAX_TABSYM_LENGTH];   
static 		int 		CURRENT_TABSYM_INDEX=0;


Command*  			create_command(char* name, Table* table);
void 				destroy_command(Command* cmd);
Table*	  			create_table(char* name);
FamilyColumn*		create_family_column(char* name);
Column*				create_column(char* name);
void	  			add_table_to_cmd(Command* cmd, Table* table);
void				add_family_column_to_table(Table* table, FamilyColumn* fc);
void				add_column_to_family_column(FamilyColumn* fc, Column* col);
boolean				family_column_exists_in_table(Table* table, char* family_col, int* index_of_family_col);
boolean	 			row_exists_in_column(Column* col, char* row,int* index_of_row);
boolean			    column_exists_in_family_column(FamilyColumn* family_col, char* col,int* index_of_column);
//ValType* 			create_valType(void* val, Type type);
ValType* 			create_string_ValType(char* str);
ValType* 			create_inumber_ValType(int inum);
ValType* 			create_dnumber_ValType(double dnum);
ValType*            create_boolean_ValType(int num);
void 				add_ValType_to_row(Row* row, ValType* valtype);
Row*				create_row(char* name);
void 				add_row_to_column(Column* column, Row* row);
void 				delete_from_TABSYM(int table_index);
void 				disable_table_in_TABSYM(int table_index); // disable table in TABSYM by giving the table index
void 				enable_table_in_TABSYM(int table_index); // disable table in TABSYM by giving the table index
boolean 			table_is_enabled_in_TABSYM(int table_index);



// check if table_name exists in TABSYM and put it's corresponding index in table_index
boolean 			table_name_exists_in_TABSYM(char*	table_name, int*  table_index);
// add table to TABSYM  
void 				add_table_to_TABSYM(Table* table);
// update value of cmd in TABSYM
void 				update_cmd_ValType_in_TABSYM(int index_of_table, Command* cmd);

// Debug

void 				show_command(Command* cmd);
void				show_table(Table* table);
void				show_family_column(FamilyColumn* fc);
void				show_column(Column* col);
void				show_TABSYM();
void 				show_ValType(ValType* val);
void 				show_row(Row* row);
#endif
