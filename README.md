# CommunicationManager

This is library for communication between two endpoints.

This file is part of the [PROJECT WITHOUT A NAME YET] project.

To clone this repository, run:

    git clone https://github.com/TCC-PES-2022/CommunicationManager.git
    cd CommunicationManager
    git submodule update --init --recursive

To build, run:
    make deps && make

To install, run (default instalation path is /tmp):
    make install [DESTDIR=<path_to_intall_dir>]

To test, first build gtest:
    cd test && make gtest

Then run:
    make deps && make && make runtests

To generate test coverage, run:
    make report