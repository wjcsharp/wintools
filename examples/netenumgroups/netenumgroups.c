#include <windows.h>
#include <lm.h>
#include <stdio.h>
#include <stdlib.h>
#pragma hdrstop

#define MAXLEN 256

int main( int argc, char *argv[] )
{
	LOCALGROUP_INFO_1 *buf, *cur;
	DWORD read, total, resumeh, rc, i;
	wchar_t server[MAXLEN];

	if ( argc != 2 )
	{
		puts( "usage: nlge \\\\server" );
		return 1;
	}

	mbstowcs( server, argv[1], MAXLEN );

	resumeh = 0;
	do
	{
		buf = NULL;
		rc = NetLocalGroupEnum( server, 1, (BYTE **) &buf, 512, &read, &total, &resumeh );
		
		if ( rc != ERROR_MORE_DATA && rc != ERROR_SUCCESS )
		{
			break;
		}

		printf( "\ngot %lu entries out of %lu remaining\n", read, total );
		printf( "%-20.20s %.58s\n", "local group name", "comment" );
		printf( "%-20.20s %-58.58s\n", "--------------------",
			"----------------------------------------------------------" );

		for ( i = 0, cur = buf; i < read; ++ i, ++ cur )
		{
			// Note: the capital S in the format string will expect Unicode
			// strings, as this is a program written/compiled for ANSI.
			printf( "%-20.20S %.58S\n", cur->lgrpi1_name, cur->lgrpi1_comment );
		}

		if ( buf != NULL )
			NetApiBufferFree( buf );

	} while ( rc == ERROR_MORE_DATA );

	if ( rc != ERROR_SUCCESS )
		printf( "NLGE() returned %lu\n", rc );

	return 0;
}