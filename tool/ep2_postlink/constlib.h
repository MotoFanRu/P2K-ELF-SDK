

#ifndef CONSTLIB_H
#define CONSTLIB_H

#include "common.h"

void parseCHeader( char *name );
char* initConstlib( char *name );
int parseCHeaderLine( char* buf, int& idx );
int skipWhitespacesAndComments( char* buf, int& idx );
int skipForCommentEnd( char* buf, int& idx );
int skipForSymbol( char* buf, int& idx, char c );
int skipWhitespaces( char* buf, int& idx );
int skipLine( char* buf, int& idx );
int readConstName( char* buf, int& idx, char* dest, int& dstidx );
int readConstVal( char* buf, int& idx );
bool isHexDigit( char c );
int checkOnDefine( char* buf, int& idx );


#endif