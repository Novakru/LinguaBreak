	.text
	.globl main
	.attribute arch, "rv64gc"
func:
.func_0:
	addi		sp,sp,-416
	jal			x0,.func_512
.func_2:
	slli		t1,t0,2
	addi		t2,sp,0
	add			t1,t2,t1
	addiw		t2,x0,0
	sw			t2,0(t1)
	addiw		t2,t0,1
	add			t0,t2,x0
	add			t1,t2,x0
	addiw		t3,x0,100
	blt			t2,t3,.func_2
	jal			x0,.func_513
.func_7:
	addiw		t0,x0,1
	bgt			a1,t0,.func_14
	jal			x0,.func_505
.func_14:                         ; s[1]=1
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,1
	sw			t1,0(t0)
	jal			x0,.func_15
.func_15:
	addiw		t0,x0,2
	bgt			a1,t0,.func_18
	jal			x0,.func_505
.func_18:                        ; s[2]=2
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,2
	sw			t1,0(t0)
	jal			x0,.func_19
.func_19:
	addiw		t0,x0,3
	bgt			a1,t0,.func_22
	jal			x0,.func_505
.func_22:                         ; s[3]=3
	addiw		t0,x0,1
	addiw		t1,x0,3
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,3
	sw			t1,0(t0)
	jal			x0,.func_23
.func_23:
	addiw		t0,x0,4
	bgt			a1,t0,.func_26
	jal			x0,.func_505
.func_26:
	addiw		t0,x0,1
	addiw		t1,x0,4
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,4
	sw			t1,0(t0)
	jal			x0,.func_27
.func_27:
	addiw		t0,x0,5
	bgt			a1,t0,.func_30
	jal			x0,.func_505
.func_30:
	addiw		t0,x0,1
	addiw		t1,x0,5
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,5
	sw			t1,0(t0)
	jal			x0,.func_31
.func_31:
	addiw		t0,x0,6
	bgt			a1,t0,.func_34
	jal			x0,.func_505
.func_34:
	addiw		t0,x0,1
	addiw		t1,x0,6
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,6
	sw			t1,0(t0)
	jal			x0,.func_35
.func_35:
	addiw		t0,x0,7
	bgt			a1,t0,.func_38
	jal			x0,.func_505
.func_38:
	addiw		t0,x0,1
	addiw		t1,x0,7
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,7
	sw			t1,0(t0)
	jal			x0,.func_39
.func_39:
	addiw		t0,x0,8
	bgt			a1,t0,.func_42
	jal			x0,.func_505
.func_42:
	addiw		t0,x0,1
	addiw		t1,x0,8
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,8
	sw			t1,0(t0)
	jal			x0,.func_43
.func_43:
	addiw		t0,x0,9
	bgt			a1,t0,.func_46
	jal			x0,.func_505
.func_46:
	addiw		t0,x0,1
	addiw		t1,x0,9
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,9
	sw			t1,0(t0)
	jal			x0,.func_47
.func_47:
	addiw		t0,x0,10
	bgt			a1,t0,.func_50
	jal			x0,.func_505
.func_50:
	addiw		t0,x0,1
	addiw		t1,x0,10
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,10
	sw			t1,0(t0)
	jal			x0,.func_51
.func_51:
	addiw		t0,x0,11
	bgt			a1,t0,.func_54
	jal			x0,.func_505
.func_54:
	addiw		t0,x0,1
	addiw		t1,x0,11
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,11
	sw			t1,0(t0)
	jal			x0,.func_55
.func_55:
	addiw		t0,x0,12
	bgt			a1,t0,.func_58
	jal			x0,.func_505
.func_58:
	addiw		t0,x0,1
	addiw		t1,x0,12
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,12
	sw			t1,0(t0)
	jal			x0,.func_59
.func_59:
	addiw		t0,x0,13
	bgt			a1,t0,.func_62
	jal			x0,.func_505
.func_62:
	addiw		t0,x0,1
	addiw		t1,x0,13
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,13
	sw			t1,0(t0)
	jal			x0,.func_63
.func_63:
	addiw		t0,x0,14
	bgt			a1,t0,.func_66
	jal			x0,.func_505
