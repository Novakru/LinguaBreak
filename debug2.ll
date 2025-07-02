declare i32 @getint()
declare i32 @getch()
declare float @getfloat()
declare i32 @getarray(ptr)
declare i32 @getfarray(ptr)
declare void @putint(i32)
declare void @putch(i32)
declare void @putfloat(float)
declare void @putarray(i32,ptr)
declare void @putfarray(i32,ptr)
declare void @_sysy_starttime(i32)
declare void @_sysy_stoptime(i32)
declare void @llvm.memset.p0.i32(ptr,i8,i32,i1)
declare i32 @llvm.umax.i32(i32,i32)
declare i32 @llvm.umin.i32(i32,i32)
declare i32 @llvm.smax.i32(i32,i32)
declare i32 @llvm.smin.i32(i32,i32)
define i32 @add(i32 %r0,i32 %r1)
{
L0:  ;
    br label %L2
L2:  ;
    %r6 = icmp sgt i32 %r0,0
    br i1 %r6, label %L5, label %L6
L5:  ;
    %r9 = add i32 %r0,%r1
    ret i32 %r9
L6:  ;
    %r12 = add i32 %r0,%r1
    %r14 = add i32 %r12,%r1
    ret i32 %r14
}
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r7 = call i32 @add(i32 3,i32 4)
    ret i32 %r7
}
