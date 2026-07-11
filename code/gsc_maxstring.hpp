#ifndef _GSC_MAXSTRING_HPP_
#define _GSC_MAXSTRING_HPP_

// Upper bound on any single string passed to the script engine, shared by the
// HTTP and WebSocket modules. Strings are interned in a memory tree whose
// per-allocation ceiling is MEMORY_NODE_COUNT (0x10000); exceeding it calls
// MT_Error -> Scr_TerminalError, which is fatal. Response bodies, header
// keys/values and websocket frames come from a remote peer, so they are rejected
// past this bound instead of crashing the server. CoD2x pushes them unchecked
// (its gsc_http.cpp:63).
//   CoD2rev MT_GetSize limit:
//     https://github.com/voron00/CoD2rev_Server/blob/abf692f1fd5697585ffd083e3114b2850c13a1f4/src/script/scr_memorytree.cpp#L628
//   CoD2rev MT_Error -> Scr_TerminalError:
//     https://github.com/voron00/CoD2rev_Server/blob/abf692f1fd5697585ffd083e3114b2850c13a1f4/src/script/scr_memorytree.cpp#L610-L615
#define GSC_MAX_STRING_BYTES 65000

#endif
