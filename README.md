# MALOS LIB.

Library of Hardware abstraction for MATRIX Creator usable via 0MQ.
[Protocol buffers](https://developers.google.com/protocol-buffers/docs/proto3) are used for data exchange.

## Installation
```
# Add repo and key
curl https://apt.matrix.one/doc/apt-key.gpg | sudo apt-key add -
echo "deb https://apt.matrix.one/raspbian $(lsb_release -sc) main" | sudo tee /etc/apt/sources.list.d/matrixlabs.list

# Update packages and install
sudo apt-get update
sudo apt-get upgrade

# Installation
sudo apt install libmatrixio-malos libmatrixio-malos-dev
```
Please follow the [C++ Installation Instructions](https://github.com/google/protobuf/blob/master/src/README.md)

## Pre-Requisites
cmake, git, g++  and 0MQ

```
sudo apt-get update;
sudo apt-get upgrade;
sudo apt-get install libzmq3-dev matrixio-protobuf libmatrixio-protos libmatrixio-protos-dev
```

## Cloning
```
git clone https://github.com/matrix-io/matrix-malos-lib.git
cd matrix-malos-lib
git submodule init && git submodule update
```

## Compiling
```
mkdir build && cd build
cmake .. 
make && sudo make install

```
