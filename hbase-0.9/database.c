#include 	"database.h"
#include	"dirent.h"
#include    "tabsym.h"
#include    "stdlib.h"
#include    "string.h"
#include    "errno.h"
#include    "stdio.h"
#include    "sys/stat.h"

int DATABASE_DEBUG = 0;

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
		if(DATABASE_DEBUG)
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
	if(table )
	{
		if ( table->index == 0)
		{
			return;
		}
		// strcat(const char*, const char*) will cause segmentation fault 
		// because strcat(char* dest, const char* src)
		char*  database_dir = (char*) malloc (strlen(DATABASE_DIRECTORY_NAME)*sizeof(char));
		strcpy(database_dir, DATABASE_DIRECTORY_NAME);

		// allocation of table location because it was not initialised during creation of table
		table->location = (char*) malloc( (strlen(DATABASE_DIRECTORY_NAME)+strlen(table->name)+2)*sizeof(char) );
		strcpy(table->location, strcat(database_dir, table->name));  // "database/tab_name"
		strcat(table->location, "/"); // "database/tab_name/"
		if(DATABASE_DEBUG)
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
			// tmp string to store family column schema location
			char*  fc_schema_location = (char*) malloc( (strlen(table->location)+MAX_FAMILY_COLUMN_NAME_LENGTH+10)*sizeof(char));
			int i;
			FILE* fc_schema_file; // file pointer to family column schema 
			for (i=0; i<table->index;i++)
			{
				// allocation of family location because it was not initialised during creation of family column
				table->list_family_col[i].location = (char*) malloc( (strlen(table->location)+strlen(table->list_family_col[i].name)+1)*sizeof(char));
				strcpy(table->list_family_col[i].location, table->location);
				strcat(table->list_family_col[i].location, table->list_family_col[i].name); // "database/tab_name/family_column_name"
				strcat(table->list_family_col[i].location, "/"); // "database/tab_name/family_column_name/"
				if(DATABASE_DEBUG)
				{
					printf("creating family column directory \"%s\" ...\n", table->list_family_col[i].location);
				}
				if ( mkdir(  table->list_family_col[i].location , 0777) == -1)
				{
					printf("FATAL ERROR: %s\n", strerror(errno));			
				} 

				// create family column schema file
				strcpy(fc_schema_location, table->list_family_col[i].location);
				strcat(fc_schema_location, "schema");
				fc_schema_file= fopen(fc_schema_location, "w");
				fprintf(fc_schema_file, "NAME : %s\nVERSIONS : %d", table->list_family_col[i].name, table->list_family_col[i].versions);
				fclose(fc_schema_file);
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
			//free(schema_location);
			//free(fc_schema_location);
		}
		
	}
}



void 		alter_table(char* table_name)
{
	Table* table = get_table_from_database(table_name);
	if(table)	
	{
		
		char*  fc_schema_location = (char*) malloc( (strlen(table->location)+MAX_FAMILY_COLUMN_NAME_LENGTH+10)*sizeof(char));
			int i;
			FILE* fc_schema_file; // file pointer to family column schema 
			for (i=0; i<table->index;i++)
			{
				// allocation of family location because it was not initialised during creation of family column
				table->list_family_col[i].location = (char*) malloc( (strlen(table->location)+strlen(table->list_family_col[i].name)+1)*sizeof(char));
				strcpy(table->list_family_col[i].location, table->location);
				strcat(table->list_family_col[i].location, table->list_family_col[i].name); // "database/tab_name/family_column_name"
				strcat(table->list_family_col[i].location, "/"); // "database/tab_name/family_column_name/"
				if(DATABASE_DEBUG)
				{
					printf("creating family column directory \"%s\" ...\n", table->list_family_col[i].location);
				}
				if ( mkdir(  table->list_family_col[i].location , 0777) == -1)
				{
					printf("FATAL ERROR: %s\n", strerror(errno));			
				} 

				// create family column schema file
				strcpy(fc_schema_location, table->list_family_col[i].location);
				strcat(fc_schema_location, "schema");
				fc_schema_file= fopen(fc_schema_location, "w");
				fprintf(fc_schema_file, "NAME : %s\nVERSIONS : %d", table->list_family_col[i].name, table->list_family_col[i].versions);
				fclose(fc_schema_file);
			}
			// create schema file and write the schema in it
			FILE* schema_file = fopen(table->location, "w");
			fprintf(schema_file, "name : %s\nenabled : %d\nfamily_col : ", table->name, table->enabled);
			for(i=0; i< table->index-1; i++)
			{
				fprintf(schema_file, "%s , ", table->list_family_col[i].name);
			}
			fprintf(schema_file, "%s\nnb_rows : 0",  table->list_family_col[i].name);
			fclose(schema_file);
			//free(fc_schema_location);
	}
}