.func_66:
	addiw		t0,x0,1
	addiw		t1,x0,14
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,14
	sw			t1,0(t0)
	jal			x0,.func_67
.func_67:
	addiw		t0,x0,15
	bgt			a1,t0,.func_70
	jal			x0,.func_505
.func_70:
	addiw		t0,x0,1
	addiw		t1,x0,15
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,15
	sw			t1,0(t0)
	jal			x0,.func_71
.func_71:
	addiw		t0,x0,16
	bgt			a1,t0,.func_74
	jal			x0,.func_505
.func_74:
	addiw		t0,x0,1
	addiw		t1,x0,16
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,16
	sw			t1,0(t0)
	jal			x0,.func_75
.func_75:
	addiw		t0,x0,17
	bgt			a1,t0,.func_78
	jal			x0,.func_505
.func_78:
	addiw		t0,x0,1
	addiw		t1,x0,17
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,17
	sw			t1,0(t0)
	jal			x0,.func_79
.func_79:
	addiw		t0,x0,18
	bgt			a1,t0,.func_82
	jal			x0,.func_505
.func_82:
	addiw		t0,x0,1
	addiw		t1,x0,18
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,18
	sw			t1,0(t0)
	jal			x0,.func_83
.func_83:
	addiw		t0,x0,19
	bgt			a1,t0,.func_86
	jal			x0,.func_505
.func_86:
	addiw		t0,x0,1
	addiw		t1,x0,19
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,19
	sw			t1,0(t0)
	jal			x0,.func_87
.func_87:
	addiw		t0,x0,20
	bgt			a1,t0,.func_90
	jal			x0,.func_505
.func_90:
	addiw		t0,x0,1
	addiw		t1,x0,20
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,20
	sw			t1,0(t0)
	jal			x0,.func_91
.func_91:
	addiw		t0,x0,21
	bgt			a1,t0,.func_94
	jal			x0,.func_505
.func_94:
	addiw		t0,x0,1
	addiw		t1,x0,21
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,21
	sw			t1,0(t0)
	jal			x0,.func_95
.func_95:
	addiw		t0,x0,22
	bgt			a1,t0,.func_98
	jal			x0,.func_505
.func_98:
	addiw		t0,x0,1
	addiw		t1,x0,22
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,22
	sw			t1,0(t0)
	jal			x0,.func_99
.func_99:
	addiw		t0,x0,23
	bgt			a1,t0,.func_102
	jal			x0,.func_505
.func_102:
	addiw		t0,x0,1
	addiw		t1,x0,23
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,23
	sw			t1,0(t0)
	jal			x0,.func_103
.func_103:
	addiw		t0,x0,24
	bgt			a1,t0,.func_106
	jal			x0,.func_505
.func_106:
	addiw		t0,x0,1
	addiw		t1,x0,24
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,24
	sw			t1,0(t0)
	jal			x0,.func_107
.func_107:
	addiw		t0,x0,25
	bgt			a1,t0,.func_110
	jal			x0,.func_505
.func_110:
	addiw		t0,x0,1
	addiw		t1,x0,25
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,25
	sw			t1,0(t0)
	jal			x0,.func_111
.func_111:
	addiw		t0,x0,26
	bgt			a1,t0,.func_114
	jal			x0,.func_505
.func_114:
	addiw		t0,x0,1
	addiw		t1,x0,26
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,26
	sw			t1,0(t0)
	jal			x0,.func_115
.func_115:
	addiw		t0,x0,27
	bgt			a1,t0,.func_118
	jal			x0,.func_505
.func_118:
	addiw		t0,x0,1
	addiw		t1,x0,27
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,27
	sw			t1,0(t0)
	jal			x0,.func_119
.func_119:
	addiw		t0,x0,28
	bgt			a1,t0,.func_122
	jal			x0,.func_505
.func_122:
	addiw		t0,x0,1
	addiw		t1,x0,28
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,28
	sw			t1,0(t0)
	jal			x0,.func_123
.func_123:
	addiw		t0,x0,29
	bgt			a1,t0,.func_126
	jal			x0,.func_505
.func_126:
	addiw		t0,x0,1
	addiw		t1,x0,29
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,29
	sw			t1,0(t0)
	jal			x0,.func_127
