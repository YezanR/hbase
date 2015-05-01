#ifndef TABSYM_H
#define TABSYM_H

#define MAX_TABLE_NAME_LENGTH 		20
#define MAX_COMMAND_NAME_LENGTH 	20
#define MAX_FAMILY_COLUMN_NUMBER 	20
#define MAX_COLUMN_NUMBER 			100


typedef struct 
{
	char* name;
	Table table;
}Command;

typedef struct 
{
	char* name;
	FamilyColumn* list_family_col;
	int   index;
}Table;

typedef struct 
{
	char* name;
	Column* list_column;
	int index;
}FamilyColumn;

typedef struct 
{
	char* name;
	double* value;
}Column;

Command*  			create_command(char* name, Table* table);
Table*	  			create_table(char* name);
FamilyColumn*		create_family_column(char* name);
Column*				create_column(char* name, double* value);
void	  			add_table_to_cmd(Command* cmd, Table* table);
void				add_family_column_to_table(Table* table, FamilyColumn* fc);
void				add_column_to_family_column(FamilyColumn* fc, Column* col);

// affichage

void 				show_command(Command* cmd);



#endif