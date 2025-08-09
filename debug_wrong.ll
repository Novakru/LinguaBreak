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
@a = global i32 0
@sum = global i32 0
@count = global i32 0
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = add i32 0,0
    store i32 %r0, ptr @sum
    %r1 = call i32 @getA()
    store i32 %r1, ptr @a
    %r4 = add i32 %r0,%r1
    store i32 %r4, ptr @sum
    call void @f1(i32 %r1)
    call void @f2()
    call void @f3()
    br label %L2
L2:  ;
    %r23 = add i32 1,0
    %r24 = icmp ne i32 %r23,0
    br i1 %r24, label %L6, label %L37
L6:  ;
    %r25 = add i32 1,0
    %r26 = icmp ne i32 %r25,0
    br i1 %r26, label %L9, label %L37
L9:  ;
    %r28 = add i32 0,0
    br label %L10
L10:  ;
    %r58 = phi i32 [%r1,%L9],[%r61,%L28],[%r48,%L31]
    %r55 = phi i32 [%r28,%L9],[%r46,%L28],[%r51,%L31]
    %r30 = add i32 3,0
    %r31 = icmp slt i32 %r55,%r30
    br i1 %r31, label %L13, label %L37
L13:  ;
    br label %L14
L14:  ;
    %r61 = phi i32 [%r58,%L13],[%r37,%L23]
    %r32 = add i32 1,0
    %r33 = icmp ne i32 %r32,0
    br i1 %r33, label %L18, label %L25
L18:  ;
    %r34 = add i32 1,0
    %r35 = icmp ne i32 %r34,0
    br i1 %r35, label %L21, label %L23
L21:  ;
    call void @f1(i32 %r61)
    call void @f2()
    call void @f3()
    br label %L25
L23:  ;
    %r37 = call i32 @getA()
    br label %L14
L25:  ;
    %r39 = add i32 1,0
    %r40 = icmp eq i32 %r55,%r39
    br i1 %r40, label %L28, label %L30
L28:  ;
    %r42 = call i32 @getA()
    call void @f1(i32 %r42)
    call void @f2()
    call void @f3()
    %r45 = add i32 1,0
    %r46 = add i32 %r55,%r45
    br label %L10
L30:  ;
    call void @f1(i32 %r61)
    call void @f2()
    call void @f3()
    br label %L31
L31:  ;
    %r48 = call i32 @getA()
    %r50 = add i32 1,0
    %r51 = add i32 %r55,%r50
    br label %L10
L37:  ;
    %r52 = load i32, ptr @sum
    call void @putint(i32 %r52)
    %r53 = add i32 0,0
    ret i32 %r53
}
define void @f3()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = call i32 @getA()
    %r2 = load i32, ptr @sum
    %r4 = add i32 %r2,%r1
    store i32 %r4, ptr @sum
    %r8 = add i32 %r4,%r1
    store i32 %r8, ptr @sum
    %r13 = add i32 %r8,%r1
    store i32 %r13, ptr @sum
    ret void
}
define void @f2()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load i32, ptr @sum
    %r1 = load i32, ptr @a
    %r2 = add i32 %r0,%r1
    store i32 %r2, ptr @sum
    %r4 = call i32 @getA()
    %r7 = add i32 %r2,%r1
    store i32 %r7, ptr @sum
    ret void
}
define void @f1(i32 %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r2 = load i32, ptr @sum
    %r4 = add i32 %r2,%r0
    store i32 %r4, ptr @sum
    %r5 = call i32 @getA()
    %r8 = add i32 %r4,%r5
    store i32 %r8, ptr @sum
    br label %L2
L2:  ;
    %r9 = add i32 1,0
    %r10 = icmp ne i32 %r9,0
    br i1 %r10, label %L5, label %L6
L5:  ;
    %r12 = call i32 @getA()
    %r13 = load i32, ptr @sum
    %r15 = add i32 %r13,%r12
    store i32 %r15, ptr @sum
    br label %L6
L6:  ;
    %r16 = load i32, ptr @sum
    %r18 = add i32 %r16,%r5
    store i32 %r18, ptr @sum
    %r21 = add i32 %r18,%r5
    store i32 %r21, ptr @sum
    ret void
}
define i32 @getA()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = load i32, ptr @count
    %r1 = add i32 1,0
    %r2 = add i32 %r0,%r1
    store i32 %r2, ptr @count
    ret i32 %r2
}
