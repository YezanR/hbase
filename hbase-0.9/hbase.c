#include "stdio.h"
#include "hbase.h"
#include "tabsym.h"
#include "stdlib.h"
#include "string.h"
#include "database.h"
#include "error.h"
#include <pwd.h>
// Global variables
typetoken 		current_token;
boolean 		follow_token;
Command* 		cmd;		// current command
int 			table_index= 0;  // index of current table within TABSYM
int             eoc = 0;
boolean 		has_already_showed_syntax_error=false;
char* 			error_token;

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
	if ( _list_command() )
	{
		current_token = _read_token();
		if ( current_token == EXIT )
		{
			return 1;
		}
	}
	else
	{
		current_token = _read_token();
		_prog();
	}
	return 0;
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
		afficher_sm_erreurs();
		reset_sm_erreurs();
		eoc = 0;
		has_already_showed_syntax_error=false;
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
		if ( !has_already_showed_syntax_error )
		{
			printf("\nError in %d: Invalid command\n", yylineno);
			has_already_showed_syntax_error = true;
		}
			
		result = false;
		//afficher_sx_erreurs();
		//reset_sx_erreurs();
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
			if ( table_exists_in_database(cmd->table->name, NULL) )
			{
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_ALREADY_EXIST);
			}
			else 
			{					
				add_table_to_TABSYM(cmd->table); // optional
				add_table_to_database(cmd->table); 
			}
			result = true;
		}
		else
		{
			printf("\nSyntax Error: Expected end of command \n");
			printf("USAGE : create <TABLE_NAME>, <LIST_COLUMN_FAMILY> [, TIMESTAMP]\n");
			result = false;
		}

	}
	else if(_put())
	{
		current_token = _read_token();
		if ( current_token == EOC )
		{
			// if table exist then we can update it
			char* table_location = (char*) malloc ( (strlen(cmd->table->name)+strlen(DATABASE_DIRECTORY_NAME)+2)*sizeof(char));
			if ( table_exists_in_database(cmd->table->name, table_location) )
			{
			//update_cmd_ValType_in_database(table_location, cmd);
				/*
				if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index))
				{
					update_cmd_ValType_in_TABSYM(table_index, cmd);	
				}*/
				update_table_in_database(cmd->table->name, cmd);
			}			
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command \n", yylineno);
			printf("USAGE : put <TABLE_NAME>, <ROW_NAME>, <COLUMN_FAMILY:qualifier>, <VALUE> [, TIMESTAMP]\n");
			result = false;
		}
	}
	else if(_alter())
	{
		current_token = _read_token();
		if(current_token == EOC )
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else if(_scan())
	{
		current_token = _read_token();
		if ( current_token == EOC )
		{
		    Table* table = get_table_from_database(cmd->table->name);
			if(table)
			{
				show_table(table);
			}
			else
			{
				creer_sm_erreur(yylineno, yytext, TABLE_DOESNT_EXIST);
			}
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : scan <TABLE_NAME>\n");
			result = false;
		}
	}
	else if(_drop())
	{
		current_token = _read_token();
		if ( current_token == EOC )
		{
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
					if ( delete_table_from_database(cmd->table) )
					{
						printf("Table '%s' successfully deleted\n", cmd->table->name);
					}
					/*
					if(table_name_exists_in_TABSYM(cmd->table->name, &table_index))
					{
						if(DEBUG)
						{
							printf("Table '%s' exists in TABSYM\n", cmd->table->name);
							printf("Deleting from TABSYM...\n");
						}
						delete_from_TABSYM(table_index);
					}*/
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
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : drop <TABLE_NAME>\n");
			result = false;
		}
	}
	else if(_disable())
	{
		current_token = _read_token();
		if(current_token == EOC)
		{
			printf("\ndisabling (%s) ...\n", cmd->table->name);
			char* table_location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char) );

			if (DEBUG)
			{
				printf("disabling (%s) ...\n", cmd->table->name);
			}
		    if ( !table_exists_in_database(cmd->table->name , table_location))
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}	
			else
			{
				if( disable_table(table_location, 0) )
				{
					printf("Table '%s' successfully disabled\n", cmd->table->name);
				}
				else
				{
					printf("Failed to disable '%s'\n", cmd->table->name);
				}
				
			}			
			result  = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : disable <TABLE_NAME>\n");
			result = false;
		}
	}	

	else if(_enable())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
			char* table_location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char) );

			if ( !table_exists_in_database(cmd->table->name , table_location))
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}
			else
			{	
				if( enable_table(table_location, 0) )
				{
					printf("Table '%s' successfully enabled\n", cmd->table->name);
				}
				else
				{
					printf("Failed to enable '%s'\n", cmd->table->name);
				}
			}		
			
			result  = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : disable <TABLE_NAME>\n");
			result = false;
		}
	}	

	else if(_is_enabled())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
		    if ( !table_exists_in_database(cmd->table->name , NULL))
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}	
			else
			{
				Table* t = get_table_from_database(cmd->table->name);
				if(t)
				{
					if(t->enabled == true)
					{
						printf("\nYES, table '%s' is enabled\n", cmd->table->name);
					}
					else
					{
						printf("\nNO, table '%s' is not enabled\n", cmd->table->name);
					}
				}
			}			
			result  = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : is_enabled <TABLE_NAME>\n");
			result = false;
		}
	}
	else if(_is_disabled())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
		    if ( !table_exists_in_database(cmd->table->name , NULL))
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}	
			else
			{
				Table* t = get_table_from_database(cmd->table->name);
				if(t)
				{
					if(t->enabled == false)
					{
						printf("\nYES, table '%s' is disabled\n", cmd->table->name);
					}
					else
					{
						printf("\nNO, table '%s' is not disabled\n", cmd->table->name);
					}
				}
			}		
			result  = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : is_disabled <TABLE_NAME>\n");
			result = false;
		}
	}
	else if(_describe())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
			result  = true;
			int i;
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
					printf("\nDescribing Table '%s' ...\n\n", cmd->table->name);
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
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : describe <TABLE_NAME>\n");
			result = false;
		}
	}
	else if(_list())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
			int i, nb_tables;
			char** tab_name_list = list_tables_by_name_in_database(&nb_tables);
			printf("\nListing tables ...\n\n");
			if(nb_tables != 0)
			{
				for (i=0 ; i < nb_tables; ++i)
				{
			 		printf("%s\n",tab_name_list[i]);
				}
				printf("%d row(s)\n",nb_tables );
				printf("==> [ \"%s\"",tab_name_list[0]);
				for (i=1 ; i < nb_tables; ++i)
				{
			 		printf(" , \"%s\"",tab_name_list[i]);
				}
				printf(" ]\n\n");
			}
			else
			{
				printf("NOTHING to show\n\n");
			}
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->name);
			printf("USAGE : list\n");
			result = false;
		}
	}
	else if(_whoami())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : whoami\n");
			result = false;
		}
	}
	else if(_version())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : version\n");
			result = false;
		}
	}
	
	else if(_exists())
	{
		
		current_token = _read_token();
		if(current_token == EOC)
		{
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : exists <TABLE_NAME>\n");
			result = false;
		}
	}
	else if(__help())
	{
		
		current_token = _read_token();	
		if(current_token == EOC)
		{
			printf("\n");
					printf(" Create\n");
					printf("Create table; pass table name, a dictionary of specifications per\n");
           			printf("column family, and optionally a dictionary of table configuration.\n");
           			printf("Dictionaries are described below in the GENERAL NOTES section.\n");
        			printf("Examples:\n");

           			printf("hbase> create 't1', {NAME => 'f1', VERSIONS => 5}\n");
           			printf(" create 't1', {NAME => 'f1'}, {NAME => 'f2'}, {NAME => 'f3'}\n");
           			printf(" # The above in shorthand would be the following:\n");
           			printf(" create 't1', 'f1', 'f2', 'f3'\n");
           			printf(" create 't1', {NAME => 'f1', VERSIONS => 1, TTL => 2592000, BLOCKCACHE => true}\n");
				
					printf("\n");
					printf("Alter : \n");
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
				

				printf("\n");
				printf("Count\n");
				printf("Count the number of rows in a table. This operation may take a LONG\n");
				printf("time (Run '$HADOOP_HOME/bin/hadoop jar hbase.jar rowcount' to run a\n");
				printf("counting mapreduce job). Current count is shown every 1000 rows by\n");
				printf("default. Count interval may be optionally specified. Examples:\n");
				printf("hbase> count 't1'\n");
				printf("hbase> count 't1', 100000");
				
				
				printf("\n");
				printf("Describe\n");
				printf("Describe the named table: e.g. hbase> describe 't1' \n");
				
			
			printf("\n");
			printf("Delete : \n");
			printf("Put a delete cell value at specified table/row/column and optionally \n");
            printf("timestamp coordinates.  Deletes must match the deleted cell's\n");
            printf("coordinates exactly.  When scanning, a delete cell suppresses older\n");
            printf("versions. Takes arguments like the 'put' command described below\n");
            
                                    
			printf("\n");
			printf("Delete all\n");
			printf(" Delete all cells in a given row; pass a table name, row, and optionally \n");
            printf("a column and timestamp \n");
            
                                    
			printf("\n");
		    printf("Disable : \n");
			printf(" Disable the named table: e.g. \nhbase> disable 't1' \n");
                                    
		
			printf("\n");
			printf("Drop\n");
			printf("Drop the named table. Table must first be disabled. If table has \n");
            printf("more than one region, run a major compaction on .META.: \n");
            printf("hbase> major_compact .META. \n");
            
                                    
            printf("\n");
			printf("Enable : \n");
           	printf(" Enable the named table \n");
           	
             
            printf("\n");                       
			printf("Exists : \n");
            printf(" Does the named table exist? e.g. \nhbase> exists 't1'\n");

            printf("\n");
            printf("Exit : \n");
            printf(" Type hbase> exit to leave the HBase Shell\n");
            
                                    
	
            printf("\n");
            printf("Get : \n");
            printf("Get row or cell contents; pass table name, row, and optionally \n");
            printf ("a dictionary of column(s), timestamp and versions.  Examples:\n");
            printf("hbase> get 't1', 'r1'\n");
            printf("hbase> get 't1', 'r1', {COLUMN => 'c1'} \n");
            printf("hbase> get 't1', 'r1', {COLUMN => ['c1', 'c2', 'c3']}\n");
            printf("hbase> get 't1', 'r1', {COLUMN => 'c1', TIMESTAMP => ts1}\n");
            printf("hbase> get 't1', 'r1', {COLUMN => 'c1', TIMESTAMP => ts1,VERSIONS => 4}\n");
            
    
    		printf("\n");
    		printf("List : \n");
            printf("List all tables in hbase \n");
            
                                   
		
            printf("\n");
            printf("Put : \n");
            printf("Put a cell 'value' at specified table/row/column and optionally \n");
            printf("timestamp coordinates.  To put a cell value into table 't1' at \n");
            printf("row 'r1' under column 'c1' marked with the time 'ts1', do: \n");
			printf(" hbase> put 't1', 'r1', 'c1', 'value', ts1 \n");
			
                                                        
			printf("\n");
			printf("Scan : \n");
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
    		

    		printf("\n");
    		printf("Status : \n");
           	printf("Show cluster status. Can be 'summary', 'simple', or 'detailed'. The \n");

          	printf(" default is 'summary'. Examples: \n");
           	printf(" hbase> status\n");
          	printf("hbase> status 'simple'  \n");
           	printf(" hbase> status 'summary' \n");
           	printf(" hbase> status 'detailed' \n");   
			                          
		
			printf("\n");
		   printf("Shutdown\n");
           printf("Shut down the cluster. \n");
           
                                    
			printf("\n");
			printf("Truncat : \n");
           printf("Disables, drops and recreates the specified table.\n");
           
                                    
		
			printf("\n");
			printf("Versions : \n");
           printf("Output this HBase version\n");
           
       
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : help\n");
			result = false;
		}
	}
	else if(_truncate())
	{
		current_token = _read_token();
		printf("\nNOTE : truncate command not handled yet, command with no effect\n");
		if(current_token == EOC)
		{
			result = true;
		}
		else
		{
			printf("\nSyntax Error in line %d: Expected end of command after '%s'\n", yylineno, cmd->table->name);
			printf("USAGE : truncate <TABLE_NAME>\n");
			result = false;
		}
	}
	else if( current_token == EXIT )
	{
		printf("exit\n");
		result = true;
	}
	else
	{
		if ( current_token != EOC )
		{
			if (!eoc)
			{
				strcpy(error_token, yytext);
				eoc = 1;
			}
			if(DEBUG)	{ printf("Error: in _command(), unexpected EOC\n");}
			result = false;
		}
		else
		{
			eoc = 0;
			if(DEBUG)	{ printf("Error: in _command(), unkown command %s\n", yytext);}	
			result = true;
		}
	}

	return result;
}

