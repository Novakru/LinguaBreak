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
    br label %L2
L2:  ;
    %r119 = phi i32 [0,%L0],[%r118,%L63]
    %r106 = phi i32 [0,%L0],[%r74,%L63]
    %r6 = icmp slt i32 %r106,20
    br i1 %r6, label %L6, label %L64
L6:  ;
    %r118 = phi i32 [%r119,%L2],[%r114,%L61]
    %r112 = phi i32 [%r106,%L2],[%r111,%L61]
    %r99 = phi i32 [0,%L2],[%r68,%L61]
    %r11 = icmp slt i32 %r99,10
    br i1 %r11, label %L10, label %L63
L10:  ;
    %r114 = phi i32 [%r118,%L6],[%r113,%L60]
    %r111 = phi i32 [%r112,%L6],[%r110,%L60]
    %r105 = phi i32 [%r99,%L6],[%r104,%L60]
    %r92 = phi i32 [0,%L6],[%r65,%L60]
    %r16 = icmp slt i32 %r92,5
    br i1 %r16, label %L14, label %L61
L14:  ;
    %r113 = phi i32 [%r114,%L10],[%r58,%L47]
    %r110 = phi i32 [%r111,%L10],[%r110,%L47]
    %r104 = phi i32 [%r105,%L10],[%r104,%L47]
    %r97 = phi i32 [%r92,%L10],[%r97,%L47]
    %r85 = phi i32 [0,%L10],[%r24,%L47]
    %r21 = icmp slt i32 %r85,3
    br i1 %r21, label %L18, label %L60
L18:  ;
    %r24 = add i32 %r85,1
    %r26 = icmp sge i32 %r24,3
    br i1 %r26, label %L22, label %L40
L22:  ;
    %r28 = icmp ne i32 %r85,0
    br i1 %r28, label %L26, label %L39
L26:  ;
    %r30 = icmp ne i32 %r85,0
    br i1 %r30, label %L31, label %L29
L29:  ;
    %r32 = icmp eq i32 %r85,0
    %r33 = zext i1 %r32 to i32
    %r34 = icmp ne i32 %r33,0
    br i1 %r34, label %L31, label %L38
L31:  ;
    %r39 = sub i32 %r85,-1
    %r41 = icmp sge i32 %r39,3
    br i1 %r41, label %L60, label %L38
L38:  ;
    br label %L39
L39:  ;
    br label %L40
L40:  ;
    br label %L41
L41:  ;
    %r77 = phi i32 [0,%L40],[%r49,%L44]
    %r46 = icmp slt i32 %r77,2
    br i1 %r46, label %L44, label %L47
L44:  ;
    %r49 = add i32 %r77,1
    br label %L41
L47:  ;
    %r58 = add i32 %r113,1
    br label %L14
L60:  ;
    %r65 = add i32 %r97,1
    br label %L10
L61:  ;
    %r68 = add i32 %r105,1
    br label %L6
L63:  ;
    %r74 = add i32 %r112,1
    br label %L2
L64:  ;
    ret i32 %r119
}
