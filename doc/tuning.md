# :rocket: Tuning and performance

This section describes some general recommendations on how to keep a server stable and running smooth with a high player count. Unless stated otherwise, the following basics apply to every server (on Linux), no matter if libcod is installed or not. The term "code" refers to script code (.gsc files) in that context.

## :computer: Hardware and operating system

- Choose a CPU with a high single-core performance.
- A low process niceness (on the operating system level) can help to reduce lag spikes.
- Do not force the cod2_lnxded process to run on a single CPU when using any of the threaded features of libcod:
  - Asynchronous command execution (`execute_async_create` and `execute_async_create_nosave`)
  - Asynchronous MySQL queries (`mysql_async_create_query` and `mysql_async_create_query_nosave`)
  - The integrated proxy servers (referring to the "proxy" dvars)
- Kernel tweaks (e.g., adjusting kernel tick frequency) might help too, but that depends on overall system use and configuration.

## :beginner: Basics

- Keep the code simple. Avoid unnecessary loop nesting that would run redundant checks.
- Build stable code. When building a mod, run the server with developer mode on so that the server stops once any major issue arises. Errors can generate a large amount of log entries (example: undefined variables after a player disconnected), which creates unnecessary load on the server and disk.
- Keep the code stable. Disable developer mode on productive long-term runtime and check the server logs every once in a while. Libcod users might want to enable the `logErrors` dvar.
- Use buffered logging (`set logfile "1"` instead of `set logfile "2"`) to minimize I/O time, especially on servers that run on HDDs instead of SSDs.
- Minimize the number of calls to "expensive" functions such as those used for obstacle tracing (`BulletTrace`, `SightTracePassed` etc.) as they use up quite some CPU time.
- Players with high FPS (on the client side) cause more load on the server, than players with lower FPS. This is due to additional player movement traces that are done for high client FPS. Therefore, server load could be reduced by imposing a rather conservative `com_maxfps` dvar value (e.g., 125) on clients. Keep in mind that this could make certain map areas unreachable (e.g., such that require 333 FPS jumps).

## :mag: The impact of libcod

Although the extensions that libcod provides do have some base overhead in terms of CPU load, most features are disabled by default by hiding them behind dvars. 64 active players on a server are still possible, especially if the installed mod (if any) is not too complex. The added security features (such as packet rate limiting) do cause a small overhead, but are considered a necessity and cannot be disabled via simple configuration options like dvars.

Additionally, the following remarks can matter in terms of performance:
- Avoid calling blocking functions such as `execute` and `system` during gameplay. If possible, run such tasks right after map load, or use the asynchronous variants to avoid lags during runtime.
- Building libcod in debug mode disables compiler optimizations, therefore degrading its performance to some extent.
- To further reduce the number of calls to trace functions that are done by default every server frame, refer to the `sv_isLookingAtOnDemand` dvar.
- The impact of an enabled `sv_fastDownload` dvar with multiple players downloading files in parallel is currently untested.
- The `CodeCallback_DPrintf` callback function could be called quite frequently (multiple times per server frame), especially if developer mode is enabled.
- The `CodeCallback_NotifyDebug` callback function could be called quite frequently too, especially if there is a lot of triggers and players.
- The `CodeCallback_HitchWarning` callback function can be used to *identify* significant lags during server runtime.

However, it should be noted that all custom script callback functions are disabled until enabled excplicitly by adding the respective function to the _callbacksetup.gsc file, so there is no performance degradation by default.

## :bar_chart: Notes on benchmarking
- Test clients ("bots") are less of an effort for a server than real players. Keep that in mind for any sort of mod/server benchmarking efforts.
- The `getMicroseconds` and `getMilliseconds` functions (libcod only) can be used for benchmarking specific portions of code. However, keep in mind that these functions will introduce some overhead on their own.

## :question: Where's the rest?
Got some more recommendations? We're happy to extend the list, see the contact information [here](README.md#-contact--contributing).