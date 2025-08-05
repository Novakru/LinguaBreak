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
@state = global i32 19260817
@buffer = global [32768x i32] zeroinitializer
define void @pseudo_sha1(ptr %r0,i32 %r1,ptr %r2)
{
L0:  ;
    %r100 = alloca [80 x i32]
    br label %L1
L1:  ;
    %r42 = getelementptr i32, ptr %r0, i32 %r1
    store i32 128, ptr %r42
    %r46 = add i32 %r1,1
    %r446 = getelementptr i32, ptr %r0, i32 %r46
    br label %L2
L2:  ;
    %r54 = phi ptr [%r446,%L1],[%r447,%L5]
    %r343 = phi i32 [%r46,%L1],[%r58,%L5]
    %r49 = srem i32 %r343,64
    %r51 = icmp ne i32 %r49,60
    br i1 %r51, label %L5, label %L6
L5:  ;
    store i32 0, ptr %r54
    %r58 = add i32 %r343,1
    %r447 = getelementptr i32, ptr %r54, i32 1
    br label %L2
L6:  ;
    %r61 = sdiv i32 %r1,16777216
    %r63 = srem i32 %r61,256
    %r65 = getelementptr i32, ptr %r0, i32 %r343
    store i32 %r63, ptr %r65
    %r69 = sdiv i32 %r1,65536
    %r71 = srem i32 %r69,256
    %r75 = getelementptr i32, ptr %r65, i32 1
    store i32 %r71, ptr %r75
    %r79 = sdiv i32 %r1,256
    %r81 = srem i32 %r79,256
    %r85 = getelementptr i32, ptr %r65, i32 2
    store i32 %r81, ptr %r85
    %r89 = srem i32 %r1,256
    %r93 = getelementptr i32, ptr %r65, i32 3
    store i32 %r89, ptr %r93
    %r97 = add i32 %r343,4
    call void @llvm.memset.p0.i32(ptr %r100,i8 0,i32 320,i1 0)
    %r101 = getelementptr [80 x i32], ptr %r100, i32 0, i32 0
    store i32 0, ptr %r101
    br label %L7
L7:  ;
    %r342 = phi i32 [1732584193,%L6],[%r275,%L48]
    %r341 = phi i32 [-271733879,%L6],[%r278,%L48]
    %r340 = phi i32 [-1732584194,%L6],[%r281,%L48]
    %r339 = phi i32 [271733878,%L6],[%r284,%L48]
    %r338 = phi i32 [-1009589776,%L6],[%r287,%L48]
    %r327 = phi i32 [0,%L6],[%r326,%L48]
    %r321 = phi i32 [0,%L6],[%r320,%L48]
    %r315 = phi i32 [0,%L6],[%r290,%L48]
    %r105 = icmp slt i32 %r315,%r97
    br i1 %r105, label %L132, label %L49
L11:  ;
    %r162 = phi ptr [%r466,%L132],[%r467,%L14]
    %r156 = phi ptr [%r464,%L132],[%r465,%L14]
    %r144 = phi ptr [%r461,%L132],[%r462,%L14]
    %r132 = phi ptr [%r458,%L132],[%r459,%L14]
    %r121 = phi ptr [%r455,%L132],[%r456,%L14]
    %r314 = phi i32 [%r166,%L14],[0,%L132]
    %r115 = icmp slt i32 %r314,16
    br i1 %r115, label %L14, label %L133
L14:  ;
    %r122 = load i32, ptr %r121
    %r124 = mul i32 %r122,16777216
    %r133 = load i32, ptr %r132
    %r135 = mul i32 %r133,65536
    %r136 = add i32 %r124,%r135
    %r145 = load i32, ptr %r144
    %r147 = mul i32 %r145,256
    %r148 = add i32 %r136,%r147
    %r157 = load i32, ptr %r156
    %r160 = add i32 %r148,%r157
    store i32 %r160, ptr %r162
    %r166 = add i32 %r314,1
    %r456 = getelementptr i32, ptr %r121, i32 4
    %r459 = getelementptr i32, ptr %r132, i32 4
    %r462 = getelementptr i32, ptr %r144, i32 4
    %r465 = getelementptr i32, ptr %r156, i32 4
    %r467 = getelementptr i32, ptr %r162, i32 1
    br label %L11
L16:  ;
    %r195 = phi ptr [%r448,%L133],[%r449,%L19]
    %r313 = phi i32 [%r199,%L19],[%r314,%L133]
    %r169 = icmp slt i32 %r313,80
    br i1 %r169, label %L19, label %L21
L19:  ;
    %r172 = sub i32 %r313,3
    %r173 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r172
    %r174 = load i32, ptr %r173
    %r177 = sub i32 %r313,8
    %r178 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r177
    %r179 = load i32, ptr %r178
    %r361 = add i32 %r174,%r179
    %r345 = sub i32 %r174,%r361
    %r346 = add i32 %r345,%r179
    %r347 = sub i32 %r346,%r361
    %r183 = sub i32 %r313,14
    %r184 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r183
    %r185 = load i32, ptr %r184
    %r385 = add i32 %r347,%r185
    %r370 = sub i32 %r347,%r385
    %r371 = add i32 %r370,%r185
    %r372 = sub i32 %r371,%r385
    %r189 = sub i32 %r313,16
    %r190 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r189
    %r191 = load i32, ptr %r190
    %r404 = add i32 %r372,%r191
    %r396 = sub i32 %r372,%r404
    %r397 = add i32 %r396,%r191
    %r398 = sub i32 %r397,%r404
    %r407 = mul i32 %r398,2
    %r408 = srem i32 %r398,2
    %r409 = add i32 %r407,%r408
    store i32 %r409, ptr %r195
    %r199 = add i32 %r313,1
    %r449 = getelementptr i32, ptr %r195, i32 1
    br label %L16
L21:  ;
    %r261 = phi ptr [%r466,%L16],[%r451,%L47]
    %r336 = phi i32 [%r263,%L47],[%r342,%L16]
    %r334 = phi i32 [%r336,%L47],[%r341,%L16]
    %r332 = phi i32 [%r368,%L47],[%r340,%L16]
    %r330 = phi i32 [%r332,%L47],[%r339,%L16]
    %r328 = phi i32 [%r330,%L47],[%r338,%L16]
    %r326 = phi i32 [%r325,%L47],[%r327,%L16]
    %r320 = phi i32 [%r319,%L47],[%r321,%L16]
    %r311 = phi i32 [%r272,%L47],[0,%L16]
    %r203 = icmp slt i32 %r311,80
    br i1 %r203, label %L25, label %L48
L25:  ;
    %r206 = icmp slt i32 %r311,20
    br i1 %r206, label %L53, label %L30
L30:  ;
    %r218 = icmp slt i32 %r311,40
    br i1 %r218, label %L67, label %L35
L35:  ;
    %r227 = icmp slt i32 %r311,60
    br i1 %r227, label %L57, label %L40
L40:  ;
    %r243 = icmp slt i32 %r311,80
    br i1 %r243, label %L71, label %L44
L44:  ;
    %r322 = phi i32 [%r384,%L71],[%r326,%L40]
    %r316 = phi i32 [-899497722,%L71],[%r320,%L40]
    br label %L45
L45:  ;
    %r323 = phi i32 [%r434,%L57],[%r322,%L44]
    %r317 = phi i32 [-1894007588,%L57],[%r316,%L44]
    br label %L46
L46:  ;
    %r324 = phi i32 [%r377,%L67],[%r323,%L45]
    %r318 = phi i32 [1859775361,%L67],[%r317,%L45]
    br label %L47
L47:  ;
    %r325 = phi i32 [%r418,%L53],[%r324,%L46]
    %r319 = phi i32 [1518500249,%L53],[%r318,%L46]
    %r358 = mul i32 %r336,32
    %r359 = srem i32 %r336,32
    %r360 = add i32 %r358,%r359
    %r255 = add i32 %r360,%r325
    %r257 = add i32 %r255,%r328
    %r259 = add i32 %r257,%r319
    %r262 = load i32, ptr %r261
    %r263 = add i32 %r259,%r262
    %r366 = mul i32 %r334,1073741824
    %r367 = srem i32 %r334,1073741824
    %r368 = add i32 %r366,%r367
    %r272 = add i32 %r311,1
    %r451 = getelementptr i32, ptr %r261, i32 1
    br label %L21
L48:  ;
    %r275 = add i32 %r342,%r336
    %r278 = add i32 %r341,%r334
    %r281 = add i32 %r340,%r332
    %r284 = add i32 %r339,%r330
    %r287 = add i32 %r338,%r328
    %r290 = add i32 %r315,64
    br label %L7
L49:  ;
    %r293 = getelementptr i32, ptr %r2, i32 0
    store i32 %r342, ptr %r293
    %r297 = getelementptr i32, ptr %r2, i32 1
    store i32 %r341, ptr %r297
    %r301 = getelementptr i32, ptr %r2, i32 2
    store i32 %r340, ptr %r301
    %r305 = getelementptr i32, ptr %r2, i32 3
    store i32 %r339, ptr %r305
    %r309 = getelementptr i32, ptr %r2, i32 4
    store i32 %r338, ptr %r309
    ret void
L53:  ;
    %r348 = add i32 %r334,%r332
    %r362 = sub i32 -1,%r334
    %r373 = add i32 %r362,%r330

    %r405 = add i32 %r348,%r373
    %r400 = sub i32 %r348,%r405
    %r401 = add i32 %r400,%r373
    %r402 = sub i32 %r401,%r405

    %r420 = add i32 %r402,%r405
    %r416 = sub i32 %r402,%r420
    %r417 = add i32 %r416,%r405
    %r418 = sub i32 %r417,%r420
    br label %L47
L57:  ;
    %r353 = add i32 %r334,%r332
    %r364 = add i32 %r334,%r330

    %r403 = add i32 %r353,%r364
    %r391 = sub i32 %r353,%r403
    %r392 = add i32 %r391,%r364
    %r393 = sub i32 %r392,%r403

    %r419 = add i32 %r393,%r403
    %r412 = sub i32 %r393,%r419
    %r413 = add i32 %r412,%r403
    %r414 = sub i32 %r413,%r419

    %r421 = add i32 %r332,%r330

    %r429 = add i32 %r414,%r421
    %r426 = sub i32 %r414,%r429
    %r427 = add i32 %r426,%r421
    %r428 = sub i32 %r427,%r429

    %r435 = add i32 %r428,%r429
    %r432 = sub i32 %r428,%r435
    %r433 = add i32 %r432,%r429
    %r434 = sub i32 %r433,%r435

    br label %L45
L67:  ;
    %r363 = add i32 %r334,%r332
    %r350 = sub i32 %r334,%r363
    %r351 = add i32 %r350,%r332
    %r352 = sub i32 %r351,%r363

    %r389 = add i32 %r352,%r330
    %r375 = sub i32 %r352,%r389
    %r376 = add i32 %r375,%r330
    %r377 = sub i32 %r376,%r389

    br label %L46
L71:  ;
    %r365 = add i32 %r334,%r332
    %r355 = sub i32 %r334,%r365
    %r356 = add i32 %r355,%r332
    %r357 = sub i32 %r356,%r365

    %r394 = add i32 %r357,%r330
    %r382 = sub i32 %r357,%r394
    %r383 = add i32 %r382,%r330
    %r384 = sub i32 %r383,%r394
    
    br label %L44
L132:  ;
    %r455 = getelementptr i32, ptr %r0, i32 %r315
    %r458 = getelementptr i32, ptr %r455, i32 1
    %r461 = getelementptr i32, ptr %r455, i32 2
    %r464 = getelementptr i32, ptr %r455, i32 3
    %r466 = getelementptr i32, ptr %r100, i32 0
    br label %L11
L133:  ;
    %r448 = getelementptr i32, ptr %r100, i32 %r314
    br label %L16
}
define i32 @main()
{
L0:  ;
    %r15 = alloca [5 x i32]
    %r4 = alloca [5 x i32]
    br label %L1
L1:  ;
    call void @llvm.memset.p0.i32(ptr %r4,i8 0,i32 20,i1 0)
    %r5 = getelementptr [5 x i32], ptr %r4, i32 0, i32 0
    store i32 0, ptr %r5
    %r7 = getelementptr i32, ptr %r5, i32 1
    store i32 0, ptr %r7
    %r9 = getelementptr i32, ptr %r7, i32 1
    store i32 0, ptr %r9
    %r11 = getelementptr i32, ptr %r9, i32 1
    store i32 0, ptr %r11
    %r13 = getelementptr i32, ptr %r11, i32 1
    store i32 0, ptr %r13
    %r16 = call i32 @getint()
    store i32 %r16, ptr @state
    %r17 = call i32 @getint()
    call void @_sysy_starttime(i32 147)
    %r21 = getelementptr [5 x i32], ptr %r15, i32 0, i32 0
    store i32 0, ptr %r21
    %r25 = getelementptr i32, ptr %r21, i32 1
    store i32 0, ptr %r25
    %r29 = getelementptr i32, ptr %r25, i32 1
    store i32 0, ptr %r29
    %r33 = getelementptr i32, ptr %r29, i32 1
    store i32 0, ptr %r33
    %r37 = getelementptr i32, ptr %r33, i32 1
    store i32 0, ptr %r37
    br label %L2
L2:  ;
    %r89 = phi i32 [%r17,%L1],[%r80,%L15]
    %r41 = icmp sgt i32 %r89,0
    br i1 %r41, label %L24, label %L16
L6:  ;
    %r53 = phi ptr [%r103,%L24],[%r104,%L18]
    %r88 = phi i32 [%r57,%L18],[0,%L24]
    %r48 = icmp slt i32 %r88,32000
    br i1 %r48, label %L18, label %L10
L10:  ;
    %r58 = getelementptr [32768 x i32], ptr @buffer, i32 0
    %r60 = getelementptr [5 x i32], ptr %r4, i32 0
    call void @pseudo_sha1(ptr %r58,i32 32000,ptr %r60)
    %r105 = getelementptr i32, ptr %r15, i32 0
    %r107 = getelementptr i32, ptr %r4, i32 0
    br label %L11
L11:  ;
    %r69 = phi ptr [%r107,%L10],[%r108,%L14]
    %r66 = phi ptr [%r105,%L10],[%r106,%L14]
    %r86 = phi i32 [0,%L10],[%r77,%L14]
    %r64 = icmp slt i32 %r86,5
    br i1 %r64, label %L14, label %L15
L14:  ;
    %r67 = load i32, ptr %r66
    %r70 = load i32, ptr %r69
    %r101 = add i32 %r67,%r70
    %r98 = sub i32 %r67,%r101
    %r99 = add i32 %r98,%r70
    %r100 = sub i32 %r99,%r101
    store i32 %r100, ptr %r66
    %r77 = add i32 %r86,1
    %r106 = getelementptr i32, ptr %r66, i32 1
    %r108 = getelementptr i32, ptr %r69, i32 1
    br label %L11
L15:  ;
    %r80 = sub i32 %r89,1
    br label %L2
L16:  ;
    call void @_sysy_stoptime(i32 170)
    %r83 = getelementptr [5 x i32], ptr %r15, i32 0
    call void @putarray(i32 5,ptr %r83)
    ret i32 0
L18:  ;
    %r90 = load i32, ptr @state
    %r91 = mul i32 %r90,8192
    %r92 = add i32 %r90,%r91
    store i32 %r92, ptr @state
    %r93 = sdiv i32 %r92,131072
    %r94 = add i32 %r92,%r93
    store i32 %r94, ptr @state
    %r95 = mul i32 %r94,32
    %r96 = add i32 %r94,%r95
    store i32 %r96, ptr @state
    %r51 = srem i32 %r96,256
    store i32 %r51, ptr %r53
    %r57 = add i32 %r88,1
    %r104 = getelementptr i32, ptr %r53, i32 1
    br label %L6
L24:  ;
    %r103 = getelementptr i32, ptr @buffer, i32 0
    br label %L6
}
