#include "stdafx.h"
#include "RangeLib.h"

#define BITE_SIZE 1024

BOOL ToolDropFileEmpty(WCHAR *theFilePath)
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

BOOL ToolDropFileFromResource(WCHAR *theFilePath, LPCWSTR theResourceID)
{
	DWORD dwSize;
	DWORD dwWritten;

	HANDLE hFile;
	HRSRC hResource;
	HGLOBAL hLoaded;
	LPBYTE lpResourceData;

	//hResource = FindResourceW(GetModuleHandle(NULL), MAKEINTRESOURCE(theResourceID), L"BINARY");
	hResource = FindResourceW(GetModuleHandle(NULL), theResourceID, L"BINARY");
	if (NULL == hResource)
	{
		xprintf(OPLOG_NORMAL, L"! Could not find decoy resource for unpacking. Error:%08X\n", GetLastError());
		return FALSE;
	}

	dwSize = SizeofResource(GetModuleHandle(NULL), hResource);
	if (0 == dwSize)
	{
		xprintf(OPLOG_NORMAL, L"! Could not get size of decoy resource for unpacking. Error:%08X\n", GetLastError());
		return FALSE;
	}

	hLoaded = LoadResource(NULL, hResource);
	if (NULL == hLoaded)
	{
		xprintf(OPLOG_NORMAL, L"! Could not load decoy resource for unpacking. Error:%08X\n", GetLastError());
		return FALSE;
	}

	// no need to unlock per msdn
	lpResourceData = (LPBYTE)LockResource(hLoaded);
	if (NULL == lpResourceData)
	{
		xprintf(OPLOG_NORMAL, L"! Could not lock decoy resource for unpacking. Error:%08X\n", GetLastError());
		return FALSE;
	}

	hFile = CreateFileW(
		theFilePath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
	{
		xprintf(OPLOG_NORMAL, L"! Could not create output file for decoy unpacking. Error:%08X\n", GetLastError());
		return FALSE;
	}

	// chunk out the resource, de-XOR it, and write to target file
	DWORD curr = 0;
	while (curr < dwSize)
	{
		BYTE pacman[BITE_SIZE];
		DWORD to_read = 0;

		if ((dwSize - curr) < BITE_SIZE) to_read = (dwSize - curr);
		else to_read = BITE_SIZE;

		CopyMemory(pacman, lpResourceData + curr, to_read); // assumes that entire buffer is always copied

		// do someting to buffer
		for (DWORD i = 0; i < to_read; i++)
		{
			BYTE c = pacman[i];
			c = c ^ 0x80;			// pick an XOR byte here
			pacman[i] = c;
		}

		DWORD totalWritten = 0;
		while (totalWritten < to_read)
		{
			if (FALSE == WriteFile(
				hFile,
				(pacman + totalWritten),
				(to_read - totalWritten),
				&dwWritten,
				NULL))
			{
				xprintf(OPLOG_NORMAL, L"! Could not write output file for decoy unpacking. Error:%08X\n", GetLastError());
				CloseHandle(hFile);
				return FALSE;
			}
			totalWritten += dwWritten;
		}
		
		curr += to_read;
	}

	CloseHandle(hFile);
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


