# Security-related features & adaptions

- Added several security fixes/exploit patches known from other Call of Duty&reg; games
- Blocked the download of `_svr_` (server-sided) .iwd files via direct server download
- Blocked directory traversal possibilities via direct server download
- Blocked the download of (stock) IW .iwd files via direct server download 
- Blocked the client console `printentities` command if cheats are disabled
- Added compile flag `ENABLE_UNSAFE` to toggle the availability of potentially unsafe script functions that would make it easy to harm the server with malicious map scripts
- Time-constant string comparison for server passwords
- Fixed some bugs that resulted in server crashes, to improve server availability
- Packet rate-limiting is always enabled in this fork
- Excluded client userinfo updates from flood protection