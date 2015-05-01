
#ifndef  TYPES_H
#define  TYPES_H

typedef enum 
{
	true = 1, false=0
}boolean;

typedef enum 
{
	STRING_VAL, INUMBER_VAL, DNUMBER_VAL, BOOL_VAL
}Type;

typedef struct
{
	union 
	{
		char*  	string; // to store strings 
		int    	inumber; // to store integers
		double  dnumber; // to store double
		int     boolean; // to store booleans
	}Value;
	Type    type;
}ValType;



typedef struct 
{
	char* 		name;
	ValType* 	valType;
	int 		index;
}Row;

typedef struct 
{
	char* 	name;
	Row* 	list_row;
	int 	index;
}Column;

typedef struct 
{
	char* 	name;
	Column* list_column;
	int 	index; // last_index of list_column
}FamilyColumn;

typedef struct 
{

	char* 			name;	
	FamilyColumn* 	list_family_col;
	int   			index; // last_index of list_family_col
	boolean			enabled;
}Table;

typedef struct 
{
	char* 		name;
	Table* 		table;
	char* 		put_rname;
	char* 		put_family_col_name;
	char* 	    put_column;
	ValType*	put_valType;
}Command;

#endif
