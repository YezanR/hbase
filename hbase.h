#ifndef _ANALYSER_HABSE_H
#define _ANALYSER_HABSE_H

extern int yylex();

typedef enum 
{
	true = 1, false=0
}booleen;

typedef enum
{

}typetoken;

booleen _command();
booleen _pointed_command_name();
booleen _arg();
booleen _argaux();
booleen _argaux_aux();
booleen _val();
booleen _map();
booleen _expor();
booleen _exporaux();
booleen _expand();
booleen _expandaux();
booleen _aux();
booleen _key();


#endif
