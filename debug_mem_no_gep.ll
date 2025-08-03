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
@TOKEN_NUM = global i32 0
@TOKEN_OTHER = global i32 1
@last_char = global i32 32
@num = global i32 0
@other = global i32 0
@cur_token = global i32 0
define i32 @main()
{
L0:  ;
    br label %L1
L1:  ;
    %r1 = call i32 @getint()
    %r2 = call i32 @getch()
    %r3 = call i32 @next_token()
    br label %L2
L2:  ;
    %r13 = phi i32 [%r1,%L1],[%r10,%L5]
    %r5 = icmp ne i32 %r13,0
    br i1 %r5, label %L5, label %L6
L5:  ;
    %r6 = call i32 @eval()
    call void @putint(i32 %r6)
    call void @putch(i32 10)
    %r10 = sub i32 %r13,1
    br label %L2
L6:  ;
    ret i32 0
}
define i32 @eval()
{
L0:  ;
    %r1 = alloca [256 x i32]
    %r0 = alloca [256 x i32]
    br label %L1
L1:  ;
    call void @llvm.memset.p0.i32(ptr %r0,i8 0,i32 1024,i1 0)
    call void @llvm.memset.p0.i32(ptr %r1,i8 0,i32 1024,i1 0)
    %r2 = load i32, ptr @cur_token
    %r4 = icmp ne i32 %r2,0
    br i1 %r4, label %L36, label %L6
L6:  ;
    %r7 = load i32, ptr @num
    br label %L38
L7:  ;
    %r9 = load i32, ptr @cur_token
    %r11 = icmp eq i32 %r9,1
    br i1 %r11, label %L10, label %L28
L10:  ;
    %r13 = load i32, ptr @other
    br label %L40
L16:  ;
    %r19 = call i32 @next_token()
    %r85 = getelementptr [256 x i32], ptr %r1, i32 0, i32 0
    br label %L49
L23:  ;
    %r46 = load i32, ptr @cur_token
    %r48 = icmp ne i32 %r46,0
    br i1 %r48, label %L57, label %L27
L27:  ;
    %r51 = load i32, ptr @num
    br label %L59
L28:  ;
    %r53 = call i32 @next_token()
    %r104 = getelementptr [256 x i32], ptr %r1, i32 0, i32 0
    br label %L61
L34:  ;
    %r74 = phi i32 [-1,%L36],[-1,%L57],[%r114,%L65]
    ret i32 %r74
L36:  ;
    call void @putch(i32 112)
    call void @putch(i32 97)
    call void @putch(i32 110)
    call void @putch(i32 105)
    call void @putch(i32 99)
    call void @putch(i32 33)
    call void @putch(i32 10)
    br label %L34
L38:  ;
    %r75 = getelementptr [256 x i32], ptr %r0, i32 0, i32 0
    %r76 = load i32, ptr %r75
    %r77 = add i32 %r76,1
    store i32 %r77, ptr %r75
    %r78 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r77
    store i32 %r7, ptr %r78
    %r8 = call i32 @next_token()
    br label %L7
L40:  ;
    %r79 = icmp eq i32 %r13,43
    br i1 %r79, label %L41, label %L42
L41:  ;
    br label %L44
L42:  ;
    %r80 = icmp eq i32 %r13,45
    br i1 %r80, label %L41, label %L43
L43:  ;
    %r81 = icmp eq i32 %r13,42
    br i1 %r81, label %L45, label %L46
L44:  ;
    %r84 = phi i32 [10,%L41],[20,%L45],[0,%L47]
    %r16 = icmp eq i32 %r84,0
    %r17 = zext i1 %r16 to i32
    %r18 = icmp ne i32 %r17,0
    br i1 %r18, label %L28, label %L16
L45:  ;
    br label %L44
L46:  ;
    %r83 = icmp eq i32 %r13,47
    br i1 %r83, label %L45, label %L47
L47:  ;
    %r82 = icmp eq i32 %r13,37
    br i1 %r82, label %L45, label %L44
L49:  ;
    %r86 = load i32, ptr %r85
    %r22 = icmp ne i32 %r86,0
    br i1 %r22, label %L51, label %L55
L51:  ;
    %r89 = getelementptr [256 x i32], ptr %r1, i32 0, i32 %r86
    %r90 = load i32, ptr %r89
    br label %L67
L53:  ;
    %r92 = load i32, ptr %r85
    %r93 = getelementptr [256 x i32], ptr %r1, i32 0, i32 %r92
    %r94 = load i32, ptr %r93
    %r95 = sub i32 %r92,1
    store i32 %r95, ptr %r85
    br label %L76
L55:  ;
    %r98 = add i32 %r86,1
    store i32 %r98, ptr %r85
    %r99 = getelementptr [256 x i32], ptr %r1, i32 0, i32 %r98
    store i32 %r13, ptr %r99
    br label %L23
L57:  ;
    call void @putch(i32 112)
    call void @putch(i32 97)
    call void @putch(i32 110)
    call void @putch(i32 105)
    call void @putch(i32 99)
    call void @putch(i32 33)
    call void @putch(i32 10)
    br label %L34
L59:  ;
    %r101 = load i32, ptr %r75
    %r102 = add i32 %r101,1
    store i32 %r102, ptr %r75
    %r103 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r102
    store i32 %r51, ptr %r103
    %r52 = call i32 @next_token()
    br label %L7
L61:  ;
    %r105 = load i32, ptr %r104
    %r56 = icmp ne i32 %r105,0
    br i1 %r56, label %L63, label %L65
L63:  ;
    %r108 = getelementptr [256 x i32], ptr %r1, i32 0, i32 %r105
    %r109 = load i32, ptr %r108
    %r110 = sub i32 %r105,1
    store i32 %r110, ptr %r104
    br label %L78
L65:  ;
    %r112 = load i32, ptr %r75
    %r113 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r112
    %r114 = load i32, ptr %r113
    br label %L34
L67:  ;
    %r115 = icmp eq i32 %r90,43
    br i1 %r115, label %L68, label %L69
L68:  ;
    br label %L71
L69:  ;
    %r116 = icmp eq i32 %r90,45
    br i1 %r116, label %L68, label %L70
L70:  ;
    %r117 = icmp eq i32 %r90,42
    br i1 %r117, label %L72, label %L73
L71:  ;
    %r120 = phi i32 [10,%L68],[20,%L72],[0,%L74]
    br label %L80
L72:  ;
    br label %L71
L73:  ;
    %r119 = icmp eq i32 %r90,47
    br i1 %r119, label %L72, label %L74
L74:  ;
    %r118 = icmp eq i32 %r90,37
    br i1 %r118, label %L72, label %L71
L76:  ;
    %r122 = load i32, ptr %r75
    %r123 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r122
    %r124 = load i32, ptr %r123
    %r125 = sub i32 %r122,1
    store i32 %r125, ptr %r75
    br label %L89
L78:  ;
    %r127 = load i32, ptr %r75
    %r128 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r127
    %r129 = load i32, ptr %r128
    %r130 = sub i32 %r127,1
    store i32 %r130, ptr %r75
    br label %L91
L80:  ;
    %r131 = icmp eq i32 %r13,43
    br i1 %r131, label %L81, label %L82
L81:  ;
    br label %L84
L82:  ;
    %r132 = icmp eq i32 %r13,45
    br i1 %r132, label %L81, label %L83
L83:  ;
    %r133 = icmp eq i32 %r13,42
    br i1 %r133, label %L85, label %L86
L84:  ;
    %r136 = phi i32 [10,%L81],[20,%L85],[0,%L87]
    %r28 = icmp sge i32 %r120,%r136
    br i1 %r28, label %L53, label %L55
L85:  ;
    br label %L84
L86:  ;
    %r135 = icmp eq i32 %r13,47
    br i1 %r135, label %L85, label %L87
L87:  ;
    %r134 = icmp eq i32 %r13,37
    br i1 %r134, label %L85, label %L84
L89:  ;
    %r139 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r125
    %r140 = load i32, ptr %r139
    %r141 = sub i32 %r125,1
    store i32 %r141, ptr %r75
    br label %L93
L91:  ;
    %r144 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r130
    %r145 = load i32, ptr %r144
    %r146 = sub i32 %r130,1
    store i32 %r146, ptr %r75
    br label %L105
L93:  ;
    %r147 = icmp eq i32 %r94,43
    br i1 %r147, label %L94, label %L95
L94:  ;
    %r148 = add i32 %r140,%r124
    br label %L96
L95:  ;
    %r149 = icmp eq i32 %r94,45
    br i1 %r149, label %L97, label %L98
L96:  ;
    %r157 = phi i32 [%r148,%L94],[%r150,%L97],[%r152,%L99],[%r154,%L101],[%r156,%L103],[0,%L102]
    br label %L117
L97:  ;
    %r150 = sub i32 %r140,%r124
    br label %L96
L98:  ;
    %r151 = icmp eq i32 %r94,42
    br i1 %r151, label %L99, label %L100
L99:  ;
    %r152 = mul i32 %r140,%r124
    br label %L96
L100:  ;
    %r153 = icmp eq i32 %r94,47
    br i1 %r153, label %L101, label %L102
L101:  ;
    %r154 = sdiv i32 %r140,%r124
    br label %L96
L102:  ;
    %r155 = icmp eq i32 %r94,37
    br i1 %r155, label %L103, label %L96
L103:  ;
    %r156 = srem i32 %r140,%r124
    br label %L96
L105:  ;
    %r158 = icmp eq i32 %r109,43
    br i1 %r158, label %L106, label %L107
L106:  ;
    %r159 = add i32 %r145,%r129
    br label %L108
L107:  ;
    %r160 = icmp eq i32 %r109,45
    br i1 %r160, label %L109, label %L110
L108:  ;
    %r168 = phi i32 [%r159,%L106],[%r161,%L109],[%r163,%L111],[%r165,%L113],[%r167,%L115],[0,%L114]
    br label %L119
L109:  ;
    %r161 = sub i32 %r145,%r129
    br label %L108
L110:  ;
    %r162 = icmp eq i32 %r109,42
    br i1 %r162, label %L111, label %L112
L111:  ;
    %r163 = mul i32 %r145,%r129
    br label %L108
L112:  ;
    %r164 = icmp eq i32 %r109,47
    br i1 %r164, label %L113, label %L114
L113:  ;
    %r165 = sdiv i32 %r145,%r129
    br label %L108
L114:  ;
    %r166 = icmp eq i32 %r109,37
    br i1 %r166, label %L115, label %L108
L115:  ;
    %r167 = srem i32 %r145,%r129
    br label %L108
L117:  ;
    %r170 = load i32, ptr %r75
    %r171 = add i32 %r170,1
    store i32 %r171, ptr %r75
    %r172 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r171
    store i32 %r157, ptr %r172
    br label %L49
L119:  ;
    %r174 = load i32, ptr %r75
    %r175 = add i32 %r174,1
    store i32 %r175, ptr %r75
    %r176 = getelementptr [256 x i32], ptr %r0, i32 0, i32 %r175
    store i32 %r168, ptr %r176
    br label %L61
}
define i32 @next_token()
{
L0:  ;
    br label %L2
L2:  ;
    %r0 = load i32, ptr @last_char
    %r26 = icmp eq i32 %r0,32
    br i1 %r26, label %L20, label %L21
L7:  ;
    %r4 = load i32, ptr @last_char
    br label %L26
L10:  ;
    %r7 = load i32, ptr @last_char
    %r9 = sub i32 %r7,48
    store i32 %r9, ptr @num
    br label %L31
L14:  ;
    %r13 = load i32, ptr @num
    %r15 = mul i32 %r13,10
    %r16 = load i32, ptr @last_char
    %r17 = add i32 %r15,%r16
    %r19 = sub i32 %r17,48
    store i32 %r19, ptr @num
    br label %L31
L15:  ;
    store i32 0, ptr @cur_token
    br label %L17
L16:  ;
    %r21 = load i32, ptr @last_char
    store i32 %r21, ptr @other
    br label %L33
L17:  ;
    %r24 = load i32, ptr @cur_token
    ret i32 %r24
L20:  ;
    br label %L22
L21:  ;
    %r27 = icmp eq i32 %r0,10
    br i1 %r27, label %L20, label %L22
L22:  ;
    %r28 = phi i32 [1,%L20],[0,%L21]
    %r2 = icmp ne i32 %r28,0
    br i1 %r2, label %L24, label %L7
L24:  ;
    %r29 = call i32 @getch()
    store i32 %r29, ptr @last_char
    br label %L2
L26:  ;
    %r30 = icmp sge i32 %r4,48
    br i1 %r30, label %L27, label %L28
L27:  ;
    %r31 = icmp sle i32 %r4,57
    br i1 %r31, label %L29, label %L28
L28:  ;
    br label %L29
L29:  ;
    %r32 = phi i32 [1,%L27],[0,%L28]
    %r6 = icmp ne i32 %r32,0
    br i1 %r6, label %L10, label %L16
L31:  ;
    %r33 = call i32 @getch()
    store i32 %r33, ptr @last_char
    br label %L35
L33:  ;
    %r34 = call i32 @getch()
    store i32 %r34, ptr @last_char
    store i32 1, ptr @cur_token
    br label %L17
L35:  ;
    %r35 = icmp sge i32 %r33,48
    br i1 %r35, label %L36, label %L37
L36:  ;
    %r36 = icmp sle i32 %r33,57
    br i1 %r36, label %L38, label %L37
L37:  ;
    br label %L38
L38:  ;
    %r37 = phi i32 [1,%L36],[0,%L37]
    %r12 = icmp ne i32 %r37,0
    br i1 %r12, label %L14, label %L15
}
