qboolean custom_SV_MapExists(const char *name);
void custom_SVC_RemoteCommand(netadr_t from, msg_t *msg, qboolean from_script);
void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message);