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
@W = global i32 12
@H = global i32 12
@N = global i32 24
@PI = global float 0x400921fb60000000
@TWO_PI = global float 0x401921fb60000000
@MAX_STEP = global i32 10
@MAX_DISTANCE = global float 0x4000000000000000
@EPSILON = global float 0x3eb0c6f7a0000000
@RAND_MAX = global i32 100000006
@seed = global i32 0
define void @write_pgm()
{
L0:  ;
    br label %L1
L1:  ;
    call void @putch(i32 80)
    call void @putch(i32 50)
    call void @putch(i32 10)
    call void @putint(i32 12)
    call void @putch(i32 32)
    call void @putint(i32 12)
    call void @putch(i32 32)
    call void @putint(i32 255)
    call void @putch(i32 10)
    br label %L2
L2:  ;
    %r56 = phi i32 [0,%L1],[%r50,%L15]
    %r13 = icmp slt i32 %r56,12
    br i1 %r13, label %L6, label %L16
L6:  ;
    %r54 = phi i32 [%r46,%L14],[0,%L2]
    %r18 = icmp slt i32 %r54,12
    br i1 %r18, label %L9, label %L15
L9:  ;
    %r21 = sitofp i32 %r54 to float
    %r24 = sitofp i32 %r56 to float
    %r29 = fdiv float %r21,0x4028000000000000
    %r33 = fdiv float %r24,0x4028000000000000
    %r34 = call float @sample(float %r29,float %r33)
    %r36 = fmul float %r34,0x406fe00000000000
    %r37 = fptosi float %r36 to i32
    %r40 = icmp sgt i32 %r37,255
    br i1 %r40, label %L13, label %L14
L13:  ;
    br label %L14
L14:  ;
    %r51 = phi i32 [255,%L13],[%r37,%L9]
    call void @putint(i32 %r51)
    call void @putch(i32 32)
    %r46 = add i32 %r54,1
    br label %L6
L15:  ;
    call void @putch(i32 10)
    %r50 = add i32 %r56,1
    br label %L2
L16:  ;
    ret void
}
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    call void @write_pgm()
    ret i32 0
}
define float @sample(float %r0,float %r1)
{
L0:  ;
    br label %L2
L2:  ;
    %r45 = phi float [%r35,%L21],[0x0,%L0]
    %r44 = phi i32 [%r38,%L21],[0,%L0]
    %r10 = icmp slt i32 %r44,24
    br i1 %r10, label %L8, label %L6
L6:  ;
    %r42 = fdiv float %r45,0x4038000000000000
    ret float %r42
L8:  ;
    %r46 = load i32, ptr @seed
    %r47 = mul i32 %r46,19980130
    %r48 = add i32 %r47,23333
    %r49 = srem i32 %r48,100000007
    store i32 %r49, ptr @seed
    %r50 = load i32, ptr @seed
    %r51 = icmp slt i32 %r50,0
    br i1 %r51, label %L10, label %L11
L10:  ;
    %r52 = load i32, ptr @seed
    %r53 = add i32 %r52,100000007
    store i32 %r53, ptr @seed
    br label %L11
L11:  ;
    %r54 = load i32, ptr @seed
    %r13 = sitofp i32 %r54 to float
    %r20 = fdiv float %r13,0x4197d78420000000
    %r21 = sitofp i32 %r44 to float
    %r22 = fadd float %r21,%r20
    %r23 = fmul float 0x401921fb60000000,%r22
    %r26 = fdiv float %r23,0x4038000000000000
    %r31 = call float @my_cos(float %r26)
    %r33 = call float @my_sin(float %r26)
    br label %L12
L12:  ;
    %r55 = alloca [2 x float]
    br label %L13
L13:  ;
    %r56 = phi float [0x0,%L12],[%r57,%L14]
    %r58 = phi i32 [0,%L12],[%r59,%L14]
    %r60 = icmp slt i32 %r58,10
    br i1 %r60, label %L15, label %L16
L14:  ;
    %r73 = load float, ptr %r67
    %r57 = fadd float %r56,%r73
    %r59 = add i32 %r58,1
    br label %L13
L15:  ;
    %r61 = fcmp olt float %r56,0x4000000000000000
    br i1 %r61, label %L17, label %L16
L16:  ;
    br label %L21
L17:  ;
    %r62 = fmul float %r31,%r56
    %r63 = fadd float %r0,%r62
    %r64 = fmul float %r33,%r56
    %r65 = fadd float %r1,%r64
    br label %L28
L19:  ;
    %r67 = getelementptr [2 x float], ptr %r55, i32 0, i32 0
    %r68 = load float, ptr %r67
    %r69 = fcmp olt float %r68,0x3eb0c6f7a0000000
    br i1 %r69, label %L20, label %L14
L20:  ;
    %r70 = getelementptr float, ptr %r67, i32 1
    %r71 = load float, ptr %r70
    br label %L21
L21:  ;
    %r74 = phi float [%r71,%L20],[0x0,%L16]
    %r35 = fadd float %r45,%r74
    %r38 = add i32 %r44,1
    br label %L2
L24:  ;
    %r78 = fcmp olt float %r104,%r127
    br i1 %r78, label %L25, label %L26
L25:  ;
    %r81 = getelementptr [2 x float], ptr %r55, i32 0, i32 0
    store float %r104, ptr %r81
    %r82 = getelementptr float, ptr %r81, i32 1
    store float 0x4008000000000000, ptr %r82
    br label %L19
L26:  ;
    %r83 = getelementptr [2 x float], ptr %r55, i32 0, i32 0
    store float %r127, ptr %r83
    %r84 = getelementptr float, ptr %r83, i32 1
    store float 0x0, ptr %r84
    br label %L19
L28:  ;
    %r85 = fsub float %r63,0x3fd99999a0000000
    %r86 = fsub float %r65,0x3fd99999a0000000
    %r87 = fmul float %r85,%r85
    %r88 = fmul float %r86,%r86
    %r89 = fadd float %r87,%r88
    br label %L30
L30:  ;
    %r90 = fdiv float %r89,0x4020000000000000
    %r91 = fadd float %r90,0x3fe0000000000000
    %r92 = fmul float 0x4000000000000000,%r89
    %r93 = fadd float 0x4010000000000000,%r89
    %r94 = fdiv float %r92,%r93
    %r95 = fadd float %r91,%r94
    br label %L31
L31:  ;
    %r96 = phi float [%r95,%L30],[%r97,%L32]
    %r98 = phi i32 [10,%L30],[%r99,%L32]
    %r100 = icmp ne i32 %r98,0
    br i1 %r100, label %L32, label %L33
L32:  ;
    %r101 = fdiv float %r89,%r96
    %r102 = fadd float %r96,%r101
    %r97 = fdiv float %r102,0x4000000000000000
    %r99 = sub i32 %r98,1
    br label %L31
L33:  ;
    %r104 = fsub float %r96,0x3fb99999a0000000
    br label %L35
L35:  ;
    %r108 = fsub float %r63,0x3fe3333340000000
    %r109 = fsub float %r65,0x3fe3333340000000
    %r110 = fmul float %r108,%r108
    %r111 = fmul float %r109,%r109
    %r112 = fadd float %r110,%r111
    br label %L37
L37:  ;
    %r113 = fdiv float %r112,0x4020000000000000
    %r114 = fadd float %r113,0x3fe0000000000000
    %r115 = fmul float 0x4000000000000000,%r112
    %r116 = fadd float 0x4010000000000000,%r112
    %r117 = fdiv float %r115,%r116
    %r118 = fadd float %r114,%r117
    br label %L38
L38:  ;
    %r119 = phi float [%r118,%L37],[%r120,%L39]
    %r121 = phi i32 [10,%L37],[%r122,%L39]
    %r123 = icmp ne i32 %r121,0
    br i1 %r123, label %L39, label %L40
L39:  ;
    %r124 = fdiv float %r112,%r119
    %r125 = fadd float %r119,%r124
    %r120 = fdiv float %r125,0x4000000000000000
    %r122 = sub i32 %r121,1
    br label %L38
L40:  ;
    %r127 = fsub float %r119,0x3fa99999a0000000
    br label %L24
}
define float @my_cos(float %r0)
{
L0:  ;
    br label %L1
L1:  ;
    %r7 = fadd float %r0,0x3ff921fb60000000
    %r8 = call float @my_sin(float %r7)
    ret float %r8
}
define float @my_sin(float %r0)
{
L0:  ;
    br label %L2
L2:  ;
    %r4 = fcmp ogt float %r0,0x401921fb60000000
    br i1 %r4, label %L6, label %L5
L5:  ;
    %r8 = fcmp olt float %r0,0xc01921fb60000000
    br i1 %r8, label %L6, label %L7
L6:  ;
    %r13 = fdiv float %r0,0x401921fb60000000
    %r14 = fptosi float %r13 to i32
    %r18 = sitofp i32 %r14 to float
    %r19 = fmul float %r18,0x401921fb60000000
    %r20 = fsub float %r0,%r19
    br label %L7
L7:  ;
    %r39 = phi float [%r20,%L6],[%r0,%L5]
    %r23 = fcmp ogt float %r39,0x400921fb60000000
    br i1 %r23, label %L11, label %L12
L11:  ;
    %r26 = fsub float %r39,0x401921fb60000000
    br label %L12
L12:  ;
    %r38 = phi float [%r26,%L11],[%r39,%L7]
    br label %L13
L13:  ;
    %r30 = fcmp olt float %r38,0xc00921fb60000000
    br i1 %r30, label %L16, label %L17
L16:  ;
    %r33 = fadd float %r38,0x401921fb60000000
    br label %L17
L17:  ;
    %r37 = phi float [%r33,%L16],[%r38,%L13]
    %r35 = call float @my_sin_impl(float %r37)
    ret float %r35
}
define float @my_sin_impl(float %r0)
{
L0:  ;
    br label %L9
L6:  ;
    %r9 = fdiv float %r0,0x4008000000000000
    %r10 = call float @my_sin_impl(float %r9)
    %r17 = fmul float 0x4008000000000000,%r10
    %r18 = fmul float 0x4010000000000000,%r10
    %r19 = fmul float %r18,%r10
    %r20 = fmul float %r19,%r10
    %r21 = fsub float %r17,%r20
    br label %L7
L7:  ;
    %r13 = phi float [%r0,%L10],[%r21,%L6]
    ret float %r13
L9:  ;
    %r14 = fcmp ogt float %r0,0x0
    br i1 %r14, label %L10, label %L11
L10:  ;
    %r16 = phi float [%r0,%L9],[%r15,%L11]
    %r5 = fcmp ole float %r16,0x3eb0c6f7a0000000
    br i1 %r5, label %L7, label %L6
L11:  ;
    %r15 = fsub float 0x0,%r0
    br label %L10
}