/*
* this function takes a file name and delete it's extension 
* We'll need this function to retrieve column names within a family column,
* because column files are named 'column_name.data' to avoid conflict with the 'schema' file
*/
static   void   exclude_extention(char* file_name)
{
	if(file_name)
	{
		int i=0;
		while(file_name[i] && (file_name[i] != '.') )
		{
			i++;
		}
		file_name[i] = '\0';
	}
}

char **  list_columns_by_name_in_database(char* fc_location, int* nb_columns)
{
	int i;
	*nb_columns=0;
	char**	list_columns = (char**)malloc(MAX_COLUMN_NUMBER*sizeof(char*));
	for (i=0; i<MAX_COLUMN_NUMBER; i++)
	{
		list_columns[i] = (char*)malloc(MAX_COLUMN_NAME_LENGTH*sizeof(char));
	}
	DIR 	*family_column_directory;
	struct dirent	*dir;
	family_column_directory = opendir(fc_location);
	if(family_column_directory)
	{
		while( (dir = readdir(family_column_directory)) != NULL )
		{
			if ( strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0 && strcmp(dir->d_name, "schema") != 0)
			{
				strcpy(list_columns[(*nb_columns)++], dir->d_name);
			}
		}
		closedir(family_column_directory);
		if(DATABASE_DEBUG)
		{
				printf("total columns : %d\n", *nb_columns);	
		}
	}
	else
	{
		printf("FATAL ERROR : %s\n", strerror(errno));
	}
	return list_columns;
}


boolean		table_exists_in_database(char* table_name, char* location)
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
		 	if ( location )
		 	{
		 		strcpy(location, DATABASE_DIRECTORY_NAME);
		 		strcat(location, table_name);
		 		strcat(location, "/");
		 	}
		 	return  true;
		 }
	}
	// destroy the list
	//free(tab_name_list);
	return  false;
}

boolean		remove_dir(char* path)
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
					if(DATABASE_DEBUG)
					{
						printf("deleting file '%s', location : %s \n", dir->d_name, path);
					}
					unlink(path);
				}
				else if ( dir->d_type == 4)
				{
					if(DATABASE_DEBUG)
					{
						printf("deleting directory '%s', location : '%s'  \n", dir->d_name, path);
					}
					strcat(path, "/");
					remove_dir(path);
				}
				else
				{
					if(DATABASE_DEBUG)
					{
						printf("Unkow file type\n");
					}
				}
			}
			if(DATABASE_DEBUG)
			{
				printf("remove_dir()\n");
			}
			closedir(directory);
			if (rmdir(root_dir) == -1 )
			{
				printf("ERROR: %s, %s\n",root_dir, strerror(errno) );
				return false;
			}
			return true;
		}
		else
		{
			return false;
			printf("ERROR: %s\n", strerror(errno));
		}
}

