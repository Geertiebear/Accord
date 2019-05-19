#!/bin/bash

brew install cereal mysql-connector-c libevent openssl yaml-cpp argon2 pkg-config imagemagick
cp travis/libargon2.pc /usr/local/share/pkgconfig/libargon2.pc
pkg-config --libs argon2
pkg-config --libs libargon2
