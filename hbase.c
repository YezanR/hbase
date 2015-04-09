#define debug 0
#include "stdio.h"
#include "hbase.h"

typetoken current_token;
boolean follow_token;

typetoken _lire_token()
{
	if(follow_token == true)
	{
		return current_token;
	}
	else
	{
		return (typetoken)yylex();
	}
}

//	_command  -->  pointed_command_name arg  
boolean _command()
{
	boolean result;
	if(_pointed_command_name())
	{
		current_token = _lire_token();
		if(_arg())
		{
			result = true;
		}
		else
		{
			result = false;
			if(debug)	{ printf("arg error\n") };
		}
	}
	else
	{
		result = false;
		if(debug)	{ printf("pointed_command_name error\n"); }
	}
}

// pointed_command_name --> IDF.command_name | command_name
boolean _pointed_command_name()
{
	if(current_token == IDF)
	{
		current_token = _lire_token();
		if(current_token == POINT)
		{
			current_token = _lire_token();
			if(_command_name())
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
	}
	else if(_command_name())
	{
		result = true;
	}
	else
	{
		result = false;
	}
}

//command_name --> create | put | alter | scan | status | ...
boolean _command_name()
{
	if(current_token == CREATE)
}
int main(int argc, char const *argv[])
{
	current_token = _lire_token();
	
	if(_command())
	{
		printf("Execution reussi :D \n");
	}
	else
	{
		printf("Error :( \n");
	}
	return 0;
}
