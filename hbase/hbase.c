#include "stdio.h"
#include "hbase.h"
#include "tabsym.h"
#include "stdlib.h"
#include "string.h"

// Global variables
typetoken 		current_token;
boolean 		follow_token;
Command* 		cmd;		// current command
int 			table_index= 0;  // index of current table within TABSYM


typetoken _read_token()
{
	if(follow_token == true)
	{
		follow_token = false;
		return current_token;
	}
	else
	{
		return (typetoken)yylex();
	}
}

// _prog  -->  list_command exit 

boolean _prog()
{
	boolean result;
	if ( _list_command() )
	{
		current_token = _read_token();
		if ( current_token == EXIT )
		{
			result = true;
		}
	}
	else
	{
		result = false;
	}

}

// list_command --> command list_command_aux | epsilon   
boolean _list_command()
{
	boolean result;
	if ( current_token == EXIT )
	{
		follow_token = true;
		result = true;
	}
    if ( _command() )
	{
		
		current_token = _read_token();
		if ( _list_command_aux() )
		{
			result = true;
		}
		else
		{
			
			result = false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}
// list_command_aux -->  list_command | epsilon
boolean _list_command_aux()
{
	boolean result;
	// follow
	if ( current_token == EXIT)
	{
		follow_token = true;
		result = true;
	}
	else if ( _list_command() )
	{
		result = true;
	}
	else
	{
		if ( DEBUG ) { printf("ERROR : in _list_command_aux(), unkown rule\n");}
		result = false;
	}
	return result;
}


//command--> create EOC | put EOC | alter EOC | scan EOC | status EOC| ...

boolean _command()
{
	boolean result;
	if( _create())
	{
		current_token = _read_token();
		if ( current_token == EOC )
		{
			result = true;
			if (DEBUG)
			{
				printf("correct command\n");
				printf("Showing command : \n");
				show_command(cmd);
			}
		}
	}
	else if(_put())
	{
		current_token = _read_token();
		if ( current_token == EOC )
		{
			if ( DEBUG ) { 
				printf("correct command\n");
				show_TABSYM();
			}
			result = true;
		}
	}
	else if(_alter())
	{
		result  = true;
	}
	else if(_scan())
	{
		result  = true;
	}
	else if(_drop())
	{
		result  = true;
	}
	else if( current_token == EXIT )
	{
		result = true;
	}

	else
	{
		result = false;
		if ( current_token == EOC )
		{
			if(DEBUG)	{ printf("Error: in _command(), unexpected EOC\n");}
		}
		else
		{
			if(DEBUG)	{ printf("Error: in _command(), unkown command %s\n", yytext);}	
		}
	}
	return result;
}

// create --> CREATE tab_name, create_arg
boolean _create()
{
	boolean result;
	if ( current_token == CREATE)
	{
		cmd = create_command("create", NULL);
		current_token = _read_token();
		if ( _table_name() )
		{
			current_token = _read_token();
			if (current_token == COMMA )
			{
				current_token = _read_token();
				if ( _create_arg() )
				{
					// if table already exists then you can't create it !
					if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index))
					{
						if (DEBUG)
						{
							printf("ERROR: table name already exists '%s'\n", cmd->table->name);
						}
					}
					else
					{
						// if the create command is correct then add this new table to TABSYM/DATABASE
						add_table_to_TABSYM(cmd->table); 
					}
					result = true;
				}
				else
				{
					result = false;
				}
			}
			else
			{
				if (DEBUG) printf("ERROR: in _create() unkown operator %s\n", yytext);
				result = false;
			}
		}
		else
		{
			if (DEBUG) printf("ERROR: in _create() wrong table name %s\n", yytext);
			result = false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _put()
{
	boolean result;
	if(current_token ==PUT){
			current_token = _read_token();
			cmd = create_command("put", NULL);
			if(_table_name()){
				current_token = _read_token();
				if(current_token == COMMA){
					current_token = _read_token();
					if(_row_name()){
						current_token = _read_token();
						if(current_token==COMMA){
							
							current_token = _read_token();
							if(_family_column_put())
							{
								current_token=_read_token();
								if(current_token==COMMA)
								{
									current_token = _read_token();
									if(_value())
									{
										// if table exist then we can update it
										if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index) )
										{
											update_cmd_ValType_in_TABSYM(table_index, cmd);
										}
										else
										{
											if (DEBUG)
											{
												printf("ERROR: table name doesn't exist '%s'\n", cmd->table->name);
											}
										}
										current_token = _read_token();
										if(_TS()){
											result=true;
										}else{
											result=false;
										}
									}else{
										result=false;
									}
								}else{
									result=false;
								}
							}else{
								result=false;
							}
						}else{
							result=false;
						}
					}else{
						result=false;
					}
				}else{
					result=false;
				}
			}else{
				result=false;
			}
	}else{
		result=false;
	}
	return result;
}
boolean _drop()
{
	boolean result;
	return result;
}
boolean _alter()
{
	boolean result;
	return result;
}
boolean _scan()
{
	boolean result;
	return result;
}


// table_name --> 'IDF'
boolean _table_name()
{
	boolean result;
	if ( current_token == APOST)
	{
		current_token = _read_token();
		if ( current_token == IDF )
		{
			char* tab_name = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
			strcpy(tab_name, yytext);
			current_token = _read_token();
			if ( current_token == APOST)
			{
				Table* tab = create_table(tab_name);
				add_table_to_cmd(cmd, tab);
				result = true;
			}
			else
			{
				if ( DEBUG ) { printf("ERROR : in _table_name(), missing \' \n");}
				result = false;
			}
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}


// create_arg --> family_column | list_map
boolean _create_arg()
{
	boolean result;
	if( _family_column())
	{
		result = true;
	}
	else if( _list_map())
	{
		result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

// family_column --> 'IDF' family_aux
boolean _family_column()
{
	boolean result;
	if ( current_token == APOST)
	{
		current_token = _read_token();
		if ( current_token == IDF)
		{
			// create temp family_col
			char* 	family_col = (char*)malloc((strlen(yytext)+1)*sizeof(char));
			strcpy(family_col, yytext);
			current_token = _read_token();
			if ( current_token == APOST)
			{
				// if family column already exists in table then error
				if ( family_column_exists_in_table(cmd->table, family_col, NULL))
				{
					if ( DEBUG )
					{
						printf("Semantic Error: family column (%s) already exists in table (%s)\n",family_col, cmd->table->name);
					}
				}

				// else add family column to current table 
				else
				{
					FamilyColumn* fc =  create_family_column(family_col);
					add_family_column_to_table(cmd->table, fc);
				}
				current_token = _read_token();
				if ( _family_aux())
				{		
					result = true;
				}
				else
				{

					result = false;
				}
			}
			else
			{
				if ( DEBUG ) { printf("ERROR : in _family_column(), missing \'\n");}
				result = false;
			}
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}
// family_aux --> ,family_column | epsilon
// follow(family_aux) = EOC
boolean _family_aux()
{
	boolean result;
    if ( current_token == EOC )
	{
		follow_token = true;
		result = true;
	}
	else if ( current_token == COMMA )
	{
		current_token = _read_token();
		if ( _family_column() )
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else
	{
		if ( DEBUG) { printf("ERROR : in family_aux(), unexpected token %s\n", yytext);}
		result = false;
	}
	return result;
}
// list_map		-->	   {map} list_map_aux
boolean _list_map()
{
	boolean result;
	if ( current_token == AOPEN )
	{
		current_token = _read_token();
		if ( _map())
		{
			current_token = _read_token();
			if ( _list_map_aux() )
			{
				result = true;
			}
			else
			{
				result = false;
			}
		}
		else
		{
			result = false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _list_map_aux()
{
	boolean result;
	return result;
}

// General rule : map --> key => value map_aux

boolean _map()
{
	boolean result;
	if ( _key() )
	{
		current_token = _read_token();
		if ( current_token == ARROW )
		{
			current_token = _read_token();
			if ( _value() )
			{
				current_token = _read_token();
				if ( _map_aux())
				{
					result = true;
				}
				else
				{
					result = false;
				}
			}
			else
			{
				if ( DEBUG) printf("Error : in _map(), invalid val\n");			
			}
		}
		else
			{
				if ( DEBUG) printf("Error : in _map(), expected ARROW before val\n");			
			}
	}
	else
	{
		if ( DEBUG) printf("Error : in _map(), invalid key\n");
		result = false;
	}
	return result;
}

boolean _key()
{
	boolean result;
	if ( current_token == NAME)
	{
		
		result = true;
	}
	else if ( current_token == FILTER )
	{
		result = true;
	}
	else if ( current_token == METHOD )
	{
		result = true;
	}
	else if (current_token == VERSIONS)
	{
		result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

// map_aux --> , map | epsilon
boolean _map_aux()
{
	boolean result;
	if ( current_token == ACLOSE)
	{
		follow_token = true;
		result = true;
	}
	if ( current_token == COMMA)
	{
		current_token = _read_token();
		if ( _map() )
		{
			result = true;
		}
	}
	return result;
}

// aux --> 'IDF aux_aux | INUMBER | DNUMBER | TRUE | FALSE | 'ALPHANUM'
boolean _aux()
{
	boolean result;
	if(current_token == APOST)
	{
		current_token = _read_token();
		if(current_token == IDF )
		{
			char* family_col = (char*)malloc(MAX_FAMILY_COLUMN_NAME_LENGTH*sizeof(char));
			strcpy(family_col, yytext);
			current_token = _read_token();
			if ( _aux_aux() )
			{
				FamilyColumn* fc =  create_family_column(family_col);
				add_family_column_to_table(cmd->table, fc);
				result = true;
			}
			else
			{
				result = false;
				if(DEBUG)	{printf("Error: in _aux_aux() \n");}
			}
		}
		else
		{
			result = false;
			if(DEBUG)	{printf("IDF error\n");}
		}
	}
	
	else if(current_token == INUMBER)
	{
		result = true;
	}
	else if(current_token == DNUMBER)
	{
		result = true;
	}
	else if(current_token == TRUE)
	{
		result = true;
	}
	else if(current_token == FALSE)
	{
		result = true;
	}
	else
	{
		result = false;
	}
	return result;	
}

// aux_aux --> :IDF' | '
boolean _aux_aux()
{
	boolean result;
	if ( current_token == COLON )
	{
		current_token = _read_token();
		if ( current_token == IDF )
		{
			current_token = _read_token();
			if ( current_token == APOST )
			{
				result = true;
			}
			else
			{
				result = false;
				if ( DEBUG) printf("Error: in _aux_aux() APOST not fount\n");
			}
		}
		else
		{
			result = false;
		    if ( DEBUG) printf("Error: in _aux_aux() IDF error \n");
		}
	}
	else if ( current_token == APOST )
	{
		result = true;
	}
	else
	{
		result = false;
		if ( DEBUG) printf("Error: in _aux_aux() expected :IDF' or ' but %s found \n", yytext);
	}
	return result;
}

// value --> 'IDF' | INUMBER | DNUMBER | TRUE | FALSE
boolean _value()
{
	boolean result;
	if(current_token == APOST)
	{
		current_token = _read_token();
		if(current_token == IDF )
		{
			char* val = (char*)malloc(MAX_STRING_VAL_LENGTH*sizeof(char));
			strcpy(val, yytext);
			current_token = _read_token();
			if ( current_token == APOST )
			{
				cmd->put_valType = create_string_ValType(val);
				result = true;
			}
			else
			{
				result = false;
				if(DEBUG)	{printf("Error: in _aux_aux() \n");}
			}
		}
		else
		{
			result = false;
			if(DEBUG)	{printf("IDF error\n");}
		}
	}
	
	else if(current_token == INUMBER)
	{
		int  val =  atoi(yytext);
		cmd->put_valType = create_inumber_ValType(val);
		result = true;
	}
	else if(current_token == DNUMBER)
	{
		double val = atof(yytext);
		cmd->put_valType = create_dnumber_ValType(val);
		result = true;
	}
	else if(current_token == TRUE)
	{
		cmd->put_valType = create_boolean_ValType(1);
		result = true;
	}
	else if(current_token == FALSE)
	{
		cmd->put_valType = create_boolean_ValType(0);
		result = true;
	}
	else
	{
		result = false;
	}
	return result;	
}

//row_name				-->		'IDF'
boolean _row_name()
{
	if ( DEBUG)
	{
		printf("In _row_name()\n");
	}
	boolean result;
	if(current_token==APOST)
	{
		current_token = _read_token();
		if(current_token==IDF)
		{
			char* r_name = (char*)malloc(strlen(yytext)*sizeof(char));
			strcpy(r_name, yytext);
			current_token = _read_token();
			if(current_token==APOST)
			{
				strcpy(cmd->put_rname, r_name);
				result=true;
			}
			else{
				result=false;
			}
		}
		else{
			result=false;
		}
	}
	else{
		result=false;
	}
	if ( DEBUG)
	{
		printf("Out of _row_name()\n");
	}
	return result;
}
//family_column_put			-->		'IDF f_aux
boolean _family_column_put()
{
	if ( DEBUG )
	{
		printf("In _family_column_put()\n");
	}
	boolean result;
	if(current_token==APOST){
		current_token = _read_token();
		if(current_token==IDF){
			char* family_col = (char*) malloc((strlen(yytext)+1)*sizeof(char));
			strcpy(family_col, yytext);
			current_token=_read_token();
			if(_f_aux()){
				strcpy(cmd->put_family_col_name, family_col);
				result=true;
			}else{
				result=false;
			}
		}else{
			result=false;
		}
	}else{
		result=false;
	}
	if ( DEBUG )
	{
		printf("Out of _family_column_put()\n");
	}
	return result;
}
//f_aux					-->     	:col_aux'|'
boolean _f_aux()
{
	if ( DEBUG )
	{
		printf("In _f_aux()\n");
	}
	boolean result;
	if(current_token==APOST){
		result=true;
	}
	else if(current_token==COLON){
		current_token = _read_token();
		if(_col_aux()){
			current_token = _read_token();
			if(current_token==APOST){
				result=true;
			}else{
				result=false;
			}
		}else{
			result=false;
		}
		
	}
	else{
		result=false;
	}
	if ( DEBUG )
	{
		printf("Out of _f_aux()\n");
	}

	return result;
}
//col_aux					-->		IDF|epsilon
boolean _col_aux()
{
	boolean result;
	if(current_token==APOST)
	{
	   follow_token = true;
	   result = true;	
	}
	else if(current_token==IDF)
	{
		cmd->put_column = (char*) malloc((strlen(yytext)+1)*sizeof(char));
		strcpy(cmd->put_column, yytext);
		result=true;
	}
	else{
		result=false;
	}
	return result;
}
//TS	-->		,IDF|epsilon
boolean _TS()
{
	boolean result;
	if(current_token==EOC)
	{
		follow_token=true;
		result=true;
	}else if(current_token==COMMA){
		current_token=_read_token();
		if(current_token==IDF){
			result=true;
		}else{
			result=false;
		}
			
	}else{
		result=false;
	}
	return result;
}



int main(int argc, char const *argv[])
{
	current_token = _read_token();	
	if ( _prog() )
	{

		printf("correct\n");
	}
	return 0;
}


