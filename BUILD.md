# Building the plugin
This plugin has a dependency on [libmysql](https://github.com/mysql/mysql-server/tree/5.5), that has been included in the tree at the appropriate branch as a submodule.

This guide should only be applicable to Windows. Linux users can just run `make` in the [src](src/) directory.

## Building libmysql
Follow [the MySQL installation guide](http://dev.mysql.com/doc/refman/5.5/en/source-installation.html) for information on how to build MySQL.

The following commands successfully produced a build on Windows.

    $ mkdir libmysql/lvpbuild
    $ cd libmysql/lvpbuild
    $ cmake -DCMAKE_C_FLAGS="-m32" -DCMAKE_CXX_FLAGS="-m32" ..
    $ CMAKE_C_FLAGS="-m32" CMAKE_CXX_FLAGS="-m32" cmake --build . --config relwithdebinfo --target libmysql

A minor change to my_pthread.h may be necessary if a compile error occurs there on Windows. On line 103, add:

  #define timespec my_timespec

## Building mysql-plugin
Either use the `Makefile` on Linux, or the Visual Studio 2015 project on Windows. They are set up to work correctly with the static libmysql binary you've build above.
