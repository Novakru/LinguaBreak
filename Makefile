# 更新SRCDIR包含所有必要目录
SRCDIR += .
SRCDIR += ./tools
SRCDIR += ./llvm/generate
SRCDIR += ./llvm/semant
# SRCDIR += ./front_end  # 取消注释(multi defined)

# 显式列出所有源文件（确保包含实现文件）
SRCS := $(wildcard *.cc)
SRCS += $(wildcard llvm/generate/*.cc)
SRCS += $(wildcard llvm/semant/*.cc)
SRCS += front_end/sysy_lexer.cc 
SRCS += front_end/sysy_parser.tab.cc

# 添加-Wall -Wextra警告选项
# CFLAGS += -Wall -Wextra

NAME = SysYc
BIN_DIR ?= ./bin
OBJDIR ?= ./obj
BINARY ?= $(BIN_DIR)/$(NAME)

.DEFAULT_GOAL = SysYc

CC = clang++
LD = clang++
INCLUDES = $(addprefix -I, $(SRCDIR))
CFLAGS += -O2 -g -MMD -std=c++17 $(INCLUDES)

# 自动搜索所有源文件
SRCS := $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.cc))
SRCS += front_end/sysy_lexer.cc front_end/sysy_parser.tab.cc

# 生成对应的 .o 目标文件
OBJS := $(patsubst %.cc, $(OBJDIR)/%.o, $(SRCS))

# 生成 .d 依赖文件
-include $(OBJS:.o=.d)

$(OBJDIR)/%.o : %.cc
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: SysYc clean-obj clean-all lexer parser format

SysYc: $(BINARY)

$(BINARY): $(OBJS)
	@echo + LD $@
	@mkdir -p $(BIN_DIR)
	@$(LD) $(OBJS) -o $(BINARY) -O2 -std=c++20

lexer: front_end/sysy_lexer.l
	@echo "[FLEX] Generating lexer..."
	flex -o front_end/sysy_lexer.cc front_end/sysy_lexer.l
	@echo "[FLEX] Lexer generated successfully"

parser: front_end/sysy_parser.y
	@echo "[BISON] Generating parser..."
	bison -dv front_end/sysy_parser.y -o front_end/sysy_parser.tab.cc
	@rm -f front_end/sysy_parser.output
	@sed -i '1s/^/#include "..\/include\/ast.h"\n/' front_end/sysy_parser.tab.hh
	@echo "[BISON] Parser generated successfully"

clean-obj:
	@echo "Cleaning object files..."
	@rm -rf $(OBJDIR)/*

clean-all:
	@echo "Cleaning all build files..."
	@rm -rf $(OBJDIR)/*
	@rm -rf $(BIN_DIR)/*

format:
	@echo "Formatting source files..."
	clang-format -style=file -i $(SRCS)
	clang-format -style=file -i $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.h))
