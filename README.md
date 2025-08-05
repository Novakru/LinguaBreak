# 2025编译系统实现赛（RiscV赛道）LinguaBreak技术文档

## 编译器语言系统
- 前端：SysY2022 （详见[SysY2022语言定义](https://gitlab.eduxiji.net/csc1/nscscc/compiler2023/-/blob/master/SysY2022%E8%AF%AD%E8%A8%80%E5%AE%9A%E4%B9%89-V1.pdf)）

- 中端：LLVM IR

- 后端：Risc-V（64位）

## 开发环境及工具
- Clang 15.0+

- riscv64-unknown-linux-gnu-gcc 12.2+

- qemu-riscv64 7.0+

- flex 2.6+

- bison 3.8+

## 系统架构
### 工作流程
<img src="./documents/framework.png"></img>

### 中端优化技术
#### Analysis Pass
- Build CFG

- Build Dominator Tree

- Alias Analysis

- Memdep

- Loop Analysis

- Scalar Evolution

#### Transform Pass
- mem2reg

- Simplify CFG

- Aggressive Dead Code Elimination

- Function Inline

- Peehpole

- Sparse Conditional Constant Propagation

- Tail Call Elimination

- Basic Common SubExpression elimination

- Loop Invariant Code Motion

- Loop Strength Reduce

- Loop Simplify

- Loop Rotate

### 后端优化技术
- Peephole

- Strength Reduce

## 参考仓库

本项目框架参考自[南开大学2024秋季编译原理课程教学实验](https://github.com/yuhuifishash/NKU-Compilers2024-RV64GC)

