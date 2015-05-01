#include	"zz_error.h"
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
		printf("ligne %d : %s %s \n", erreur->error_line, erreur->source_name, semanticErrorTypeToString(erreur->error_type));
	}
}

void	afficher_sm_erreurs()
{
	printf("%d erreur%c semantique%c\n", SMERROR_CURRENT_INDEX,  SMERROR_CURRENT_INDEX > 1 ? 's':'\0', SMERROR_CURRENT_INDEX > 1 ? 's':'\0');
	int i=0;
	for(i=0; i<SMERROR_CURRENT_INDEX; i++)
	{
		afficher_sm_erreur(smError_Stack[i]);
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
		default:
			strcpy(result, "Unknown error" );
			return result;
	}

}
