#include <stdio.h>
#include <Windows.h>

// insert signature substrings here in this block to trigger EPP solution
char gszTriggerString[] =
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX" \
"XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";

char gszAlinaLine[] =
"windefender.exe" \
" terminate " \
" forcing " \
" Executing " \
" failed " \
" success " \
" Exception " \
" at " \
" Imagebase " \
" pid " \
" to " \
"InternetOpen " \
"InternetConnect " \
"HttpOpenRequest " \
"HttpSendRequest " \
"HttpQueryInfo " \
" new " \
" logging " \
" on " \
" off " \
" delete " \
" file " \
" Download & Execute " \
" no " \
" last request " \
" seconds ago " \
" command " \
" get and call " \
" called pipe and encountered version " \
" i am " \
" pipe " \
" thread " \
" restarting " \
" httpRequest " \
" length " \
" checksum " \
" fallback " \
" verification " \
" should be " \
" is " \
" CreateFileA " \
" WriteFile " \
" wrote " \
" UrlDownloadToFileA " \
" open " \
" read " \
" process ";

// would trigger fancybear except that this is a PE and that malware uses python
char gszFancyBearLine[] =
"pop.gmail.com\0" \
"jassnovember30@gmail.com\0" \
"30Jass11\0" \
"smtp.gmail.com\0" \
"userdf783@mailtransition.com\0" \
"ginabetz75@gmail.com\0" \
"75Gina75\0" \
"104.152.187.66\0" \
"/updates/\0";

WCHAR gszZeroAccessLine[] =
L"\\Google\\Desktop\\Install\\{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}\\#.\\ \0" \
L"%wS\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\User Shell Folders\0" \
L"Local AppData\0" \
L"\\GLOBAL??\\ \0";

int wmain(int argc, WCHAR* argv[])
{		
	wprintf(L"Hello Cyber Range Kit\n");

	return 0;
}

