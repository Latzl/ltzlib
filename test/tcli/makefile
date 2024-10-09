# OBJS += $(call GET_OBJS,./xxx/,xxx)
OBJS += $(call GET_OBJS,./gtest/thread/,gtest/thread)

include ./tcli.mk
INCS += -I$(TCLI_MKDIR)/../../include

# gtest/thread
$(OBJ_DIR)/gtest/thread/%.o: ./gtest/thread/%.cpp $(HEADERS)
	@mkdir -p $(OBJ_DIR)/gtest/thread
	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

# # xxx
# $(OBJ_DIR)/xxx/%.o: ./xxx/%.cpp $(HEADERS)
# 	@mkdir -p $(OBJ_DIR)/xxx
# 	$(CXX) $(INCS) $(FLAGS) -c $< -o $@

