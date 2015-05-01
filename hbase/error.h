#define	    	MAX_ERROR	 		80
#define		MAX_ERROR_LENGTH	40


typedef enum 
{
	TABLE_ALREADY_EXIST, FAMILY_COL_ALREADY_EXIST, TABLE_DOESNT_EXIST
}semanticErrorType;

typedef enum 
{
	UNKOWN_COMMAND, MISSING_COMMA, MISSING_COTE
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
	syntaxErrorType			error_type;
}smError;



static smError*  		   	   smError_Stack[MAX_ERROR];
static int 				   SMERROR_CURRENT_INDEX=0;


void  			creer_file_erreur();
void			creer_sm_erreur(int error_line, char* source_name, semanticErrorType error_type);
int 			nombre_sm_erreurs();
void			afficher_sm_erreurs();
char*	    		semanticErrorTypeToString(semanticErrorType type);




