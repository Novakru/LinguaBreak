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
@t = global [1005x [2x i32]] zeroinitializer
@dp = global [1005x [35x i32]] zeroinitializer
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r15 = call i32 @getint()
    %r16 = call i32 @getint()
    %r167 = icmp sle i32 1,%r15
    br i1 %r167, label %L2, label %L33
L2:  ;
    %r169 = phi i32 [%r46,%L2],[1,%L1]
    %r21 = call i32 @getint()
    %r26 = srem i32 %r21,2
    %r27 = getelementptr [1005 x [2 x i32]], ptr @t, i32 0, i32 %r169, i32 %r26
    store i32 1, ptr %r27
    %r31 = sub i32 %r169,1
    %r33 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r31, i32 0
    %r34 = load i32, ptr %r33
    %r37 = getelementptr [1005 x [2 x i32]], ptr @t, i32 0, i32 %r169, i32 1
    %r38 = load i32, ptr %r37
    %r39 = add i32 %r34,%r38
    %r42 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r169, i32 0
    store i32 %r39, ptr %r42
    %r46 = add i32 %r169,1
    %r168 = icmp sle i32 %r46,%r15
    br i1 %r168, label %L2, label %L33
L7:  ;
    %r172 = phi i32 [%r133,%L21],[1,%L33]
    br label %L11
L11:  ;
    %r158 = phi i32 [%r64,%L20],[1,%L7]
    %r54 = icmp sle i32 %r158,%r16
    br i1 %r54, label %L15, label %L21
L15:  ;
    %r57 = sub i32 %r172,1
    %r59 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r57, i32 %r158
    %r60 = load i32, ptr %r59
    %r64 = add i32 %r158,1
    %r66 = srem i32 %r64,2
    %r67 = getelementptr [1005 x [2 x i32]], ptr @t, i32 0, i32 %r172, i32 %r66
    %r68 = load i32, ptr %r67
    %r69 = add i32 %r60,%r68
    %r75 = sub i32 %r158,1
    %r76 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r57, i32 %r75
    %r77 = load i32, ptr %r76
    %r85 = load i32, ptr %r67
    %r86 = add i32 %r77,%r85
    %r87 = icmp sgt i32 %r69,%r86
    br i1 %r87, label %L18, label %L19
L18:  ;
    %r93 = load i32, ptr %r59
    %r101 = load i32, ptr %r67
    %r102 = add i32 %r93,%r101
    %r105 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r172, i32 %r158
    store i32 %r102, ptr %r105
    br label %L20
L19:  ;
    %r114 = load i32, ptr %r76
    %r122 = load i32, ptr %r67
    %r123 = add i32 %r114,%r122
    %r126 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r172, i32 %r158
    store i32 %r123, ptr %r126
    br label %L20
L20:  ;
    br label %L11
L21:  ;
    %r133 = add i32 %r172,1
    %r171 = icmp sle i32 %r133,%r15
    br i1 %r171, label %L7, label %L36
L23:  ;
    %r157 = phi i32 [%r152,%L31],[0,%L36]
    %r156 = phi i32 [%r155,%L31],[0,%L36]
    %r139 = icmp sle i32 %r157,%r16
    br i1 %r139, label %L27, label %L32
L27:  ;
    %r143 = getelementptr [1005 x [35 x i32]], ptr @dp, i32 0, i32 %r15, i32 %r157
    %r144 = load i32, ptr %r143
    %r145 = icmp slt i32 %r156,%r144
    br i1 %r145, label %L30, label %L31
L30:  ;
    %r149 = load i32, ptr %r143
    br label %L31
L31:  ;
    %r155 = phi i32 [%r149,%L30],[%r156,%L27]
    %r152 = add i32 %r157,1
    br label %L23
L32:  ;
    ret i32 %r156
L33:  ;
    %r161 = phi i32 [1,%L1],[%r46,%L2]
    %r170 = icmp sle i32 1,%r15
    br i1 %r170, label %L7, label %L36
L36:  ;
    %r160 = phi i32 [1,%L33],[%r133,%L21]
    br label %L23
}
