
#include "stdafx.h"
#include "generate.h"

BOOL GenerateFileName(DWORD key, WCHAR *outFileName)
{
	_snwprintf_s(outFileName, MAX_PATH - 2, _TRUNCATE, L"%ws%08X.bin", L"testfile", key);
	return TRUE;
}

BOOL GenerateServiceName(DWORD key, WCHAR *outFileName)
{
	_snwprintf_s(outFileName, MAX_PATH - 2, _TRUNCATE, L"%ws%08X", L"SomeServiceName", key);
	return TRUE;
}

BOOL GenerateServiceDisplayName(DWORD key, WCHAR *outFileName)
{
	_snwprintf_s(outFileName, MAX_PATH - 2, _TRUNCATE, L"%ws%08X", L"SomeServiceDisplayName", key);
	return TRUE;
}

BOOL GenerateFilePath(PROG_FILEMODE theMode, WCHAR *theFilename, WCHAR *outFilePath)
{
	WCHAR szWorkingDirectory[MAX_PATH];
	WCHAR *envName = L"CD";
	HANDLE hFile = 0;

	if (theMode == PROG_FILEMODE::OPF_CWD)
	{
		xprintf(OPLOG_DEBUG, L"~ Attempting to get current working directory\n");

		if (GetCurrentDirectory(MAX_PATH - 2, szWorkingDirectory) == 0)
		{
			xprintf(OPLOG_NORMAL, L"! Could not get current working directory. Error:%08X\n", GetLastError());
			return FALSE;
		}
	}
	else
	{
		switch (theMode)
		{
		case PROG_FILEMODE::OPF_APPDATA:
			envName = L"APPDATA";
			break;
		case PROG_FILEMODE::OPF_WINDOWS:
			envName = L"WINDIR";
			break;
		}

		xprintf(OPLOG_DEBUG, L"~ Attempting to get environment variable %ws\n", envName);

		if (GetEnvironmentVariable(envName, szWorkingDirectory, MAX_PATH) == 0)
		{
			xprintf(OPLOG_NORMAL, L"! Could not get environment variable. Error:%08X\n", GetLastError());
			return FALSE;
		}
	}

	_snwprintf_s(outFilePath, MAX_PATH - 2, _TRUNCATE, L"%ws\\%ws", szWorkingDirectory, theFilename);

	xprintf(OPLOG_VERBOSE, L"> Generated full file path %ws\n", outFilePath);

	return TRUE;
}

BOOL GenerateRegValueNameForExe(DWORD key, WCHAR *outValueName)
{
	_snwprintf_s(outValueName, MAX_PATH - 2, _TRUNCATE, L"%ws", L"test_decoy");
	return TRUE;
}