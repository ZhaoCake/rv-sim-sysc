CXX = g++
CXXFLAGS = -g -std=c++17 -Wall -I$(SYSTEMC_HOME)/include
LDFLAGS = -L$(SYSTEMC_HOME)/lib-linux64 -lsystemc -lm

# 定义源文件和构建目录
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin
TEST_BIN_DIR = $(BUILD_DIR)/tests

# 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/core/*.cpp $(SRC_DIR)/memory/*.cpp $(SRC_DIR)/register/*.cpp)
# 测试源文件列表
TEST_SRCS = $(wildcard $(TEST_DIR)/*.cpp)

# 将源文件路径转换为对应的目标文件路径
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
# 测试目标文件(不包括main.cpp)
CORE_OBJS = $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
# 获取测试可执行文件名称（去掉.cpp后缀）
TEST_BINS = $(patsubst $(TEST_DIR)/%.cpp,$(TEST_BIN_DIR)/%,$(TEST_SRCS))

.PHONY: all clean directories tests

# 默认目标
all: directories $(BIN_DIR)/riscv-sim

# 测试目标
tests: directories $(TEST_BINS)

# 创建必要的目录结构
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/memory
	@mkdir -p $(BUILD_DIR)/register
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(TEST_BIN_DIR)

# 链接规则 - 主程序
$(BIN_DIR)/riscv-sim: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	@echo "编译完成，可执行文件位于: $(BIN_DIR)/riscv-sim"

# 测试可执行文件链接规则
$(TEST_BIN_DIR)/%: $(TEST_DIR)/%.cpp $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) $< $(CORE_OBJS) -o $@ $(LDFLAGS)
	@echo "测试编译完成: $@"

# 编译规则 - 为每个.cpp文件创建对应的.o文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -rf $(BUILD_DIR)

# 运行规则
run: all
	$(BIN_DIR)/riscv-sim $(ARGS)

# 运行特定测试
runtest-%: $(TEST_BIN_DIR)/%
	$< $(ARGS)
