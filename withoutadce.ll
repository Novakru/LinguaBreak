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
define i32 @main()
{
L0:  ;L0,  idom:0 inv_idom:7
    br label %L2
L2:  ;L1,  idom:1 inv_idom:7
    br label %L6
L5:  ;L3,  idom:3 DF:7 inv_idom:7 inv_DF:2
    br label %L7
L6:  ;L4,  idom:4 DF:7 inv_idom:7 inv_DF:2
    br label %L7
L7:  ;L5, L6,  idom:2 inv_idom:0
    %r36 = phi i32 [-1,%L5],[4,%L6]
    call void @putint(i32 %r36)
    ret i32 0
}
