THIS_MKDIR = $(shell dirname $(abspath $(lastword $(MAKEFILE_LIST))))
OBJS += $(call GET_OBJS,./gtest/thread/,gtest/thread)
OBJS += $(call GET_OBJS,./gtest/dl_mgr/,gtest/dl_mgr)


include ./tcli.mk
INCS += -I$(TCLI_MKDIR)/../../include

# # xxx
# $(OBJ_DIR)/xxx/%.o: ./xxx/%.cpp $(HEADERS)
# 	@mkdir -p $(OBJ_DIR)/xxx
# 	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

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
