#!/bin/bash

flex -i --yylineno hbase.l
gcc -o lex.hbase lex.yy.c -lfl
if [ $# -eq 1 ]
then
	./lex.hbase < $1
else
	./lex.hbase
fi

