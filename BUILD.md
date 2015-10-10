# Building the plugin
This plugin has a dependency on [libmysql](https://github.com/mysql/mysql-server/tree/5.5), that has been included in the tree at the appropriate branch as a submodule.

## Building libmysql
Follow [the MySQL installation guide](http://dev.mysql.com/doc/refman/5.5/en/source-installation.html) for information on how to build MySQL.

The following commands successfully produced a build on Windows and Linux.

  $ mkdir libmysql/lvpbuild
  $ cd libmysql/lvpbuild
  $ cmake ..
  $ cmake --build . --config relwithdebinfo --target libmysql

A minor change to my_pthread.h may be necessary if a compile error occurs there on Windows. On line 103, add:

  #define timespec my_timespec

### Building on Linux
I had to manually build a 32-bit ncurses library in order to get a working build on Linux, and explicitly tell cmake to build a 32-bit library.

Download a 5.9 [ncurses build](http://ftp.gnu.org/gnu/ncurses/) and extract it as ncurses-5.9. Then:

  $ cd ncurses-5.9
  $ CFLAGS=-m32 CXXFLAGS=-m32 ./configure --with-shared

Instead of just running `cmake ..`, run:

  $ cmake .. -DCMAKE_C_FLAGS=-m32 -DCMAKE_CXX_FLAGS=-m32 -DCURSES_LIBRARY=ncurses-5.9/lib/libncurses.so -DCURSES_INCLUDE_PATH=ncurses-5.9/include/

## Building mysql-plugin
Either use the `Makefile` on Linux, or the Visual Studio 2015 project on Windows. They are set up to work correctly with the static libmysql binary you've build above.
