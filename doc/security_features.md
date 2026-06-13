# 🚨 Security-related features & adaptions

- Added several security fixes/exploit patches known from other Call of Duty&reg; games
- Added `g_banIPs` dvar support (max. 63 IPs), for use with the `addip`, `removeip` and `listip` console commands
- Added the possibility to prevent script code from reading and/or writing to cvars (`protect` console command)
- Blocked the download of `_svr_` (server-sided) .iwd files via direct server download
- Blocked directory traversal possibilities via direct server download
- Blocked the download of (stock) IW .iwd files via direct server download
- Blocked the client console `printentities` command if cheats are disabled
- Blocked leakage of script errors on server crash (see `sv_genericServerErrorMessage` dvar)
- Added compile flag `ENABLE_UNSAFE` to toggle the availability of potentially unsafe script functions that would make it easy to harm the server with malicious mod or map scripts
- Time-constant string comparison for server passwords
- Fixed known issues with stock voting mechanism (`g_allowVote` dvar)
- Fixed some bugs that resulted in server crashes, to improve server availability
- Packet rate-limiting is always enabled in this fork
- Added optional CoD2x client gate `sv_cod2x_require_hwid` and the `getHWID()` / `getHWID2()` / `getCod2xProtocol()` script methods

## ⚠️ A note on HWID / CoD2x identity (read before relying on it)

The CoD2x client reports a hardware id via the `cl_hwid2` userinfo dvar (a 32-char hex MD5) and its
version via `protocol_cod2x`. These reach the server in the connect userinfo and are exposed to
script through `getHWID()`, `getHWID2()` and `getCod2xProtocol()`, and can be enforced at connect
with `sv_cod2x_require_hwid`.

**These values are client-controlled and have no cryptographic authority.** They are plain userinfo
strings.

### What the gate does and does not do

`sv_cod2x_require_hwid` implements the only hardening that is actually possible — the same check
CoD2x performs on its own servers (`src/shared/server.cpp`): a connect-time cross-check that the
client is genuinely CoD2x (`protocol_cod2x` present and at least the configured version) **and**
that its `cl_hwid2` is a well-formed 32-char hex string. With the gate enabled:

  * stock / 1.x clients and non-CoD2x clients are rejected, so anyone who connects has a CoD2x
    identity present.
