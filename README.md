### Pre-Requisites
cmake, git, g++  and 0MQ

```
sudo apt-get update;
sudo apt-get upgrade;
sudo apt-get install libzmq3-dev cmake g++ git;
```

# MALOS LIB.

Library of Hardware abstraction for MATRIX Creator usable via 0MQ.
[Protocol buffers](https://developers.google.com/protocol-buffers/docs/proto3) are used for data exchange.

Please follow the [C++ Installation Instructions](https://github.com/google/protobuf/blob/master/src/README.md)

### Cloning
```
git clone https://github.com/matrix-io/matrix-malos-lib.git
cd matrix-malos-lib
git submodule init && git submodule update
```

### Compiling
```
mkdir build && cd build
cmake .. 
make && sudo make install

```

