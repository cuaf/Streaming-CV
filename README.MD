Overview
========

You must already have openCV installed, and pkgconfig. 

The makefile should be set up to correctly find the libraries, so just:

make clean &&
make

On the receiving end: ./netcvs8080

On the sender: ./netcvc 127.0.0.1 8080

We should probably rewrite this to send one opencv matrix per UDP packet, might be faster or more resilient to dropouts. 

