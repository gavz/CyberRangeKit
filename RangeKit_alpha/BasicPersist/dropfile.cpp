#include "stdafx.h"

BOOL ToolDropFile(WCHAR *theFilePath)
{
	HANDLE hFile;

	xprintf(OPLOG_VERBOSE, L"> Dropping file to full path %ws\n", theFilePath);

	xprintf(OPLOG_DEBUG, L"~ Attempting to create file\n");

	hFile = CreateFile(
		theFilePath,
		GENERIC_READ | GENERIC_WRITE,	// access token rights (us -> file)
		FILE_SHARE_READ,				// share mode
		NULL,							// security attributes
		CREATE_NEW,						// if the file is already there we will fail out
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		xprintf(OPLOG_NORMAL, L"! Error attempting to drop file. ERROR: %08X\n", GetLastError());
		return FALSE;
	}

	// @@ optionally write signature-triggers to file here

	xprintf(OPLOG_DEBUG, L"~ Attempting to close file handle\n");

	if (CloseHandle(hFile) == 0)
	{
		xprintf(OPLOG_NORMAL, L"! Error attempting to close file handle. ERROR: %08X\n", GetLastError());
		return FALSE;
	}

	xprintf(OPLOG_DEBUG, L"~ File operations completed successfully\n");

	return TRUE;
}

BOOL ToolCleanFile(WCHAR *theFilePath)
{
	BOOL bResult;

	xprintf(OPLOG_VERBOSE, L"> Removing file from full path %ws\n", theFilePath);

	xprintf(OPLOG_DEBUG, L"~ Attempting to delete file\n");

	bResult = DeleteFile(theFilePath);

	if (FALSE == bResult)
	{
		xprintf(OPLOG_NORMAL, L"! File was not removed! [WARNING - FILE ARTIFACT IS STILL PRESENT] Error:%08X\n", GetLastError());
		return FALSE;
	}

	return TRUE;
}