boolean		delete_table_from_database(Table* table)  
{
	if ( table )
	{
	 	if( remove_dir(table->location) ) // "database/t1/"
	 	{
	 		return true;
	 	}
	}
	else
	{
		if(DATABASE_DEBUG)
			printf("ERROR: table null pointer\n");
		return false;
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

// return a column object by passing the family column location and the column name
Column*		get_column_from_database(char* family_column_location, char* column_name)
{
	char*	col_location = (char*) malloc( (strlen(family_column_location)+strlen(column_name)+2)*sizeof(char));
	strcpy(col_location, family_column_location);
	strcat(col_location, column_name);

	FILE* column_file = fopen(col_location, "r");
	char	row_name[MAX_ROW_NAME_LENGTH];
	char	val[MAX_VALUE_LENGTH];
	char    type[10];
	if( column_file)
	{
		exclude_extention(column_name);
		Column* column = create_column(column_name);
		column->location = (char*) malloc( (strlen(col_location)+2)*sizeof(char));
		strcpy(column->location, col_location);
		// column storage format is =>  row_name : <value> ; %% type := <val_typr> %% 
		while( fscanf(column_file,"%s : %s ; // type := %s //", row_name, val, type) != EOF )
		{
			Row* row = create_row(row_name);
			switch ( atoi(type) )
			{
				case STRING_VAL:
						add_ValType_to_row(row, create_string_ValType(val));
					break;
				case INUMBER_VAL:
						add_ValType_to_row(row, create_inumber_ValType(atoi(val)));
					break;
				case DNUMBER_VAL:
						add_ValType_to_row(row, create_dnumber_ValType(atof(val)));
					break;
				default:
						add_ValType_to_row(row, create_string_ValType(val));
			}
			add_row_to_column(column, row);
		}
		return column;
	}
	else
	{
		printf("ERROR : %s, %s\n",col_location, strerror(errno));
	}
	return NULL;
}

FamilyColumn*		get_family_column_from_database(char* table_location, char* fc_name)
{
	char*	fc_location = (char*) malloc( (strlen(table_location)+strlen(fc_name)+2)*sizeof(char));
	strcpy(fc_location, table_location);
	strcat(fc_location, fc_name);
	strcat(fc_location, "/");
	FamilyColumn* fc = get_family_column_from_database_by_location(fc_location);
	//free(fc_location);
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
	if (DATABASE_DEBUG)
	{
		printf("Opening schema located in '%s'\n", schema_location);
	}
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

		// now collecte the column that belongs to this family column
		char**	list_columns = (char**)malloc(MAX_COLUMN_NUMBER*sizeof(char*));
		int column_index;
		for (column_index=0; column_index<MAX_COLUMN_NUMBER; column_index++)
		{
			list_columns[column_index] = (char*)malloc(MAX_COLUMN_NAME_LENGTH*sizeof(char));
		}
		
		list_columns = list_columns_by_name_in_database(fc_location, &column_index);
		int i;
		for(i=0; i<column_index;i++)
		{

			Column* column = get_column_from_database(fc_location, list_columns[i]);
			if ( column )
			{
				add_column_to_family_column(fc, column);
			}
		}
		return fc;
	}			
	else
	{
		printf("FATAL ERROR: family column '%s' schema file, %s\n",val , strerror(errno) );				
	}
	//free(schema_location);
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
					reset_buffer(val, strlen(val));
					while ( fscanf(schema_file, " , %s", val) != 0)
					{
						FamilyColumn *fam = get_family_column_from_database(table->location, val);
						if ( fam )
						{
							add_family_column_to_table(table, fam);
						}
						reset_buffer(val, strlen(val));
					}
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
		if ( DATABASE_DEBUG)
		{
			printf("FATAL ERROR: table '%s' schema file, '%s' %s\n",table_name, schema_location, strerror(errno) );
		}
	}
	return NULL;
}

