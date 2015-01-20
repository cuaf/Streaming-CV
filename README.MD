Overview
========


On a full computer:
=============

You must already have openCV installed, and pkgconfig on the host.

The makefile should be set up to correctly find the libraries, so just:

make clean &&
make

On the receiving end: ./netcvs8080

On the sender: ./netcvc 127.0.0.1 8080

We should probably rewrite this to send one opencv matrix per UDP packet, might be faster or more resilient to dropouts.


On the Pi
======

The pi is only going to send.
For this you need to have a load of other projects compiled - mostly OpenCV,

Start installing
------------
```
sudo apt-get install build-essential
sudo apt-get install cmake cmake-curses-gui
sudo apt-get install pkg-config
sudo apt-get install  python-dev python-numpy
sudo apt-get install mosh
sudo apt-get install libX11-devel

 sudo apt-get install libpng12-0 libpng12-dev libpng++-dev libpng3 libpnglite-dev zlib1g-dbg zlib1g zlib1g-dev pngtools

 ```

wget the latest version of opencv 2.4.10

unzip it, cd to the dir.

```
mkdir release
ccmake ..
```

Have it generate a config file, then look through the options.

```
make
sudo make install
```

Wait for ages (just leave it for circa 5 hours).

Now you have an openCV install!

I had various issues with configure not finding libraries at this point - try
```
sudo ldconfig
```

Next you need the libraries for the pi to speak to the videocore section of the processor (this controls the interface to the pi camera.)

git clone this: https://github.com/cedricve/raspicam/
Do the usual build dir, cmake and sudo make install routine.

Now you should be able to just pull down this repository on the pi, and use cmake.
I didn't translate the cmake script to a normal makefile as the raspicam routines already rely on cmake to pull in libraries. This should correctly find the other libraries installed in /opt/vc/lib for the videocore, and the opencv libraries too.

For what it's worth, I found CMake didn't always find libraries; this may be due to flaws in the provided CMakeLists.txt. I was able to fix this by the following line to CMakeLists of the example programs supplied in the other git repositories.
```
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/local/lib/cmake/)
```

Demo
------

If this is all set up correctly, you should be able to run make in this directory to produce the netcvs and netcvc programs. These wil send and receive from PCs/Macs over TCP with ./netcvc 127.0.01 8080 and ./netcvs 8080.

Only sending is implemented on the Pi at present, as I don't have an HDMI monitor or LCD breakout on the pi.

For this use cmake and find piSender in the build subdirectory. This takes the same arguments as netcvc - an ip address and port number.




