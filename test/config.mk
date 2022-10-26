# version
VERSION = 0.1

GTEST_ROOT	:=./googletest
DESTDIR 	?= /tmp
DEP_PATH 	?= $(DESTDIR)

CXX				?=
CXXFLAGS 		+= -Wall
CXXFLAGS 		+= -Wextra
CXXFLAGS		+= -pthread
CXXFLAGS 		+= -fprofile-arcs -ftest-coverage --coverage
COBJFLAGS 		:= $(CXXFLAGS) -c
LDFLAGS  		:= -L$(DEP_PATH)/lib
LDLIBS   		:= -lcommunicationmanager -larinc615a -ltransfer -ltftp -ltftpd -lblsecurity 
LDLIBS 			+= -lgcrypt -lgpg-error -lgtest -lgcov -lpthread -lcjson
INCFLAGS 		:= -I$(DEP_PATH)/include

debug: COBJFLAGS 		+= $(DBGFLAGS)
debugdeps: DEP_RULE    	:= debug
testdeps: DEP_RULE    	:= test