#!/bin/bash

brew install cereal mysql-connector-c libevent openssl yaml-cpp argon2 pkg-config imagemagick
ls /usr/local/share/pkgconfig
pkg-config --libs argon2
pkg-config --libs libargon2
