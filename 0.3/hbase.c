#include "stdio.h"
#include "hbase.h"
#include "tabsym.h"
#include "stdlib.h"
#include "string.h"
#include "database.h"
#include "error.h"
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
			if ( DEBUG ) 
			{ 
				printf("correct command\n");
				show_TABSYM();
			}
			result = true;
		}
	}
	else if(_alter())
	{
		current_token = _read_token();
		if(current_token == EOC )
		{
			result = true;
		}
	}
	else if(_scan())
	{
		result  = true;
	}
	else if(_drop())
	{
		current_token = _read_token();
		if ( current_token == EOC )
		{
			result = true;
			if (DEBUG)
			{
				printf("correct syntax\n");
				printf("Showing command ...\n");
				show_command(cmd);
				printf("showing tabsym ...\n");
				show_TABSYM(cmd);
			}
		}
	}
	
	else if(_disable())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			result  = true;
		}
	}	

	else if(_enable())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			result  = true;
		}
	}	

	else if(_is_enabled())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			result  = true;
		}
	}
	else if(_is_disabled())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			result  = true;
		}
	}
	else if(_describe())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			result  = true;
			if (DEBUG)
			{
				printf("correct command\n");
				printf("Showing command : \n");
				show_command(cmd);
				printf("showing tabsym\n");
				show_TABSYM(cmd);
			}
		}
	}	
	/*else if(_help())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			result = true;
		}
	}*/
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
						creer_sm_erreur(yylineno, cmd->table->name, TABLE_ALREADY_EXIST);
						if (DEBUG)
						{
							printf("ERROR: table name '%s' already exists in TABSYM \n", cmd->table->name);
						}
					}
					// if the create command is correct then add this new table to TABSYM/DATABASE
					else
					{
						add_table_to_TABSYM(cmd->table);
					}
					if ( table_exists_in_database(cmd->table->name) )
					{
						creer_sm_erreur(yylineno, cmd->table->name, TABLE_ALREADY_EXIST);
						if (DEBUG)
						{
							printf("ERROR: table name '%s' already exists in database \n", cmd->table->name);
						}
					}
					else
					{
						add_table_to_database(cmd->table); 
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
										if (DEBUG)
										{
											printf("putting in (%s) ...", cmd->table->name);
										}
										if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index) )
										{
											if (DEBUG)
											{
												printf("(%s) exists in TABSYM at position : %d\n",cmd->table->name, table_index);
											}
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
	if(current_token == DROP)
	{
		cmd = create_command("drop", NULL);
		current_token = _read_token();
		if(_table_name())
		{
			//char* table_location = (char*) malloc( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char));
			if ( table_exists_in_database(cmd->table->name, NULL))
			{
				Table* tab = get_table_from_database(cmd->table->name);
				//show_table(tab);
				if(tab->enabled == false ) 
				{
					cmd->table->location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char));
					strcpy(cmd->table->location, DATABASE_DIRECTORY_NAME);
					strcat(cmd->table->location, cmd->table->name);
					strcat(cmd->table->location, "/");  // "database/table_name/"
					if(DEBUG)
					{
						printf("deleting directory '%s', location : %s  ...\n", cmd->table->name, cmd->table->location);
					}
					delete_table_from_database(cmd->table);

					if(table_name_exists_in_TABSYM(cmd->table->name, &table_index))
					{
						if(DEBUG)
						{
							printf("Table '%s' exists in TABSYM\n", cmd->table->name);
							printf("Deleting from TABSYM...\n");
						}
						delete_from_TABSYM(table_index);
					}
				}
				else
				{
					creer_sm_erreur(yylineno+1, cmd->table->name, TABLE_IS_ENABLED);
				}
			}
			else
			{
				creer_sm_erreur(yylineno+1, cmd->table->name, TABLE_DOESNT_EXIST);
				if(DEBUG)
				{
					printf("ERROR table_name do not exist\n");
				}
			}
			result  = true;
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


// create_arg --> family_column create_arg_aux| list_map create_arg_aux
boolean _create_arg()
{
	boolean result;
	if( _family_column())
	{
		current_token=_read_token();
		if(_create_arg_aux())
		{
		result = true;
		}
	}
	else if( _list_map())
	{
		current_token=_read_token();
		if(_create_arg_aux())
		{
		result = true;
		}
	}
	else
	{
		result = false;
	}
	return result;
}
boolean _create_arg_aux(){
	boolean result;
	if(current_token==EOC){
		follow_token = true;
		result = true;
	}
	else if ( current_token == COMMA )
	{
		current_token = _read_token();
		if ( _create_arg())
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
 return result;
}
// family_column --> 'IDF' 
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
				
						
					result = true;
				
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

// list_map		-->	   {map} 
boolean _list_map()
{
	boolean result;
	if ( current_token == AOPEN )
	{
		current_token = _read_token();
		if ( _map())
		{
			current_token = _read_token();
			if(current_token == ACLOSE)
			{
					result = true;
				
	    	}else{
	    		result= false;
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


// General rule : map --> key => value map_aux

boolean _map()
{
	boolean result;
	if ( current_token==NAME)
	{
		current_token = _read_token();
		if ( current_token == ARROW )
		{
			current_token = _read_token();
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
							if ( _map_aux())
							{
								result = true;
							}
							else{
								result = false;
							}
						}else{
							result = false;
						}
				    }else{
				    	result = false;
					}
			   }else{
			   	   result = false;
			   }
	   }else{
	   		result = false;
	   }
   }else{
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

boolean _map_next()
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

boolean _describe()
{
	boolean result;
	int i;
	if(current_token == DESCRIBE)
	{
		current_token = _read_token();
		cmd = create_command("describe", NULL);
		if(_table_name())
		{
			char* table_location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char) );
			if ( !table_exists_in_database(cmd->table->name , table_location))
			{
				creer_sm_erreur(yylineno+1, cmd->table->name, TABLE_DOESNT_EXIST);
			}
			else
			{	
				Table* t = get_table_from_database(cmd->table->name);
				if(t)
				{
					if(t->enabled == true)
					{
						printf("Table %s is ENABLED\n",cmd->table->name);
					}
					else
					{
						printf("Table %s is DISABLED\n", cmd->table->name);
					}
					printf("%s\n",cmd->table->name );
					printf("COLUMN FAMILIES DESCRIPTION\n");
					for (i=0; i<t->index; i++)
					{
						printf("{ NAME => '%s', VERSIONS => '%d' }\n",t->list_family_col[i].name, t->list_family_col[i].versions );
					}
					printf("%d row(s)\n", t->nb_rows );
				}

			}			
			result = true;
		}
		else
		{
			result  =false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _enable()
{
	boolean result;
	if(current_token == ENABLE)
	{
		current_token = _read_token();
		if(_table_name())
		{
			if (DEBUG)
			{
				printf("enabling (%s) ...", cmd->table->name);
			}
			if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index) )
			{
				if (DEBUG)
				{
					printf("(%s) exists in TABSYM at position : %d\n",cmd->table->name, table_index);
				}
				enable_table(table_index);		
			}
			else
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}			
			result = true;
		}
		else
		{
			result  =false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _disable()
{
	boolean result;
	if(current_token == DISABLE)
	{
		current_token = _read_token();
		if(_table_name())
		{
			if (DEBUG)
			{
				printf("disabling (%s) ...", cmd->table->name);
			}
			if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index) )
			{
				if (DEBUG)
				{
					printf("(%s) exists in TABSYM at position : %d\n",cmd->table->name, table_index);
				}
				disable_table(table_index);	
			}
			else
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}				
			result = true;
		}

		else
		{
			result  =false;
		}
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _is_enabled()
{
	boolean result;
	if(current_token == IS_ENABLED)
	{
		current_token = _read_token();
		if(_table_name())
		{
			if(cmd->table->enabled == true && table_exists_in_database(cmd->table->name))
			{
				printf("YES, the table is enabled\n"); 
				result = true;                
			}
			else if(cmd->table->enabled == false && table_exists_in_database(cmd->table->name))
			{
				printf("NO\n");
				result = true;
			}
			else
			{
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
				result = true;
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
boolean _is_disabled()
{
	boolean result;
	if(current_token == IS_DISABLED)
	{
		current_token = _read_token();
		if(_table_name())
		{
			if(cmd->table->enabled == false && table_name_exists_in_TABSYM(cmd->table->name, &table_index))
			{
				printf("YES, the table is disabled\n");
			}
			else if (cmd->table->enabled == true && table_name_exists_in_TABSYM(cmd->table->name, &table_index))
			{
				printf("NO\n");
			}
			else
			{
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
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
		result = false;
	}
	return result;
}

/*boolean _help(){
	boolean result;
	if(current_token== HELP)
	{
		current_token=_read_token();
		if(current_token==APOST){
			current_token=_read_token();
			if(current_token==CREATE){
				current_token=_read_token();
				if(current_token==APOST){
					printf("Create table; pass table name, a dictionary of specifications per\n");
           			printf("column family, and optionally a dictionary of table configuration.\n");
           			printf("Dictionaries are described below in the GENERAL NOTES section.\n");
        			printf("Examples:\n");

           			printf("hbase> create 't1', {NAME => 'f1', VERSIONS => 5}\n");
           			printf(" create 't1', {NAME => 'f1'}, {NAME => 'f2'}, {NAME => 'f3'}\n");
           			printf(" # The above in shorthand would be the following:\n");
           			printf(" create 't1', 'f1', 'f2', 'f3'\n");
           			printf(" create 't1', {NAME => 'f1', VERSIONS => 1, TTL => 2592000, BLOCKCACHE => true}\n");
				result=true;
				}
			}else if(current_token==ALTER){
				current_token=_read_token();
				if(current_token==APOST){
					printf("Alter column family schema;  pass table name and a dictionary\n");
					printf("specifying new column family schema. Dictionaries are described\n");
					printf("below in the GENERAL NOTES section.  Dictionary must include name\n");
					printf("of column family to alter.  For example,\n \n");
					printf("To change or add the 'f1' column family in table 't1' from defaults \nto instead keep a maximum of 5 cell VERSIONS, do: \n"); 		
					printf("hbase> alter 't1', {NAME => 'f1', VERSIONS => 5}\n");
					printf("To delete the 'f1' column family in table 't1', do:\n hbase> alter 't1', {NAME => 'f1', METHOD => 'delete'}\n");
					printf("You can also change table-scope attributes like MAX_FILESIZE\n MEMSTORE_FLUSHSIZE and READONLY\n");
					printf("For example, to change the max size of a family to 128MB, do: \n ");
					printf("hbase> alter 't1', {METHOD => 'table_att', MAX_FILESIZE => '134217728'}\n");
					result=true;
				}

		}else if (current_token== COUNT){
			current_token=_read_token();
			if(current_token==APOST){
				printf("Count the number of rows in a table. This operation may take a LONG\n");
				printf("time (Run '$HADOOP_HOME/bin/hadoop jar hbase.jar rowcount' to run a\n");
				printf("counting mapreduce job). Current count is shown every 1000 rows by\n");
				printf("default. Count interval may be optionally specified. Examples:\n");
				printf("hbase> count 't1'\n");
				printf("hbase> count 't1', 100000");
				result=true;
			}
		}else if (current_token==DESCRIBE){
			current_token=_read_token();
			if(current_token==APOST){
				printf("Describe the named table: e.g. hbase> describe 't1' \n");
				result=true;
			}
		}
		//
		else if (current_token==DELETE){
			current_token=_read_token();
			if(current_token==APOST){
			printf("Put a delete cell value at specified table/row/column and optionally \n");
            printf("timestamp coordinates.  Deletes must match the deleted cell's\n");
            printf("coordinates exactly.  When scanning, a delete cell suppresses older\n");
            printf("versions. Takes arguments like the 'put' command described below\n");
            result=true;
                                    }
		}
		else if (current_token==DELETE_ALL){
			current_token=_read_token();
			if(current_token==APOST){
			printf(" Delete all cells in a given row; pass a table name, row, and optionally \n");
            printf("a column and timestamp \n");
            result=true;
                                    }
		}
		else if (current_token==DISABLE){
			current_token=_read_token();
			if(current_token==APOST){
			printf(" Disable the named table: e.g. \nhbase> disable 't1' \n");
                                    }
		}
		else if (current_token==DROP){
			current_token=_read_token();
			if(current_token==APOST){
			printf("Drop the named table. Table must first be disabled. If table has \n");
            printf("more than one region, run a major compaction on .META.: \n");
            printf("hbase> major_compact .META. \n");
            result=true;
                                    }
		}

		else if (current_token==ENABLE){
			current_token=_read_token();
			if(current_token==APOST){

           	printf(" Enable the named table \n");
           	result=true;
                                    }
		}
		else if (current_token==EXISTS){
			current_token=_read_token();
			if(current_token==APOST){

            printf(" Does the named table exist? e.g. \nhbase> exists 't1'\n");
            result=true;
                                    }
		}
		else if (current_token==EXIT){
			current_token=_read_token();
			if(current_token==APOST){

            printf(" Type hbase> exit to leave the HBase Shell\n");
            result=true;
                                    }
		}
		else if (current_token==GET){
			current_token=_read_token();
			if(current_token==APOST){

            printf("Get row or cell contents; pass table name, row, and optionally \n");
            printf ("a dictionary of column(s), timestamp and versions.  Examples:\n");
            printf("hbase> get 't1', 'r1'\n");
            printf("hbase> get 't1', 'r1', {COLUMN => 'c1'} \n");
            printf("hbase> get 't1', 'r1', {COLUMN => ['c1', 'c2', 'c3']}\n");
            printf("hbase> get 't1', 'r1', {COLUMN => 'c1', TIMESTAMP => ts1}\n");
            printf("hbase> get 't1', 'r1', {COLUMN => 'c1', TIMESTAMP => ts1,VERSIONS => 4}\n");
            result=true;
                                    }
		}
		else if (current_token==LIST){
			current_token=_read_token();
			if(current_token==APOST){

            printf("List all tables in hbase \n");
            result=true;
                                    }
		}
		else if (current_token==PUT){
			current_token=_read_token();
			if(current_token==APOST){

            printf("Put a cell 'value' at specified table/row/column and optionally \n");
            printf("timestamp coordinates.  To put a cell value into table 't1' at \n");
            printf("row 'r1' under column 'c1' marked with the time 'ts1', do: \n");
			printf(" hbase> put 't1', 'r1', 'c1', 'value', ts1 \n");
			result=true;
                                   }
		}
		else if (current_token==TOOLS){
			current_token=_read_token();
			if(current_token==APOST){

            printf("Listing of hbase surgery tools \n");
            result=true;
                                    }
		}
		else if (current_token==SCAN){
			current_token=_read_token();
			if(current_token==APOST){

           	printf("Scan a table; pass table name and optionally a dictionary of scanner\n");
           	printf("specifications.  Scanner specifications may include one or more of\n");
           	printf("the following: LIMIT, STARTROW, STOPROW, TIMESTAMP, or COLUMNS.  If\n");
           	printf("no columns are specified, all columns will be scanned.  To scan all\n");
           	printf("members of a column family, leave the qualifier empty as in\n");
           	printf("'col_family:'.  Examples:\n");
           	printf("  hbase> scan '.META.'\n");
           	printf("hbase> scan '.META.', {COLUMNS => 'info:regioninfo'}\n");
           	printf(" hbase> scan 't1', {COLUMNS => ['c1', 'c2'], LIMIT => 10,STARTROW => 'xyz'}\n");

 			printf("For experts, there is an additional option -- CACHE_BLOCKS -- which \n");
  			printf("switches block caching for the scanner on (true) or off (false).  By \n");
   			printf("default it is enabled.  Examples:\n");
    		printf("hbase> scan 't1', {COLUMNS => ['c1', 'c2'], CACHE_BLOCKS => false} \n");
    		result=true;

                                    }
		}
		else if (current_token==STATUS){
			current_token=_read_token();
			if(current_token==APOST){

           	printf("Show cluster status. Can be 'summary', 'simple', or 'detailed'. The \n");

          	printf(" default is 'summary'. Examples: \n");
           	printf(" hbase> status\n");
          	printf("hbase> status 'simple'  \n");
           	printf(" hbase> status 'summary' \n");
           	printf(" hbase> status 'detailed' \n");   
			result=true;                          }
		}
		else if (current_token==SHUTDOWN){
			current_token=_read_token();
			if(current_token==APOST){

           printf("Shut down the cluster. \n");
           result=true;
                                    }
		}
		else if (current_token==TRUNCAT){
			current_token=_read_token();
			if(current_token==APOST){

           printf("Disables, drops and recreates the specified table.\n");
           result=true;
                                    }
		}
		else if (current_token==VERSION){
			current_token=_read_token();
			if(current_token==APOST){

           printf("Output this HBase version\n");
           result=true;
                                    }
		}else{
			resut=false;
		}


		//
	}else{
		result=false;
	}
}else{
	result=false
}
	return result;
}*/

boolean _alter(){
	boolean result;
	if ( current_token == ALTER)
	{
		cmd = create_command("alter", NULL);
		current_token = _read_token();
		if(_table_name()){
			current_token=_read_token();
			if(current_token==COMMA){
				current_token=_read_token();	
				if(_alter_aux()){
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
	return result;	
}
boolean _alter_aux(){
	boolean result;
//alter 't1', NAME => 'f1', VERSIONS => 5
	if(_map()){
		
		result = true;	
	}
//  alter 't1', MAX_FILESIZE => '134217728'
	else if(_map_alter())
	{
		result=true;
	}
//alter 't1', 'f1', {NAME => 'f2', IN_MEMORY => true}, {NAME => 'f3', VERSIONS => 5}
	else if( _create_arg()){
		
			result = true;	
	}
//alter 't1', 'delete' => 'f1'
	else if(current_token==DELETE){
		current_token=_read_token();
		if ( current_token == ARROW ){
			current_token = _read_token();
			if(_family_column()){
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
	return result;
}

boolean _map_alter()
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
//follow c'etait ACLOSE pour le cas {key=>val} On doit ajouter follow=EOC pour ce cas key=>val
boolean _map_aux()
{
	boolean result;
	if ( current_token == ACLOSE)
	{
		follow_token = true;
		result = true;
	}
	if ( current_token == EOC)
	{
		follow_token = true;
		result = true;
	}
	if ( current_token == COMMA)
	{
		current_token = _read_token();
		if ( _map_next() )
		{
			result = true;
		}
	}
	return result;
}
//hbase.l ajoute  "'delete'"					{ return DELETE; } et "alter"					{ return ALTER; }


int main(int argc, char const *argv[])
{
	int nb_tab;
	current_token = _read_token();

	if ( _prog() )
	{
		if (nombre_sm_erreurs() == 0)
		{
			if (DEBUG) printf("0 erreurs sémantiques\n");
			printf("Correct\n");	
		}
		else
		{
			afficher_sm_erreurs();
		}
	}
	char tname[20];
	strcpy(tname, "t1");
	Table*	t = get_table_from_database(tname);
	show_table(t);
	//list_tables_by_name_in_database(&nb_tab);
	return 0;
}

