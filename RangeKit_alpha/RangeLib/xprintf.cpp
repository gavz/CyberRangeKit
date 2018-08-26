
#include "stdafx.h"
#include <stdarg.h>
#include "RangeLib.h"

CRITICAL_SECTION gConsoleCriticalSection; // for pretty printing
HANDLE gConsole = NULL;
CONSOLE_SCREEN_BUFFER_INFO gConInfo;

void xprint_init()
{
	// gConsole as NULL serves as sentinel for initialization
	if (NULL == gConsole)
	{
		gConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(gConsole, &gConInfo);
		InitializeCriticalSection(&gConsoleCriticalSection);
	}
}

void xprint_console(CONST WCHAR control_char, WCHAR *pString)
{
	// the critical section ensures we don't stomp console messages when multi-threaded
	EnterCriticalSection(&gConsoleCriticalSection);

	WORD old_background = gConInfo.wAttributes & 0x00F0;
	WORD old_foreground = gConInfo.wAttributes & 0x000F;

	if (g_progopts.pretty_print)
	{
		switch (control_char)
		{
		case L'[':
			// normal tool state-change updates
			SetConsoleTextAttribute(gConsole, old_background | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case L'!':
			// error
			SetConsoleTextAttribute(gConsole, old_background | FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case L'@':
			// events
			// SetConsoleTextAttribute(gConsole, 0x09);
			SetConsoleTextAttribute(gConsole, old_background | FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
			break;
		case L'#':
			// banners, important prompts to user
			SetConsoleTextAttribute(gConsole, old_background | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		case L'$':
			// commentary
			SetConsoleTextAttribute(gConsole, 0x0F);
			break;
		case L'>':
			// verbose running output...
			SetConsoleTextAttribute(gConsole, old_background | FOREGROUND_GREEN);
			
			break;
		case L'~':
			// very verbose / debug logging
			SetConsoleTextAttribute(gConsole, old_background | 0x08);
			break;
		default:
			SetConsoleTextAttribute(gConsole, 0x08); // 0,7,8,15 => shades of grey
			break;
		}
	}

	wprintf(pString);

	if (g_progopts.pretty_print)
	{
		SetConsoleTextAttribute(gConsole, gConInfo.wAttributes); // put back original
	}

	LeaveCriticalSection(&gConsoleCriticalSection);
}

void xputchar(PROG_LOGMODE theLogMode, CONST WCHAR control_char, CONST WCHAR pChar)
{
	WCHAR aString[2];
	xprint_init();
	
	if (theLogMode <= g_progopts.log_mode)
	{
		aString[0] = pChar;
		aString[1] = L'\0';
		xprint_console(control_char, aString);
	}
}

void __cdecl xprintf(PROG_LOGMODE theLogMode, CONST WCHAR *wfmt, ...)
{
	va_list argp;
	va_start(argp, wfmt);

	xprint_init();

	if (theLogMode <= g_progopts.log_mode)
	{
		//if you just want to print to console you can uncomment this
		//vwprintf(wfmt, argp);
	
		WCHAR _tw[1024];
		_vsnwprintf_s(_tw, 1022, _TRUNCATE, wfmt, argp); // pad end to be overly cautious

		xprint_console(_tw[0], _tw);
	}

	va_end(argp);
}

/*
FILE *gLogFile = NULL;
HANDLE gConsole = NULL;
CONSOLE_SCREEN_BUFFER_INFO gConInfo;
CRITICAL_SECTION gConsoleCriticalSection;

void __cdecl logprintf(
	const char *format,
	...
)
{
	va_list arglist;
	va_start(arglist, format);

	char _t[1024];
	_vsnprintf_s(_t, 1022, _TRUNCATE, format, arglist);

	char _tt[1024];
	_snprintf_s(_tt, 1022, _TRUNCATE, "%s", _t);

	//OutputDebugString(_t);

	if (NULL == gConsole)
	{
		gConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(gConsole, &gConInfo);
		InitializeCriticalSection(&gConsoleCriticalSection);
	}

	EnterCriticalSection(&gConsoleCriticalSection);
	switch (_tt[0])
	{
	case '[':
		SetConsoleTextAttribute(gConsole, BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	case '!':
		SetConsoleTextAttribute(gConsole, 0x0C);
		break;
	case '@':
		SetConsoleTextAttribute(gConsole, 0x09);
		break;
	case '#':
		SetConsoleTextAttribute(gConsole, 0x0A);
		break;
	case '$':
		SetConsoleTextAttribute(gConsole, 0x0F);
		break;
	case '>':
		SetConsoleTextAttribute(gConsole, FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		break;
	default:
		SetConsoleTextAttribute(gConsole, 0x08); // 0,7,8,15 => shades of grey
		break;
	}

	printf(_tt);
	SetConsoleTextAttribute(gConsole, gConInfo.wAttributes);
	printf("\n");
	LeaveCriticalSection(&gConsoleCriticalSection);

	if (NULL == gLogFile)
	{
#ifdef _DEBUG
		// changed this to destroy old file first
		fopen_s(&gLogFile, "C:\\WORLDSERVER.LOG", "a");
#endif
	}

	if (gLogFile)
	{
		SYSTEMTIME stime;
		GetLocalTime(&stime);
		fprintf(gLogFile, "[ %d ][ %d %d %d %d ] %s \r\n", GetCurrentThreadId(),
			stime.wHour,
			stime.wMinute,
			stime.wSecond,
			stime.wMilliseconds,
			_t);
	}
}
*/