// create --> CREATE tab_name, create_arg
boolean _create()
{
	boolean result;
	boolean error = false;
	if ( current_token == CREATE)
	{
		cmd = create_command("create", NULL);
		current_token = _read_token();
		if ( _table_name() )
		{
			// if table already exists then you can't create it !
			
			current_token = _read_token();
			if (current_token == COMMA )
			{
				current_token = _read_token();
				if ( _create_arg() )
				{
					/*
					if ( table_name_exists_in_TABSYM(cmd->table->name, &table_index))
					{
						creer_sm_erreur(yylineno+1, cmd->table->name, TABLE_ALREADY_EXIST);
						error = true;
						if (DEBUG)
						{
							printf("ERROR: table name '%s' already exists in TABSYM \n", cmd->table->name);
						}
					}*/
					// if the create command is correct then add this new table to TABSYM/DATABASE
					result = true;
				}
				else
				{
					printf("\nSyntax Error in line %d: Expected expression after \',\' but end of command encountered\n", yylineno);
					printf("USAGE : create <TABLE_NAME>, <LIST_COLUMN_FAMILY> [, TIMESTAMP]\n");
					result = false;
				}
			}
			else
			{
				printf("Error in line %d : missing \",\" after '%s'\n", yylineno-1, cmd->table->name);
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
	char* table_location;
	if(current_token ==PUT){
			current_token = _read_token();
			cmd = create_command("put", NULL);

			if(_table_name())
			{
				current_token = _read_token();
				if(current_token == COMMA){
					current_token = _read_token();
					if(_row_name())
					{
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
										/*
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
										}*/
										current_token = _read_token();
										if(_TS())
										{
											result=true;
										}
										else
										{
										
											result=false;
										}
									}
									else
									{
										result=false;
									}
								}
								else
								{
									printf("Syntax Error in line %d: Expected \",\" after '%s'\n", cmd->put_family_col_name);
									result=false;
								}
							}
							else
							{
								printf("USAGE : put <TABLE_NAME>, <ROW_NAME>, <COLUMN_FAMILY:qualifier>, <VALUE> [, TIMESTAMP]\n");
								result=false;
							}
						}
						else
						{
							printf("Syntax Error in line %d: Expected \",\" after '%s'\n", yylineno, cmd->put_rname);
							printf("USAGE : put <TABLE_NAME>, <ROW_NAME>, <COLUMN_FAMILY:qualifier>, <VALUE> [, TIMESTAMP]\n");
							result=false;
						}
					}
					else
					{
						printf("USAGE : put <TABLE_NAME>, <ROW_NAME>, <COLUMN_FAMILY:qualifier>, <VALUE> [, TIMESTAMP]\n");
						result=false;
					}
				}else{
					result=false;
				}
			}
			else
			{
				printf("USAGE : put <TABLE_NAME>, <ROW_NAME>, <COLUMN_FAMILY:qualifier>, <VALUE> [, TIMESTAMP]\n");
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
// scan ==> SCAN tab_name
boolean _scan()
{
	boolean result;
	int index_of_table;
	if ( current_token == SCAN)
	{
		cmd = create_command("scan", NULL);
		current_token = _read_token();
		if( _table_name() )
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


// table_name --> 'IDF'
boolean _table_name()
{
	boolean result;
	if ( current_token == APOST)
	{
		current_token = _read_token();
		error_token = init_error_token(yytext);
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
				printf("\nSyntax Error in line %d : Missing \"'\" at end of table name %s\n",yylineno, error_token);
				//creer_sx_erreur(yylineno, yytext, MISSING_COTE);
				if ( DEBUG ) { printf("ERROR : in _table_name(), missing \' \n");}
				result = false;
			}
		}
		else
		{
			printf("\nSyntax Error in line %d : wrong table name, only alphanumeric values are allowed\n", yylineno);
			//creer_sx_erreur(yylineno+1, yytext, WRONG_TABLENAME);
			result = false;
		}
	}
	else
	{
		printf("\nSyntax Error in line %d : Missing \"'\" before table name\n",yylineno);
		//creer_sx_erreur(yylineno+1, yytext, MISSING_COTE);
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
		else
		{
			result = false;
		}
	}
	else if( _list_map())
	{
		current_token=_read_token();
		if(_create_arg_aux())
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
//_create_arg_aux-->, _create_arg |epsilon
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
	else
	{
		result = false;	
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
					//creer_sm_erreur(yylineno+1, family_col, FAMILY_COL_REPEATED_IGNORING_IT);
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
				//creer_sx_erreur(yylineno, yytext, MISSING_COTE);
				printf("Syntax Error in line %d : missing cote\n", yylineno-1);
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
		//printf("Error in line %d : missing cote\n",yylineno);
		//creer_sx_erreur(yylineno+1, yytext, WRONG_TABLENAME);
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
			// if we have parsed the map and found the NAME key then we can add versions to this column family
			// else we cannot set a versions to an unnamed column family !!
			if (  cmd->table->list_family_col[cmd->table->index-1].created )
			{
				if( cmd->versions != 0)
					cmd->table->list_family_col[cmd->table->index-1].versions = cmd->versions;
			}
			else
			{
				creer_sm_erreur(yylineno+1, cmd->table->name, FAMILY_COLUMN_NO_NAME_KEY);
			}
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


//  map --> NAME => 'idf' map_aux|VERSIONS=>INUMBER map_aux
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
			   }
			   else
			   {
			   		printf("\nSyntax Error in line %d: Expected \"'\" after NAME => \n", yylineno);
			   		printf("Hint : {NAME => 'alphanumeric', VERSIONS => INTEGER OR DOUBLE}\n");
			   	   result = false;
			   }
	   }
	   else
	   {
	   		printf("\nSyntax Error in line %d: Expected \"=>\" after key\n", yylineno);
	   		result = false;
	   }
   }
    else if(current_token ==VERSIONS)
   {
   			current_token=_read_token();
   		   	if(current_token== ARROW)
		   	{
   				current_token=_read_token();
				if(current_token==INUMBER)	
				{
					cmd->versions= atoi(yytext);
				
					current_token=_read_token();
					if ( _map_aux())
					{
						result = true;
					}
				}
				else
				{
					printf("\nSyntax Error in line %d: Expected integer value after VERSIONS =>\n", yylineno);
					printf("Hint : {NAME => 'alphanumeric', VERSIONS => INTEGER OR DOUBLE}\n");
					result = false;
				}
		   	}
		   	else
		   	{
		   		printf("\nSyntax Error in line %d: Expected \"=>\" after key\n", yylineno);
		   		result = false;
		   	}
   }
   else{
   		printf("\nSyntax Error : Unknown key '%s', only NAME and VERSIONS are available for now ..\n", yytext);
   		result = false;
   }
   return result;
}

boolean _key()
{
	boolean result;
	if ( current_token == MAX_FILESIZE)
	{
		current_token=_read_token();
		if ( current_token == ARROW )
		{
			current_token = _read_token();
			if(current_token==INUMBER)
			{
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
/*
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
}*/


// value --> 'IDF' | INUMBER | DNUMBER | TRUE | FALSE | 'INUMBER' | DNUMBER | 'TRUE' | 'FALSE'
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
			printf("Syntax Error in line %d : invalid value, only alphanumeric, integer, double and boolean are allowed\n", yylineno);
			printf("Examples :  'mystring', '5', '10.5'\n");
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
		printf("Syntax Error in line %d : invalid value, only alphanumeric, integer, double and boolean are allowed\n", yylineno);
		printf("Examples :  'mystring', 5, 10.5\n");
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
		char* tmp_token = (char*) malloc( (strlen(yytext)+1)*sizeof(char));
		strcpy(tmp_token, yytext);
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
			else
			{
				printf("Syntax Error in line %d: Missing \"'\" after %s\n", yylineno, tmp_token);
				result=false;
			}
		}
		else
		{
			printf("Syntax Error in line %d: Invalid row name, only alphanumeric values are allowed ([a-zA-Z][a-zA-Z0-9]*)\n", yylineno);
			result=false;
		}
	}
	else
	{
		printf("Syntax Error in line %d: Missing \"'\" before %s\n", yylineno, yytext);
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
	if(current_token==APOST)
	{
		current_token = _read_token();
		if(current_token==IDF)
		{
			char* family_col = (char*) malloc((strlen(yytext)+1)*sizeof(char));
			strcpy(family_col, yytext);
			current_token=_read_token();
			if(_f_aux())
			{
				strcpy(cmd->put_family_col_name, family_col);
				result=true;
			}
			else
			{
				printf("\nSyntax Error in line %d: column family name must be followed by qualifier\n", yylineno);
				printf("e.g: 'f1:col1'\n");
				result=false;
			}
		}
		else
		{
			printf("\nSyntax Error in line %d: Invalid column family name, only alphanumeric strings are allowed\n", yylineno);
			result=false;
		}
	}
	else
	{
		printf("\nSyntax Error in line %d: Missing \"'\" before column family name\n", yylineno-1);
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
		//append_error_token(error_token, yytext);
		if(_col_aux()){
			current_token = _read_token();
			if(current_token==APOST){
				result=true;
			}else{
				printf("Syntax Error in line %d: Expected \"'\" after column name\n", yylineno);
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
	else
	{
		printf("\nSyntax Error in line %d: Expected alphanumeric qualifier after Column Family \n", yylineno);
		printf("Hint: 'fcol1:column'\n");
		result=false;
	}
	return result;
}
//TS	-->		,INUMBER|epsilon
boolean _TS()
{
	boolean result;
	if(current_token==EOC)
	{
		follow_token=true;
		result=true;
	}
	else if(current_token==COMMA)
	{
		current_token=_read_token();
		if(current_token==INUMBER)
		{
			result=true;
		}
		else
		{
			printf("Syntax Error in line %d: timestamp must be an integer\n", yylineno-1);
			result=false;
		}
			
	}else
	{
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

boolean _list()
{
	boolean result;
	int i, nb_tables;
	if(current_token == _LIST)
	{
		cmd = create_command("list", NULL);
		result = true;
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
		cmd = create_command("enable", NULL);
		if(_table_name())
		{
			if (DEBUG)
			{
				printf("enabling (%s) ...", cmd->table->name);
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
		cmd = create_command("disable", NULL);
		if(_table_name())
		{
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
		cmd = create_command("is_enabled", NULL);
		current_token = _read_token();
		if(_table_name())
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
boolean _is_disabled()
{
	boolean result;
	if(current_token == IS_DISABLED)
	{
		cmd = create_command("is_disabled", NULL);
		current_token = _read_token();
		if(_table_name())
		{
			char* table_location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char) );	
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

boolean __help(){
	boolean result;
	if(current_token== _HELP)
	{
		 result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

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
	else if(current_token==DELETE)
	{
		current_token=_read_token();
		if ( current_token == ARROW )
		{
			current_token = _read_token();
			if(_family_column())
			{
				result=true;
			}
			else
			{
				result=false;
			}
		}
		else
		{
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
				result = false;
				if ( DEBUG) printf("Error : in _map(), invalid val\n");			
			}
		}
		else
		{
			result = false;
			printf("Syntax Error in line %d : Expected \"=>\" after key\n");			
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
// map_aux --> , map | epsilon
boolean _map_aux()
{
	boolean result;
	if ( current_token == ACLOSE)
	{
		follow_token = true;
		result = true;
	}
	else if ( current_token == EOC)
	{
		follow_token = true;
		result = true;
	}
	else if ( current_token == COMMA)
	{
		current_token = _read_token();
		if ( _map() )
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

boolean _whoami()
{
	boolean result;
	if(current_token == WHOAMI)
	{
	  uid_t uid = geteuid();
	  struct passwd *pw = getpwuid(uid);
	  if (pw)
	  {
	    printf("%s\n", pw->pw_name); 
	  }
	  result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _version()
{
	boolean result;
	if(current_token == VERSION)
	{
		printf("HBase License Version 1.0, May 2015  \n");
		result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

boolean _truncate()
{
	boolean result;
	boolean error = false;
	if(current_token == TRUNCATE)
	{

		cmd = create_command("create", NULL);
		current_token = _read_token();
		if(_table_name())
		{

			printf("disabling...");
			char* table_location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char) );

			if (DEBUG)
			{
				printf("disabling (%s) ...", cmd->table->name);
			}
		    if ( !table_exists_in_database(cmd->table->name , table_location))
			{
				if(DEBUG)
				{
					printf("Semantic ERROR : cannot enable table (%s), table doesn't exist\n",cmd->table->name);
				}
				creer_sm_erreur(yylineno, cmd->table->name, TABLE_DOESNT_EXIST);
			}	
			else
			{
				printf("table location : '%s'\n", table_location);
				disable_table(table_location, 0);
			}	
			//cmd = create_command("create", NULL);


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
					printf("deleting table...\n");
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

	
				//printf("Showing command : \n");
				//show_command(cmd);
				printf("Creating table %s...\n", cmd->table->name);
				add_table_to_TABSYM(cmd->table);
				add_table_to_database(cmd->table);
				printf("done\n");
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

boolean _exists()
{
	boolean result;
	if(current_token == EXISTS)
	{
		cmd = create_command("is_enabled", NULL);
		current_token = _read_token();
		if(_table_name())
		{
			char* table_location = (char*) malloc ( (strlen(DATABASE_DIRECTORY_NAME)+strlen(cmd->table->name)+2)*sizeof(char) );

		    if ( table_exists_in_database(cmd->table->name , table_location))
			{
				printf("YES, the table exists\n");
			}	
			else
			{
				printf("No, the table doesn't exist\n");
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

int main(int argc, char const *argv[])
{
	error_token = (char*)malloc(MAX_ERROR_TOKEN_LENGTH*sizeof(char));
	current_token = _read_token();
	_prog();
	//Table* t = get_table_from_database("t1");
	//disable_table(t->location, 0);
	/*char tname[20];
	strcpy(tname, "t2");
	Table*	t = get_table_from_database(tname);
	show_table(t);*/
	//list_tables_by_name_in_database(&nb_tab);
	return 0;
}
