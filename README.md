## What is Accord?

Accord is a chat/voice program designed to replace discord. It is written in C++ using Qt/QtQuick.

## Dependencies

* Qt5
* Cereal
* libmysqlclient (can be mysql-connector-c or libmariadb)
* LibEvent
* Boost
* OpenSSL
* yaml-cpp
* libargon2
* ImageMagick

## Build & Run instructions

### Windows

These are windows specific instructions for installing all required dependencies.
So far Accord has only been successfully built using msys2 (https://www.msys2.org/).
Contributions for building with Visual Studio are welcome!
To download all the required packages run the following command.
`pacman -S git mingw-w64-x86_64-{cmake,make,gcc,qt5,cereal,libmariadbclient,libevent,boost,openssl,yaml-cpp,argon2,imagemagick}`

**Note:** To run the server on windows you will need to set the `MAGICK_CODER_MODULE_PATH` to the imagemagick
coders folder for your system. For the mingw package it is roughly under `/mingw64/lib/ImageMagick-VERSION/modules/coders`.
Not setting the environment variable to the coders folder will result in a crash when a user tries to create a community.
As an example, if you wish to run the server your command may look like this
`MAGICK_CODER_MODULE_PATH="/mingw64/lib/ImageMagick-7.0.7/modules-Q16HDRI/coders" ./accordserver`.

### General

1. Clone the repo
2. Run `mkdir build && cd build`
3. If you're on msys2 run `cmake -G "Unix Makefiles" .. && make`, else `cmake .. && make`
4. Run `cd ../config.yml.example config.yml`
5. Run `openssl req -x509 -newkey rsa:4096 -keyout pkey.pem -out cert.pem -days
 1001 -nodes` to generate required certificate and key.
6. Populate the config file
7. Run `./accordserver --initdatabase`
8. Server should now be up and running with a properly initialized database
9. Client can be run using `./accordclient` and cli client can be run using `./accordcliclient`
