Geecxx
======

IRC bot written in C++11.

Main goal: URL detection and web page title retrieval.

Build
=====

This project is using CMake as its build system.

```
$ git clone https://github.com/romainletendart/geecxx.git
$ cd geecxx
$ mkdir build && cd build
$ cmake ..
$ make
```

After that, you should get an executable called `geecxx`.

Usage
=====

```
./geecxx SERVER PORT CHANNEL [KEY] [NICKNAME]
```

For more information, run the program without any arguments.
