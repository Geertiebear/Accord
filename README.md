## What is Accord?

Accord is a chat/voice program designed to replace discord. It is written in C++ using Qt/QtQuick.

## Dependencies

* Qt
* MySQL++
* LibEvent
* Boost
* OpenSSL
* yaml-cpp
* libargon2

## Running

1. Clone the repo
2. Run `mkdir build && cd build`
3. Run `cmake .. && make`
4. Run `cd ../config.yml.example config.yml`
5. Run `openssl req -x509 -newkey rsa:4096 -keyout pkey.pem -out cert.pem -days
 1001 -nodes` to generate a openssl requirements.
6. Populate the config file
7. Run `./accordserver --initdatabase`
8. Server should now be up and running with a properly initialized database
9. Client can be run using `./accordclient` and cli client can be run using `./accordcliclient`
