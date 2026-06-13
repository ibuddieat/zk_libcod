#include "gsc_extra.hpp"

#if COMPILE_HTTP == 1
#include "gsc_http.hpp"
#include "gsc_websocket.hpp"
#endif

/*
 * Strong definition of the post-frame extension hook declared weakly in
 * gsc_extra.hpp. libcod calls this once per server frame after G_RunFrame.
 * Modules that need per-frame work hook in here.
 */
void extra_G_RunFrame_After(int levelTime)
{
	#if COMPILE_HTTP == 1
	gsc_http_poll();
	gsc_websocket_poll();
	#endif
}
