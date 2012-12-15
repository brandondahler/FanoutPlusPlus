FanoutPlusPlus
==============

Fanout notification server in C++

To Build (Latest tools)
--------

    ./autogen.sh
    ./configure
    make


To Build (Old autoconf)
--------

Edit configure.ac, change AC_PREREQ to lower version.  
Hope the above steps work.


To Build (libevent2 not avaliable)
--------

Libevent2 will need to be built and statically linked into the program.  
Download latest libevent-2.x.x-stable.tar.gz, find link at http://libevent.org/.


Untar from archive into some directory.

    tar -xvzf libevent-2*.tar.gz

Build libevent2.

    cd libevent-2*
    ./configure
    make

Copy static library to FanoutPlusPlus src directory.

    mkdir [FanoutPlusPlus directory]/src/libevent
    cp .libs/libevent_core.a [FanoutPlusPlus directory]/src/libevent/
    cp -r include [FanoutPlusPlus directory]/src/libevent/
    cd [FanoutPlusPlus directory]
   
Build with special configure line.

    ./autogen.sh
    ./configure CPPFLAGS="-I src/libevent/include -I libevent/include" LDFLAGS="-L src/libevent -L libevent" LIBS="-static -levent_core -dynamic -lrt"
	make
