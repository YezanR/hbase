#include "tabsym.h"

Command* create_command(char* name, Table* tab)
{
	Command* cmd = (Command*) malloc(sizeof(Command));
	cmd->name = (char*)malloc(MAX_COMMAND_NAME_LENGTH*sizeof(char));
	strcpy(cmd->name, name);
	cmd->table = tab;
	return cmd;
}

Table*  create_table(char* name)
{
	Table* tab = (Table*) malloc(sizeof(Table));
	tab->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(tab->name, name);
	tab->list_family_col = (FamilyColumn*)malloc(MAX_FAMILY_COLUMN_NUMBER*sizeof(FamilyColumn));
	tab->index = 0;
	return tab;
}

FamilyColumn*  create_family_column(char* name)
{
	FamilyColumn* fc = (FamilyColumn*) malloc(sizeof(FamilyColumn));
	fc->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(fc->name, name);
	fc->list_column = (Column*)malloc(MAX_COLUMN_NUMBER*sizeof(Column));
	fc->index = 0;
	return fc;
}

Column*  create_column(char* name, double* val)
{
	Column* col = (Column*) malloc(sizeof(Column));
	col->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(col->name, name);
	col->value = val;
	return tab;
}

void	  add_table_to_cmd(Command* cmd, Table* table)
{
	if ( cmd )
	{
		cmd->table = table;
	}
	else
	{
		if ( DEBUG )
			printf("ERROR: cmd NULL pointer\n");
	}
}

void   	  add_family_column_to_table(Table* table, FamilyColumn* fc)
{
	if ( table )
	{
		table->list_family_col[table->index++] = fc;
	}
	else
	{
		if ( DEBUG )
			printf("ERROR: table NULL pointer\n");	
	}
}
void	  add_column_to_family_column(FamilyColumn* fc, Column* col)
{
	if ( fc )
	{
		fc->list_column[fc->index++] = col;
	}
	else
	{
		if ( DEBUG )
			printf("ERROR: family column NULL pointer\n");	
	}
}