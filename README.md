# CommunicationManager

This is library for communication between two endpoints using ARINC-615A protocol.

This repository is part of project [ARIEL](https://github.com/TCC-PES-2022).

To clone this repository, run:

    git clone https://github.com/TCC-PES-2022/CommunicationManager.
    cd CommunicationManager
    git submodule update --init --recursive

Before building your project, you may need to install some dependencies. To do so, run:

    sudo apt update
    sudo apt install -y build-essential libcjson-dev libgcrypt-dev
    
For tests, you'll also need
    
    sudo apt install -y libgtest-dev cmake lcov

Export the instalation path to the environment:
    export DESTDIR=<path_to_install>

You can also define this variable in your `.bashrc` file. or when calling any make rule. The default installation path is `/tmp`.

To build, run:

    make deps && make

To install, run:

    make install


To test, first build gtest:

    cd /usr/src/gtest
    sudo cmake CMakeLists.txt
    sudo make
    sudo cp lib/*.a /usr/lib
    sudo mkdir -p /usr/local/lib/gtest/
    sudo ln -s /usr/lib/libgtest.a /usr/local/lib/gtest/libgtest.a
    sudo ln -s /usr/lib/libgtest_main.a /usr/local/lib/gtest/libgtest_main.a

To test and generate test coverage, run:

    cd test && chmod +x blmodule
    make report