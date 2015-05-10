#include "tabsym.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int  DEBUG = 0;

Command* create_command(char* name, Table* tab)
{
	Command* cmd = (Command*) malloc(sizeof(Command));
	cmd->name = (char*)malloc(MAX_COMMAND_NAME_LENGTH*sizeof(char));
	strcpy(cmd->name, name);
	cmd->table = tab;
	cmd->put_rname = (char*)malloc(MAX_COMMAND_NAME_LENGTH*sizeof(char));
	cmd->put_column = (char*)malloc(MAX_COMMAND_NAME_LENGTH*sizeof(char));
	cmd->put_family_col_name = (char*)malloc(MAX_COMMAND_NAME_LENGTH*sizeof(char));
	cmd->versions = 0;
	return cmd;
}


void 	destroy_command(Command* cmd)
{
	cmd->name = NULL;
	cmd->table = NULL;
	cmd->put_rname = NULL;
	cmd->put_column = NULL;
	cmd->put_family_col_name = NULL;
	cmd = NULL;
}

Table*  create_table(char* name)
{
	Table* tab = (Table*) malloc(sizeof(Table));
	tab->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(tab->name, name);
	int i;
	tab->list_family_col = (FamilyColumn*)malloc(MAX_FAMILY_COLUMN_NUMBER*sizeof(FamilyColumn));
	for (i = 0; i < MAX_FAMILY_COLUMN_NUMBER; ++i)
	{
		tab->list_family_col[i].created = 0;
	}
	tab->index = 0;
	tab->enabled = true;
	return tab;
}

FamilyColumn*  create_family_column(char* name)
{
	FamilyColumn* fc = (FamilyColumn*) malloc(sizeof(FamilyColumn));
	fc->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(fc->name, name);
	fc->list_column = (Column*)malloc(MAX_COLUMN_NUMBER*sizeof(Column));
	fc->list_column[0] = *create_column(DEFAULT_COLUMN_NAME);
	fc->index = 1;
	fc->versions = 1;
	fc->created = 1;
	return fc;
}

Column*  create_column(char* name)
{
	if(DEBUG)
	{
		printf("In create_column()\n");
	}
	Column* col = (Column*) malloc(sizeof(Column));
	col->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(col->name, name);
	col->list_row = (Row*)malloc(MAX_ROW_NUMBER*sizeof(Row));
	col->index = 0;
	if(DEBUG)
	{
		printf("Out of create_column()\n");
	}
	return col;
}
Row*  create_row(char* name)
{
	if(DEBUG)
	{
		printf("In create_row()\n");
	}
	Row* row = (Row*) malloc(sizeof(Row));
	row->name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	strcpy(row->name, name);
	if(DEBUG)
	{
		printf("Out of create_row()\n");
	}
	return row;
}
/*
ValType*   create_ValType(void* v, Type type)
{
	if(DEBUG)
	{
		printf("In create_ValType()\n");
	}
	ValType* val = (ValType*) malloc(sizeof(ValType));
	//val->value = v;
	switch ( type )
	{
		case STRING_VAL:
			val->string = (char*) malloc ( MAX_COMMAND_NAME_LENGTH*sizeof(char));
			strcpy(val->string, (char*)v); 
			break;
		case INUMBER_VAL:
			val->inumber =(int)*v; 
			break;
		case STRING_VAL:
			val->dnumber =(double)*v; 
			break;
	}
	val->type = type;
	if(DEBUG)
	{
		printf("Out of create_ValType()\n");
	}
	return val;
}
*/
ValType*   create_string_ValType(char* str)
{
	ValType* val = (ValType*) malloc(sizeof(ValType));
	val->Value.string = (char*) malloc ( MAX_STRING_VAL_LENGTH*sizeof(char));
    strcpy(val->Value.string, str); 
    val->type = STRING_VAL;
    return val;
}
ValType*   create_inumber_ValType(int inum)
{
	ValType* val = (ValType*) malloc(sizeof(ValType));
	val->Value.inumber = inum; 
    val->type = INUMBER_VAL;
    return val;
}
ValType*   create_dnumber_ValType(double dnum)
{
	ValType* val = (ValType*) malloc(sizeof(ValType));
	val->Value.dnumber = dnum; 
    val->type = DNUMBER_VAL;
    return val;
}

