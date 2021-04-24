About
==============

The libeventloop is a C++ wrapper about libevent.


License
==============

See file LICENSE.en.txt


Version
==============

0.0.1-alpha. Unstable.


Description
==============

Detailed documentation will be written later.
Backward compatibility may be broken until stable version.


Build
==============

First move to build subdirectory.

cd build

Next run cmake to build Makefile.

cmake -G "Unix Makefiles" [-DCMAKE_INSTALL_PREFIX:PATH=<install prefix; /usr/local default>] [-DCMAKE_BUILD_TYPE=Debug|Release] ..

Then run make and optionally make test.
Finalally run make install to install.

make
[make test]
make install
