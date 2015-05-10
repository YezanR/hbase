#include	"error.h"
#include	"stdio.h"
#include	"stdlib.h"
#include    "string.h"

void	creer_sm_erreur(int error_line, char* source_name, semanticErrorType type)
{
	smError* err = (smError*)malloc(sizeof(smError));
	err->error_line = error_line;
	err->source_name = (char*)malloc(strlen(source_name)*sizeof(char));
	strcpy(err->source_name, source_name);
	err->error_type = type;
	smError_Stack[SMERROR_CURRENT_INDEX++] = err; // ajouter l'erreur la file d'erreur
}

int     nombre_sm_erreurs()
{
	return 	SMERROR_CURRENT_INDEX;
}

void afficher_sm_erreur(smError* erreur)
{
	if ( erreur )
	{
		printf("\nError in line %d : %s %s \n", (erreur->error_line)-1, erreur->source_name, semanticErrorTypeToString(erreur->error_type));
	}
	else
	{
		printf("erreur null pointer\n");
	}
}

void	afficher_sm_erreurs()
{
	if (SMERROR_CURRENT_INDEX != 0)
	{
		//printf("%d erreur%c semantique%c\n", SMERROR_CURRENT_INDEX,  SMERROR_CURRENT_INDEX > 1 ? 's':'\0', SMERROR_CURRENT_INDEX > 1 ? 's':'\0');
		int i=0;
		for(i=0; i<SMERROR_CURRENT_INDEX; i++)
		{
			afficher_sm_erreur(smError_Stack[i]);
		}
	}
}

char*	semanticErrorTypeToString(semanticErrorType type)
{
	char* result = (char*) malloc(MAX_ERROR_LENGTH*sizeof(char));
	switch (type)
	{
		case TABLE_ALREADY_EXIST:
			strcpy(result, "table already exist!");
			return result;
		case FAMILY_COL_ALREADY_EXIST:
			strcpy(result, "family column already exist!");
			return result;
		case TABLE_DOESNT_EXIST:
			strcpy(result, "table doesn't exist!");
			return result;
		case TABLE_IS_ENABLED:
			strcpy(result, "table is enabled");
			return result;
		case TABLE_IS_DISABLED:
			strcpy(result, "table is disabled!");
			return result;
		case FAMILY_COLUMN_NO_NAME_KEY:
			strcpy(result, "Family column must have a NAME key, consider adding  {NAME => 'Column_family_name'");
			return result;
		case FAMILY_COL_REPEATED_IGNORING_IT:
			strcpy(result, "Column family repeated, repetition will be ignored");
			return result;		
		default:
			strcpy(result, "Unknown error" );
			return result;
	}

}

void   reset_sm_erreurs()
{
	int i;
	for(i=0; i<SMERROR_CURRENT_INDEX;i++)
	{
		smError_Stack[i] = NULL;
	}
	SMERROR_CURRENT_INDEX=0;
}

char*	init_error_token(char* text)
{
	char* error_token = (char*) malloc( MAX_ERROR_TOKEN_LENGTH*sizeof(char));
	strcpy(error_token, text);
	return error_token;
}

void 		append_error_token(char* error_token, char* text)
{
	if ( error_token )
	{
		strcat(error_token, text);
	}
	else
	{
		error_token = init_error_token( text);
	}
}

void	creer_sx_erreur(int error_line, char* source_name, syntaxErrorType type)
{
	sxError* err = (sxError*)malloc(sizeof(sxError));
	err->error_line = error_line;
	err->source_name = (char*)malloc(strlen(source_name)*sizeof(char));
	strcpy(err->source_name, source_name);
	err->error_type = type;
	sxError_Stack[SXERROR_CURRENT_INDEX++] = err; // ajouter l'erreur la file d'erreur
}

int     nombre_sx_erreurs()
{
	return 	SXERROR_CURRENT_INDEX;
}

void afficher_sx_erreur(sxError* erreur)
{
	if ( erreur )
	{
		printf("line %d : %s \n", (erreur->error_line)-1, syntaxicErrorTypeToString(erreur->error_type));
	}
	else
	{
		printf("erreur null pointer\n");
	}
}

void	afficher_sx_erreurs()
{
	if (SXERROR_CURRENT_INDEX != 0)
	{
		printf("%d erreur%c syntaxique%c\n", SXERROR_CURRENT_INDEX,  SXERROR_CURRENT_INDEX > 1 ? 's':'\0', SXERROR_CURRENT_INDEX > 1 ? 's':'\0');
		int i=0;
		for(i=0; i<SXERROR_CURRENT_INDEX; i++)
		{
			afficher_sx_erreur(sxError_Stack[i]);
		}
	}
}

char*	syntaxicErrorTypeToString(syntaxErrorType type)
{
	char* result = (char*) malloc(MAX_ERROR_LENGTH*sizeof(char));
	switch (type)
	{
		case UNKOWN_COMMAND :
			strcpy(result, "Unknown command!");
			return result;
		case MISSING_COMMA:
			strcpy(result, "Missing comma");
			return result;
		case MISSING_COTE:
			strcpy(result, "Missing \"'\"");
			return result;
		case WRONG_TABLENAME:
			strcpy(result, "Wrong table name");
			return result;
		default:
			strcpy(result, "Unknown error" );
			return result;
	}

}

void   reset_sx_erreurs()
{
	int i;
	for(i=0; i<SXERROR_CURRENT_INDEX;i++)
	{
		sxError_Stack[i] = NULL;
	}
	SXERROR_CURRENT_INDEX=0;
}

