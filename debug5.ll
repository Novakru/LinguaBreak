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
    %r472 = srem i32 %r46,64
    %r473 = icmp ne i32 %r472,60
    br i1 %r473, label %L2, label %L6
L2:  ;
    %r476 = phi i32 [%r58,%L2],[%r46,%L1]
    %r49 = srem i32 %r476,64
    %r54 = getelementptr i32, ptr %r0, i32 %r476
    store i32 0, ptr %r54
    %r58 = add i32 %r476,1
    %r474 = srem i32 %r58,64
    %r475 = icmp ne i32 %r474,60
    br i1 %r475, label %L2, label %L6
L6:  ;
    %r343 = phi i32 [%r46,%L1],[%r58,%L2]
    %r61 = sdiv i32 %r1,16777216
    %r63 = srem i32 %r61,256
    %r65 = getelementptr i32, ptr %r0, i32 %r343
    store i32 %r63, ptr %r65
    %r69 = sdiv i32 %r1,65536
    %r71 = srem i32 %r69,256
    %r74 = add i32 %r343,1
    %r75 = getelementptr i32, ptr %r0, i32 %r74
    store i32 %r71, ptr %r75
    %r79 = sdiv i32 %r1,256
    %r81 = srem i32 %r79,256
    %r84 = add i32 %r343,2
    %r85 = getelementptr i32, ptr %r0, i32 %r84
    store i32 %r81, ptr %r85
    %r89 = srem i32 %r1,256
    %r92 = add i32 %r343,3
    %r93 = getelementptr i32, ptr %r0, i32 %r92
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
    br i1 %r105, label %L158, label %L49
L11:  ;
    %r479 = phi i32 [%r166,%L11],[0,%L158]
    %r119 = mul i32 %r479,4
    %r120 = add i32 %r315,%r119
    %r121 = getelementptr i32, ptr %r0, i32 %r120
    %r122 = load i32, ptr %r121
    %r124 = mul i32 %r122,16777216
    %r131 = add i32 %r120,1
    %r132 = getelementptr i32, ptr %r0, i32 %r131
    %r133 = load i32, ptr %r132
    %r135 = mul i32 %r133,65536
    %r136 = add i32 %r124,%r135
    %r143 = add i32 %r120,2
    %r144 = getelementptr i32, ptr %r0, i32 %r143
    %r145 = load i32, ptr %r144
    %r147 = mul i32 %r145,256
    %r148 = add i32 %r136,%r147
    %r155 = add i32 %r120,3
    %r156 = getelementptr i32, ptr %r0, i32 %r155
    %r157 = load i32, ptr %r156
    %r160 = add i32 %r148,%r157
    %r162 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r479
    store i32 %r160, ptr %r162
    %r166 = add i32 %r479,1
    %r478 = icmp slt i32 %r166,16
    br i1 %r478, label %L11, label %L159
L16:  ;
    %r492 = phi i32 [%r199,%L16],[%r314,%L159]
    %r172 = sub i32 %r492,3
    %r173 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r172
    %r174 = load i32, ptr %r173
    %r177 = sub i32 %r492,8
    %r178 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r177
    %r179 = load i32, ptr %r178
    %r364 = add i32 %r174,%r179
    %r345 = sub i32 %r174,%r364
    %r346 = add i32 %r345,%r179
    %r348 = sub i32 %r346,%r364
    %r183 = sub i32 %r492,14
    %r184 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r183
    %r185 = load i32, ptr %r184
    %r402 = add i32 %r348,%r185
    %r380 = sub i32 %r348,%r402
    %r381 = add i32 %r380,%r185
    %r383 = sub i32 %r381,%r402
    %r189 = sub i32 %r492,16
    %r190 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r189
    %r191 = load i32, ptr %r190
    %r423 = add i32 %r383,%r191
    %r417 = sub i32 %r383,%r423
    %r418 = add i32 %r417,%r191
    %r420 = sub i32 %r418,%r423
    %r437 = mul i32 %r420,2
    %r438 = srem i32 %r420,2
    %r439 = add i32 %r437,%r438
    %r195 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r492
    store i32 %r439, ptr %r195
    %r199 = add i32 %r492,1
    %r491 = icmp slt i32 %r199,80
    br i1 %r491, label %L16, label %L160
L21:  ;
    %r482 = phi i32 [%r263,%L73],[%r342,%L160]
    %r483 = phi i32 [%r482,%L73],[%r341,%L160]
    %r484 = phi i32 [%r371,%L73],[%r340,%L160]
    %r485 = phi i32 [%r484,%L73],[%r339,%L160]
    %r486 = phi i32 [%r485,%L73],[%r338,%L160]
    %r487 = phi i32 [%r325,%L73],[%r327,%L160]
    %r488 = phi i32 [%r319,%L73],[%r321,%L160]
    %r489 = phi i32 [%r272,%L73],[0,%L160]
    %r206 = icmp slt i32 %r489,20
    br i1 %r206, label %L53, label %L30
L30:  ;
    %r218 = icmp slt i32 %r489,40
    br i1 %r218, label %L67, label %L35