.func_127:
	addiw		t0,x0,30
	bgt			a1,t0,.func_130
	jal			x0,.func_505
.func_130:
	addiw		t0,x0,1
	addiw		t1,x0,30
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,30
	sw			t1,0(t0)
	jal			x0,.func_131
.func_131:
	addiw		t0,x0,31
	bgt			a1,t0,.func_134
	jal			x0,.func_505
.func_134:
	addiw		t0,x0,1
	addiw		t1,x0,31
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,31
	sw			t1,0(t0)
	jal			x0,.func_135
.func_135:
	addiw		t0,x0,32
	bgt			a1,t0,.func_138
	jal			x0,.func_505
.func_138:
	addiw		t0,x0,1
	addiw		t1,x0,32
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,32
	sw			t1,0(t0)
	jal			x0,.func_139
.func_139:
	addiw		t0,x0,33
	bgt			a1,t0,.func_142
	jal			x0,.func_505
.func_142:
	addiw		t0,x0,1
	addiw		t1,x0,33
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,33
	sw			t1,0(t0)
	jal			x0,.func_143
.func_143:
	addiw		t0,x0,34
	bgt			a1,t0,.func_146
	jal			x0,.func_505
.func_146:
	addiw		t0,x0,1
	addiw		t1,x0,34
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,34
	sw			t1,0(t0)
	jal			x0,.func_147
.func_147:
	addiw		t0,x0,35
	bgt			a1,t0,.func_150
	jal			x0,.func_505
.func_150:
	addiw		t0,x0,1
	addiw		t1,x0,35
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,35
	sw			t1,0(t0)
	jal			x0,.func_151
.func_151:
	addiw		t0,x0,36
	bgt			a1,t0,.func_154
	jal			x0,.func_505
.func_154:
	addiw		t0,x0,1
	addiw		t1,x0,36
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,36
	sw			t1,0(t0)
	jal			x0,.func_155
.func_155:
	addiw		t0,x0,37
	bgt			a1,t0,.func_158
	jal			x0,.func_505
.func_158:
	addiw		t0,x0,1
	addiw		t1,x0,37
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,37
	sw			t1,0(t0)
	jal			x0,.func_159
.func_159:
	addiw		t0,x0,38
	bgt			a1,t0,.func_162
	jal			x0,.func_505
.func_162:
	addiw		t0,x0,1
	addiw		t1,x0,38
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,38
	sw			t1,0(t0)
	jal			x0,.func_163
.func_163:
	addiw		t0,x0,39
	bgt			a1,t0,.func_166
	jal			x0,.func_505
.func_166:
	addiw		t0,x0,1
	addiw		t1,x0,39
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,39
	sw			t1,0(t0)
	jal			x0,.func_167
.func_167:
	addiw		t0,x0,40
	bgt			a1,t0,.func_170
	jal			x0,.func_505
.func_170:
	addiw		t0,x0,1
	addiw		t1,x0,40
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,40
	sw			t1,0(t0)
	jal			x0,.func_171
.func_171:
	addiw		t0,x0,41
	bgt			a1,t0,.func_174
	jal			x0,.func_505
.func_174:
	addiw		t0,x0,1
	addiw		t1,x0,41
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,41
	sw			t1,0(t0)
	jal			x0,.func_175
.func_175:
	addiw		t0,x0,42
	bgt			a1,t0,.func_178
	jal			x0,.func_505
.func_178:
	addiw		t0,x0,1
	addiw		t1,x0,42
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,42
	sw			t1,0(t0)
	jal			x0,.func_179
.func_179:
	addiw		t0,x0,43
	bgt			a1,t0,.func_182
	jal			x0,.func_505
.func_182:
	addiw		t0,x0,1
	addiw		t1,x0,43
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,43
	sw			t1,0(t0)
	jal			x0,.func_183
.func_183:
	addiw		t0,x0,44
	bgt			a1,t0,.func_186
	jal			x0,.func_505
.func_186:
	addiw		t0,x0,1
	addiw		t1,x0,44
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,44
	sw			t1,0(t0)
	jal			x0,.func_187
.func_187:
	addiw		t0,x0,45
	bgt			a1,t0,.func_190
	jal			x0,.func_505
