#pragma once

// TODO: reference additional headers your program requires here

#include <Windows.h>
#include <stdio.h>
#include <conio.h>

#define VERSION L"1.0"

#include "progopts.h"

void xprintf(PROG_LOGMODE theLogMode, CONST WCHAR *wfmt, ...);
void xputchar(PROG_LOGMODE theLogMode, CONST WCHAR control_char, CONST WCHAR pChar);