L35:  ;
    %r227 = icmp slt i32 %r489,60
    br i1 %r227, label %L57, label %L40
L40:  ;
    %r243 = icmp slt i32 %r489,80
    br i1 %r243, label %L71, label %L44
L44:  ;
    %r322 = phi i32 [%r401,%L113],[%r487,%L40]
    %r316 = phi i32 [-899497722,%L113],[%r488,%L40]
    br label %L45
L45:  ;
    %r323 = phi i32 [%r446,%L157],[%r322,%L44]
    %r317 = phi i32 [-1894007588,%L157],[%r316,%L44]
    br label %L46
L46:  ;
    %r324 = phi i32 [%r391,%L109],[%r323,%L45]
    %r318 = phi i32 [1859775361,%L109],[%r317,%L45]
    br label %L47
L47:  ;
    %r325 = phi i32 [%r386,%L139],[%r324,%L46]
    %r319 = phi i32 [1518500249,%L139],[%r318,%L46]
    br label %L61
L48:  ;
    %r336 = phi i32 [%r263,%L73],[%r342,%L160]
    %r334 = phi i32 [%r482,%L73],[%r341,%L160]
    %r332 = phi i32 [%r371,%L73],[%r340,%L160]
    %r330 = phi i32 [%r484,%L73],[%r339,%L160]
    %r328 = phi i32 [%r485,%L73],[%r338,%L160]
    %r326 = phi i32 [%r325,%L73],[%r327,%L160]
    %r320 = phi i32 [%r319,%L73],[%r321,%L160]
    %r311 = phi i32 [%r272,%L73],[0,%L160]
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
    %r349 = add i32 %r483,%r484
    br label %L65
L57:  ;
    %r355 = add i32 %r483,%r484
    br label %L69
L61:  ;
    %r361 = mul i32 %r482,32
    %r362 = srem i32 %r482,32
    %r363 = add i32 %r361,%r362
    %r255 = add i32 %r363,%r325
    %r257 = add i32 %r255,%r486
    %r259 = add i32 %r257,%r319
    %r261 = getelementptr [80 x i32], ptr %r100, i32 0, i32 %r489
    %r262 = load i32, ptr %r261
    %r263 = add i32 %r259,%r262
    br label %L73
L65:  ;
    %r365 = sub i32 -1,%r483
    br label %L77
L67:  ;
    %r366 = add i32 %r483,%r484
    %r351 = sub i32 %r483,%r366
    %r352 = add i32 %r351,%r484
    br label %L79
L69:  ;
    %r367 = add i32 %r483,%r485
    br label %L101
L71:  ;
    %r368 = add i32 %r483,%r484
    %r357 = sub i32 %r483,%r368
    %r358 = add i32 %r357,%r484
    br label %L83
L73:  ;
    %r369 = mul i32 %r483,1073741824
    %r370 = srem i32 %r483,1073741824
    %r371 = add i32 %r369,%r370
    %r272 = add i32 %r489,1
    %r481 = icmp slt i32 %r272,80
    br i1 %r481, label %L21, label %L48
L77:  ;
    %r373 = add i32 %r365,%r485
    br label %L107
L79:  ;
    %r354 = sub i32 %r352,%r366
    br label %L99
L83:  ;
    %r360 = sub i32 %r358,%r368
    br label %L103
L99:  ;
    %r408 = add i32 %r354,%r485
    %r388 = sub i32 %r354,%r408
    %r389 = add i32 %r388,%r485
    br label %L109
L101:  ;
    %r409 = add i32 %r355,%r367
    %r393 = sub i32 %r355,%r409
    %r394 = add i32 %r393,%r367
    br label %L111
L103:  ;
    %r410 = add i32 %r360,%r485
    %r398 = sub i32 %r360,%r410
    %r399 = add i32 %r398,%r485
    br label %L113
L107:  ;
    %r412 = add i32 %r349,%r373
    %r404 = sub i32 %r349,%r412
    %r405 = add i32 %r404,%r373
    br label %L117
L109:  ;
    %r391 = sub i32 %r389,%r408
    br label %L46
L111:  ;
    %r396 = sub i32 %r394,%r409
    br label %L131
L113:  ;
    %r401 = sub i32 %r399,%r410
    br label %L44
L117:  ;
    %r407 = sub i32 %r405,%r412
    br label %L135
L131:  ;
    %r436 = add i32 %r396,%r409
    %r426 = sub i32 %r396,%r436
    %r427 = add i32 %r426,%r409
    br label %L137
L135:  ;
    %r440 = add i32 %r407,%r412
    %r432 = sub i32 %r407,%r440
    %r433 = add i32 %r432,%r412
    br label %L139
L137:  ;
    %r429 = sub i32 %r427,%r436
    br label %L141
L139:  ;
    %r435 = sub i32 %r433,%r440
    br label %L47
L141:  ;
    %r443 = add i32 %r484,%r485
    br label %L147
L147:  ;
    %r452 = add i32 %r377,%r443
    %r448 = sub i32 %r377,%r452
    %r449 = add i32 %r448,%r443
    br label %L149
L149:  ;
    %r451 = sub i32 %r449,%r452
    br label %L155
