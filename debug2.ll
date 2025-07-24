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
    %r114 = alloca [2 x [3 x [4 x i32]]]
    %r96 = alloca [5 x [3 x i32]]
    %r64 = alloca [5 x [3 x i32]]
    %r33 = alloca [5 x [3 x i32]]
    %r2 = alloca [5 x [3 x i32]]
    %r1 = alloca [5 x [3 x i32]]
    br label %L1
L1:  ;
    call void @llvm.memset.p0.i32(ptr %r1,i8 0,i32 60,i1 0)
    call void @llvm.memset.p0.i32(ptr %r2,i8 0,i32 60,i1 0)
    %r3 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 0, i32 0
    store i32 1, ptr %r3
    %r5 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 0, i32 1
    store i32 2, ptr %r5
    %r7 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 0, i32 2
    store i32 3, ptr %r7
    %r9 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 1, i32 0
    store i32 4, ptr %r9
    %r11 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 1, i32 1
    store i32 5, ptr %r11
    %r13 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 1, i32 2
    store i32 6, ptr %r13
    %r15 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 2, i32 0
    store i32 7, ptr %r15
    %r17 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 2, i32 1
    store i32 8, ptr %r17
    %r19 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 2, i32 2
    store i32 9, ptr %r19
    %r21 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 3, i32 0
    store i32 10, ptr %r21
    %r23 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 3, i32 1
    store i32 11, ptr %r23
    %r25 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 3, i32 2
    store i32 12, ptr %r25
    %r27 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 4, i32 0
    store i32 13, ptr %r27
    %r29 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 4, i32 1
    store i32 14, ptr %r29
    %r31 = getelementptr [5 x [3 x i32]], ptr %r2, i32 0, i32 4, i32 2
    store i32 15, ptr %r31
    call void @llvm.memset.p0.i32(ptr %r33,i8 0,i32 60,i1 0)
    %r34 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 0, i32 0
    store i32 1, ptr %r34
    %r36 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 0, i32 1
    store i32 2, ptr %r36
    %r38 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 0, i32 2
    store i32 3, ptr %r38
    %r40 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 1, i32 0
    store i32 4, ptr %r40
    %r42 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 1, i32 1
    store i32 5, ptr %r42
    %r44 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 1, i32 2
    store i32 6, ptr %r44
    %r46 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 2, i32 0
    store i32 7, ptr %r46
    %r48 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 2, i32 1
    store i32 8, ptr %r48
    %r50 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 2, i32 2
    store i32 9, ptr %r50
    %r52 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 3, i32 0
    store i32 10, ptr %r52
    %r54 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 3, i32 1
    store i32 11, ptr %r54
    %r56 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 3, i32 2
    store i32 12, ptr %r56
    %r58 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 4, i32 0
    store i32 13, ptr %r58
    %r60 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 4, i32 1
    store i32 14, ptr %r60
    %r62 = getelementptr [5 x [3 x i32]], ptr %r33, i32 0, i32 4, i32 2
    store i32 15, ptr %r62
    call void @llvm.memset.p0.i32(ptr %r64,i8 0,i32 60,i1 0)
    %r65 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 0, i32 0
    store i32 1, ptr %r65
    %r67 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 0, i32 1
    store i32 2, ptr %r67
    %r69 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 0, i32 2
    store i32 3, ptr %r69
    %r71 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 1, i32 0
    store i32 4, ptr %r71
    %r73 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 1, i32 1
    store i32 5, ptr %r73
    %r75 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 1, i32 2
    store i32 6, ptr %r75
    %r77 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 2, i32 0
    store i32 7, ptr %r77
    %r79 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 2, i32 1
    store i32 8, ptr %r79
    %r81 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 2, i32 2
    store i32 9, ptr %r81
    %r83 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 3, i32 0
    store i32 10, ptr %r83
    %r85 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 3, i32 1
    store i32 11, ptr %r85
    %r87 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 3, i32 2
    store i32 12, ptr %r87
    %r89 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 4, i32 0
    store i32 13, ptr %r89
    %r91 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 4, i32 1
    store i32 14, ptr %r91
    %r93 = getelementptr [5 x [3 x i32]], ptr %r64, i32 0, i32 4, i32 2
    store i32 15, ptr %r93
    call void @llvm.memset.p0.i32(ptr %r96,i8 0,i32 60,i1 0)
    %r97 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 0, i32 0
    store i32 1, ptr %r97
    %r99 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 0, i32 1
    store i32 2, ptr %r99
    %r101 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 0, i32 2
    store i32 3, ptr %r101
    %r103 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 1, i32 0
    store i32 4, ptr %r103
    %r105 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 2, i32 0
    store i32 7, ptr %r105
    %r107 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 3, i32 0
    store i32 10, ptr %r107
    %r109 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 3, i32 1
    store i32 11, ptr %r109
    %r111 = getelementptr [5 x [3 x i32]], ptr %r96, i32 0, i32 3, i32 2
    store i32 12, ptr %r111
    call void @llvm.memset.p0.i32(ptr %r114,i8 0,i32 96,i1 0)
    %r115 = getelementptr [2 x [3 x [4 x i32]]], ptr %r114, i32 0, i32 0, i32 0, i32 0
    store i32 1, ptr %r115
    %r117 = getelementptr [2 x [3 x [4 x i32]]], ptr %r114, i32 0, i32 0, i32 0, i32 1
    store i32 2, ptr %r117
    %r119 = getelementptr [2 x [3 x [4 x i32]]], ptr %r114, i32 0, i32 0, i32 0, i32 2
    store i32 3, ptr %r119
    %r121 = getelementptr [2 x [3 x [4 x i32]]], ptr %r114, i32 0, i32 0, i32 0, i32 3
    store i32 4, ptr %r121
    %r123 = getelementptr [2 x [3 x [4 x i32]]], ptr %r114, i32 0, i32 0, i32 1, i32 0
    store i32 5, ptr %r123
    ret i32 4
}
