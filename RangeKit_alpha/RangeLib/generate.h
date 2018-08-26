#pragma once

// these functions generate deterministic values that are repeatable and consistent
// this allows for keys to be passed downstream for subsequent generation, as opposed to temporal values or strings  
// aka "downstream positive control"

BOOL GenerateLogName(DWORD key, WCHAR *outFileName);
BOOL GenerateFileName(DWORD key, WCHAR *outFileName);
BOOL GenerateFilePath(PROG_FILEMODE theMode, WCHAR *theFilename, WCHAR *outFilePath, WCHAR *outWorkingDirectory = NULL);
BOOL GenerateRegValueNameForExe(DWORD key, WCHAR *outValueName);
BOOL GenerateServiceName(DWORD key, WCHAR *outFileName);
BOOL GenerateServiceDisplayName(DWORD key, WCHAR *outFileName);