L155:  ;
    %r460 = add i32 %r451,%r452
    %r456 = sub i32 %r451,%r460
    %r457 = add i32 %r456,%r452
    br label %L157
L157:  ;
    %r459 = sub i32 %r457,%r460
    br label %L45
L158:  ;
    %r477 = icmp slt i32 0,16
    br i1 %r477, label %L11, label %L159
L159:  ;
    %r314 = phi i32 [0,%L158],[%r166,%L11]
    %r490 = icmp slt i32 %r314,80
    br i1 %r490, label %L16, label %L160
L160:  ;
    %r313 = phi i32 [%r314,%L159],[%r199,%L16]
    %r480 = icmp slt i32 0,80
    br i1 %r480, label %L21, label %L48
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
    %r7 = getelementptr [5 x i32], ptr %r4, i32 0, i32 1
    store i32 0, ptr %r7
    %r9 = getelementptr [5 x i32], ptr %r4, i32 0, i32 2
    store i32 0, ptr %r9
    %r11 = getelementptr [5 x i32], ptr %r4, i32 0, i32 3
    store i32 0, ptr %r11
    %r13 = getelementptr [5 x i32], ptr %r4, i32 0, i32 4
    store i32 0, ptr %r13
    %r16 = call i32 @getint()
    store i32 %r16, ptr @state
    %r17 = call i32 @getint()
    call void @_sysy_starttime(i32 161)
    %r21 = getelementptr [5 x i32], ptr %r15, i32 0, i32 0
    store i32 0, ptr %r21
    %r25 = getelementptr [5 x i32], ptr %r15, i32 0, i32 1
    store i32 0, ptr %r25
    %r29 = getelementptr [5 x i32], ptr %r15, i32 0, i32 2
    store i32 0, ptr %r29
    %r33 = getelementptr [5 x i32], ptr %r15, i32 0, i32 3
    store i32 0, ptr %r33
    %r37 = getelementptr [5 x i32], ptr %r15, i32 0, i32 4
    store i32 0, ptr %r37
    %r117 = icmp sgt i32 %r17,0
    br i1 %r117, label %L31, label %L16
L2:  ;
    %r119 = phi i32 [%r80,%L25],[%r17,%L31]
    br i1 %r114, label %L6, label %L10
L6:  ;
    %r116 = phi i32 [%r57,%L18],[0,%L2]
    br label %L18
L10:  ;
    %r88 = phi i32 [0,%L2],[%r57,%L18]
    %r58 = getelementptr [32768 x i32], ptr @buffer, i32 0
    %r60 = getelementptr [5 x i32], ptr %r4, i32 0
    call void @pseudo_sha1(ptr %r58,i32 32000,ptr %r60)
    %r111 = icmp slt i32 0,5
    br i1 %r111, label %L11, label %L15
L11:  ;
    %r113 = phi i32 [%r77,%L24],[0,%L10]
    br label %L14
L14:  ;
    %r66 = getelementptr [5 x i32], ptr %r15, i32 0, i32 %r113
    %r67 = load i32, ptr %r66
    %r69 = getelementptr [5 x i32], ptr %r4, i32 0, i32 %r113
    %r70 = load i32, ptr %r69
    br label %L22
L15:  ;
    %r86 = phi i32 [0,%L10],[%r77,%L24]
    %r80 = sub i32 %r119,1
    br label %L25
L16:  ;
    %r89 = phi i32 [%r17,%L1],[%r80,%L25]
    call void @_sysy_stoptime(i32 184)
    %r83 = getelementptr [5 x i32], ptr %r15, i32 0
    call void @putarray(i32 5,ptr %r83)
    ret i32 0
L18:  ;待优化
    %r91 = load i32, ptr @state
    %r92 = mul i32 %r91,8192
    %r93 = add i32 %r90,%r92
    store i32 %r93, ptr @state
    %r95 = load i32, ptr @state
    %r96 = sdiv i32 %r95,131072
    %r97 = add i32 %r94,%r96
    store i32 %r97, ptr @state
    %r99 = load i32, ptr @state
    %r100 = mul i32 %r99,32
    %r101 = add i32 %r98,%r100
    store i32 %r101, ptr @state
    %r102 = load i32, ptr @state
    %r51 = srem i32 %r102,256
    %r53 = getelementptr [32768 x i32], ptr @buffer, i32 0, i32 %r116
    store i32 %r51, ptr %r53
    %r57 = add i32 %r116,1
    %r115 = icmp slt i32 %r57,32000
    br i1 %r115, label %L6, label %L10
L22:  ;
    %r108 = add i32 %r67,%r70
    %r104 = sub i32 %r67,%r108
    %r105 = add i32 %r104,%r70
    br label %L24
L24:  ;
    %r107 = sub i32 %r105,%r108
    store i32 %r107, ptr %r66
    %r77 = add i32 %r113,1
    %r112 = icmp slt i32 %r77,5
    br i1 %r112, label %L11, label %L15
L25:  ;
    %r118 = icmp sgt i32 %r80,0
    br i1 %r118, label %L2, label %L16
L31:  ;
    %r114 = icmp slt i32 0,32000
    br label %L2
}
