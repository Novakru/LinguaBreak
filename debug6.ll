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
    br label %L1
L1:  ;
    call void @_sysy_starttime(i32 399)
    %r2 = call i32 @getint()
    %r4 = call i32 @getint()
    %r7 = call i32 @func(i32 %r2,i32 %r4)
    call void @putint(i32 %r7)
    call void @putch(i32 10)
    call void @_sysy_stoptime(i32 404)
    ret i32 0
}
define i32 @func(i32 %r0,i32 %r1)
{
L0:  ;
    %r8 = alloca [100 x i32]
    br label %L512
L2:  ;
    %r749 = phi i32 [%r20,%L2],[0,%L512]
    %r16 = getelementptr [100 x i32], ptr %r8, i32 0, i32 %r749
    store i32 0, ptr %r16
    %r20 = add i32 %r749,1
    %r748 = icmp slt i32 %r20,100
    br i1 %r748, label %L2, label %L513
L7:  ;
    %r756 = phi i32 [%r735,%L514],[0,%L519]
    %r757 = phi i32 [%r719,%L514],[0,%L519]
    br i1 %r26, label %L14, label %L505
L14:  ;
    %r29 = getelementptr [100 x i32], ptr %r8, i32 0, i32 1
    store i32 1, ptr %r29
    br label %L15
L15:  ;
    %r33 = icmp sgt i32 %r1,2
    br i1 %r33, label %L18, label %L505
L18:  ;
    %r36 = getelementptr [100 x i32], ptr %r8, i32 0, i32 2
    store i32 2, ptr %r36
    br label %L19
L19:  ;
    %r40 = icmp sgt i32 %r1,3
    br i1 %r40, label %L22, label %L505
L22:  ;
    %r43 = getelementptr [100 x i32], ptr %r8, i32 0, i32 3
    store i32 3, ptr %r43
    br label %L23
L23:  ;
    %r47 = icmp sgt i32 %r1,4
    br i1 %r47, label %L26, label %L505
L26:  ;
    %r50 = getelementptr [100 x i32], ptr %r8, i32 0, i32 4
    store i32 4, ptr %r50
    br label %L27
L27:  ;
    %r54 = icmp sgt i32 %r1,5
    br i1 %r54, label %L30, label %L505
L30:  ;
    %r57 = getelementptr [100 x i32], ptr %r8, i32 0, i32 5
    store i32 5, ptr %r57
    br label %L31
L31:  ;
    %r61 = icmp sgt i32 %r1,6
    br i1 %r61, label %L34, label %L505
L34:  ;
    %r64 = getelementptr [100 x i32], ptr %r8, i32 0, i32 6
    store i32 6, ptr %r64
    br label %L35
L35:  ;
    %r68 = icmp sgt i32 %r1,7
    br i1 %r68, label %L38, label %L505
L38:  ;
    %r71 = getelementptr [100 x i32], ptr %r8, i32 0, i32 7
    store i32 7, ptr %r71
    br label %L39
L39:  ;
    %r75 = icmp sgt i32 %r1,8
    br i1 %r75, label %L42, label %L505
L42:  ;
    %r78 = getelementptr [100 x i32], ptr %r8, i32 0, i32 8
    store i32 8, ptr %r78
    br label %L43
L43:  ;
    %r82 = icmp sgt i32 %r1,9
    br i1 %r82, label %L46, label %L505
L46:  ;
    %r85 = getelementptr [100 x i32], ptr %r8, i32 0, i32 9
    store i32 9, ptr %r85
    br label %L47
L47:  ;
    %r89 = icmp sgt i32 %r1,10
    br i1 %r89, label %L50, label %L505
L50:  ;
    %r92 = getelementptr [100 x i32], ptr %r8, i32 0, i32 10
    store i32 10, ptr %r92
    br label %L51
L51:  ;
    %r96 = icmp sgt i32 %r1,11
    br i1 %r96, label %L54, label %L505
L54:  ;
    %r99 = getelementptr [100 x i32], ptr %r8, i32 0, i32 11
    store i32 11, ptr %r99
    br label %L55
L55:  ;
    %r103 = icmp sgt i32 %r1,12
    br i1 %r103, label %L58, label %L505
L58:  ;
    %r106 = getelementptr [100 x i32], ptr %r8, i32 0, i32 12
    store i32 12, ptr %r106
    br label %L59
L59:  ;
    %r110 = icmp sgt i32 %r1,13
    br i1 %r110, label %L62, label %L505
L62:  ;
    %r113 = getelementptr [100 x i32], ptr %r8, i32 0, i32 13
    store i32 13, ptr %r113
    br label %L63
L63:  ;
    %r117 = icmp sgt i32 %r1,14
    br i1 %r117, label %L66, label %L505
L66:  ;
    %r120 = getelementptr [100 x i32], ptr %r8, i32 0, i32 14
    store i32 14, ptr %r120
    br label %L67
L67:  ;
    %r124 = icmp sgt i32 %r1,15
    br i1 %r124, label %L70, label %L505
L70:  ;
    %r127 = getelementptr [100 x i32], ptr %r8, i32 0, i32 15
    store i32 15, ptr %r127
    br label %L71
L71:  ;
    %r131 = icmp sgt i32 %r1,16
    br i1 %r131, label %L74, label %L505
L74:  ;
    %r134 = getelementptr [100 x i32], ptr %r8, i32 0, i32 16
    store i32 16, ptr %r134
    br label %L75
L75:  ;
    %r138 = icmp sgt i32 %r1,17
    br i1 %r138, label %L78, label %L505
L78:  ;
    %r141 = getelementptr [100 x i32], ptr %r8, i32 0, i32 17
    store i32 17, ptr %r141
    br label %L79
L79:  ;
    %r145 = icmp sgt i32 %r1,18
    br i1 %r145, label %L82, label %L505
L82:  ;
    %r148 = getelementptr [100 x i32], ptr %r8, i32 0, i32 18
    store i32 18, ptr %r148
    br label %L83
L83:  ;
    %r152 = icmp sgt i32 %r1,19
    br i1 %r152, label %L86, label %L505
L86:  ;
    %r155 = getelementptr [100 x i32], ptr %r8, i32 0, i32 19
    store i32 19, ptr %r155
    br label %L87
L87:  ;
    %r159 = icmp sgt i32 %r1,20
    br i1 %r159, label %L90, label %L505
L90:  ;
    %r162 = getelementptr [100 x i32], ptr %r8, i32 0, i32 20
    store i32 20, ptr %r162
    br label %L91
L91:  ;
    %r166 = icmp sgt i32 %r1,21
    br i1 %r166, label %L94, label %L505
L94:  ;
    %r169 = getelementptr [100 x i32], ptr %r8, i32 0, i32 21
    store i32 21, ptr %r169
    br label %L95
L95:  ;
    %r173 = icmp sgt i32 %r1,22
    br i1 %r173, label %L98, label %L505
L98:  ;
    %r176 = getelementptr [100 x i32], ptr %r8, i32 0, i32 22
    store i32 22, ptr %r176
    br label %L99
L99:  ;
    %r180 = icmp sgt i32 %r1,23
    br i1 %r180, label %L102, label %L505
L102:  ;
    %r183 = getelementptr [100 x i32], ptr %r8, i32 0, i32 23
    store i32 23, ptr %r183
    br label %L103
L103:  ;
    %r187 = icmp sgt i32 %r1,24
    br i1 %r187, label %L106, label %L505
L106:  ;
    %r190 = getelementptr [100 x i32], ptr %r8, i32 0, i32 24
    store i32 24, ptr %r190
    br label %L107
L107:  ;
    %r194 = icmp sgt i32 %r1,25
    br i1 %r194, label %L110, label %L505
L110:  ;
    %r197 = getelementptr [100 x i32], ptr %r8, i32 0, i32 25
    store i32 25, ptr %r197
    br label %L111
L111:  ;
    %r201 = icmp sgt i32 %r1,26
    br i1 %r201, label %L114, label %L505
L114:  ;
    %r204 = getelementptr [100 x i32], ptr %r8, i32 0, i32 26
    store i32 26, ptr %r204
    br label %L115
L115:  ;
    %r208 = icmp sgt i32 %r1,27
    br i1 %r208, label %L118, label %L505
L118:  ;
    %r211 = getelementptr [100 x i32], ptr %r8, i32 0, i32 27
    store i32 27, ptr %r211
    br label %L119
L119:  ;
    %r215 = icmp sgt i32 %r1,28
    br i1 %r215, label %L122, label %L505
L122:  ;
    %r218 = getelementptr [100 x i32], ptr %r8, i32 0, i32 28
    store i32 28, ptr %r218
    br label %L123
L123:  ;
    %r222 = icmp sgt i32 %r1,29
    br i1 %r222, label %L126, label %L505
L126:  ;
    %r225 = getelementptr [100 x i32], ptr %r8, i32 0, i32 29
    store i32 29, ptr %r225
    br label %L127
L127:  ;
    %r229 = icmp sgt i32 %r1,30
    br i1 %r229, label %L130, label %L505
L130:  ;
    %r232 = getelementptr [100 x i32], ptr %r8, i32 0, i32 30
    store i32 30, ptr %r232
    br label %L131
L131:  ;
    %r236 = icmp sgt i32 %r1,31
    br i1 %r236, label %L134, label %L505
L134:  ;
    %r239 = getelementptr [100 x i32], ptr %r8, i32 0, i32 31
    store i32 31, ptr %r239
    br label %L135
L135:  ;
    %r243 = icmp sgt i32 %r1,32
    br i1 %r243, label %L138, label %L505
L138:  ;
    %r246 = getelementptr [100 x i32], ptr %r8, i32 0, i32 32
    store i32 32, ptr %r246
    br label %L139
L139:  ;
    %r250 = icmp sgt i32 %r1,33
    br i1 %r250, label %L142, label %L505
L142:  ;
    %r253 = getelementptr [100 x i32], ptr %r8, i32 0, i32 33
    store i32 33, ptr %r253
    br label %L143
L143:  ;
    %r257 = icmp sgt i32 %r1,34
    br i1 %r257, label %L146, label %L505
L146:  ;
    %r260 = getelementptr [100 x i32], ptr %r8, i32 0, i32 34
    store i32 34, ptr %r260
    br label %L147
L147:  ;
    %r264 = icmp sgt i32 %r1,35
    br i1 %r264, label %L150, label %L505
L150:  ;
    %r267 = getelementptr [100 x i32], ptr %r8, i32 0, i32 35
    store i32 35, ptr %r267
    br label %L151
L151:  ;
    %r271 = icmp sgt i32 %r1,36
    br i1 %r271, label %L154, label %L505
L154:  ;
    %r274 = getelementptr [100 x i32], ptr %r8, i32 0, i32 36
    store i32 36, ptr %r274
    br label %L155
L155:  ;
    %r278 = icmp sgt i32 %r1,37
    br i1 %r278, label %L158, label %L505
L158:  ;
    %r281 = getelementptr [100 x i32], ptr %r8, i32 0, i32 37
    store i32 37, ptr %r281
    br label %L159
L159:  ;
    %r285 = icmp sgt i32 %r1,38
    br i1 %r285, label %L162, label %L505
L162:  ;
    %r288 = getelementptr [100 x i32], ptr %r8, i32 0, i32 38
    store i32 38, ptr %r288
    br label %L163
L163:  ;
    %r292 = icmp sgt i32 %r1,39
    br i1 %r292, label %L166, label %L505
L166:  ;
    %r295 = getelementptr [100 x i32], ptr %r8, i32 0, i32 39
    store i32 39, ptr %r295
    br label %L167
L167:  ;
    %r299 = icmp sgt i32 %r1,40
    br i1 %r299, label %L170, label %L505
L170:  ;
    %r302 = getelementptr [100 x i32], ptr %r8, i32 0, i32 40
    store i32 40, ptr %r302
    br label %L171
L171:  ;
    %r306 = icmp sgt i32 %r1,41
    br i1 %r306, label %L174, label %L505
L174:  ;
    %r309 = getelementptr [100 x i32], ptr %r8, i32 0, i32 41
    store i32 41, ptr %r309
    br label %L175
L175:  ;
    %r313 = icmp sgt i32 %r1,42
    br i1 %r313, label %L178, label %L505
L178:  ;
    %r316 = getelementptr [100 x i32], ptr %r8, i32 0, i32 42
    store i32 42, ptr %r316
    br label %L179
L179:  ;
    %r320 = icmp sgt i32 %r1,43
    br i1 %r320, label %L182, label %L505
L182:  ;
    %r323 = getelementptr [100 x i32], ptr %r8, i32 0, i32 43
    store i32 43, ptr %r323
    br label %L183
L183:  ;
    %r327 = icmp sgt i32 %r1,44
    br i1 %r327, label %L186, label %L505
L186:  ;
    %r330 = getelementptr [100 x i32], ptr %r8, i32 0, i32 44
    store i32 44, ptr %r330
    br label %L187
L187:  ;
    %r334 = icmp sgt i32 %r1,45
    br i1 %r334, label %L190, label %L505
L190:  ;
    %r337 = getelementptr [100 x i32], ptr %r8, i32 0, i32 45
    store i32 45, ptr %r337
    br label %L191
L191:  ;
    %r341 = icmp sgt i32 %r1,46
    br i1 %r341, label %L194, label %L505
L194:  ;
    %r344 = getelementptr [100 x i32], ptr %r8, i32 0, i32 46
    store i32 46, ptr %r344
    br label %L195
L195:  ;
    %r348 = icmp sgt i32 %r1,47
    br i1 %r348, label %L198, label %L505
L198:  ;
    %r351 = getelementptr [100 x i32], ptr %r8, i32 0, i32 47
    store i32 47, ptr %r351
    br label %L199
L199:  ;
    %r355 = icmp sgt i32 %r1,48
    br i1 %r355, label %L202, label %L505
L202:  ;
    %r358 = getelementptr [100 x i32], ptr %r8, i32 0, i32 48
    store i32 48, ptr %r358
    br label %L203
L203:  ;
    %r362 = icmp sgt i32 %r1,49
    br i1 %r362, label %L206, label %L505
L206:  ;
    %r365 = getelementptr [100 x i32], ptr %r8, i32 0, i32 49
    store i32 49, ptr %r365
    br label %L207
L207:  ;
    %r369 = icmp sgt i32 %r1,50
    br i1 %r369, label %L210, label %L505
L210:  ;
    %r372 = getelementptr [100 x i32], ptr %r8, i32 0, i32 50
    store i32 50, ptr %r372
    br label %L211
L211:  ;
    %r376 = icmp sgt i32 %r1,51
    br i1 %r376, label %L214, label %L505
L214:  ;
    %r379 = getelementptr [100 x i32], ptr %r8, i32 0, i32 51
    store i32 51, ptr %r379
    br label %L215
L215:  ;
    %r383 = icmp sgt i32 %r1,52
    br i1 %r383, label %L218, label %L505
L218:  ;
    %r386 = getelementptr [100 x i32], ptr %r8, i32 0, i32 52
    store i32 52, ptr %r386
    br label %L219
L219:  ;
    %r390 = icmp sgt i32 %r1,53
    br i1 %r390, label %L222, label %L505
L222:  ;
    %r393 = getelementptr [100 x i32], ptr %r8, i32 0, i32 53
    store i32 53, ptr %r393
    br label %L223
L223:  ;
    %r397 = icmp sgt i32 %r1,54
    br i1 %r397, label %L226, label %L505
L226:  ;
    %r400 = getelementptr [100 x i32], ptr %r8, i32 0, i32 54
    store i32 54, ptr %r400
    br label %L227
L227:  ;
    %r404 = icmp sgt i32 %r1,55
    br i1 %r404, label %L230, label %L505
L230:  ;
    %r407 = getelementptr [100 x i32], ptr %r8, i32 0, i32 55
    store i32 55, ptr %r407
    br label %L231
L231:  ;
    %r411 = icmp sgt i32 %r1,56
    br i1 %r411, label %L234, label %L505
L234:  ;
    %r414 = getelementptr [100 x i32], ptr %r8, i32 0, i32 56
    store i32 56, ptr %r414
    br label %L235
L235:  ;
    %r418 = icmp sgt i32 %r1,57
    br i1 %r418, label %L238, label %L505
L238:  ;
    %r421 = getelementptr [100 x i32], ptr %r8, i32 0, i32 57
    store i32 57, ptr %r421
    br label %L239
L239:  ;
    %r425 = icmp sgt i32 %r1,58
    br i1 %r425, label %L242, label %L505
L242:  ;
    %r428 = getelementptr [100 x i32], ptr %r8, i32 0, i32 58
    store i32 58, ptr %r428
    br label %L243
L243:  ;
    %r432 = icmp sgt i32 %r1,59
    br i1 %r432, label %L246, label %L505
L246:  ;
    %r435 = getelementptr [100 x i32], ptr %r8, i32 0, i32 59
    store i32 59, ptr %r435
    br label %L247
L247:  ;
    %r439 = icmp sgt i32 %r1,60
    br i1 %r439, label %L250, label %L505
L250:  ;
    %r442 = getelementptr [100 x i32], ptr %r8, i32 0, i32 60
    store i32 60, ptr %r442
    br label %L251
L251:  ;
    %r446 = icmp sgt i32 %r1,61
    br i1 %r446, label %L254, label %L505
L254:  ;
    %r449 = getelementptr [100 x i32], ptr %r8, i32 0, i32 61
    store i32 61, ptr %r449
    br label %L255
L255:  ;
    %r453 = icmp sgt i32 %r1,62
    br i1 %r453, label %L258, label %L505
L258:  ;
    %r456 = getelementptr [100 x i32], ptr %r8, i32 0, i32 62
    store i32 62, ptr %r456
    br label %L259
L259:  ;
    %r460 = icmp sgt i32 %r1,63
    br i1 %r460, label %L262, label %L505
L262:  ;
    %r463 = getelementptr [100 x i32], ptr %r8, i32 0, i32 63
    store i32 63, ptr %r463
    br label %L263
L263:  ;
    %r467 = icmp sgt i32 %r1,64
    br i1 %r467, label %L266, label %L505
L266:  ;
    %r470 = getelementptr [100 x i32], ptr %r8, i32 0, i32 64
    store i32 64, ptr %r470
    br label %L267
L267:  ;
    %r474 = icmp sgt i32 %r1,65
    br i1 %r474, label %L270, label %L505
L270:  ;
    %r477 = getelementptr [100 x i32], ptr %r8, i32 0, i32 65
    store i32 65, ptr %r477
    br label %L271
L271:  ;
    %r481 = icmp sgt i32 %r1,66
    br i1 %r481, label %L274, label %L505
L274:  ;
    %r484 = getelementptr [100 x i32], ptr %r8, i32 0, i32 66
    store i32 66, ptr %r484
    br label %L275
L275:  ;
    %r488 = icmp sgt i32 %r1,67
    br i1 %r488, label %L278, label %L505
L278:  ;
    %r491 = getelementptr [100 x i32], ptr %r8, i32 0, i32 67
    store i32 67, ptr %r491
    br label %L279
L279:  ;
    %r495 = icmp sgt i32 %r1,68
    br i1 %r495, label %L282, label %L505
L282:  ;
    %r498 = getelementptr [100 x i32], ptr %r8, i32 0, i32 68
    store i32 68, ptr %r498
    br label %L283
L283:  ;
    %r502 = icmp sgt i32 %r1,69
    br i1 %r502, label %L286, label %L505
L286:  ;
    %r505 = getelementptr [100 x i32], ptr %r8, i32 0, i32 69
    store i32 69, ptr %r505
    br label %L287
L287:  ;
    %r509 = icmp sgt i32 %r1,70
    br i1 %r509, label %L290, label %L505
L290:  ;
    %r512 = getelementptr [100 x i32], ptr %r8, i32 0, i32 70
    store i32 70, ptr %r512
    br label %L291
L291:  ;
    %r516 = icmp sgt i32 %r1,71
    br i1 %r516, label %L294, label %L505
L294:  ;
    %r519 = getelementptr [100 x i32], ptr %r8, i32 0, i32 71
    store i32 71, ptr %r519
    br label %L295
L295:  ;
    %r523 = icmp sgt i32 %r1,72
    br i1 %r523, label %L298, label %L505
L298:  ;
    %r526 = getelementptr [100 x i32], ptr %r8, i32 0, i32 72
    store i32 72, ptr %r526
    br label %L299
L299:  ;
    %r530 = icmp sgt i32 %r1,73
    br i1 %r530, label %L302, label %L505
L302:  ;
    %r533 = getelementptr [100 x i32], ptr %r8, i32 0, i32 73
    store i32 73, ptr %r533
    br label %L303
L303:  ;
    %r537 = icmp sgt i32 %r1,74
    br i1 %r537, label %L306, label %L505
L306:  ;
    %r540 = getelementptr [100 x i32], ptr %r8, i32 0, i32 74
    store i32 74, ptr %r540
    br label %L307
L307:  ;
    %r544 = icmp sgt i32 %r1,75
    br i1 %r544, label %L310, label %L505
L310:  ;
    %r547 = getelementptr [100 x i32], ptr %r8, i32 0, i32 75
    store i32 75, ptr %r547
    br label %L311
L311:  ;
    %r551 = icmp sgt i32 %r1,76
    br i1 %r551, label %L314, label %L505
L314:  ;
    %r554 = getelementptr [100 x i32], ptr %r8, i32 0, i32 76
    store i32 76, ptr %r554
    br label %L315
L315:  ;
    %r558 = icmp sgt i32 %r1,77
    br i1 %r558, label %L318, label %L505
L318:  ;
    %r561 = getelementptr [100 x i32], ptr %r8, i32 0, i32 77
    store i32 77, ptr %r561
    br label %L319
L319:  ;
    %r565 = icmp sgt i32 %r1,78
    br i1 %r565, label %L322, label %L505
L322:  ;
    %r568 = getelementptr [100 x i32], ptr %r8, i32 0, i32 78
    store i32 78, ptr %r568
    br label %L323
L323:  ;
    %r572 = icmp sgt i32 %r1,79
    br i1 %r572, label %L326, label %L505
L326:  ;
    %r575 = getelementptr [100 x i32], ptr %r8, i32 0, i32 79
    store i32 79, ptr %r575
    br label %L327
L327:  ;
    %r579 = icmp sgt i32 %r1,80
    br i1 %r579, label %L330, label %L505
L330:  ;
    %r582 = getelementptr [100 x i32], ptr %r8, i32 0, i32 80
    store i32 80, ptr %r582
    br label %L331
L331:  ;
    %r586 = icmp sgt i32 %r1,81
    br i1 %r586, label %L334, label %L505
L334:  ;
    %r589 = getelementptr [100 x i32], ptr %r8, i32 0, i32 81
    store i32 81, ptr %r589
    br label %L335
L335:  ;
    %r593 = icmp sgt i32 %r1,82
    br i1 %r593, label %L338, label %L505
L338:  ;
    %r596 = getelementptr [100 x i32], ptr %r8, i32 0, i32 82
    store i32 82, ptr %r596
    br label %L339
L339:  ;
    %r600 = icmp sgt i32 %r1,83
    br i1 %r600, label %L342, label %L505
L342:  ;
    %r603 = getelementptr [100 x i32], ptr %r8, i32 0, i32 83
    store i32 83, ptr %r603
    br label %L343
L343:  ;
    %r607 = icmp sgt i32 %r1,84
    br i1 %r607, label %L346, label %L505
L346:  ;
    %r610 = getelementptr [100 x i32], ptr %r8, i32 0, i32 84
    store i32 84, ptr %r610
    br label %L347
L347:  ;
    %r614 = icmp sgt i32 %r1,85
    br i1 %r614, label %L350, label %L505
L350:  ;
    %r617 = getelementptr [100 x i32], ptr %r8, i32 0, i32 85
    store i32 85, ptr %r617
    br label %L351
L351:  ;
    %r621 = icmp sgt i32 %r1,86
    br i1 %r621, label %L354, label %L505
L354:  ;
    %r624 = getelementptr [100 x i32], ptr %r8, i32 0, i32 86
    store i32 86, ptr %r624
    br label %L355
L355:  ;
    %r628 = icmp sgt i32 %r1,87
    br i1 %r628, label %L358, label %L505
L358:  ;
    %r631 = getelementptr [100 x i32], ptr %r8, i32 0, i32 87
    store i32 87, ptr %r631
    br label %L359
L359:  ;
    %r635 = icmp sgt i32 %r1,88
    br i1 %r635, label %L362, label %L505
L362:  ;
    %r638 = getelementptr [100 x i32], ptr %r8, i32 0, i32 88
    store i32 88, ptr %r638
    br label %L363
L363:  ;
    %r642 = icmp sgt i32 %r1,89
    br i1 %r642, label %L366, label %L505
L366:  ;
    %r645 = getelementptr [100 x i32], ptr %r8, i32 0, i32 89
    store i32 89, ptr %r645
    br label %L367
L367:  ;
    %r649 = icmp sgt i32 %r1,90
    br i1 %r649, label %L370, label %L505
L370:  ;
    %r652 = getelementptr [100 x i32], ptr %r8, i32 0, i32 90
    store i32 90, ptr %r652
    br label %L371
L371:  ;
    %r656 = icmp sgt i32 %r1,91
    br i1 %r656, label %L374, label %L505
L374:  ;
    %r659 = getelementptr [100 x i32], ptr %r8, i32 0, i32 91
    store i32 91, ptr %r659
    br label %L375
L375:  ;
    %r663 = icmp sgt i32 %r1,92
    br i1 %r663, label %L378, label %L505
L378:  ;
    %r666 = getelementptr [100 x i32], ptr %r8, i32 0, i32 92
    store i32 92, ptr %r666
    br label %L379
L379:  ;
    %r670 = icmp sgt i32 %r1,93
    br i1 %r670, label %L382, label %L505
L382:  ;
    %r673 = getelementptr [100 x i32], ptr %r8, i32 0, i32 93
    store i32 93, ptr %r673
    br label %L383
L383:  ;
    %r677 = icmp sgt i32 %r1,94
    br i1 %r677, label %L386, label %L505
L386:  ;
    %r680 = getelementptr [100 x i32], ptr %r8, i32 0, i32 94
    store i32 94, ptr %r680
    br label %L387
L387:  ;
    %r684 = icmp sgt i32 %r1,95
    br i1 %r684, label %L390, label %L505
L390:  ;
    %r687 = getelementptr [100 x i32], ptr %r8, i32 0, i32 95
    store i32 95, ptr %r687
    br label %L391
L391:  ;
    %r691 = icmp sgt i32 %r1,96
    br i1 %r691, label %L394, label %L505
L394:  ;
    %r694 = getelementptr [100 x i32], ptr %r8, i32 0, i32 96
    store i32 96, ptr %r694
    br label %L395
L395:  ;
    %r698 = icmp sgt i32 %r1,97
    br i1 %r698, label %L398, label %L505
L398:  ;
    %r701 = getelementptr [100 x i32], ptr %r8, i32 0, i32 97
    store i32 97, ptr %r701
    br label %L399
L399:  ;
    %r705 = icmp sgt i32 %r1,98
    br i1 %r705, label %L402, label %L505
L402:  ;
    %r708 = getelementptr [100 x i32], ptr %r8, i32 0, i32 98
    store i32 98, ptr %r708
    br label %L403
L403:  ;
    %r712 = icmp sgt i32 %r1,99
    br i1 %r712, label %L406, label %L505
L406:  ;
    %r715 = getelementptr [100 x i32], ptr %r8, i32 0, i32 99
    store i32 99, ptr %r715
    br label %L505
L505:  ;
    %r719 = add i32 %r757,1
    br i1 %r750, label %L506, label %L510
L506:  ;
    %r752 = phi i32 [%r729,%L509],[%r756,%L505]
    %r753 = phi i32 [%r732,%L509],[0,%L505]
    br label %L509
L509:  ;
    %r727 = getelementptr [100 x i32], ptr %r8, i32 0, i32 %r753
    %r728 = load i32, ptr %r727
    %r729 = add i32 %r752,%r728
    %r732 = add i32 %r753,1
    %r751 = icmp slt i32 %r732,100
    br i1 %r751, label %L506, label %L510
L510:  ;
    %r743 = phi i32 [%r756,%L505],[%r729,%L509]
    %r738 = phi i32 [0,%L505],[%r732,%L509]
    %r735 = srem i32 %r743,65535
    br label %L514
L511:  ;
    %r742 = phi i32 [0,%L513],[%r735,%L514]
    %r741 = phi i32 [0,%L513],[%r719,%L514]
    ret i32 %r742
L512:  ;
    %r747 = icmp slt i32 0,100
    br i1 %r747, label %L2, label %L513
L513:  ;
    %r740 = phi i32 [0,%L512],[%r20,%L2]
    %r754 = icmp slt i32 0,%r0
    br i1 %r754, label %L519, label %L511
L514:  ;
    %r755 = icmp slt i32 %r719,%r0
    br i1 %r755, label %L7, label %L511
L519:  ;
    %r26 = icmp sgt i32 %r1,1
    %r750 = icmp slt i32 0,100
    br label %L7
}
