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
@global_int = global i32 303
@global_float = global float 0x40091eb860000000
@space = global i32 32
@maxN = global i32 10000
@sorted_array = global [10000x i32] zeroinitializer
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    call void @_sysy_starttime(i32 90)
    %r34 = getelementptr i32, ptr @sorted_array, i32 0
    br label %L3
L3:  ;
    %r9 = phi ptr [%r34,%L1],[%r35,%L4]
    %r7 = phi i32 [0,%L1],[%r33,%L4]
    %r4 = phi i32 [0,%L1],[%r5,%L4]
    %r6 = icmp slt i32 %r4,10000
    br i1 %r6, label %L4, label %L5
L4:  ;
    %r8 = srem i32 %r7,10000
    store i32 %r8, ptr %r9
    %r5 = add i32 %r4,1
    %r33 = add i32 %r7,303
    %r35 = getelementptr i32, ptr %r9, i32 1
    br label %L3
L5:  ;
    %r10 = getelementptr [10000 x i32], ptr @sorted_array, i32 0
    call void @bubble_sort(ptr %r10,i32 10000)
    br label %L8
L8:  ;
    %r15 = phi i32 [0,%L5],[%r16,%L9]
    %r17 = phi i32 [9999,%L5],[%r18,%L9]
    %r19 = icmp sle i32 %r15,%r17
    br i1 %r19, label %L10, label %L11
L9:  ;
    %r16 = phi i32 [%r20,%L12],[%r15,%L13]
    %r18 = phi i32 [%r17,%L12],[%r21,%L13]
    br label %L8
L10:  ;
    %r22 = add i32 %r15,%r17
    %r23 = sdiv i32 %r22,2
    %r26 = getelementptr [10000 x i32], ptr @sorted_array, i32 0, i32 %r23
    %r27 = load i32, ptr %r26
    %r28 = icmp eq i32 %r27,303
    br i1 %r28, label %L11, label %L15
L11:  ;
    %r24 = phi i32 [%r23,%L10],[-1,%L8]
    call void @putint(i32 303)
    call void @putch(i32 32)
    call void @putint(i32 %r24)
    call void @putch(i32 32)
    call void @putint(i32 15)
    call void @putch(i32 32)
    call void @putint(i32 5)
    call void @putch(i32 32)
    call void @putint(i32 50)
    call void @putch(i32 32)
    call void @putint(i32 2)
    call void @putch(i32 32)
    call void @putint(i32 0)
    call void @putch(i32 32)
    call void @_sysy_stoptime(i32 94)
    ret i32 0
L12:  ;
    %r20 = add i32 %r23,1
    br label %L9
L13:  ;
    %r21 = sub i32 %r23,1
    br label %L9
L15:  ;
    %r30 = load i32, ptr %r26
    %r31 = icmp slt i32 %r30,303
    br i1 %r31, label %L12, label %L13
}
define void @bubble_sort(ptr %r0,i32 %r1)
{
L0:  ;
    br label %L19
L2:  ;
    %r56 = phi i32 [%r53,%L15],[0,%L19]
    %r14 = icmp slt i32 %r56,%r13
    br i1 %r14, label %L17, label %L16
L6:  ;
    %r27 = phi ptr [%r61,%L17],[%r62,%L14]
    %r22 = phi ptr [%r59,%L17],[%r60,%L14]
    %r55 = phi i32 [%r26,%L14],[0,%L17]
    %r20 = icmp slt i32 %r55,%r13
    br i1 %r20, label %L10, label %L15
L10:  ;
    %r23 = load i32, ptr %r22
    %r26 = add i32 %r55,1
    %r28 = load i32, ptr %r27
    %r29 = icmp sgt i32 %r23,%r28
    br i1 %r29, label %L13, label %L14
L13:  ;
    %r33 = load i32, ptr %r22
    %r38 = load i32, ptr %r27
    store i32 %r38, ptr %r22
    store i32 %r33, ptr %r27
    br label %L14
L14:  ;
    %r60 = getelementptr i32, ptr %r22, i32 1
    %r62 = getelementptr i32, ptr %r27, i32 1
    br label %L6
L15:  ;
    %r53 = add i32 %r56,1
    br label %L2
L16:  ;
    ret void
L17:  ;
    %r59 = getelementptr i32, ptr %r0, i32 0
    %r61 = getelementptr i32, ptr %r0, i32 1
    br label %L6
L19:  ;
    %r13 = sub i32 %r1,1
    br label %L2
}
