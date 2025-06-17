#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>
#include "front_end/sysy_parser.tab.hh"
#include "include/symbol.h"
#include "llvm/semant/semant.h"
#include "include/ir.h"

#include "llvm/optimize/transform/simplify_cfg.h"
#include "llvm/optimize/analysis/dominator_tree.h"
#include "llvm/optimize/transform/mem2reg.h"
#include "llvm/optimize/transform/adce.h"
#include "llvm/optimize/transform/peephole.h"
#include "llvm/optimize/transform/sccp.h"
#include "llvm/optimize/transform/tailcallelim.h"

//-target
#include"back_end/basic/riscv_def.h"
#include"back_end/basic/register.h"
#include"back_end/inst_process/inst_select/inst_select.h"
#include"back_end/register_allocation/linear_scan/linear_scan.h"
#include"back_end/inst_process/inst_print/inst_print.h"


extern FILE *yyin;
extern char *yytext;
extern YYSTYPE yylval;
extern int error_num;
extern Program ASTroot;
extern LLVMIR llvmIR;
extern int yylex();
extern void dumpTokens(FILE* output, int token, int line_number, char *yytext, YYSTYPE yylval);
extern std::vector<std::string> error_msgs;
IdTable id_table;
int line = 1;
bool optimize_flag = false;

void print_usage() {
    std::cerr << "Usage:\n";
    std::cerr << "  compiler -S [-o output_file] input_file [-O1]\n";
    std::cerr << "  compiler -lexer [-o output_file] input_file\n";
    std::cerr << "  compiler -parser [-o output_file] input_file\n";
    std::cerr << "  compiler -semant [-o output_file] input_file\n";
    std::cerr << "  compiler -llvm [-o output_file] input_file\n";
    std::cerr << "  compiler -select [-o output_file] input_file\n";
}

int main(int argc, char** argv) {
    char* input_file = nullptr;
    char* output_file = nullptr;
    bool optimize = false;
    int option = -1;  // -1: invalid, 0: -lexer, 1: -parser, etc.

    for (int i = 1; i < argc; ) {
        if (strcmp(argv[i], "-lexer") == 0) {
            option = 0;
            i++;
        } else if (strcmp(argv[i], "-parser") == 0) {
            option = 1;
            i++;
        } else if (strcmp(argv[i], "-semant") == 0) {
            option = 2;
            i++;
        } else if (strcmp(argv[i], "-llvm") == 0) {
            option = 3;
            i++;
        } else if (strcmp(argv[i], "-select") == 0) {
            option = 4;
            i++;
        } else if (strcmp(argv[i], "-S") == 0) {
            option = 5;
            i++;
        } else if (strcmp(argv[i], "-O1") == 0) {
            optimize = true;
            i++;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                std::cerr << "Error: -o requires an output file\n";
                print_usage();
                return 1;
            }
            output_file = argv[i+1];
            i += 2;
        } else if (argv[i][0] != '-') {
            input_file = argv[i];
            i++;
        } else {
            std::cerr << "Error: Unknown option " << argv[i] << "\n";
            print_usage();
            return 1;
        }
    }

    // 验证参数
    if (!input_file) {
        std::cerr << "Error: No input file specified\n";
        print_usage();
        return 1;
    }

    if (option == -1) {
        std::cerr << "Error: No action specified (-S, -lexer, etc.)\n";
        print_usage();
        return 1;
    }

    // 打开输入文件
    FILE* input = fopen(input_file, "r");
    if (!input) {
        perror("Error opening input file");
        return 1;
    }

    // 设置输出流
    std::ostream& out = output_file ? *(new std::ofstream(output_file)) : std::cout;
    if (output_file && !static_cast<std::ofstream&>(out).is_open()) {
        std::cerr << "Error opening output file: " << output_file << "\n";
        fclose(input);
        return 1;
    }

    // 【1】词法分析
    if (option == 0) {
        out << "%-10s %-20s %-15s %-10s\n" << "line" << "token" << "type" << "value" << "\n";
        out << "------------------------------------------------------\n";
        
        yyin = input;
        int token;
        while ((token = yylex()) != 0) {
            // 需要适配dumpTokens到ostream或继续使用FILE*
            FILE* tmp = tmpfile();
            dumpTokens(tmp, token, line, yytext, yylval);
            rewind(tmp);
            char buffer[256];
            while (fgets(buffer, sizeof(buffer), tmp)) {
                out << buffer;
            }
            fclose(tmp);
        }
        
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    // 【2】语法分析
    yyin = input;
    yyparse();
    if (error_num > 0) {
        out << "Parser error in line " << line << "\n";
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    if (option == 1) {
        ASTroot->printAST(out, 0);
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    // 【3】语义分析
    ASTroot->TypeCheck();
    if (error_msgs.size() > 0) {
        out << "Semant error\n";
        for (auto msg : error_msgs) {
            out << msg << "\n";
        }
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    if (option == 2) {
        ASTroot->printAST(out, 0);
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    // 【4】IR生成
    ASTroot->codeIR();
    llvmIR.CFGInit();
    SimplifyCFGPass(&llvmIR).Execute();

    // 【5】优化
	// 提交到 oj 时需要默认优化全开
    // if (optimize) {
        TailCallElimPass(&llvmIR).Execute();
        DomAnalysis dom(&llvmIR);
        dom.Execute();
        (Mem2RegPass(&llvmIR, &dom)).Execute();
        SimplifyCFGPass(&llvmIR).EOBB();  // maybe invalid ?
        DomAnalysis inv_dom(&llvmIR);
        inv_dom.invExecute();
        (ADCEPass(&llvmIR, &inv_dom)).Execute();
        PeepholePass(&llvmIR).ImmResultReplaceExecute();
        SCCPPass(&llvmIR).Execute();
        SimplifyCFGPass(&llvmIR).RebuildCFG();
    // }

    if (option == 3) {
        llvmIR.printIR(out);
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    // 【6】后端
    if (option == 4 || option == 5) {
        MachineUnit* m_unit = new RiscV64Unit(&llvmIR);
        m_unit->SelectInstructionAndBuildCFG();
        
        if (option == 5) {
            RiscV64RegisterAllocTools regs;
            FastLinearScan(m_unit, &regs).Execute();
            m_unit->LowerStack();
        }
        
        RiscV64Printer(out, m_unit).emit();
        fclose(input);
        if (output_file) delete &out;
        return 0;
    }

    fclose(input);
    if (output_file) delete &out;
    return 0;
}