THIS_MKDIR := $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
OBJS += $(call GET_OBJS,./gtest/thread/,gtest/thread)
OBJS += $(call GET_OBJS,./gtest/dl_mgr/,gtest/dl_mgr)
OBJS += $(call GET_OBJS,./gtest/string/,gtest/string)
OBJS += $(call GET_OBJS,./gtest/task/,gtest/task)
OBJS += $(call GET_OBJS,./gtest/scheduler/,gtest/scheduler)
OBJS += $(call GET_OBJS,./scheduler/,scheduler)

# OBJS += $(call GET_OBJS,./xxx/,xxx)

include ../../third_party/ltztest/tcli.mk
INCS += -I$(THIS_MKDIR)/../../include

# # xxx
# HEADERS += $(THIS_MKDIR)/../../include/ltz/hhh/*
# $(OBJ_DIR)/xxx/%.o: ./xxx/%.cpp $(HEADERS)
# 	@mkdir -p $(OBJ_DIR)/xxx
# 	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# scheduler
HEADERS += $(THIS_MKDIR)/../../include/ltz/scheduler/*
$(OBJ_DIR)/scheduler/%.o: ./scheduler/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/scheduler
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# gtest/scheduler
HEADERS += $(THIS_MKDIR)/../../include/ltz/scheduler/*
$(OBJ_DIR)/gtest/scheduler/%.o: ./gtest/scheduler/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest/scheduler
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# gtest/string
$(OBJ_DIR)/gtest/string/%.o: ./gtest/string/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest/string
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

DL_TEST_TARGET = $(TARGET_DIR)/test_dl.so
# gtest/dl_mgr
$(OBJ_DIR)/gtest/dl_mgr/%.o: ./gtest/dl_mgr/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest/dl_mgr
	$(CXX) $(INCS) $(FLAGS) -DDL_TEST_PATH=\"$(DL_TEST_TARGET)\" -c $< -o $@
	@mkdir -p $(TARGET_DIR)
	make -C ./dl_mgr/test_dl/ TARGET=$(DL_TEST_TARGET)

# gtest/thread
$(OBJ_DIR)/gtest/thread/%.o: ./gtest/thread/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest/thread
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@
