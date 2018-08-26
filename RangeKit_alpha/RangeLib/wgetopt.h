#pragma once

extern int optind, opterr;
extern WCHAR *optarg;
extern WCHAR *optmod;

WCHAR wgetopt(int argc, WCHAR *argv[], WCHAR *optstring);