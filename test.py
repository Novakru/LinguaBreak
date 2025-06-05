import argparse
import subprocess
import os

def execute(command):
    return subprocess.run(command, capture_output=True, text=True)

def execute_with_stdin_out(command):
    return os.system(command) % 255

def check_file(file1, file2):
    try:
        result = execute(["diff", "--strip-trailing-cr", file1, file2, "-b"])
    except Exception as e:
        print(f"\033[91mUnknown Error on \033[0m{file1}, \033[91mPlease check your output file\033[0m")
        return 1
    else:
        return result.returncode

def add_returncode(file, ret):
    need_newline = False
    with open(file, "r") as f:
        try:
            content = f.read()
        except Exception as e:
            print(f"\033[91mUnknown Error on \033[0m{file}, \033[91mPlease check your output file\033[0m")
            return False
        else:
            if len(content) > 0:
                if not content.endswith("\n"):
                    need_newline = True

    with open(file, "a+") as f:
        if need_newline:
            f.write("\n")
        f.write(str(ret))
        f.write("\n")
    return False

def execute_compilation(input_file, output_file, compile_option, opt_level):
    cmd = ["timeout", "100", "./bin/SysYc", input_file, compile_option, output_file]
    if compile_option in ["-llvm", "-select", "-target"]:
        cmd.append(opt_level)
    # print(f"执行命令: {' '.join(cmd)}")
    result = execute(cmd)
    return result.returncode == 0

def execute_llvm(input_file, output_file, stdin, stdout, testout, opt_level):
    if not execute_compilation(input_file, output_file, "-llvm", opt_level):
        print(f"\033[93mCompile Error on \033[0m{input_file}")
        return 0

    if execute(["clang-15", output_file, "-c", "-o", "tmp.o", "-w"]).returncode != 0:
        print(f"\033[93mOutput Error on \033[0m{input_file}")
        return 0

    if execute(["clang-15", "-static", "tmp.o", "lib/libsysy_x86.a"]).returncode != 0:
        execute(["rm", "-rf", "tmp.o"])
        print(f"\033[93mLink Error on \033[0m{input_file}")
        return 0

    execute(["rm", "-rf", "tmp.o"])

    cmd = f"timeout 10 ./a.out {'< '+stdin if stdin != 'none' else ''} > {testout} 2>/dev/null"
    result = execute_with_stdin_out(cmd)

    if result == 124:
        print(f"\033[93mTime Limit Exceed on \033[0m{input_file}")
        return 0
    elif result == 139:
        print(f"\033[95mRunTime Error on \033[0m{input_file}")
        return 0

    add_returncode(testout, result)

    if check_file(testout, stdout) == 0:
        print(f"\033[92mAccept \033[0m{input_file}")
        return 1
    else:
        print(f"\033[91mWrong Answer on \033[0m{input_file}")
        return 0

def execute_asm(input_file, output_file, stdin, stdout, testout, opt_level):
    if not execute_compilation(input_file, output_file, "-target", opt_level):
        print(f"\033[93mCompile Error on \033[0m{input_file}")
        return 0

    if execute(["riscv64-unknown-linux-gnu-gcc", output_file, "-c", "-o", "tmp.o", "-w"]).returncode != 0:
        print(f"\033[93mOutput Error on \033[0m{input_file}")
        return 0

    if execute(["riscv64-unknown-linux-gnu-gcc", "-static", "tmp.o", "lib/libsysy_rv.a"]).returncode != 0:
        execute(["rm", "-rf", "tmp.o"])
        print(f"\033[93mLink Error on \033[0m{input_file}")
        return 0

    execute(["rm", "-rf", "tmp.o"])

    cmd = f"timeout 10 qemu-riscv64 ./a.out {'< '+stdin if stdin != 'none' else ''} > {testout} 2>/dev/null"
    result = execute_with_stdin_out(cmd)

    if result == 124:
        print(f"\033[93mTime Limit Exceed on \033[0m{input_file}")
        return 0
    elif result == 139:
        print(f"\033[95mRunTime Error on \033[0m{input_file}")
        return 0

    add_returncode(testout, result)

    if check_file(testout, stdout) == 0:
        print(f"\033[92mAccept \033[0m{input_file}")
        return 1
    else:
        print(f"\033[91mWrong Answer on \033[0m{input_file}")
        return 0

parser = argparse.ArgumentParser()
parser.add_argument('input_folder')
parser.add_argument('output_folder')
parser.add_argument('opt', choices=['0', '1'], default='0', help="optimization level")
parser.add_argument('step', choices=["lexer", "parser", "semant", "llvm", "select", "target"])
args = parser.parse_args()

input_folder = args.input_folder
output_folder = args.output_folder
step = args.step
opt_level = "-O" + str(args.opt)

ac = 0
total = 0

for file in os.listdir(input_folder):
    if file.endswith(".sy"):
        total += 1
        name = file[:-3]
        input_path = f"{input_folder}/{file}"
        output_path = f"{output_folder}/{name}.out"

        stdin_file = f"{input_folder}/{name}.in"
        stdout_file = f"{input_folder}/{name}.out"

        stdin = stdin_file if os.path.exists(stdin_file) else "none"
        testout = "tmp.out"

        if step == "llvm":
            output_file = f"{output_folder}/{name}.ll"
            ac += execute_llvm(input_path, output_file, stdin, stdout_file, testout, opt_level)
        elif step == "target":
            output_file = f"{output_folder}/{name}.s"
            ac += execute_asm(input_path, output_file, stdin, stdout_file, testout, opt_level)
        else:
            if execute_compilation(input_path, output_path, f"-{step}", opt_level):
                print(f"\033[92mAccept (Compilation) \033[0m{input_path}")
                ac += 1
            else:
                print(f"\033[93mCompile Error on \033[0m{input_path}")

print(f"{step.upper()}Test-Grade:{ac}/{total}")

os.system("rm -f tmp.out a.out")