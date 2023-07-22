void custom_G_CheckHitTriggerDamage(gentity_t *pActivator, float *vStart, float *vEnd, int iDamage, meansOfDeath_t iMOD);
void custom_player_die(gentity_t *self, gentity_t *inflictor, gentity_t *attacker, int damage, meansOfDeath_t meansOfDeath, int iWeapon, const float *vDir, hitLocation_t hitLoc, int psTimeOffset);
qboolean custom_SV_MapExists(const char *name);
void custom_SVC_RemoteCommand(netadr_t from, msg_t *msg, qboolean from_script);
void Scr_CodeCallback_Error(qboolean terminal, qboolean emit, const char *internal_function, char *message);