TCLI_MKDIR = $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
CXX = g++
FLAGS = -g -O0 -std=c++14 -Wall
BUILD_DIR = ./build
OBJ_DIR = $(BUILD_DIR)/obj
TARGET_DIR = $(BUILD_DIR)/bin
TARGET = $(TARGET_DIR)/tcli
FLAGS += -fno-access-control

INCS +=

LIBS += -Wl,-Bstatic -ltcli -Wl,-Bdynamic
LIBS += -lpthread -lrt -ldl

# call GET_OBJS,src_dir,created_obj_dir
define GET_OBJS
$(patsubst %.cpp, $(OBJ_DIR)/$(2)/%.o,$(notdir $(wildcard $(1)/*.cpp)))
endef

# gtest
OBJS += $(call GET_OBJS,./gtest/,gtest)
$(OBJ_DIR)/gtest/%.o: ./gtest/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# .
HEADERS += $(wildcard ./*.hpp)
OBJS += $(call GET_OBJS,./,.)
$(OBJ_DIR)/%.o: ./%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	@mkdir -p $(TARGET_DIR)
	$(CXX) $(INCS) $(FLAGS) -o $@ $^ $(LIBS)

.PHONY: all clean test

all : $(TARGET)

clean:
	rm -r $(BUILD_DIR)

test: