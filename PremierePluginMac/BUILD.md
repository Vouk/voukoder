Build on MAC
------------

1. wxWidgets 3.1.2

   wget https://github.com/wxWidgets/wxWidgets/releases/download/v3.1.2/wxWidgets-3.1.2.tar.bz2
   mkdir build_release
   cd build_release
   ../configure --disable-shared --enable-stl --with-cxx=14 --with-macosx-sdk=/Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk --with-osx_cocoa --disable-debug --enable-optimise --prefix=$(pwd)
   make -j4 install
   
