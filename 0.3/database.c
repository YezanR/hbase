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
				printf(" %s : %d\n", dir->d_name, dir->d_type);
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
				fprintf(schema_file, "%s,", table->list_family_col[i].name);
			}
			fprintf(schema_file, "%s\n",  table->list_family_col[i].name);
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
/*
static 	 char**   list_files_in_directory(char* path, int* nb_elements)
{

	int i;
	if(nb_elements)
	{
		*nb_elements=0;
	}
	char**	list_files = (char**)malloc(MAX_NUMBER_OF_FILES_IN_DIR*sizeof(char*));
	for (i=0; i<MAX_TABLES_NUMBER; i++)
	{
		list_files[i] = (char*)malloc(MAX_TABLE_NAME_LENGTH*sizeof(char));
	}
	DIR 	*dir;
	struct dirent	*dir;
	dir = opendir(path);
	if(dir)
	{
		while( (dir = readdir(dir)) != NULL )
		{
			if ( strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0)
			{
				strcpy(list_files[(*nb_elements)++], dir->d_name);
				//*nb_elements = *nb_elements+1;
			}
		}
		closedir(dir);
		if(DEBUG)
		{
			if(nb_elements)
			{
				printf("total tables : %d\n", *nb_elements);
			}
		}
	}
	else
	{
		printf("FATAL ERROR: unable to open database directory, is directory exists? permissions ?\n");
	}
	return list_files;
}
*/

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
