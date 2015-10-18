# mysql-plugin
The MySQL plugin enables the Pawn runtime to communicate with a MySQL server in an asynchronous way, in order to not block execution of the rest of the gamemode. The plugin has currently been build against MySQL 5.5.

See [BUILD.md](BUILD.md) for instructions on how to build this plugin on Windows. On Linux, just running `make` in the [src](src/) directory should work.

## Natives
See [a_mysql.pwn](server/a_mysql.pwn) for the native functions exposed by this function, and a generic explanation on how to use this plugin.
