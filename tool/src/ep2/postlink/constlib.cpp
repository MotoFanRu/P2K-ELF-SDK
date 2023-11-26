
#include "constlib.h"

char*		constNamesStrTbl;
int			constNamesStrIdx;
Ldr_Sym*	constNamesSymTbl;
UINT32		constNamesCount;

bool		commentFlag;


/*char			*strtab;
UINT32		stridx, ldrapidx;
Elf32_Sym	*symtab;
UINT32		symcnt;*/

void parseCHeader( char *name )
{
	int		i = 0;
	char	*file;

	file = initConstlib(name);


	for(;;)
	{
		parseCHeaderLine(file, i);
		skipLine(file, i);
		if(file[i]==1)
			break;
	}



	free(file);
	//free(constNamesSymTbl);
	//free(constNamesStrTbl);

}

char* initConstlib( char *name )
{
	FILE	*f;
	int		lineCnt = 0;
	char	*file;
	UINT32	fSize;
	int		i = 0;

	f = fopen(name, "rb");
	fSize = getFileSize(f);

	file = (char*)malloc(fSize*sizeof(char)+2);
	fread(file, fSize, 1, f);
	fclose(f);
	file[fSize] = '\0';
	file[fSize+1] = 1;

	// Подсчитаем и отформатируем строки
	while( file[i] != 1 )
	{
		if( (file[i] == '\n') || (file[i] == '\r') )
		{
			lineCnt++;
			file[i] = 0;
		}

		i++;
	}

	constNamesSymTbl = (Ldr_Sym*)malloc(lineCnt*sizeof(Ldr_Sym));
	constNamesStrTbl = (char*)malloc(fSize*sizeof(char));

	constNamesStrIdx = constNamesCount = 0;

	commentFlag = false;

	return file;
}

int parseCHeaderLine( char* buf, int& idx )
{
	int stridx, val, newidx = constNamesStrIdx;

	if( skipWhitespacesAndComments(buf, idx) == -1)
		return -1;

	if(checkOnDefine(buf, idx) == -1)
		return -1;

	if( skipWhitespacesAndComments(buf, idx) == -1)
		return -1;

	stridx = readConstName(buf, idx, constNamesStrTbl, newidx);
	if( stridx == -1)
		return -1;

	if( skipWhitespacesAndComments(buf, idx) == -1)
		return -1;

	val = readConstVal(buf, idx );
	if( val == -1)
		return -1;
	
	constNamesStrIdx = newidx;
	constNamesSymTbl[constNamesCount].st_value = val;
	constNamesSymTbl[constNamesCount].st_name = stridx;

	printf("%s %d\n", &constNamesStrTbl[constNamesSymTbl[constNamesCount].st_name], constNamesSymTbl[constNamesCount].st_value );

	constNamesCount++;

	return idx;
}

int skipWhitespacesAndComments( char* buf, int& idx )
{
	
	if( skipWhitespaces(buf, idx) == -1 )
		return -1;

	if( commentFlag ) //a comment was started earlier
	{
		return skipForCommentEnd(buf, idx);
	}

	if( buf[idx] == '/' ) //may be a comment
	{
		idx++;
		if( buf[idx] == '/' ) //full line comment
		{
			//skipLine(buf, ++idx);
			return -1;
		}

		if( buf[idx] == '*' ) //start of a comment
		{
			return skipForCommentEnd(buf, ++idx);
		}
	}

	return idx;

}

int skipForCommentEnd( char* buf, int& idx )
{

	for(;;)
	{
//		idx++;
		if( skipForSymbol(buf, idx, '*') == -1 )
		{
			commentFlag = true;
			return -1;
		}
		if( buf[idx+1] == '/' ) //end of a comment found
		{
			idx+=2;
			commentFlag = false;
			skipWhitespaces(buf, idx);
			return idx;
		}
	}
}

int skipForSymbol( char* buf, int& idx, char c )
{
	while( (buf[idx]!='\0') && (buf[idx]!=c) ) idx++;
	if( buf[idx] == '\0' )
		return -1;

	return idx;
}


int skipWhitespaces( char* buf, int& idx )
{
	while( ((buf[idx]==' ') || (buf[idx]=='\t')) && (buf[idx]!='\0') ) idx++;
	if( buf[idx]=='\0' )
		return -1;

	return idx;
}

int skipLine( char* buf, int& idx )
{
	for(;;)
	{
		while( (buf[idx]!='\0') && (buf[idx]!='/') ) idx++;
		if( (buf[idx]=='/') && (buf[idx+1]=='*') )
		{
			commentFlag = true;
		}
		else if( (buf[idx]=='/') && (buf[idx-1]=='*') )
		{
			commentFlag = false;
		}
		else if( buf[idx]=='\0' )
			return ++idx;
		idx++;
	}
}


int readConstName( char* buf, int& idx, char* dest, int& dstidx )
{
	int len = 0;
	int oldidx = dstidx;
	int curidx = dstidx;

	while( (buf[idx]!=' ') && (buf[idx]!='\t') && (buf[idx]!='/') && (buf[idx]!='\0') )
	{
		dest[curidx++] = buf[idx++];
		len++;
	}
	
	if( buf[idx]=='\0' )
		return -1;

	dest[curidx++] = '\0';
	dstidx = curidx;

	return oldidx;
}


int readConstVal( char* buf, int& idx )
{
	int i = 0;
	char val[32];
	int v;

	while( (buf[idx]!=' ') && (buf[idx]!='\t') && (buf[idx]!='/') && (buf[idx]!='\0') && isHexDigit(buf[idx]) )
	{
		val[i++] = buf[idx++];
	}

	/*if( buf[idx]=='\0' )
		return -1;*/

	val[i++] = '\0';

	if( (val[1] == 'x') || (val[1] == 'X') )
		v = strtoul(val, NULL, 16);
	else
		v = strtoul(val, NULL, 10);


	return v;
}

bool isHexDigit( char c )
{
	return ( (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F') || (c=='x') || (c=='X') );
}

int checkOnDefine( char* buf, int& idx )
{
	int i = 0;
	char buffer[64];

	if( readConstName(buf, idx, buffer, i) == -1 )
		return -1;

	if( strcmp(buffer, "#define") != 0 )
		return -1;

	return i;

}
