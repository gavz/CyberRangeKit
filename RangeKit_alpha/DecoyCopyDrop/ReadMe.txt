The DecoyCopyDrop is a simple PE executable that will make a second copy of itself, go resident, and then begin logging to a file.  The secondary process in the chain should remain resident and detectable.  There is no stealth.

This Decoy will make a mutex to prevent double-drop, per the standard behavior of most malware.

DecoyCopyDrop can be compiled and used with any of the Basic tools in the RangeKit.

As with all Decoys, this program is NOT malicious and will do no harm to a running system.  It serves only to provide a convenient observable for testing your security and detection software.