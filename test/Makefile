include config.mk

# path macros
BIN_PATH := bin
SRC_PATH := src
OBJ_PATH := obj
INCLUDE_PATH := include
REPORT_PATH := report

DEPS := CommunicationManager

# compile macros
TARGET_NAME := unity_test_communication_manager
TARGET := $(BIN_PATH)/$(TARGET_NAME)

# src files & obj files
SRC := $(shell find $(SRC_PATH) -type f -name "*.cpp")
OBJ := $(addprefix $(OBJ_PATH)/, $(addsuffix .o, $(notdir $(basename $(SRC)))))

# clean files list
DISTCLEAN_LIST := $(OBJ) \
				  $(BIN_PATH)/*

CLEAN_LIST := $(TARGET) 		 \
			  *.txt 			 \
			  $(OBJ_PATH)/*.gcov \
			  $(OBJ_PATH)/*.gcno \
			  $(OBJ_PATH)/*.gcda \
			  $(SRC_PATH)/*.gcov \
			  $(SRC_PATH)/*.gcno \
			  $(SRC_PATH)/*.gcda \
			  $(DISTCLEAN_LIST)

# default rule
default: all

# non-phony targets
CommunicationManager:
	@echo "\n\n *** Compiling $@ *** \n\n"
	cd .. && make test -j$(shell echo $$((`nproc`))) && make install

$(TARGET): $(DEPS) $(OBJ)
	@echo "\n\n *** Linking $@ *** \n\n"
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) \
	$(LINKFLAGS) $(INCFLAGS) $(LDFLAGS) $(LDLIBS)

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c*
	@echo "Compiling $<"	
	$(CXX) $(COBJFLAGS) -o $@ $< $(INCFLAGS)

# phony rules
.PHONY: makedir
makedir:
	@mkdir -p $(BIN_PATH) $(OBJ_PATH)

.PHONY: all
all: makedir $(TARGET)

.PHONY: debug
debug: makedir $(TARGET)

.PHONY: gtest
gtest:
	cd $(GTEST_ROOT) && cmake . && make -j$(shell echo $$((`nproc`)))

.PHONY: runtests
runtests:
	LD_LIBRARY_PATH=/opt/fls/lib ./$(TARGET)

.PHONY: report
report:
	@mkdir -p $(REPORT_PATH)
	find ../src -name "*.gcov" -exec gcov {} --object-directory ../obj \;
	lcov --capture --directory ../obj --output-file $(REPORT_PATH)/coverage.info
	genhtml $(REPORT_PATH)/coverage.info --output-directory $(REPORT_PATH)

.PHONY: clean
clean:
	@echo CLEAN $(CLEAN_LIST)
	@rm -f $(CLEAN_LIST)

.PHONY: distclean
distclean:
	@echo CLEAN $(DISTCLEAN_LIST)
	@rm -f $(DISTCLEAN_LIST)