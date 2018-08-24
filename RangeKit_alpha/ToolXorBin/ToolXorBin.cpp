#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#define BITE_SIZE 1024

int wmain(int argc, WCHAR* argv[])
{		
	HANDLE hFileIn, hFileOut;
	WCHAR  InputFilePath[MAX_PATH];
	WCHAR  OutputFilePath[MAX_PATH];

	if (argc != 2)
	{
		printf("You must supply a filename\n");
		return EXIT_FAILURE;
	}

	_snwprintf_s(InputFilePath, MAX_PATH - 2, _TRUNCATE, L"%ws", argv[1]);
	_snwprintf_s(OutputFilePath, MAX_PATH - 2, _TRUNCATE, L"%ws.encoded", argv[1]);

	hFileIn = CreateFileW(
		InputFilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFileIn)
	{
		printf("! error opening input file. Code %08X", GetLastError());
		return EXIT_FAILURE;
	}

	hFileOut = CreateFileW(
		OutputFilePath,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFileOut)
	{
		printf("! error opening output file. Code %08X", GetLastError());
		CloseHandle(hFileIn);
		return EXIT_FAILURE;
	}

	BYTE pacman[BITE_SIZE];
	DWORD dwRead = 0;
	DWORD dwWritten = 0;

	do
	{
		if (FALSE == ReadFile(hFileIn, pacman, BITE_SIZE, &dwRead, NULL))
		{
			printf("! read error %08X", GetLastError());
			break;
		}

		if (0 == dwRead)
		{
			// end of file
			break;
		}

		// do someting to buffer
		for (DWORD i = 0; i < dwRead; i++)
		{
			BYTE c = pacman[i];
			c = c ^ 0x80;			// pick an XOR byte here
			pacman[i] = c;
		}

		DWORD totalWritten = 0;
		while (totalWritten < dwRead)
		{
			if (FALSE == WriteFile(
							hFileOut, 
							(pacman + totalWritten), 
							(dwRead - totalWritten), 
							&dwWritten, 
							NULL))
			{
				printf("! write error %08X", GetLastError());
				break;
			}
			totalWritten += dwWritten;
		}
	} while (BITE_SIZE == dwRead);
	
	CloseHandle(hFileIn);
	CloseHandle(hFileOut);

	return 0;
}

