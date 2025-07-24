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
@N = global i32 100010
@M = global i32 3000000
@x = global [100010x i32] zeroinitializer
@y = global [3000000x i32] zeroinitializer
@v = global [3000000x i32] zeroinitializer
@a = global [100010x i32] zeroinitializer
@b = global [100010x i32] zeroinitializer
@c = global [100010x i32] zeroinitializer
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = getelementptr [100010 x i32], ptr @x, i32 0
    %r2 = call i32 @getarray(ptr %r1)
    %r4 = sub i32 %r2,1
    %r6 = getelementptr [3000000 x i32], ptr @y, i32 0
    %r7 = call i32 @getarray(ptr %r6)
    %r8 = getelementptr [3000000 x i32], ptr @v, i32 0
    %r9 = call i32 @getarray(ptr %r8)
    %r10 = getelementptr [100010 x i32], ptr @a, i32 0
    %r11 = call i32 @getarray(ptr %r10)
    call void @_sysy_starttime(i32 39)
    %r39 = icmp slt i32 0,5
    br i1 %r39, label %L7, label %L6
L2:  ;
    %r41 = phi i32 [%r32,%L2],[0,%L7]
    call void @spmv(i32 %r4,ptr %r1,ptr %r6,ptr %r8,ptr %r10,ptr %r23)
    call void @spmv(i32 %r4,ptr %r1,ptr %r6,ptr %r8,ptr %r23,ptr %r10)
    %r32 = add i32 %r41,1
    %r40 = icmp slt i32 %r32,5
    br i1 %r40, label %L2, label %L6
L6:  ;
    %r38 = phi i32 [0,%L1],[%r32,%L2]
    call void @_sysy_stoptime(i32 47)
    %r35 = getelementptr [100010 x i32], ptr @b, i32 0
    call void @putarray(i32 %r4,ptr %r35)
    ret i32 0
L7:  ;
    %r23 = getelementptr [100010 x i32], ptr @b, i32 0
    br label %L2
}
define void @spmv(i32 %r0,ptr %r1,ptr %r2,ptr %r3,ptr %r4,ptr %r5)
{
L0:  ;
    br label %L2
L2:  ;
    %r98 = phi i32 [%r26,%L5],[0,%L0]
    %r19 = icmp slt i32 %r98,%r0
    br i1 %r19, label %L5, label %L7
L5:  ;
    %r22 = getelementptr i32, ptr %r5, i32 %r98
    store i32 0, ptr %r22
    %r26 = add i32 %r98,1
    br label %L2
L7:  ;
    %r97 = phi i32 [0,%L2],[%r37,%L16]
    %r30 = icmp slt i32 %r97,%r0
    br i1 %r30, label %L10, label %L21
L10:  ;
    %r32 = getelementptr i32, ptr %r1, i32 %r97
    %r33 = load i32, ptr %r32
    %r37 = add i32 %r97,1
    %r38 = getelementptr i32, ptr %r1, i32 %r37
    %r39 = load i32, ptr %r38
    br label %L11
L11:  ;
    %r96 = phi i32 [%r33,%L10],[%r57,%L14]
    %r40 = icmp slt i32 %r96,%r39
    br i1 %r40, label %L14, label %L15
L14:  ;
    %r42 = getelementptr i32, ptr %r2, i32 %r96
    %r43 = load i32, ptr %r42
    %r44 = getelementptr i32, ptr %r5, i32 %r43
    %r45 = load i32, ptr %r44
    %r47 = getelementptr i32, ptr %r3, i32 %r96
    %r48 = load i32, ptr %r47
    %r49 = add i32 %r45,%r48
    %r52 = load i32, ptr %r42
    %r53 = getelementptr i32, ptr %r5, i32 %r52
    store i32 %r49, ptr %r53
    %r57 = add i32 %r96,1
    br label %L11
L15:  ;
    %r60 = load i32, ptr %r32
    %r66 = load i32, ptr %r38
    br label %L16
L16:  ;
    %r94 = phi i32 [%r60,%L15],[%r90,%L19]
    %r67 = icmp slt i32 %r94,%r66
    br i1 %r67, label %L19, label %L7
L19:  ;
    %r69 = getelementptr i32, ptr %r2, i32 %r94
    %r70 = load i32, ptr %r69
    %r71 = getelementptr i32, ptr %r5, i32 %r70
    %r72 = load i32, ptr %r71
    %r74 = getelementptr i32, ptr %r3, i32 %r94
    %r75 = load i32, ptr %r74
    %r77 = getelementptr i32, ptr %r4, i32 %r97
    %r78 = load i32, ptr %r77
    %r80 = sub i32 %r78,1
    %r81 = mul i32 %r75,%r80
    %r82 = add i32 %r72,%r81
    %r85 = load i32, ptr %r69
    %r86 = getelementptr i32, ptr %r5, i32 %r85
    store i32 %r82, ptr %r86
    %r90 = add i32 %r94,1
    br label %L16
L21:  ;
    ret void
}
