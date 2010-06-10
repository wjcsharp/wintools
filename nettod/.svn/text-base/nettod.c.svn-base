#include <stdio.h>
#include <windows.h>
#include <lm.h>
#pragma hdrstop


static const char *dow[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char *month[] = { "???", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };


int main( int argc, char *argv[] )
{
	wchar_t server[256] = L"";
	TIME_OF_DAY_INFO *p = NULL;
	DWORD rc;

	if ( argc > 2 )
	{
		puts( "usage: nrt [servername]" );
		return 1;
	}

	if ( argc > 1 )
		mbstowcs( server, argv[1], strlen( argv[1] ) + 1 );

	rc = NetRemoteTOD( server, (LPBYTE *) &p );
	if ( rc != NERR_Success )
	{
		printf( "error %d\n", rc );
		return 2;
	}

	printf( "Unix time_t:     %lu (since 1/1/70 00:00:00 UTC)\n", p->tod_elapsedt );
	printf( "Milliseconds:    %lu (since boot)\n", p->tod_msecs );
	printf( "Current time:    %02lu:%02lu:%02lu.%02lu (UTC)\n", p->tod_hours, p->tod_mins, p->tod_secs, p->tod_hunds );
	printf( "Timezone offset: %ld (minutes from GMT)\n", p->tod_timezone );
	printf( "Tick length:     %lu (microseconds)\n", p->tod_tinterval * 100 );
	printf( "Current date:    %s, %lu %s %lu\n", dow[p->tod_weekday], p->tod_day, month[p->tod_month], p->tod_year );

	rc = NetApiBufferFree( p ); // no docs -- but I think it's necessary

	return 0;
}

