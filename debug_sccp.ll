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
L0:  ;
    br label %L1
L1:  ;
    br label %L2
L2:  ;
    br label %L3
L3:  ;
    br label %L4
L4:  ;
    %r13 = phi i32 [4,%L3],[%r14,%L5]
    %r15 = icmp slt i32 %r13,75
    br i1 %r15, label %L6, label %L7
L5:  ;
    %r14 = phi i32 [%r22,%L11],[%r13,%L6]
    br label %L4
L6:  ;
    %r16 = icmp slt i32 %r13,100
    br i1 %r16, label %L8, label %L5
L7:  ;
    %r6 = add i32 %r13,0
    call void @putint(i32 %r6)
    ret i32 0
L8:  ;
    %r17 = add i32 %r13,42
    br label %L9
L9:  ;
    %r18 = icmp sgt i32 %r17,99
    br i1 %r18, label %L10, label %L11
L10:  ;
    br label %L14
L11:  ;
    %r22 = phi i32 [%r21,%L13],[%r17,%L9]
    br label %L5
L12:  ;
    br label %L13
L13:  ;
    %r21 = phi i32 [168,%L12],[%r17,%L15]
    br label %L11
L14:  ;
    br label %L15
L15:  ;
    br label %L12
}