ValType*   create_boolean_ValType(int inum)
{
	ValType* val = (ValType*) malloc(sizeof(ValType));
	if ( inum ==1 || inum == 0)
	{
		val->Value.boolean = inum;
	}
	else
	{
		val->Value.boolean = 0;
	}
    val->type = BOOL_VAL;
    return val;
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
		table->list_family_col[table->index++] = *fc;
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
		fc->list_column[fc->index++] = *col;
	}
	else
	{
		if ( DEBUG )
			printf("ERROR: family column NULL pointer\n");	
	}
}

void 	add_ValType_to_row(Row* row, ValType* valtype)
{
	if ( row )
	{
		row->valType = valtype;
	}
	else
	{
		if ( DEBUG )
			printf("ERROR: row NULL pointer\n");	
	}

}

void   add_row_to_column(Column* column, Row* row)
{
	if(DEBUG)
	{
		printf("In add_row_to_column()\n");
	}
	if ( column )
	{
		column->list_row[column->index++] = *row;
	}
	else
	{
		if ( DEBUG )
			printf("ERROR: family column NULL pointer\n");	
	}
	if(DEBUG)
	{
		printf("Out of add_row_to_column()\n");
	}
}
boolean	  family_column_exists_in_table(Table* table, char* family_col,int* index_of_family_col)
{
	if ( DEBUG )
	{
		printf("In family_column_exists_in_table()\n");
	}
	 if ( table)
	 {
	 	int i;
	 	for ( i=0; i<table->index ; i++)
	 	{
	 		if ( strcmp(table->list_family_col[i].name, family_col) == 0)
	 		{
	 			if(index_of_family_col)
	 			{
	 				*index_of_family_col=i;
	 			}
	 			return true;
	 		}		
	 	}
	 }
	 if ( DEBUG )
	{
		printf("Out of  family_column_exists_in_table()\n");
	}
	 return false;
}
boolean	  column_exists_in_family_column(FamilyColumn* family_col, char* col,int* index_of_column)
{
	if ( DEBUG )
	{
		printf("In column_exists_in_family_column()\n");
	}
	 if ( family_col)
	 {
	 	int i;
	 	for ( i=0; i<family_col->index ; i++)
	 	{
	 		if ( strcmp(family_col->list_column[i].name, col) == 0)
	 		{
	 			*index_of_column=i;
	 			return true;
	 		}	
	 	}
	 }
	 if ( DEBUG )
	{
		printf("Out of column_exists_in_family_column()\n");
	}
	 return false;
}
boolean	  row_exists_in_column(Column* col, char* row,int* index_of_row)
{
	if (DEBUG)
	{
		printf("In row_exists_in_column()\n");
	}
	 if ( col )
	 {
	 	int i;
	 	for ( i=0; i<col->index ; i++)
	 	{
	 		if ( strcmp(col->list_row[i].name, row) == 0)
	 		{
	 			*index_of_row=i;
	 			return true;
	 		}	
	 	}
	 }
	 return false;
}
void      show_column(Column* col)
{
	if ( col )
	{
		int i;
		printf("      <column>\n");
		printf("		name : %s\n", col->name);
		for(i=0; i<col->index; i++ )
		{
			show_row(&col->list_row[i]);
		}
		printf("      </column>\n");
	}
	else
	{
		printf("DEBUG ERROR: column null pointer\n");
	}
}

void      show_family_column(FamilyColumn* fc)
{   
	if ( fc )
	{
		printf("    <family_column name='%s' versions = '%d' >\n", fc->name, fc->versions);
		int i;
		for ( i=0; i<fc->index; i++ )
		{
			show_column(&fc->list_column[i]);	
		}
		printf("    </family_column>\n");
	}
	else
	{
		printf("DEBUG ERROR: family_column null pointer\n");
	}
}

static   boolean   row_name_exists_in_visted_rows(char** stack, int stack_size, char* row_name)
{
	int i;
	for(i=0; i<stack_size;i++)
	{
		if( strcmp(stack[i], row_name) == 0 )
		{
			return true;
		}
	}
	return false;
}