.func_190:
	addiw		t0,x0,1
	addiw		t1,x0,45
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,45
	sw			t1,0(t0)
	jal			x0,.func_191
.func_191:
	addiw		t0,x0,46
	bgt			a1,t0,.func_194
	jal			x0,.func_505
.func_194:
	addiw		t0,x0,1
	addiw		t1,x0,46
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,46
	sw			t1,0(t0)
	jal			x0,.func_195
.func_195:
	addiw		t0,x0,47
	bgt			a1,t0,.func_198
	jal			x0,.func_505
.func_198:
	addiw		t0,x0,1
	addiw		t1,x0,47
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,47
	sw			t1,0(t0)
	jal			x0,.func_199
.func_199:
	addiw		t0,x0,48
	bgt			a1,t0,.func_202
	jal			x0,.func_505
.func_202:
	addiw		t0,x0,1
	addiw		t1,x0,48
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,48
	sw			t1,0(t0)
	jal			x0,.func_203
.func_203:
	addiw		t0,x0,49
	bgt			a1,t0,.func_206
	jal			x0,.func_505
.func_206:
	addiw		t0,x0,1
	addiw		t1,x0,49
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,49
	sw			t1,0(t0)
	jal			x0,.func_207
.func_207:
	addiw		t0,x0,50
	bgt			a1,t0,.func_210
	jal			x0,.func_505
.func_210:
	addiw		t0,x0,1
	addiw		t1,x0,50
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,50
	sw			t1,0(t0)
	jal			x0,.func_211
.func_211:
	addiw		t0,x0,51
	bgt			a1,t0,.func_214
	jal			x0,.func_505
.func_214:
	addiw		t0,x0,1
	addiw		t1,x0,51
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,51
	sw			t1,0(t0)
	jal			x0,.func_215
.func_215:
	addiw		t0,x0,52
	bgt			a1,t0,.func_218
	jal			x0,.func_505
.func_218:
	addiw		t0,x0,1
	addiw		t1,x0,52
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,52
	sw			t1,0(t0)
	jal			x0,.func_219
.func_219:
	addiw		t0,x0,53
	bgt			a1,t0,.func_222
	jal			x0,.func_505
.func_222:
	addiw		t0,x0,1
	addiw		t1,x0,53
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,53
	sw			t1,0(t0)
	jal			x0,.func_223
.func_223:
	addiw		t0,x0,54
	bgt			a1,t0,.func_226
	jal			x0,.func_505
.func_226:
	addiw		t0,x0,1
	addiw		t1,x0,54
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,54
	sw			t1,0(t0)
	jal			x0,.func_227
.func_227:
	addiw		t0,x0,55
	bgt			a1,t0,.func_230
	jal			x0,.func_505
.func_230:
	addiw		t0,x0,1
	addiw		t1,x0,55
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,55
	sw			t1,0(t0)
	jal			x0,.func_231
.func_231:
	addiw		t0,x0,56
	bgt			a1,t0,.func_234
	jal			x0,.func_505
.func_234:
	addiw		t0,x0,1
	addiw		t1,x0,56
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,56
	sw			t1,0(t0)
	jal			x0,.func_235
.func_235:
	addiw		t0,x0,57
	bgt			a1,t0,.func_238
	jal			x0,.func_505
.func_238:
	addiw		t0,x0,1
	addiw		t1,x0,57
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,57
	sw			t1,0(t0)
	jal			x0,.func_239
.func_239:
	addiw		t0,x0,58
	bgt			a1,t0,.func_242
	jal			x0,.func_505
.func_242:
	addiw		t0,x0,1
	addiw		t1,x0,58
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,58
	sw			t1,0(t0)
	jal			x0,.func_243
.func_243:
	addiw		t0,x0,59
	bgt			a1,t0,.func_246
	jal			x0,.func_505
.func_246:
	addiw		t0,x0,1
	addiw		t1,x0,59
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,59
	sw			t1,0(t0)
	jal			x0,.func_247
.func_247:
	addiw		t0,x0,60
	bgt			a1,t0,.func_250
	jal			x0,.func_505
.func_250:
	addiw		t0,x0,1
	addiw		t1,x0,60
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,60
	sw			t1,0(t0)
	jal			x0,.func_251
