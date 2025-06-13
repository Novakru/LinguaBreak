import argparse
import os


# Usage examples:
# python3 grade.py 3 1 last (测试lab3进阶要求，使用functional_test目录)
# python3 grade.py 5 0 curr (测试lab5基本要求，使用functional_test_curr目录)


parser = argparse.ArgumentParser()
parser.add_argument('lab', type=int, default=1)
parser.add_argument('is_advance', type=int, default=1)
parser.add_argument('test_version', choices=['last', 'curr'], default='last', 
                   help="'last' for functional_test, 'curr' for functional_test_curr")
args = parser.parse_args()

# Determine test directory based on test_version argument
test_dir = "functional_test_curr" if args.test_version == "curr" else "functional_test"

if(args.lab == 1):
    print("lab1和lab2不支持自动评测, 请自行检查输入和输出")
elif(args.lab == 2):
    os.system("rm -rf test_output/functional_testIR/*.ll")
    if(args.is_advance==False):
        os.system(f"python3 test.py testcase/{test_dir}/Basic test_output/functional_testIR 0 semant")
    else:
        os.system(f"python3 test.py testcase/{test_dir}/Advanced test_output/functional_testIR 0 semant")
elif(args.lab == 3):
    os.system("rm -rf test_output/functional_testIR/*.ll")
    if(args.is_advance==False):
        os.system(f"python3 test.py testcase/{test_dir}/Basic test_output/functional_testIR 0 llvm")
    else:
        os.system(f"python3 test.py testcase/{test_dir}/Advanced test_output/functional_testIR 0 llvm")
elif(args.lab == 4):
    os.system("rm -rf test_output/functional_testIR/*.ll")
    if(args.is_advance==False):
        os.system(f"python3 test.py testcase/{test_dir}/Basic test_output/functional_testIR 1 llvm")
    else:
        os.system(f"python3 test.py testcase/{test_dir}/Advanced test_output/functional_testIR 1 llvm")
elif(args.lab == 5):
    os.system("rm -rf test_output/functional_testAsm/*.s")
    if(args.is_advance==False):
        os.system(f"python3 test.py testcase/{test_dir}/Basic test_output/functional_testSelect 1 select")
    else:
        os.system(f"python3 test.py testcase/{test_dir}/Advanced test_output/functional_testSelect 1 select")
elif(args.lab == 6):
    os.system("rm -rf test_output/functional_testAsm/*.s")
    if(args.is_advance==False):
        os.system(f"python3 test.py testcase/{test_dir}/Basic test_output/functional_testAsm 1 target")
    else:
        os.system(f"python3 test.py testcase/{test_dir}/Advanced test_output/functional_testAsm 1 target")
else:
    print("未知lab, 请检查输入")