void	 show_rows_with_name(Table* table, char* name)
{
	int i,j,k;
	for ( i=0; i<table->index; i++ )
	{
			char* fc = table->list_family_col[i].name; 
			for( j=0; j<table->list_family_col[i].index; j++ )	
			{
				char* col = table->list_family_col[i].list_column[j].name;
				for( k=0; k<table->list_family_col[i].list_column[j].index; k++)
				{
					if ( strcmp(table->list_family_col[i].list_column[j].list_row[k].name, name) == 0 )
					{
						printf("  %s\t\t\t  column=%s:%s, val=", name, fc, col);
						show_ValType(table->list_family_col[i].list_column[j].list_row[k].valType);
						printf("\n");
					}
				}
					

		}
	}
}

void      show_table(Table* table)
{
	if ( table )
	{
		printf("\nScanning table ..'%s'\n\n", table->name);
		int i,j,k;
		char**	visited_rows = (char**)malloc((table->nb_rows+1)*sizeof(char*));
		for (i=0; i<table->nb_rows+1; i++)
		{
			visited_rows[i] = (char*)malloc(MAX_ROW_NAME_LENGTH*sizeof(char));
		}
		int   visted_rows_table_index=0;
		printf("-----------------------------------------------------------------\n");
		printf("\t\t\t%s\n", table->name);
		printf("-----------------------------------------------------------------\n");
		printf("  ROW\t\t\t\tCOLUMN+CELL\n");
		for ( i=0; i<table->index; i++ )
		{
			for( j=0; j<table->list_family_col[i].index; j++ )	
			{
				for( k=0; k<table->list_family_col[i].list_column[j].index; k++)
				{
					if ( !row_name_exists_in_visted_rows(visited_rows, table->nb_rows, table->list_family_col[i].list_column[j].list_row[k].name) )
					{
						show_rows_with_name(table, table->list_family_col[i].list_column[j].list_row[k].name);
						visited_rows[visted_rows_table_index++] = table->list_family_col[i].list_column[j].list_row[k].name;
					}
				}
			}	

		}
		printf("-----------------------------------------------------------------\n");	
	}
	else
	{
		printf("DEBUG ERROR: table null pointer\n");
	}
}



/*
void      show_table(Table* table)
{
	if ( table )
	{
		printf("-----------------------------------------------------------------\n");
		printf("\t\t\t\t%s\t\t\t\t\n", table->name);
		printf("-----------------------------------------------------------------\n");
		printf("  ROW\t\t\t\tCOLUMN+CELL\n");
		int i;
		for ( i=0; i<table->index; i++ )
		{
			show_family_column(&table->list_family_col[i]);	
		}
		printf("  </table>\n");
	}
	else
	{
		printf("DEBUG ERROR: table null pointer\n");
	}
}*/

void      show_command(Command* cmd)
{
	if ( cmd )
	{
		printf("<command>\n");
		printf(" %s\n", cmd->name);
		show_table(cmd->table);
		printf("</command>\n");
	}
	else
	{
		printf("DEBUG ERROR: command null pointer\n");
	}
}

boolean   table_name_exists_in_TABSYM(char*	table_name, int*  table_index)
{
	int i;
	for( i=0; i<CURRENT_TABSYM_INDEX; i++)
	{
		if ( TABSYM[i] )
		{
			if( strcmp(TABSYM[i]->name, table_name) == 0 )
			{
				if ( table_index )
					*table_index = i;
				return true;
			}
		}

	}
	return false;
}

