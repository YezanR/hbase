#ifndef _ERROR_H
#define _ERROR_H

#define	    	MAX_ERROR	 		80
#define			MAX_ERROR_LENGTH		40
#define 		MAX_ERROR_TOKEN_LENGTH  100


typedef enum 
{
	TABLE_ALREADY_EXIST, FAMILY_COL_ALREADY_EXIST, TABLE_DOESNT_EXIST, TABLE_IS_ENABLED, TABLE_IS_DISABLED, FAMILY_COLUMN_NO_NAME_KEY,FAMILY_COL_REPEATED_IGNORING_IT
}semanticErrorType;

typedef enum 
{
	UNKOWN_COMMAND, MISSING_COMMA, MISSING_COTE,  WRONG_TABLENAME
}syntaxErrorType;


typedef struct 
{
	int 				error_line;
	char*				source_name;
	semanticErrorType		error_type;
}smError;

typedef struct 
{
	int 				error_line;
	char*				source_name;
	//syntaxErrorType		error_type;
	syntaxErrorType		error_type;
}sxError;



static smError*  		   smError_Stack[MAX_ERROR];
static int 				   SMERROR_CURRENT_INDEX=0;

static sxError*  		   	   sxError_Stack[MAX_ERROR];
static int 				   	   SXERROR_CURRENT_INDEX=0;


// semantic errors
void  			creer_file_erreur();
void			creer_sm_erreur(int error_line, char* source_name, semanticErrorType error_type);
int 			nombre_sm_erreurs();
void			afficher_sm_erreurs();
char*	    	semanticErrorTypeToString(semanticErrorType type);
void			reset_sm_erreurs();


// syntax errors
char* 			init_error_token(char* text);
void 			append_error_token(char* error_token, char* text);
void			creer_sx_erreur(int error_line, char* source_name, syntaxErrorType error_type);
int 			nombre_sx_erreurs();
void			afficher_sx_erreurs();
char*			syntaxicErrorTypeToString(syntaxErrorType type);
void			reset_sx_erreurs();

#endif