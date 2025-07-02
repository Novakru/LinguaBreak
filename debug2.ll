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
@n = global i32 0
define i32 @main()
{
L0:  ;
    %r1 = alloca [10 x i32]
    br label %L1
L1:  ;
    store i32 10, ptr @n
    %r4 = getelementptr [10 x i32], ptr %r1, i32 0, i32 0
    store i32 4, ptr %r4
    %r8 = getelementptr [10 x i32], ptr %r1, i32 0, i32 1
    store i32 3, ptr %r8
    %r12 = getelementptr [10 x i32], ptr %r1, i32 0, i32 2
    store i32 9, ptr %r12
    %r16 = getelementptr [10 x i32], ptr %r1, i32 0, i32 3
    store i32 2, ptr %r16
    %r20 = getelementptr [10 x i32], ptr %r1, i32 0, i32 4
    store i32 0, ptr %r20
    %r24 = getelementptr [10 x i32], ptr %r1, i32 0, i32 5
    store i32 1, ptr %r24
    %r28 = getelementptr [10 x i32], ptr %r1, i32 0, i32 6
    store i32 6, ptr %r28
    %r32 = getelementptr [10 x i32], ptr %r1, i32 0, i32 7
    store i32 5, ptr %r32
    %r36 = getelementptr [10 x i32], ptr %r1, i32 0, i32 8
    store i32 7, ptr %r36
    %r40 = getelementptr [10 x i32], ptr %r1, i32 0, i32 9
    store i32 8, ptr %r40
    %r46 = getelementptr [10 x i32], ptr %r1, i32 0
    %r47 = load i32, ptr @n
    br label %L7
L2:  ;
    %r66 = phi i32 [%r48,%L6],[%r63,%L5]
    %r50 = load i32, ptr @n
    %r51 = icmp slt i32 %r66,%r50
    br i1 %r51, label %L5, label %L6
L5:  ;
    %r56 = getelementptr [10 x i32], ptr %r1, i32 0, i32 %r66
    %r57 = load i32, ptr %r56
    call void @putint(i32 %r57)
    call void @putch(i32 10)
    %r63 = add i32 %r66,1
    br label %L2
L6:  ;
    ret i32 0
L7:  ;
    br label %L8
L8:  ;
    %r67 = sdiv i32 %r47,2
    %r68 = sub i32 %r67,1
    br label %L9
L9:  ;
    %r69 = phi i32 [%r68,%L1],[%r70,%L11]
    %r71 = icmp sgt i32 %r69,-1
    br i1 %r71, label %L10, label %L11
L10:  ;
    %r72 = sub i32 %r47,1
    %r73 = getelementptr i32, ptr %r46
    br label %L12
L11:  ;
    %r74 = sub i32 %r47,1
    br label %L13
L12:  ;
    br label %L17
L13:  ;
    %r75 = phi i32 [%r74,%L6],[%r76,%L11]
    %r77 = icmp sgt i32 %r75,0
    br i1 %r77, label %L14, label %L15
L14:  ;
    %r78 = getelementptr i32, ptr %r46
    %r79 = add i32 0,0
    br label %L16
L15:  ;
    ret i32 0
L16:  ;
    br label %L29
L17:  ;
    %r80 = mul i32 %r69,2
    %r81 = add i32 %r80,1
    br label %L18
L18:  ;
    %r82 = phi i32 [%r69,%L1],[%r83,%L19]
    %r84 = phi i32 [%r81,%L1],[%r85,%L19]
    %r86 = add i32 %r72,1
    %r87 = icmp slt i32 %r84,%r86
    br i1 %r87, label %L19, label %L20
L19:  ;
    %r88 = icmp slt i32 %r84,%r72
    br i1 %r88, label %L21, label %L22
L20:  ;
    ret i32 0
L21:  ;
    %r89 = getelementptr i32, ptr %r73, i32 %r84
    %r90 = load i32, ptr %r89
    %r91 = add i32 %r84,1
    %r92 = getelementptr i32, ptr %r73, i32 %r91
    %r93 = load i32, ptr %r92
    %r94 = icmp slt i32 %r90,%r93
    br i1 %r94, label %L23, label %L22
L22:  ;
    br label %L24
L23:  ;
    %r95 = add i32 %r84,1
    br label %L24
L24:  ;
    %r83 = phi i32 [%r84,%L8],[%r95,%L10]
    br label %L25
L25:  ;
    %r96 = getelementptr i32, ptr %r73, i32 %r82
    %r97 = load i32, ptr %r96
    %r98 = getelementptr i32, ptr %r73, i32 %r83
    %r99 = load i32, ptr %r98
    %r100 = icmp sgt i32 %r97,%r99
    br i1 %r100, label %L20, label %L26
L26:  ;
    %r101 = getelementptr i32, ptr %r73
    br label %L27
L27:  ;
    br label %L28
L28:  ;
    %r102 = getelementptr i32, ptr %r101, i32 %r82
    %r103 = load i32, ptr %r102
    %r104 = getelementptr i32, ptr %r101, i32 %r83
    %r105 = load i32, ptr %r104
    %r106 = getelementptr i32, ptr %r101, i32 %r82
    store i32 %r106, ptr %r105
    %r107 = getelementptr i32, ptr %r101, i32 %r83
    store i32 %r107, ptr %r103
    ret i32 0
L29:  ;
    %r108 = getelementptr i32, ptr %r78, i32 %r79
    %r109 = load i32, ptr %r108
    %r110 = getelementptr i32, ptr %r78, i32 %r75
    %r111 = load i32, ptr %r110
    %r112 = getelementptr i32, ptr %r78, i32 %r79
    store i32 %r111, ptr %r112
    %r113 = getelementptr i32, ptr %r78, i32 %r75
    store i32 %r109, ptr %r113
    ret i32 0
}