// add a table to TABSYM if it doesn't exist
void 	add_table_to_TABSYM(Table* table)
{
	if ( !table_name_exists_in_TABSYM(table->name, NULL) && (table-> index > 0) )
	{
		TABSYM[CURRENT_TABSYM_INDEX++] = table;
	}
	else
	{
		if (DEBUG) {printf("ERROR : in add_table_to_TABSYM(),  Table name already exists (%s)\n", table->name);}
	}
}
// khassa b put
void 	update_cmd_ValType_in_TABSYM(int index_of_table, Command* cmd)
{
	if ( DEBUG )
	{
		printf("In update_cmd_ValType_in_TABSYM()\n");
	}
	if ( cmd )
	{
		int index_of_family_col;
		int index_of_column;
		int index_of_row;
		char* tab_name = (char*) malloc ( (strlen(cmd->table->name)+1)*sizeof(char));
		char* family_col_name =   (char*) malloc ( MAX_TABLE_NAME_LENGTH*sizeof(char));
		char* column_name = (char*) malloc ( MAX_COLUMN_NAME_LENGTH*sizeof(char));
		char* row_name = (char*) malloc ( MAX_ROW_NAME_LENGTH*sizeof(char));

		strcpy(tab_name, cmd->table->name);
		strcpy(family_col_name, cmd->put_family_col_name);
		//strcat(family_col_name, "\n");
		strcpy(column_name, cmd->put_column);
		strcpy(row_name, cmd->put_rname);
		if ( family_column_exists_in_table(TABSYM[index_of_table], family_col_name, &index_of_family_col))
		{
			//printf("index of family_col (%s) in table  (%s) is : %d\n",family_col_name, TABSYM[index_of_table]->name, index_of_family_col);
			if ( column_exists_in_family_column(&TABSYM[index_of_table]->list_family_col[index_of_family_col], column_name, &index_of_column))
			{
				if ( row_exists_in_column(&TABSYM[index_of_table]->list_family_col[index_of_family_col].list_column[index_of_column], row_name, &index_of_row))
				{
					TABSYM[index_of_table]->list_family_col[index_of_family_col].list_column[index_of_column].list_row[index_of_row].valType = cmd->put_valType;
				}
				// if row doesn't exist then create a new row and put the new value in it
				else
				{
					Row* new_row = create_row(row_name);
					new_row->valType = cmd->put_valType;
					add_row_to_column(&TABSYM[index_of_table]->list_family_col[index_of_family_col].list_column[index_of_column], new_row);
				}
			}
			// if column doesn't exist then create a new column and put a new row in it
			else
			{
				Row* new_row_ = create_row(row_name);
				new_row_->valType = cmd->put_valType;		
				Column* new_column = create_column(column_name);
				add_row_to_column(new_column, new_row_);
				add_column_to_family_column(&TABSYM[index_of_table]->list_family_col[index_of_family_col], new_column);
			}
		}

		else 
		{
			if ( DEBUG )
				printf("family column (%s) doesn't exist in table (%s) \n", family_col_name, cmd->table->name);
		}
	}
	if ( DEBUG )
	{
		printf("Out of update_cmd_ValType_in_TABSYM()\n");
	}
}

void delete_from_TABSYM(int table_index)
{
	TABSYM[table_index] = NULL;
}


void disable_table_in_TABSYM(int index_of_table)
{
	if( TABSYM[index_of_table])
	{
		TABSYM[index_of_table]->enabled = false;
	}
	else
	{
		if(DEBUG)
		{
			printf("No table found in TABSYM with such index !\n");
		}
	}
}

void enable_table_in_TABSYM(int index_of_table)
{
	if( TABSYM[index_of_table])
	{
		TABSYM[index_of_table]->enabled = true;
	}
	else
	{
		if(DEBUG)
		{
			printf("No table found in TABSYM with such index !\n");
		}
	}
}

boolean 	table_is_enabled_in_TABSYM(int table_index)
{
	if( TABSYM[table_index])
	{
		return TABSYM[table_index]->enabled;
	}
	else
	{
		if(DEBUG)
		{
			printf("FATAL Error : no table found in TABSYM with such index !\n");
		}
	}
}



void    show_TABSYM()
{
	int i=0;
	for ( i=0; i<CURRENT_TABSYM_INDEX; i++ )
	{
		if ( TABSYM[i])
			show_table(TABSYM[i]);
	}
}


void 	    show_row(Row* row)
{
	if (row)
	{
		printf("%s : ", row->name);
		show_ValType(row->valType);
	}
	else
	{
		printf("DEBUG ERROR: row null pointer\n");
	}
}

void 		show_ValType(ValType* val)
{
	if ( val )
	{
		switch ( val->type)
		{
			case STRING_VAL:
			case BOOL_VAL:
				printf("%s", val->Value.string);
				break;
			case INUMBER_VAL:
				printf("%d", val->Value.inumber);
				break;
			case DNUMBER_VAL:
				printf("%f", val->Value.dnumber);
				break;
			default:
				printf("(NaN)\n");
		}
	}
	else
	{
		printf("DEBUG ERROR: command null pointer\n");
	}
}
