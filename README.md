# astrokit - 2020 - stephane gravois - GPL licence
# Linux embedded real time solution to enable azimutal sky tracking with microstepping motor control
#----------------------------------------------------------------------
# version 64 bits pour raspberri 3 a+ - cortex A 53 - armv8 (aarch64)
# Needs to compile : cf bin/Makefile
#   - sudo apt-get install gcc-aarch64-linux-gnu (ubuntu 18.04)
#----------------------------------------------------------------------

stef@stef-Inspiron-7537:~/git/astrokit/bin$ history | grep gcc-aarch64-linux-gnu
 1418  sudo apt-get install gcc-aarch64-linux-gnu
 1560  history | grep gcc-aarch64-linux-gnu

stef@stef-Inspiron-7537:~/git/astrokit/bin$ which /usr/bin/aarch64-linux-gnu-gcc
/usr/bin/aarch64-linux-gnu-gcc

stef@stef-Inspiron-7537:~/git/astrokit/bin$ /usr/bin/aarch64-linux-gnu-gcc -v
Using built-in specs.
COLLECT_GCC=/usr/bin/aarch64-linux-gnu-gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc-cross/aarch64-linux-gnu/7/lto-wrapper
Target: aarch64-linux-gnu
Configured with: ../src/configure -v --with-pkgversion='Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1' --with-bugurl=file:///usr/share/doc/gcc-7/README.Bugs --enable-languages=c,ada,c++,go,d,fortran,objc,obj-c++ --prefix=/usr --with-gcc-major-version-only --program-suffix=-7 --enable-shared --enable-linker-build-id --libexecdir=/usr/lib --without-included-gettext --enable-threads=posix --libdir=/usr/lib --enable-nls --with-sysroot=/ --enable-clocale=gnu --enable-libstdcxx-debug --enable-libstdcxx-time=yes --with-default-libstdcxx-abi=new --enable-gnu-unique-object --disable-libquadmath --disable-libquadmath-support --enable-plugin --enable-default-pie --with-system-zlib --enable-multiarch --enable-fix-cortex-a53-843419 --disable-werror --enable-checking=release --build=x86_64-linux-gnu --host=x86_64-linux-gnu --target=aarch64-linux-gnu --program-prefix=aarch64-linux-gnu- --includedir=/usr/aarch64-linux-gnu/include
Thread model: posix
gcc version 7.4.0 (Ubuntu/Linaro 7.4.0-1ubuntu1~18.04.1) 

