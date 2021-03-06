#!/bin/bash

sudo apt-get install -y qtbase5-dev qml-module-qtquick-controls qml-module-qtquick2 qtdeclarative5-dev libmysqlclient-dev libboost-program-options-dev libyaml-cpp-dev pkg-config libmagick++-dev libcereal-dev
wget http://mirrors.kernel.org/ubuntu/pool/main/a/argon2/libargon2-0_0~20161029-1.1_amd64.deb
sudo dpkg -i libargon2-0_0~20161029-1.1_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/a/argon2/libargon2-0-dev_0~20161029-1.1_amd64.deb
sudo dpkg -i libargon2-0-dev_0~20161029-1.1_amd64.deb
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl1.1_1.1.0g-2ubuntu4.3_amd64.deb
sudo dpkg -i libssl1.1_1.1.0g-2ubuntu4.3_amd64.deb
wget http://security.ubuntu.com/ubuntu/pool/main/o/openssl/libssl-dev_1.1.0g-2ubuntu4.3_amd64.deb
sudo dpkg -i libssl-dev_1.1.0g-2ubuntu4.3_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/libe/libevent/libevent-2.1-6_2.1.8-stable-4build1_amd64.deb
sudo dpkg -i libevent-2.1-6_2.1.8-stable-4build1_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/libe/libevent/libevent-core-2.1-6_2.1.8-stable-4build1_amd64.deb
sudo dpkg -i libevent-core-2.1-6_2.1.8-stable-4build1_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/libe/libevent/libevent-extra-2.1-6_2.1.8-stable-4build1_amd64.deb
sudo dpkg -i libevent-extra-2.1-6_2.1.8-stable-4build1_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/libe/libevent/libevent-openssl-2.1-6_2.1.8-stable-4build1_amd64.deb
sudo dpkg -i libevent-openssl-2.1-6_2.1.8-stable-4build1_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/libe/libevent/libevent-pthreads-2.1-6_2.1.8-stable-4build1_amd64.deb
sudo dpkg -i libevent-pthreads-2.1-6_2.1.8-stable-4build1_amd64.deb
wget http://mirrors.kernel.org/ubuntu/pool/main/libe/libevent/libevent-dev_2.1.8-stable-4build1_amd64.deb
sudo dpkg -i libevent-dev_2.1.8-stable-4build1_amd64.deb
