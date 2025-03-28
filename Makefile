CXX = g++
CXXFLAGS = -std=c++17 -Wall -I$(SYSTEMC_HOME)/include
LDFLAGS = -L$(SYSTEMC_HOME)/lib-linux64 -lsystemc -lm

# 定义源文件和构建目录
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

# 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.cpp $(SRC_DIR)/core/*.cpp $(SRC_DIR)/memory/*.cpp $(SRC_DIR)/register/*.cpp)
# 将源文件路径转换为对应的目标文件路径
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
# 最终可执行文件
TARGET = $(BIN_DIR)/riscv-sim

.PHONY: all clean directories

# 默认目标
all: directories $(TARGET)

# 创建必要的目录结构
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/core
	@mkdir -p $(BUILD_DIR)/memory
	@mkdir -p $(BUILD_DIR)/register
	@mkdir -p $(BIN_DIR)

# 链接规则
$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(LDFLAGS)
	@echo "编译完成，可执行文件位于: $(TARGET)"

# 编译规则 - 为每个.cpp文件创建对应的.o文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -rf $(BUILD_DIR)

# 运行规则
run: all
	$(TARGET) $(ARGS)
