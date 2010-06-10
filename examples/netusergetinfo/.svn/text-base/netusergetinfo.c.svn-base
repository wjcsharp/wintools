// compile with:
//		"cl -DUNICODE nugi.cpp" for a Unicode app
//		"cl nugi.cpp" for an MBCS app

// make sure that either both _UNICODE and UNICODE are defined, or none of them is
#if defined( _UNICODE ) && ! defined( UNICODE )
#define UNICODE 1
#endif
#if defined( UNICODE ) && ! defined( _UNICODE )
#define _UNICODE 1
#endif

#include <windows.h>
#include <lm.h>
#include <stdio.h>
#include <time.h>
#include <tchar.h>
#pragma hdrstop
#pragma comment( lib, "netapi32.lib" )



// in Unicode apps, _tprintf() maps to wprintf(), and "%s" refers to
// a Unicode string, while "%S" wants an ANSI string. In MBCS apps,
// "%s" expects an ANSI string, while "%S" wants a Unicode string. We
// could do some creative trickery with macros, but I am lazy, so I'll
// rely on "%hs" and "%ls" instead. Independent of whether printf() or
// wprintf() is ultimately invokes, "%hs" always expects an ANSI string,
// and "%ls" always expects a Unicode string.
// Since Net*() APIs on NT deal in Unicode only, the strings from the
// USER_INFO_3 struct get shown with "%ls" (but note that strings from
// elsewhere are in the ambient character size and use plain "%s").



#define lenof(a) (sizeof(a)/sizeof((a)[0]))



typedef struct {
	DWORD bits;
	const TCHAR *name;
} flag_t;



const TCHAR *getText( DWORD num, flag_t *defs )
{
	static TCHAR s[60];

	for ( ; defs->name != 0; ++ defs )
	{
		if ( defs->bits == num )
			return defs->name;
	}

	_stprintf( s, _T( "%lu [0x%lx] (unknown)" ), num, num );
	return &s[0];
}



const TCHAR *buildFlags( DWORD bits, flag_t *defs )
{
	static TCHAR s[4096];
	TCHAR *p = &s[0];

	p += _stprintf( p, _T( "0x%lx" ), bits );

	for ( ; defs->name != 0; ++ defs )
	{
		if ( ( defs->bits & bits ) == defs->bits )
		{
			if ( p != &s[0] )
			{
				_tcscpy( p, _T( ", " ) );
				p += _tcslen( p );
			}
			_tcscpy( p, defs->name );
			p += _tcslen( p );
		}
	}

	return &s[0];
}