boolean 		disable_table(char* table_location, int table_index)
{
	if ( !table_location )
	{
		disable_table_in_TABSYM(table_index);
	}
	else
	{
		disable_table_in_TABSYM(table_index);

		// disable table in database
		//locate the schema file
		char*   schema_location = (char*) malloc( (strlen(table_location)+8)*sizeof(char));
		strcpy(schema_location, table_location);
		strcat(schema_location, "schema");

		// open the schema file and look for "enabled :" and set it to 0

		FILE* schema_file = fopen(schema_location, "r+");
		if ( schema_file )
		{
			char key[20];
			char val[MAX_TABLE_NAME_LENGTH];
			int c=0;
			while ( (fscanf(schema_file, "%s : %s", key, val)) != EOF )
			{
				c += strlen(key)+strlen(val)+3;
				if ( strcmp(key, "enabled") == 0 )
				{
					if ( atoi(val) != 0)
					{
						c-1;
						fseek(schema_file, c, SEEK_SET);
						fprintf(schema_file, "%s", "0");
					}
					break;
				}
			}
			fclose(schema_file);
			return true;
		}
		else
		{
			printf("FATAL ERROR : %s, %s\n", schema_location, strerror(errno));
			return false;
		}
	}
}

boolean		 enable_table(char* table_location, int table_index)
{
	if ( !table_location )
	{
		enable_table_in_TABSYM(table_index);
	}
	else
	{
		enable_table_in_TABSYM(table_index);

		// disable table in database
		//locate the schema file
		char*   schema_location = (char*) malloc( (strlen(table_location)+8)*sizeof(char));
		strcpy(schema_location, table_location);
		strcat(schema_location, "schema");

		// open the schema file and look for "enabled :" and set it to 0

		FILE* schema_file = fopen(schema_location, "r+");
		if ( schema_file )
		{
			char key[20];
			char val[MAX_TABLE_NAME_LENGTH];
			int c=0;
			while ( (fscanf(schema_file, "%s : %s", key, val)) != EOF )
			{
				c += strlen(key)+strlen(val)+3;
				if ( strcmp(key, "enabled") == 0 )
				{
					if ( atoi(val) != 1)
					{
						c-1;
						fseek(schema_file, c, SEEK_SET);
						fprintf(schema_file, "%s", "1");
					}
					break;
				}
			}
			fclose(schema_file);
			return true;
		}
		else
		{
			printf("FATAL ERROR : %s, %s\n", schema_location, strerror(errno));
			return false;
		}
	}
}

void        edit_nb_rows_in_schema(char* table_location, int new_val)
{
	char*   schema_location = (char*) malloc( (strlen(table_location)+8)*sizeof(char));
	strcpy(schema_location, table_location);
	strcat(schema_location, "schema");

	FILE* table_schema = fopen(schema_location, "r");
		char* tmp_schema_location = (char*) malloc(strlen(schema_location)*sizeof(char));
		strcpy(tmp_schema_location, schema_location);
		if(!table_schema)
		{
			printf("ERROR : %s, %s\n", schema_location, strerror(errno));
			return false;
		}
		strcat(tmp_schema_location, ".tmp");
		// open a tmp file for editing
		if (DATABASE_DEBUG)
		{
			printf("openning %s ...\n", tmp_schema_location);
		}
		FILE* tmp = fopen(tmp_schema_location, "w"); 
		if(!tmp)
		{
			printf("FATAL ERROR : %s, %s\n", tmp_schema_location, strerror(errno));
			return;
		}
		char	key[MAX_ROW_NAME_LENGTH];
		char	val[MAX_VALUE_LENGTH];
		// read from column file and write to tmp, once the specific row is found, write it's new value
		// then continue copying ... 
		while ( fscanf(table_schema, "%s : %s", key, val) != EOF )
		{
			if ( strcmp(key, "nb_rows") == 0)
			{
				fprintf(tmp, "nb_rows : %d\n", new_val);
			}
			else if ( strcmp(key, "family_col") == 0)
			{
				fprintf(tmp, "family_col : %s", val);
				while ( fscanf(table_schema, " , %s", val) != 0)
				{
					fprintf(tmp, " , %s", val);
				}
				fprintf(tmp, "\n");
			}
			else
			{
				fprintf(tmp, "%s : %s\n", key, val);
			}
		}
		fclose(table_schema);
		unlink(table_schema);
		rename(tmp_schema_location, schema_location);
		fclose(tmp);

}