.func_251:
	addiw		t0,x0,61
	bgt			a1,t0,.func_254
	jal			x0,.func_505
.func_254:
	addiw		t0,x0,1
	addiw		t1,x0,61
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,61
	sw			t1,0(t0)
	jal			x0,.func_255
.func_255:
	addiw		t0,x0,62
	bgt			a1,t0,.func_258
	jal			x0,.func_505
.func_258:
	addiw		t0,x0,1
	addiw		t1,x0,62
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,62
	sw			t1,0(t0)
	jal			x0,.func_259
.func_259:
	addiw		t0,x0,63
	bgt			a1,t0,.func_262
	jal			x0,.func_505
.func_262:
	addiw		t0,x0,1
	addiw		t1,x0,63
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,63
	sw			t1,0(t0)
	jal			x0,.func_263
.func_263:
	addiw		t0,x0,64
	bgt			a1,t0,.func_266
	jal			x0,.func_505
.func_266:
	addiw		t0,x0,1
	addiw		t1,x0,64
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,64
	sw			t1,0(t0)
	jal			x0,.func_267
.func_267:
	addiw		t0,x0,65
	bgt			a1,t0,.func_270
	jal			x0,.func_505
.func_270:
	addiw		t0,x0,1
	addiw		t1,x0,65
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,65
	sw			t1,0(t0)
	jal			x0,.func_271
.func_271:
	addiw		t0,x0,66
	bgt			a1,t0,.func_274
	jal			x0,.func_505
.func_274:
	addiw		t0,x0,1
	addiw		t1,x0,66
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,66
	sw			t1,0(t0)
	jal			x0,.func_275
.func_275:
	addiw		t0,x0,67
	bgt			a1,t0,.func_278
	jal			x0,.func_505
.func_278:
	addiw		t0,x0,1
	addiw		t1,x0,67
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,67
	sw			t1,0(t0)
	jal			x0,.func_279
.func_279:
	addiw		t0,x0,68
	bgt			a1,t0,.func_282
	jal			x0,.func_505
.func_282:
	addiw		t0,x0,1
	addiw		t1,x0,68
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,68
	sw			t1,0(t0)
	jal			x0,.func_283
.func_283:
	addiw		t0,x0,69
	bgt			a1,t0,.func_286
	jal			x0,.func_505
.func_286:
	addiw		t0,x0,1
	addiw		t1,x0,69
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,69
	sw			t1,0(t0)
	jal			x0,.func_287
.func_287:
	addiw		t0,x0,70
	bgt			a1,t0,.func_290
	jal			x0,.func_505
.func_290:
	addiw		t0,x0,1
	addiw		t1,x0,70
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,70
	sw			t1,0(t0)
	jal			x0,.func_291
.func_291:
	addiw		t0,x0,71
	bgt			a1,t0,.func_294
	jal			x0,.func_505
.func_294:
	addiw		t0,x0,1
	addiw		t1,x0,71
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,71
	sw			t1,0(t0)
	jal			x0,.func_295
.func_295:
	addiw		t0,x0,72
	bgt			a1,t0,.func_298
	jal			x0,.func_505
.func_298:
	addiw		t0,x0,1
	addiw		t1,x0,72
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,72
	sw			t1,0(t0)
	jal			x0,.func_299
.func_299:
	addiw		t0,x0,73
	bgt			a1,t0,.func_302
	jal			x0,.func_505
.func_302:
	addiw		t0,x0,1
	addiw		t1,x0,73
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,73
	sw			t1,0(t0)
	jal			x0,.func_303
.func_303:
	addiw		t0,x0,74
	bgt			a1,t0,.func_306
	jal			x0,.func_505
.func_306:
	addiw		t0,x0,1
	addiw		t1,x0,74
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,74
	sw			t1,0(t0)
	jal			x0,.func_307
.func_307:
	addiw		t0,x0,75
	bgt			a1,t0,.func_310
	jal			x0,.func_505
.func_310:
	addiw		t0,x0,1
	addiw		t1,x0,75
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,75
	sw			t1,0(t0)
	jal			x0,.func_311
.func_311:
	addiw		t0,x0,76
	bgt			a1,t0,.func_314
	jal			x0,.func_505
