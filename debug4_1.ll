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
@n = global i32 14
@seq = global [1400x i32] zeroinitializer
@table = global [1400x [1400x i32]] zeroinitializer
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r0 = getelementptr [1400 x i32], ptr @seq, i32 0
    %r1 = call i32 @getarray(ptr %r0)
    %r2 = getelementptr [1400 x [1400 x i32]], ptr @table, i32 0
    %r3 = call i32 @getarray(ptr %r2)
    call void @_sysy_starttime(i32 81)
    %r5 = load i32, ptr @n
    call void @kernel_nussinov(i32 %r5,ptr %r0,ptr %r2)
    call void @_sysy_stoptime(i32 83)
    %r9 = load i32, ptr @n
    %r10 = load i32, ptr @n
    %r11 = mul i32 %r9,%r10
    call void @putarray(i32 %r11,ptr %r2)
    ret i32 0
}
define void @kernel_nussinov(i32 %r0,ptr %r1,ptr %r2)
{
L0:  ;
    br label %L1
L1:  ;
    %r15 = sub i32 %r0,1
    br label %L2
L2:  ;
    %r243 = phi i32 [%r15,%L1],[%r204,%L67]
    %r18 = icmp sge i32 %r243,0
    br i1 %r18, label %L5, label %L69
L5:  ;
    %r21 = add i32 %r243,1
    %r250 = mul i32 %r243,1400
    %r251 = add i32 %r250,%r21
    %r252 = getelementptr i32, ptr %r2, i32 %r251
    %r262 = getelementptr i32, ptr %r1, i32 %r21
    %r268 = mul i32 %r21,1400
    %r269 = add i32 %r268,%r21
    %r270 = getelementptr i32, ptr %r2, i32 %r269
    br label %L6
L6:  ;
    %r32 = phi ptr [%r252,%L5],[%r275,%L66]
    %r64 = phi ptr [%r270,%L5],[%r271,%L66]
    %r58 = phi ptr [%r252,%L5],[%r267,%L66]
    %r99 = phi ptr [%r262,%L5],[%r263,%L66]
    %r107 = phi ptr [%r252,%L5],[%r261,%L66]
    %r136 = phi ptr [%r252,%L5],[%r257,%L66]
    %r167 = phi ptr [%r252,%L5],[%r253,%L66]
    %r241 = phi i32 [%r21,%L5],[%r201,%L66]
    %r24 = icmp slt i32 %r241,%r0
    br i1 %r24, label %L10, label %L67
L10:  ;
    %r27 = sub i32 %r241,1
    %r29 = icmp sge i32 %r27,0
    br i1 %r29, label %L14, label %L20
L14:  ;
    %r33 = load i32, ptr %r32
    %r38 = getelementptr [1400 x i32], ptr %r2, i32 %r243, i32 %r27
    %r39 = load i32, ptr %r38
    %r40 = icmp slt i32 %r33,%r39
    br i1 %r40, label %L17, label %L20
L17:  ;
    %r46 = load i32, ptr %r38
    store i32 %r46, ptr %r32
    br label %L20
L20:  ;
    %r55 = icmp slt i32 %r21,%r0
    br i1 %r55, label %L24, label %L30
L24:  ;
    %r59 = load i32, ptr %r58
    %r65 = load i32, ptr %r64
    %r66 = icmp slt i32 %r59,%r65
    br i1 %r66, label %L27, label %L30
L27:  ;
    %r72 = load i32, ptr %r64
    store i32 %r72, ptr %r58
    br label %L30
L30:  ;
    %r81 = icmp sge i32 %r27,0
    br i1 %r81, label %L33, label %L56
L33:  ;
    %r86 = icmp slt i32 %r21,%r0
    br i1 %r86, label %L35, label %L56
L35:  ;
    %r92 = icmp slt i32 %r243,%r27
    br i1 %r92, label %L39, label %L50
L39:  ;
    %r96 = getelementptr i32, ptr %r1, i32 %r243
    %r97 = load i32, ptr %r96
    %r100 = load i32, ptr %r99
    %r101 = add i32 %r97,%r100
    %r103 = icmp eq i32 %r101,3
    br i1 %r103, label %L42, label %L43
L42:  ;
    br label %L43
L43:  ;
    %r229 = phi i32 [1,%L42],[0,%L39]
    %r108 = load i32, ptr %r107
    %r115 = getelementptr [1400 x i32], ptr %r2, i32 %r21, i32 %r27
    %r116 = load i32, ptr %r115
    %r118 = add i32 %r116,%r229
    %r119 = icmp slt i32 %r108,%r118
    br i1 %r119, label %L47, label %L56
L47:  ;
    %r127 = load i32, ptr %r115
    %r129 = add i32 %r127,%r229
    store i32 %r129, ptr %r107
    br label %L56
L50:  ;
    %r137 = load i32, ptr %r136
    %r144 = getelementptr [1400 x i32], ptr %r2, i32 %r21, i32 %r27
    %r145 = load i32, ptr %r144
    %r146 = icmp slt i32 %r137,%r145
    br i1 %r146, label %L53, label %L56
L53:  ;
    %r154 = load i32, ptr %r144
    store i32 %r154, ptr %r136
    br label %L56
L56:  ;
    %r276 = mul i32 %r243,1400
    %r277 = add i32 %r276,%r21
    %r278 = getelementptr i32, ptr %r2, i32 %r277
    %r280 = add i32 %r21,1
    %r281 = mul i32 %r280,1400
    %r282 = add i32 %r241,%r281
    %r283 = getelementptr i32, ptr %r2, i32 %r282
    br label %L57
L57:  ;
    %r177 = phi ptr [%r283,%L56],[%r284,%L65]
    %r171 = phi ptr [%r278,%L56],[%r279,%L65]
    %r235 = phi i32 [%r21,%L56],[%r175,%L65]
    %r164 = icmp slt i32 %r235,%r241
    br i1 %r164, label %L61, label %L66
L61:  ;
    %r168 = load i32, ptr %r167
    %r172 = load i32, ptr %r171
    %r175 = add i32 %r235,1
    %r178 = load i32, ptr %r177
    %r179 = add i32 %r172,%r178
    %r180 = icmp slt i32 %r168,%r179
    br i1 %r180, label %L64, label %L65
L64:  ;
    %r184 = load i32, ptr %r171
    %r190 = load i32, ptr %r177
    %r191 = add i32 %r184,%r190
    store i32 %r191, ptr %r167
    br label %L65
L65:  ;
    %r279 = getelementptr i32, ptr %r171, i32 1
    %r284 = getelementptr i32, ptr %r177, i32 1400
    br label %L57
L66:  ;
    %r201 = add i32 %r241,1
    %r253 = getelementptr i32, ptr %r167, i32 1
    %r257 = getelementptr i32, ptr %r136, i32 1
    %r261 = getelementptr i32, ptr %r107, i32 1
    %r263 = getelementptr i32, ptr %r99, i32 1
    %r267 = getelementptr i32, ptr %r58, i32 1
    %r271 = getelementptr i32, ptr %r64, i32 1
    %r275 = getelementptr i32, ptr %r32, i32 1
    br label %L6
L67:  ;
    %r204 = sub i32 %r243,1
    br label %L2
L69:  ;
    %r242 = phi i32 [%r228,%L77],[0,%L2]
    %r208 = icmp slt i32 %r242,%r0
    br i1 %r208, label %L79, label %L78
L73:  ;
    %r215 = phi ptr [%r248,%L79],[%r249,%L76]
    %r238 = phi i32 [%r225,%L76],[0,%L79]
    %r212 = icmp slt i32 %r238,%r0
    br i1 %r212, label %L76, label %L77
L76:  ;
    %r216 = load i32, ptr %r215
    %r218 = srem i32 %r216,10
    store i32 %r218, ptr %r215
    %r225 = add i32 %r238,1
    %r249 = getelementptr i32, ptr %r215, i32 1
    br label %L73
L77:  ;
    %r228 = add i32 %r242,1
    br label %L69
L78:  ;
    ret void
L79:  ;
    %r246 = mul i32 %r242,1400
    %r248 = getelementptr i32, ptr %r2, i32 %r246
    br label %L73
}