/*
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
								
								buf[buf_index++] = c;
							}
							else
							{
								//buf[buf_index++] = '\n';
								buf[buf_index] = '\0';
								FamilyColumn *fam = get_family_column_from_database(table->location, buf);
								if ( fc )
								{
									add_family_column_to_table(table, fam);
								}
								reset_buffer(buf, buf_index);
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

}*/


// look for a sprecific row and edit it's value and type
boolean 		edit_row_in_database(char* column_location, Row* edit_row, ValType* new_val)
{
	boolean edit=false;
	if(column_location)
	{
		// open the column file
		if (DATABASE_DEBUG)
		{
			printf("openning %s ...\n", column_location);
		}
		FILE* column_file = fopen(column_location, "r");
		char* tmp_column_location = (char*) malloc((strlen(column_location)+5)*sizeof(char));
		strcpy(tmp_column_location, column_location);
		if(!column_file)
		{
			printf("ERROR : %s, %s\n", column_location, strerror(errno));
			return false;
		}
		if ( !new_val )
		{
			return false;
		}
		strcat(tmp_column_location, ".tmp");
		// open a tmp file for editing
		if (DATABASE_DEBUG)
		{
			printf("openning %s ...\n", tmp_column_location);
		}
		FILE* tmp = fopen(tmp_column_location, "w"); 
		if(!tmp)
		{
			printf("FATAL ERROR : %s, %s\n", tmp_column_location, strerror(errno));
			return;
		}
		char	row_name[MAX_ROW_NAME_LENGTH];
		char	val[MAX_VALUE_LENGTH];
		char    type[10];
		// read from column file and write to tmp, once the specific row is found, write it's new value
		// then continue copying ... 
		while ( fscanf(column_file, "%s : %s ; // type := %d //", row_name, val, type) != EOF )
		{

			if ( strcmp(row_name, edit_row->name) == 0)
			{
				switch (new_val->type)
				{
					case STRING_VAL:
					case BOOL_VAL:
						fprintf(tmp, "%s : %s ; // type := %d //\n", row_name, new_val->Value.string, new_val->type);
						if (strcmp(new_val->Value.string, val) != 0)
						{

							edit = true;
						}
						break;
					case INUMBER_VAL:
						fprintf(tmp, "%s : %d ; // type := %d //\n", row_name, new_val->Value.inumber, new_val->type);
						if ( new_val->Value.inumber != atoi(val))
						{
							edit = true;
						}
						break;
					case DNUMBER_VAL:
						fprintf(tmp, "%s : %lf ; // type := %d //\n", row_name, new_val->Value.dnumber, new_val->type);
						if ( new_val->Value.dnumber != atof(val))
						{
							edit = true;
						}
						break;
					default:
						fprintf(tmp, "%s : (NaN) ; // type := UNKOWN //\n", row_name);
				}
			}
			else
			{
				fprintf(tmp, "%s : %s ; // type := %d //\n", row_name, val, atoi(type));
			}
		}
		fclose(column_file);
		if ( !unlink(column_file))
		{
			printf("FATAl ERROR: %s , %s\n", strerror(errno), column_location);
		}
		fclose(tmp);
	    if ( DATABASE_DEBUG )
	    {
			printf("renaming  '%s' to '%s' \n",tmp_column_location, column_location);
		}
		rename(tmp_column_location, column_location);
	}

	if(edit)
	{
		printf("\nRow '%s' successfully updated : old value was (", edit_row->name);
		show_ValType(edit_row->valType);
		printf("), new value is (");
		show_ValType(new_val);
		printf(")\n");
	}
	else
	{
		printf("\nRow '%s' is up to date\n",edit_row->name );
		printf("0 row(s) affected\n");
	}
	return true;
}