.func_314:
	addiw		t0,x0,1
	addiw		t1,x0,76
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,76
	sw			t1,0(t0)
	jal			x0,.func_315
.func_315:
	addiw		t0,x0,77
	bgt			a1,t0,.func_318
	jal			x0,.func_505
.func_318:
	addiw		t0,x0,1
	addiw		t1,x0,77
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,77
	sw			t1,0(t0)
	jal			x0,.func_319
.func_319:
	addiw		t0,x0,78
	bgt			a1,t0,.func_322
	jal			x0,.func_505
.func_322:
	addiw		t0,x0,1
	addiw		t1,x0,78
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,78
	sw			t1,0(t0)
	jal			x0,.func_323
.func_323:
	addiw		t0,x0,79
	bgt			a1,t0,.func_326
	jal			x0,.func_505
.func_326:
	addiw		t0,x0,1
	addiw		t1,x0,79
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,79
	sw			t1,0(t0)
	jal			x0,.func_327
.func_327:
	addiw		t0,x0,80
	bgt			a1,t0,.func_330
	jal			x0,.func_505
.func_330:
	addiw		t0,x0,1
	addiw		t1,x0,80
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,80
	sw			t1,0(t0)
	jal			x0,.func_331
.func_331:
	addiw		t0,x0,81
	bgt			a1,t0,.func_334
	jal			x0,.func_505
.func_334:
	addiw		t0,x0,1
	addiw		t1,x0,81
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,81
	sw			t1,0(t0)
	jal			x0,.func_335
.func_335:
	addiw		t0,x0,82
	bgt			a1,t0,.func_338
	jal			x0,.func_505
.func_338:
	addiw		t0,x0,1
	addiw		t1,x0,82
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,82
	sw			t1,0(t0)
	jal			x0,.func_339
.func_339:
	addiw		t0,x0,83
	bgt			a1,t0,.func_342
	jal			x0,.func_505
.func_342:
	addiw		t0,x0,1
	addiw		t1,x0,83
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,83
	sw			t1,0(t0)
	jal			x0,.func_343
.func_343:
	addiw		t0,x0,84
	bgt			a1,t0,.func_346
	jal			x0,.func_505
.func_346:
	addiw		t0,x0,1
	addiw		t1,x0,84
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,84
	sw			t1,0(t0)
	jal			x0,.func_347
.func_347:
	addiw		t0,x0,85
	bgt			a1,t0,.func_350
	jal			x0,.func_505
.func_350:
	addiw		t0,x0,1
	addiw		t1,x0,85
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,85
	sw			t1,0(t0)
	jal			x0,.func_351
.func_351:
	addiw		t0,x0,86
	bgt			a1,t0,.func_354
	jal			x0,.func_505
.func_354:
	addiw		t0,x0,1
	addiw		t1,x0,86
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,86
	sw			t1,0(t0)
	jal			x0,.func_355
.func_355:
	addiw		t0,x0,87
	bgt			a1,t0,.func_358
	jal			x0,.func_505
.func_358:
	addiw		t0,x0,1
	addiw		t1,x0,87
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,87
	sw			t1,0(t0)
	jal			x0,.func_359
.func_359:
	addiw		t0,x0,88
	bgt			a1,t0,.func_362
	jal			x0,.func_505
.func_362:
	addiw		t0,x0,1
	addiw		t1,x0,88
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,88
	sw			t1,0(t0)
	jal			x0,.func_363
.func_363:
	addiw		t0,x0,89
	bgt			a1,t0,.func_366
	jal			x0,.func_505
.func_366:
	addiw		t0,x0,1
	addiw		t1,x0,89
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,89
	sw			t1,0(t0)
	jal			x0,.func_367
.func_367:
	addiw		t0,x0,90
	bgt			a1,t0,.func_370
	jal			x0,.func_505
.func_370:
	addiw		t0,x0,1
	addiw		t1,x0,90
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,90
	sw			t1,0(t0)
	jal			x0,.func_371
.func_371:
	addiw		t0,x0,91
	bgt			a1,t0,.func_374
	jal			x0,.func_505
.func_374:
	addiw		t0,x0,1
	addiw		t1,x0,91
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,91
	sw			t1,0(t0)
	jal			x0,.func_375
