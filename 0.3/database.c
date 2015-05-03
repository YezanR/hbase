#include 	"database.h"
#include	"dirent.h"
#include    "tabsym.h"
#include    "stdlib.h"
#include    "string.h"
#include    "errno.h"
#include    "stdio.h"
#include    "sys/stat.h"

char**   list_tables_by_name_in_database(int* nb_tables)
{
	int i;
	if(nb_tables)
	{
		*nb_tables=0;
	}
	char**	list_tables = (char**)malloc(MAX_TABLES_NUMBER*sizeof(char*));
	for (i=0; i<MAX_TABLES_NUMBER; i++)
	{
		list_tables[i] = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	}
	DIR 	*database_directory;
	struct dirent	*dir;
	database_directory = opendir(DATABASE_DIRECTORY_NAME);
	if(database_directory)
	{
		while( (dir = readdir(database_directory)) != NULL )
		{
			if ( strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
			{
				strcpy(list_tables[(*nb_tables)++], dir->d_name);
			}
		}
		closedir(database_directory);
		if(DEBUG)
		{
			if(nb_tables)
			{
				printf("total tables : %d\n", *nb_tables);
			}
		}
	}
	else
	{
		printf("FATAL ERROR: unable to open database directory, is directory exists? permissions ?\n");
	}
	return list_tables;
}

void    	add_table_to_database(Table* table)
{
	if(table)
	{
		// strcat(const char*, const char*) will cause segmentation fault 
		// because strcat(char* dest, const char* src)
		char*  database_dir = (char*) malloc (strlen(DATABASE_DIRECTORY_NAME)*sizeof(char));
		strcpy(database_dir, DATABASE_DIRECTORY_NAME);

		// allocation of table location because it was not initialised during creation of table
		table->location = (char*) malloc( (strlen(DATABASE_DIRECTORY_NAME)+strlen(table->name)+2)*sizeof(char) );
		strcpy(table->location, strcat(database_dir, table->name));  // "database/tab_name"
		strcat(table->location, "/"); // "database/tab_name/"
		if(DEBUG)
		{
			printf("creating table directory \"%s\" ...\n", table->location);
		}
		if (  mkdir(table->location, 0777) == -1 )
		{
			printf("FATAL ERROR: %s\n", strerror(errno));
		}
		else
		{
			char*  schema_location = (char*) malloc( (strlen(table->location)+10)*sizeof(char));
			strcpy(schema_location, table->location);
			strcat(schema_location, "schema"); // "database/tab_name/schema"
			int i;
			for (i=0; i<table->index;i++)
			{
				// allocation of family location because it was not initialised during creation of family column
				table->list_family_col[i].location = (char*) malloc( (strlen(table->location)+strlen(table->list_family_col[i].name)+1)*sizeof(char));
				strcpy(table->list_family_col[i].location, table->location);
				strcat(table->list_family_col[i].location, table->list_family_col[i].name); // "database/tab_name/family_column_name"
				strcat(table->list_family_col[i].location, "/"); // "database/tab_name/family_column_name/"
				if(DEBUG)
				{
					printf("creating family column directory \"%s\" ...\n", table->list_family_col[i].location);
				}
				if ( mkdir(  table->list_family_col[i].location , 0777) == -1)
				{
					printf("FATAL ERROR: %s\n", strerror(errno));			
				} 
			}
			// create schema file and write the schema in it
			FILE* schema_file = fopen(schema_location, "w");
			fprintf(schema_file, "name : %s\nenabled : %d\nfamily_col : ", table->name, table->enabled);
			for(i=0; i< table->index-1; i++)
			{
				fprintf(schema_file, "%s , ", table->list_family_col[i].name);
			}
			fprintf(schema_file, "%s\nnb_rows : 0",  table->list_family_col[i].name);
			fclose(schema_file);
			free(schema_location);
		}
		
	}
}

boolean		table_exists_in_database(char* table_name)
{
	int nb_tables;
	// list table names in database
	char** tab_name_list = list_tables_by_name_in_database(&nb_tables);
	// look for the table name in the list
	int i;
	for (i=0 ; i < nb_tables; ++i)
	{
		 if (strcmp(table_name, tab_name_list[i]) == 0)
		 {
		 	return  true;
		 }
	}
	// destroy the list
	free(tab_name_list);
	return  false;
}

static  void		remove_dir(char* path)
{
	  	DIR 	*directory;
		struct dirent	*dir;
		directory = opendir(path);
		int nb_elt=0;
		char* root_dir = (char*) malloc( (strlen(path)+1)*sizeof(char));
		strcpy(root_dir, path);
		if(directory)
		{
			while( (dir = readdir(directory)) != NULL )
			{
				
				if ( strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
				{
					continue;
				}
				strcpy(path, root_dir);
				strcat(path, dir->d_name); // "database/t1/f1"
				if ( dir->d_type == 8)  // if it's a file
				{	
					if(DEBUG)
					{
						printf("deleting file '%s', location : %s \n", dir->d_name, path);
					}
					unlink(path);
				}
				else if ( dir->d_type == 4)
				{
					if(DEBUG)
					{
						printf("deleting directory '%s', location : '%s'  \n", dir->d_name, path);
					}
					remove_dir(path);
				}
				else
				{
					if(DEBUG)
					{
						printf("Unkow file type\n");
					}
				}
			}
			if(DEBUG)
			{
				printf("remove_dir()\n");
			}
			closedir(directory);
			rmdir(root_dir);
		}
		else
		{
			printf("ERROR: %s\n", strerror(errno));
		}
}

void		delete_table_from_database(Table* table)  
{
	if ( table )
	{
	 	remove_dir(table->location); // "database/t1/"
	}
	else
	{
		if(DEBUG)
			printf("ERROR: table null pointer\n");
	}
}

static	void	reset_buffer(char* buf, int buf_size)
{
	int i;
	for(i=0; i<buf_size; i++)
	{
		buf[i] = '\0';
	}
}

FamilyColumn*		get_family_column_from_database(char* table_location, char* fc_name)
{
	char*	fc_location = (char*) malloc( (strlen(table_location)+strlen(fc_name)+2)*sizeof(char));
	strcpy(fc_location, table_location);
	strcat(fc_location, fc_name);
	strcat(fc_location, "/");
	FamilyColumn* fc = get_family_column_from_database_by_location(fc_location);
	free(fc_location);
	return fc;
}

FamilyColumn*		get_family_column_from_database_by_location(char* fc_location)
{
	if(!fc_location)
	{
		printf("NULL LOCATION : %s\n", fc_location);
		return NULL;
	}
	// locate the family column schema
	char* schema_location = (char*) malloc( (strlen(fc_location)+8)*sizeof(char));
	strcpy(schema_location, fc_location);
	strcat(schema_location, "schema");
	printf("Opening schema located in '%s'\n", schema_location);
	// open family column schema and collecte meta data
	FILE* fc_schema_file = fopen(schema_location, "r");
	char	key[20];
	char	val[MAX_FAMILY_COLUMN_NAME_LENGTH];
	if(fc_schema_file)
	{
		FamilyColumn* fc = create_family_column(val);
		fc->location = (char*) malloc( (strlen(fc_location)+1)*sizeof(char));
		strcpy(fc->location, fc_location);
		while ( fscanf(fc_schema_file, "%s : %s", key, val) != EOF )
		{

			if (strcmp(key, "NAME") == 0)
			{
				strcpy(fc->name, val);
			}
			else if (strcmp(key, "VERSIONS") == 0)
			{
				fc->versions = atoi(val);
			}
			reset_buffer(key, 20);
			reset_buffer(val, strlen(val));
		}
		fclose(fc_schema_file);
		return fc;
	}			
	else
	{
		printf("FATAL ERROR: family column '%s' schema file, %s\n",val , strerror(errno) );				
	}
	free(schema_location);
	return NULL;
}

Table*		get_table_from_database(char* table_name)
{
	// locate the directory 
	char*	directory_location = (char*) malloc( (strlen(DATABASE_DIRECTORY_NAME)+strlen(table_name)+2)*sizeof(char));
	strcpy(directory_location, DATABASE_DIRECTORY_NAME);
	strcat(directory_location, table_name);
	strcat(directory_location, "/");

	// locate the schema file
	char*   schema_location = (char*) malloc( (strlen(directory_location)+8)*sizeof(char));
	strcpy(schema_location, directory_location);
	strcat(schema_location, "schema");

	char c; // read char
	int buf_index=0;
	char buf[MAX_FAMILY_COLUMN_NAME_LENGTH];
	// open schema file and get the information about table (schema)

	FILE* schema_file = fopen(schema_location, "r");

	if(schema_file)
	{
		Table*  table = create_table("t");
		// store the location
		table->location = (char*) malloc((strlen(directory_location)+1)*sizeof(char));
		strcpy(table->location, directory_location);

		char 	key[20];	
		char 	val[MAX_TABLE_NAME_LENGTH];
		while( fscanf(schema_file,"%s : %s", key, val) != EOF )
		{
			if ( strcmp(key, "name") == 0)
			{
				strcpy(table->name, val);
			}
			else if ( strcmp(key, "enabled") == 0)
			{
				table->enabled = atoi(val);
			}
			// if it's family_col then for each family column name we need to open it's location and construct
			// a family column object based on the schema of the family column which means repeat the same work
			// done to table ...
			else if ( strcmp(key, "family_col") == 0 )
			{	
			    	// locate the family column, directory_location string will be overwritin by family_column location !!
					strcpy(directory_location, table->location);
					strcat(directory_location, val); // directory_location now contains "database/table_name/family_column_name"
					strcat(directory_location, "/");
					// get_family_column
					FamilyColumn *fc = get_family_column_from_database_by_location(directory_location);
					if ( fc )
					{
						add_family_column_to_table(table, fc);
					}
					fgetc(schema_file);
					fgetc(schema_file);

						while ( (c = fgetc(schema_file)) != '\n')
						{
							if ( c== ' ')
							{
								continue;
							}
							else if ( c != ',')
							{
								printf("buf[%d] = %c\n",buf_index, buf[buf_index]);
								buf[buf_index++] = c;
							}
							else
							{
								//buf[buf_index++] = '\n';
								FamilyColumn *fam = get_family_column_from_database(table->location, buf);
								if ( fc )
								{
									add_family_column_to_table(table, fam);
								}
								reset_buffer(buf, buf_index);
								buf[buf_index] = '\n';
								buf_index = 0;
							}
						}
						buf[buf_index] = '\0';
						add_family_column_to_table(table, get_family_column_from_database(table->location, buf));
			}
			else if ( strcmp(key, "nb_rows") == 0)
			{
				table->nb_rows = atoi(val); // because nb_rows is an int
			}
		}
		//free(key);
		//free(val);
		//free(schema_location);
		//free(directory_location);
		fclose(schema_file);
		return table;
	}
	else
	{
		printf("FATAL ERROR: table '%s' schema file, '%s' %s\n",table_name, schema_location, strerror(errno) );
	}

}

/*
void 		update_table_in_database(Table* table, Command* cmd)
{
	 if ( DEBUG )
	{
		printf("Updating table '%s' in database ...\n", table->name);
	}
	if ( cmd )
	{
		int index_of_family_col;
		int index_of_column;
		int index_of_row;
		char* family_col_name =   (char*) malloc ( MAX_TABLE_NAME_LENGTH*sizeof(char));
		char* column_name = (char*) malloc ( MAX_COLUMN_NAME_LENGTH*sizeof(char));
		char* row_name = (char*) malloc ( MAX_ROW_NAME_LENGTH*sizeof(char));
		strcpy(family_col_name, cmd->put_family_col_name);
		strcpy(column_name, cmd->put_column);
		strcpy(row_name, cmd->put_rname);
		if ( family_column_exists_in_table(TABSYM[index_of_table], family_col_name, &index_of_family_col))
		{
			printf("index of family_col (%s) in table  (%s) is : %d\n",family_col_name, TABSYM[index_of_table]->name, index_of_family_col);
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
	free(row_name);
	free(family_col_name);
	free(column_name);
}*/