boolean 	add_row_in_database(char* column_location, Row* new_row)
{
	if(column_location)
	{
		FILE* column_file = fopen(column_location, "a");
		if(column_file)
		{
			if(!new_row)
			{
				return false;
			}
			switch (new_row->valType->type)
			{
				case STRING_VAL:
				case BOOL_VAL:
					fprintf(column_file, "%s : %s ; // type := %d //\n", new_row->name, new_row->valType->Value.string, new_row->valType->type);
					break;
				case INUMBER_VAL:
					fprintf(column_file, "%s : %d ; // type := %d //\n", new_row->name, new_row->valType->Value.inumber, new_row->valType->type);
					break;
				case DNUMBER_VAL:
					fprintf(column_file, "%s : %lf ; // type := %d //\n", new_row->name, new_row->valType->Value.dnumber, new_row->valType->type);
					break;
				default:
					fprintf(column_file, "%s : (NaN) ; // type := UNKOWN //\n", new_row->name);
			}
			fclose(column_file);
			return true;
		}
		else
		{
			printf("FATAL ERROR: %s, %s\n",column_location, strerror(errno));
			return false;
		}
	}
	else
	{
		printf("ERROR: column location not set\n");
		return false;
	}

}

void 		add_column_in_database(char* family_column_location, Column* new_column)
{
	 if (family_column_location)
	 {
	 	if(new_column)
	 	{
	 		new_column->location = (char*) malloc ( (strlen(family_column_location)+strlen(new_column->name)+2)*sizeof(char));
	 		strcpy(new_column->location, family_column_location);
	 		strcat(new_column->location, new_column->name);
	 		int i;
	 		for(i=0; i<new_column->index; i++)
	 		{
	 			add_row_in_database(new_column->location, &new_column->list_row[i]);
	 		}
	 	}
	 }
	 else
	 {
	 	printf("ERROR : family column location inkown\n");
	 }
}



void 		update_table_in_database(char* table_name, Command* cmd)
{
	 if ( DATABASE_DEBUG )
	{
		printf("Updating table '%s' in database ...\n", table_name);
	}
	if ( cmd )
	{
		int index_of_family_col=-1;
		int index_of_column=-1;
		int index_of_row=-1;
		Table* table = get_table_from_database(table_name);
		if ( table )
		{
			if ( family_column_exists_in_table(table, cmd->put_family_col_name, &index_of_family_col))
			{
				//printf("index of family_col (%s) in table  (%s) is : %d\n", cmd->put_family_col_name, table->name, index_of_family_col);
				if ( column_exists_in_family_column(&table->list_family_col[index_of_family_col],  cmd->put_column, &index_of_column))
				{
					if ( row_exists_in_column(&table->list_family_col[index_of_family_col].list_column[index_of_column], cmd->put_rname, &index_of_row))
					{
						//TABSYM[index_of_table]->list_family_col[index_of_family_col].list_column[index_of_column].list_row[index_of_row].valType = cmd->put_valType;
						//put_value()
						edit_row_in_database(table->list_family_col[index_of_family_col].list_column[index_of_column].location, &table->list_family_col[index_of_family_col].list_column[index_of_column].list_row[index_of_row], cmd->put_valType);
					}
					// if row doesn't exist then create a new row and put the new value in it
					else
					{
						Row* new_row = create_row(cmd->put_rname);
						new_row->valType = cmd->put_valType;
						if( add_row_in_database(table->list_family_col[index_of_family_col].list_column[index_of_column].location, new_row) )
						{
							printf("Row '%s', successfully added to '%s'\n", new_row->name, table->name);
						}
						edit_nb_rows_in_schema(table->location, table->nb_rows+1);
					}
				}
				// if column doesn't exist then create a new column and put a new row in it
				else
				{
					Row* new_row_ = create_row(cmd->put_rname);
					new_row_->valType = cmd->put_valType;		
					Column* new_column = create_column(cmd->put_column);
					add_row_to_column(new_column, new_row_);
					add_column_in_database(table->list_family_col[index_of_family_col].location, new_column);
					edit_nb_rows_in_schema(table->location, table->nb_rows+1);
				}
			}
		}
		else 
		{
			if ( DATABASE_DEBUG )
			{
				printf("Table '%s' NOT FOUND \n", table_name);
			}
		}
	}
	if ( DATABASE_DEBUG )
	{
		printf("Out of update_cmd_ValType_in_TABSYM()\n");
	}
}
