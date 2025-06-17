# 更新SRCDIR包含所有必要目录
SRCDIR += .
SRCDIR += ./tools
SRCDIR += ./llvm/generate
SRCDIR += ./llvm/semant
SRCDIR += ./llvm/optimize/analysis
SRCDIR += ./llvm/optimize/transform
SRCDIR += ./back_end/basic
SRCDIR += ./back_end/inst_process/inst_print
SRCDIR += ./back_end/inst_process/inst_select
SRCDIR += ./back_end/inst_process/phi_processing
SRCDIR += ./back_end/inst_process
SRCDIR += ./back_end/register_allocation/linear_scan
SRCDIR += ./back_end/register_allocation
SRCDIR += ./back_end/optimize
# 显式列出所有源文件（确保包含实现文件）
SRCS := $(wildcard *.cc)
SRCS += $(wildcard llvm/generate/*.cc)
SRCS += $(wildcard llvm/semant/*.cc)
SRCS += front_end/sysy_lexer.cc 
SRCS += front_end/sysy_parser.tab.cc
SRCS += $(wildcard back_end/basic/*.cc)
SRCS += $(wildcard back_end/inst_process/inst_print/*.cc)
SRCS += $(wildcard back_end/inst_process/inst_select/*.cc)
SRCS += $(wildcard back_end/inst_process/*.cc)
SRCS += $(wildcard back_end/inst_process/phi_processing/*.cc)
SRCS += $(wildcard back_end/register_allocation/linear_scan/*.cc)
SRCS += $(wildcard back_end/register_allocation/*.cc)
SRCS += $(wildcard back_end/optimize/*.cc)
# 添加-Wall -Wextra警告选项
# CFLAGS += -Wall -Wextra

OBJDIR ?= ./obj

CC = clang++
LD = clang++
INCLUDES = $(addprefix -I, $(SRCDIR))
CFLAGS += -O2 -MMD -std=c++17 $(INCLUDES)

SRCS := $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.cc))
SRCS += front_end/sysy_lexer.cc front_end/sysy_parser.tab.cc

OBJS := $(patsubst %.cc, $(OBJDIR)/%.o, $(SRCS))

DEPS := $(OBJS:.o=.d)
-include $(DEPS)

.DEFAULT_GOAL = $(BINARY)

$(OBJDIR)/%.o : %.cc
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BINARY): $(OBJS)
	@echo + LD $@
	@$(LD) $(OBJS) -o $@ -O2  -std=c++17

lexer: front_end/sysy_lexer.l
	@echo "[FLEX] Generating lexer..."
	@flex -o front_end/sysy_lexer.cc front_end/sysy_lexer.l
	@echo "[FLEX] Lexer generated successfully"

parser: front_end/sysy_parser.y
	@echo "[BISON] Generating parser..."
	@bison -dv front_end/sysy_parser.y -o front_end/sysy_parser.tab.cc
	@rm -f front_end/sysy_parser.output
	@sed -i '1s/^/#include "..\/include\/ast.h"\n/' front_end/sysy_parser.tab.hh
	@echo "[BISON] Parser generated successfully"

clean-obj:
	@echo "Cleaning object files..."
	@rm -rf $(OBJDIR)/*

clean-all: clean-obj
	@echo "Cleaning executable..."
	@rm -f $(BINARY)

format:
	@echo "Formatting source files..."
	clang-format -style=file -i $(SRCS)
	clang-format -style=file -i $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.h))