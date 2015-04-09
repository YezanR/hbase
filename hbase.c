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
	return result;
}

// pointed_command_name --> IDF.command_name | command_name
boolean _pointed_command_name()
{
	boolean result;
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
	return result;
}

//command_name --> create | put | alter | scan | status | ...
boolean _command_name()
{
	boolean result;
	if(current_token == CREATE)
	{
		result = true;
	}
	else if(current_token == PUT)
	{
		result  = true;
	}
	else if(current_token == ALTER)
	{
		result  = true;
	}
	else if(current_token == SCAN)
	{
		result  = true;
	}
	else if(current_token == STATUS)
	{
		result  = true;
	}
	else
	{
		result = false;
		if(debug)	{ printf("command_name error\n");}
	}
	return result;
}

// arg --> argaux argaux_aux | EPSILON
boolean _arg()
{
	boolean result;
	if(_argaux())
	{
		current_token = _lire_token();
		if(_argaux_aux())
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else if(current_token == EOC)
	{
		follow_token = true;
		result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

// argaux --> val | map | {arg}
boolean _argaux()
{
	if(_val())
	{
		result = true;
	}
	else if(_map())
	{
		result = true;
	}
	else if(current_token == AOPEN)
	{
		current_token = _lire_token();
		if(_arg())
		{
			current_token = _lire_token();
			if(current_token == ACLOSE)
			{
				result = true;
			}
			else
			{
				result = false;
				if(debug)	printf("Accolade fermee error\n");
			}
		}
		else
		{
			result = false;
			if(debug)	printf("Arg error\n");
		}
	}
	else
	{
		result = false;
		if(debug)	printf("Accolade ouverte error\n");
	}
	return result;
}

// argaux_aux --> ,argaux argaux_aux | EPSILON
boolean _argaux_aux()
{
	boolean result;
	if(current_token == VIRG)
	{
		current_token = _lire_token();
		if(_argaux())
		{
			current_token = _lire_token();
			if(_argaux_aux())
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
	else if(current_token == EOC)
	{
		result = true;
	}
	else
	{
		result = false;
	}
	return result;
}

// val --> 'IDF:IDF' | 'IDF' | INUMBER | DNUMBER | TRUE | FALSE | 'ALPHANUM'
boolean _val()
{
	if(current_token == APOST)
	{
		current_token = _lire_token();
		if(current_token == IDF)
		{
			current_token = _lire_token();
			if(current_token == COLON)
			{
				current_token = _lire_token();
				if(current_token == IDF)
				{
					current_token = _lire_token();
					if(current_token == APOST)
					{
						result = true;
					}
					else
					{
						result = false;
						if(debug)	{printf("APOST error\n");}
					}
				}
				else
				{
					result = false;
					if(debug)	{printf("IDF error\n");}
				}
			}
			else
			{
				result = false;
				if(debug)	{printf("COLON error\n");}
			}
		}
		else
		{
			result = false;
			if(debug)	{printf("IDF error\n");}
		}
	}
	else if (current_token == APOST)
	{
		current_token = _lire_token();
		if(current_token == IDF)
		{
			current_token == _lire_token();
			if(current_token == APOST)
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
	else if(current_token == INUMBER)
	{
		result = true;
	}
	else if(current_token == DNUMBER)
	{
		result = true;
	}
	else if(current_token == TRUE)
	{
		result = true;
	}
	else if(current_token == FALSE)
	{
		result = true;
	}
	else if(current_token == APOST)
	{
		current_token = _lire_token();
		if(current_token == ALPHANUM)
		{
			current_token = _lire_token()
			if(current_token == APOST)
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
	else
	{
		result = false;
	}
	return result;	
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