.func_375:
	addiw		t0,x0,92
	bgt			a1,t0,.func_378
	jal			x0,.func_505
.func_378:
	addiw		t0,x0,1
	addiw		t1,x0,92
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,92
	sw			t1,0(t0)
	jal			x0,.func_379
.func_379:
	addiw		t0,x0,93
	bgt			a1,t0,.func_382
	jal			x0,.func_505
.func_382:
	addiw		t0,x0,1
	addiw		t1,x0,93
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,93
	sw			t1,0(t0)
	jal			x0,.func_383
.func_383:
	addiw		t0,x0,94
	bgt			a1,t0,.func_386
	jal			x0,.func_505
.func_386:
	addiw		t0,x0,1
	addiw		t1,x0,94
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,94
	sw			t1,0(t0)
	jal			x0,.func_387
.func_387:
	addiw		t0,x0,95
	bgt			a1,t0,.func_390
	jal			x0,.func_505
.func_390:
	addiw		t0,x0,1
	addiw		t1,x0,95
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,95
	sw			t1,0(t0)
	jal			x0,.func_391
.func_391:
	addiw		t0,x0,96
	bgt			a1,t0,.func_394
	jal			x0,.func_505
.func_394:
	addiw		t0,x0,1
	addiw		t1,x0,96
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,96
	sw			t1,0(t0)
	jal			x0,.func_395
.func_395:
	addiw		t0,x0,97
	bgt			a1,t0,.func_398
	jal			x0,.func_505
.func_398:
	addiw		t0,x0,1
	addiw		t1,x0,97
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,97
	sw			t1,0(t0)
	jal			x0,.func_399
.func_399:
	addiw		t0,x0,98
	bgt			a1,t0,.func_402
	jal			x0,.func_505
.func_402:
	addiw		t0,x0,1
	addiw		t1,x0,98
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,98
	sw			t1,0(t0)
	jal			x0,.func_403
.func_403:
	addiw		t0,x0,99
	bgt			a1,t0,.func_406
	jal			x0,.func_505
.func_406:
	addiw		t0,x0,1
	addiw		t1,x0,99
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,99
	sw			t1,0(t0)
	jal			x0,.func_505
.func_505:
	addiw		t4,t3,1
	add			t0,t2,x0
	addiw		t1,x0,0
	addiw		t3,x0,0
	addiw		t5,x0,0
	addiw		t6,x0,100
	blt			t5,t6,.func_506
	jal			x0,.func_510
.func_506:
	jal			x0,.func_509
.func_509:
	slli		t2,t1,2
	addi		t3,sp,0
	add			t2,t3,t2
	lw			t2,0(t2)
	addw		t2,t0,t2
	addiw		t5,t1,1
	add			t0,t2,x0
	add			t1,t5,x0
	add			t3,t5,x0
	addiw		t6,x0,100
	blt			t5,t6,.func_506
	jal			x0,.func_510
.func_510:
	lui			t0,16
	addiw		t0,t0,-1
	remw		t2,t2,t0
	jal			x0,.func_514
.func_511:
	add			a0,t0,x0
	addi		sp,sp,416
	jalr		x0,ra,0
.func_512:
	addiw		t0,x0,0
	addiw		t1,x0,0
	addiw		t2,x0,0
	addiw		t3,x0,100
	blt			t2,t3,.func_2
	jal			x0,.func_513
.func_513:
	addiw		t0,x0,0
	addiw		t1,x0,0
	addiw		t2,x0,0
	blt			t2,a0,.func_519
	jal			x0,.func_511
.func_514:
	add			t0,t2,x0
	add			t1,t4,x0
	add			t3,t4,x0
	blt			t4,a0,.func_7
	jal			x0,.func_511
.func_519:
	addiw		t2,x0,0
	addiw		t3,x0,0
	jal			x0,.func_7
main:
.main_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	addiw		a0,x0,399
	call		_sysy_starttime
	call		getint
	addw		fp,a0,x0
	call		getint
	addw		a1,a0,x0
	add			a0,fp,x0
	call		func
	call		putint
	addiw		a0,x0,10
	call		putch
	addiw		a0,x0,404
	call		_sysy_stoptime
	addiw		a0,x0,0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
	.data