int nugi( TCHAR *machine, TCHAR *user )
{
	DWORD rc;
	USER_INFO_3 *buf = 0;
	TCHAR temp[40];
	wchar_t unicodeMachine[UNCLEN + 4], unicodeUser[UNLEN + 2];
	static flag_t privilegeTexts[] =
	{
		{ USER_PRIV_GUEST, _T( "USER_PRIV_GUEST" ) },
		{ USER_PRIV_USER, _T( "USER_PRIV_USER" ) },
		{ USER_PRIV_ADMIN, _T( "USER_PRIV_ADMIN" ) },
		{ 0, 0 }
	};
	static flag_t authFlags[] =
	{
		{ AF_OP_PRINT, _T( "AF_OP_PRINT" ) },
		{ AF_OP_COMM, _T( "AF_OP_COMM" ) },
		{ AF_OP_SERVER, _T( "AF_OP_SERVER" ) },
		{ AF_OP_ACCOUNTS, _T( "AF_OP_ACCOUNTS" ) },
		{ 0, 0 }
	};
	static flag_t acctFlags[] =
	{
		{ UF_SCRIPT, _T( "UF_SCRIPT" ) },
		{ UF_ACCOUNTDISABLE, _T( "UF_ACCOUNTDISABLE" ) },
		{ UF_HOMEDIR_REQUIRED, _T( "UF_HOMEDIR_REQUIRED" ) },
		{ UF_PASSWD_NOTREQD, _T( "UF_PASSWD_NOTREQD" ) },
		{ UF_PASSWD_CANT_CHANGE, _T( "UF_PASSWD_CANT_CHANGE" ) },
		{ UF_LOCKOUT, _T( "UF_LOCKOUT" ) },
		{ UF_DONT_EXPIRE_PASSWD, _T( "UF_DONT_EXPIRE_PASSWD" ) },
#ifdef UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED
		{ UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED, _T( "UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED" ) },
#endif
#ifdef UF_NOT_DELEGATED
		{ UF_NOT_DELEGATED, _T( "UF_NOT_DELEGATED" ) },
#endif
#ifdef UF_SMARTCARD_REQUIRED
		{ UF_SMARTCARD_REQUIRED, _T( "UF_SMARTCARD_REQUIRED" ) },
#endif
		{ UF_NORMAL_ACCOUNT, _T( "UF_NORMAL_ACCOUNT" ) },
		{ UF_TEMP_DUPLICATE_ACCOUNT, _T( "UF_TEMP_DUPLICATE_ACCOUNT" ) },
		{ UF_WORKSTATION_TRUST_ACCOUNT, _T( "UF_WORKSTATION_TRUST_ACCOUNT" ) },
		{ UF_SERVER_TRUST_ACCOUNT, _T( "UF_SERVER_TRUST_ACCOUNT" ) },
		{ UF_INTERDOMAIN_TRUST_ACCOUNT, _T( "UF_INTERDOMAIN_TRUST_ACCOUNT" ) },
		{ 0, 0 }
	};

#ifdef UNICODE
	_tcsncpy( unicodeMachine, machine, lenof( unicodeMachine ) );
	_tcsncpy( unicodeUser, user, lenof( unicodeUser ) );
#else
	mbstowcs( unicodeMachine, machine, lenof( unicodeMachine ) );
	mbstowcs( unicodeUser, user, lenof( unicodeUser ) );
#endif
	unicodeMachine[lenof( unicodeMachine ) - 1] = _T( '\0' );
	unicodeUser[lenof( unicodeUser ) - 1] = _T( '\0' );

	_tprintf( _T( "User %s on %s: " ), user, machine );

	rc = NetUserGetInfo( unicodeMachine, unicodeUser, 3, (byte **) &buf );
	if ( rc != 0 )
	{
		_tprintf( _T( "error %lu.\n" ), rc );
		return rc;
	}

	_tprintf( _T( "found.\n" ) );

	_tprintf( _T( "  Name:          %ls\n" ), buf->usri3_name );
	_tprintf( _T( "  Password:      %ls\n" ), buf->usri3_password );
	_tprintf( _T( "  Password age:  %lu days, %02lu:%02lu:%02lu\n" ), buf->usri3_password_age / 86400UL,
		buf->usri3_password_age % 86400UL / 3600UL, buf->usri3_password_age % 3600UL / 60UL,
		buf->usri3_password_age % 60UL );
	_tprintf( _T( "  Privilege:     %s\n" ), getText( buf->usri3_priv, privilegeTexts ) );
	_tprintf( _T( "  Home dir:      %ls\n" ), buf->usri3_home_dir );
	_tprintf( _T( "  Comment:       %ls\n" ), buf->usri3_comment );
	_tprintf( _T( "  Flags:         %s\n" ), buildFlags( buf->usri3_flags, acctFlags ) );
	_tprintf( _T( "  Script path:   %ls\n" ), buf->usri3_script_path );
	_tprintf( _T( "  Auth flags:    %s\n" ), buildFlags( buf->usri3_auth_flags, authFlags ) );
	_tprintf( _T( "  Full name:     %ls\n" ), buf->usri3_full_name );
	_tprintf( _T( "  User comment:  %ls\n" ), buf->usri3_usr_comment );
	_tprintf( _T( "  RAS params:    (not displayed)\n" ) );
	_tprintf( _T( "  Workstations:  %ls\n" ), buf->usri3_workstations );
	_tprintf( _T( "  Last logon:    %s" ), buf->usri3_last_logon == 0? _T( "never\n" ): _tctime( (long *) &buf->usri3_last_logon ) );
	_tprintf( _T( "  Last logoff:   %s" ), buf->usri3_last_logoff == 0? _T( "unknown\n" ): _tctime( (long *) &buf->usri3_last_logoff ) );
	_tprintf( _T( "  Expires:       %s" ), buf->usri3_acct_expires == TIMEQ_FOREVER? _T( "never\n" ): _tctime( (long *) &buf->usri3_acct_expires ) );
	_tprintf( _T( "  Max storage:   %s\n" ), buf->usri3_max_storage == USER_MAXSTORAGE_UNLIMITED? _T( "unlimited" ): _ultot( buf->usri3_max_storage, temp, 10 ) );
	_tprintf( _T( "  Logon hours:   (not displayed)\n" ) );
	_tprintf( _T( "  Bad PW count:  %s\n" ), buf->usri3_bad_pw_count == (DWORD) -1L? _T( "unlimited" ): _ultot( buf->usri3_bad_pw_count, temp, 10 ) );
	_tprintf( _T( "  # of logons:   %s\n" ), buf->usri3_num_logons == (DWORD) -1L? _T( "unknown" ): _ultot( buf->usri3_num_logons, temp, 10 ) );
	_tprintf( _T( "  Logon server:  %ls\n" ), buf->usri3_logon_server );
	_tprintf( _T( "  Country code:  %lu\n" ), buf->usri3_country_code );
	_tprintf( _T( "  Code page:     %lu\n" ), buf->usri3_code_page );
	_tprintf( _T( "  User RID:      %lu\n" ), buf->usri3_user_id );
	_tprintf( _T( "  Pgroup RID:    %lu\n" ), buf->usri3_primary_group_id );
	_tprintf( _T( "  Profile path:  %ls\n" ), buf->usri3_profile );
	_tprintf( _T( "  Home drive:    %ls\n" ), buf->usri3_home_dir_drive );
	_tprintf( _T( "  PW expired:    %s\n" ), buf->usri3_password_expired? _T( "yes" ): _T( "no" ) );
	_tprintf( _T( "\n" ) );

	return 0;
}



#ifdef UNICODE
int wmain( int argc, TCHAR *argv[] )
#else
int main( int argc, TCHAR *argv[] )
#endif
{
	TCHAR *machine = 0;
	int i;

	if ( argc < 2 )
	{
		_tprintf( _T( "usage: nugi [\\\\machine] user [user ...] [[\\\\machine] user [user ...] ...]\n" ) );
		return 1;
	}

	for ( i = 1; i < argc; ++ i )
	{
		if ( argv[i][0] == _T( '\\' ) && argv[i][1] == _T( '\\' ) )
		{
			machine = argv[i];
			continue;
		}

		nugi( machine, argv[i] );
	}

	return 0;
}

