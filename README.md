# CommunicationManager

This is library for communication between two endpoints.

This file is part of the [PROJECT WITHOUT A NAME YET] project.

To clone this repository, run:

    git clone https://github.com/TCC-PES-2022/CommunicationManager.git
    cd CommunicationManager
    git submodule update --init --recursive

Before building your project, you may need to install some dependencies. To do so, run:

    sudo apt install -y build-essential lcov

Export the instalation path to the environment:
    export DESTDIR=<path_to_install>

You can also define this variable in your `.bashrc` file. or when calling any make rule. The default installation path is `/tmp`.

To build, run:

    make deps && make

To install, run:

    make install

To test, first build gtest:

    cd test && make gtest

Make sure blmodule have exeution permission:

    chmod +x blmodule

Then run:

    make deps && make && make runtests

To generate test coverage, run:

    make report