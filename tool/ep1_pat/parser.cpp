
#include "common.h"




#define SkipLine( _buf, _index ) \
	while( _buf[_index] != '\n' ) _index++; \
	_index++

#define SkipWhitespaces( _buf, _index ) \
	while( (_buf[_index] == ' ') || (_buf[_index] == '\t') ) _index++

u32	utilGetFileSize(FILE *f)
{
	u32	 sz;
	fseek(f, 0, SEEK_END);
	sz = ftell(f);
	fseek(f, 0, SEEK_SET);
	return sz;
}


u32 parsePatFile(char *path)
{
	PARSED_DATA_T		pdata;
	FILE				*f;
	u32					filesize, result;
	char				*buf;
	int					i = 0;


	f = fopen(path, "rb");
	if( f == NULL )
		return PARSE_FILE_NOT_EXIST;

	
	filesize = utilGetFileSize(f);
	if ( filesize == 0 )
	{
		fclose(f);
		return PARSE_FILE_EMPTY;
	}

	buf = (char*)malloc( filesize+2 );
	if ( buf == 0 )
	{
		fclose(f);
		return PARSE_NO_MEMORY;
	}

	result = fread(buf, sizeof(char), filesize, f);
	if ( result < filesize )
	{
		fclose(f);
		return PARSE_FILE_READ_ERROR;
	}
	
	fclose(f);

	buf[filesize] = '\n';
	buf[filesize+1] = PARSE_EOF_MARKER;

	do
	{
		result = parseLine( buf, &i, &pdata );
		if(result == PARSE_DONE)
		{
			addPattern(&pdata);
		}
	} while( result != PARSE_EOF );
	

	free( buf );

	return PARSE_SUCCESS;
}

BOOL isInSet(char c, char *set)
{
	if(set == NULL)
		return FALSE;

	while(*set != '\0')
	{
		if(c == *set)
			return TRUE;

		set++;
	}

	return FALSE;
}

void WaitForSet(char *set, char *buf, int &index)
{
	while( !isInSet(buf[index], set) )
		index++;
}

PARSER_RESULT_T  parseLine( char *buf, int *index, PARSED_DATA_T *parsed )
{
	int			i = *index;
	int			j = 0, k = 0;
	char		patMode, cbuf;
	char		*patName, *patText;
	s16			patOff[2] = {0,0};
	u16			patNmatch;
	BOOL		patDoLoad = false;

	int			status = 0, nOff = 1, textStart, textEnd;


	if ( buf[i] == PARSE_EOF_MARKER )
		return PARSE_EOF;

	SkipWhitespaces(buf, i);

	if ( (buf[i] == '#') || (buf[i] == '\r') || (buf[i] == '\n') )
	{
		SkipLine( buf, i );
		*index = i;
		return PARSE_SKIP; 
	}


	// Name

	patName = &buf[i];

	WaitForSet(" \t\r\n", buf, i);

	buf[i++] = '\0';

	SkipWhitespaces(buf, i);


	// Mode

	if(buf[i] != 'T' && buf[i] != 'A' && buf[i] != 'D')
	{
		SkipLine( buf, i );
		*index = i;
		return PARSE_SKIP;
	}

	patMode = buf[i++];

	SkipWhitespaces(buf, i);



	// Nr of occurences [optional]

	j = i;

	WaitForSet(" \t\r\n", buf, i);

	if( (i-j) < 4 )
	{
		buf[i++] = '\0';

		patNmatch = (u16)strtol( &buf[j], 0, 10 );

		SkipWhitespaces(buf, i);
	}
	else
	{
		i = j;
		patNmatch = 0;
	}


	// Text

	if( buf[i] == '[' ) // With Load
	{
		patDoLoad = true;
		nOff = 0;
		i++;
	}

	textStart = i;
	WaitForSet("+-] \t\r\n", buf, i);

	textEnd = i;

	while(status == 0)
	{

		switch(buf[i])
		{
			case ']':
				i++;
			break;

			case '+':
			case '-':
				j = i;

				WaitForSet("] \t\r\n", buf, i);

				cbuf = buf[i];
				buf[i] = '\0';

				patOff[nOff++] = (s16)strtol( &buf[j], 0, 16 );
				
				buf[i] = cbuf;
				i++;
			break;

			case ' ':
			case '\t':
				SkipWhitespaces(buf, i);
			break;

			case '\r':
			case '\n':
				status = 1;
			break;
		}

	}

	buf[textEnd] = '\0';
	patText = &buf[textStart];

	parsed->name = patName;
	parsed->text = patText;
	parsed->mode = patMode;
	parsed->nmatch = patNmatch;
	parsed->load = patDoLoad;
	parsed->offs[0] = patOff[0];
	parsed->offs[1] = patOff[1];

	*index = i+1;

	return PARSE_DONE;
}