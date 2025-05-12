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

extern FILE *yyin;
extern char *yytext;
extern YYSTYPE yylval;
extern int error_num;
extern Program ASTroot;
extern LLVMIR llvmIR;
extern int yylex();
extern void dumpTokens(FILE* output, int token, int line_number, char *yytext, YYSTYPE yylval);
extern std::vector<std::string> error_msgs;//新增
IdTable id_table;//新增
int line = 1;

// option table 
#define nr_options 5 
const char *valid_options[] = {"-lexer", "-parser", "-semant", "-llvm", "-target", "-O1"};

int main(int argc, char** argv) {
	/* argc = 4 or 5 : ./bin/SysYc input_file -llvm output_file [-O1] */
    if (argc != 4 && argc != 5) return 1;

    FILE* input = fopen(argv[1], "r");
    FILE* output = stdout;
    
	/* option check */
	bool is_valid_option = false;
	for (int i = 0; i < nr_options; i++) {
		if (strcmp(argv[2], valid_options[i]) == 0) {
			is_valid_option = true;
			break;
		}
	}

	if (!output || !input || !is_valid_option) {
		perror("can't open input/output file or input an invalid option.");
		return 1;
	} 
	
	
	/* 【1】lexer */
	int token;
	yyin = input;
	if (strcmp(argv[2], "-lexer") == 0) {
		output = fopen(argv[3], "w");
		fprintf(output, "%-10s %-20s %-15s %-10s\n", "line", "token", "type", "value");
		fprintf(output, "------------------------------------------------------\n");
		while ((token = yylex()) != 0) {
        	dumpTokens(output, token, line, yytext, yylval);
	    }
		fclose(input), fclose(output);
		return 0;
	} 

	std::ofstream fout(argv[3]);
	if (!fout) {
        std::cerr << "Could not open file: " << argv[3] << std::endl;
        return 1;
    }
	/* 【2】parser */
	yyparse();
	if (error_num > 0) {
		fout<<"Parser error in line "<<line<<std::endl;
        fclose(input);fout.close();
        return 0;
    }
	if(strcmp(argv[2], "-parser") == 0) {
		ASTroot->printAST(fout,0);
		return 0;
	}

	/* 【3】 semant */
	ASTroot->TypeCheck();
    if (error_msgs.size() > 0) {
		fprintf(output,"Semant error\n");
        for (auto msg : error_msgs) {
            fout << msg << std::endl;
        }
        fclose(input);fout.close();
        return 0;
    }
	if(strcmp(argv[2], "-semant") == 0) {
		ASTroot->printAST(fout,0);
		return 0;
	}

	/* 【4】 irgen */
	ASTroot->codeIR();
	llvmIR.CFGInit();
	SimplifyCFGPass(&llvmIR).Execute();//已消除不可达指令和不可达块

	/* 【5】 opt */
    if (argc == 5 && strcmp(argv[4], "-O1") == 0) {
        // mem2reg
        DomAnalysis dom(&llvmIR);
        dom.Execute();   
		(Mem2RegPass(&llvmIR, &dom)).Execute();
		SimplifyCFGPass(&llvmIR).EOBB();//消除只有一条 br_uncond 指令的基本块（仅消除由mem2reg引入的，位于头部）

		// adce
		DomAnalysis inv_dom(&llvmIR);
		inv_dom.invExecute();
		(ADCEPass(&llvmIR, &inv_dom)).Execute();//消除死代码（含冗余phi指令和 br_uncond_block）
    }

	if (strcmp(argv[2], "-llvm") == 0) {
        llvmIR.printIR(fout);
        fout.close();
        return 0;
    }

	/* parser and so on */
	// if (strcmp(argv[2], "-parser") == 0) {
	// 	TODO("-parser compile option is not supported now.");
	// } else if (strcmp(argv[2], "-semant") == 0) {
	// 	TODO("-semant compile option is not supported now.");
	// } else if (strcmp(argv[2], "-llvm") == 0) {
	// 	TODO("-llvm compile option is not supported now.");
	// } else if (strcmp(argv[2], "-target") == 0) {
	// 	TODO("-target compile option is not supported now.");
	// } else {
	// 	perror("invalid compile option.");
	// }

    fclose(input);
    if (output != stdout) fclose(output);
    return 0;
}