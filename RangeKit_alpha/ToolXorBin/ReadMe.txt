A simple tool to XOR encode an arbitrary file, meant for binaries.

This is simple, but many EPP (endpoint protection) solutions look for embedded PE executables - even though an embedded executable, by itself, is not indicitive of a threat.  So a simple XOR obfuscation will be needed to bypass their scan.  This has a legitimate use since the Range tools typically contain one or more decoy EXE's.  Without using this bypass, the endpoint security product might actually target and quarantine your range tool!

