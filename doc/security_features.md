# Security-felated features & adaptions

- Added several security fixes/exploit patches known from other Call of Duty&reg; games
- Blocked the download of of `_svr_` (server-sided) .iwd files
- Blocked the client console `printentities` command if cheats are disabled
- Added compile flag `ENABLE_UNSAFE` to toggle the availability of potentially unsafe script functions that would make it easy to harm the server with malicious map scripts
- Fixed some bugs that resulted in server crashes, to improve server availability
- Packet rate-limiting is always enabled in this fork
- Excluded client userinfo updates from flood protection