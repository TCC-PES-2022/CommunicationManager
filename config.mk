# version
VERSION = 0.1

# paths
DEST 	:= /opt/fls
DEPS 	:= ARINC615AManager

INSTALL_PATH 	:= $(DEST)
DEP_PATH 		:= $(DEST)

CXX 		?=
CXXFLAGS 	:= -Wall -Werror -std=c++11 -pthread
DBGFLAGS 	:= -g -ggdb
TESTFLAGS 	:= -fprofile-arcs -ftest-coverage --coverage -lgcov
LINKFLAGS 	:= -shared
LDFLAGS  	:= -L$(DEP_PATH)/lib
LDLIBS   	:= -larinc615a -ltransfer -ltftp -ltftpd -lcjson

COBJFLAGS 	:= $(CXXFLAGS) -c -fPIC
test: COBJFLAGS 	+= $(TESTFLAGS)
test: LINKFLAGS 	+= -lgcov
debug: COBJFLAGS 	+= $(DBGFLAGS)
