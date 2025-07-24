	.text
	.globl main
	.attribute arch, "rv64gc"
main:
.main_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-64
	jal			x0,.main_1
.main_1:
	addi		a0,sp,20
	addiw		a1,x0,0
	addiw		a2,x0,20
	call		memset
	slli		t0,x0,2
	addi		t1,sp,20
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,20
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,20
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,3
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,20
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,4
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,20
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	call		getint
1:
	auipc		t0,%pcrel_hi(state)
	sw			a0,%pcrel_lo(1b)(t0)
	call		getint
	addw		fp,a0,x0
	addiw		a0,x0,161
	call		_sysy_starttime
	slli		t0,x0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,3
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,4
	mul			t0,t1,t0
	slli		t0,t0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	add			t0,fp,x0
	addiw		t1,x0,0
	bgt			fp,t1,.main_31
	jal			x0,.main_16
.main_2:
	addiw		t0,x0,0
	addiw		t1,x0,0
	addiw		t2,x0,0
	lui			t3,8
	addiw		t3,t3,-768
	blt			t2,t3,.main_6
	jal			x0,.main_10
.main_6:
	jal			x0,.main_18
.main_10:
	slli		t0,x0,2
	la			t1,buffer
	add			a0,t0,t1
	slli		t0,x0,2
	addi		t1,sp,20
	add			a2,t1,t0
	lui			t0,8
	addiw		a1,t0,-768
	call		pseudo_sha1
	addiw		t0,x0,0
	addiw		t1,x0,0
	addiw		t2,x0,0
	addiw		t3,x0,5
	blt			t2,t3,.main_11
	jal			x0,.main_15
.main_11:
	jal			x0,.main_14
.main_14:
	addi		t1,t0,0
	slli		t1,t1,2
	addi		t2,sp,0
	add			t1,t2,t1
	lw			t2,0(t1)
	addi		t3,t0,0
	slli		t3,t3,2
	addi		t4,sp,20
	add			t3,t4,t3
	lw			t3,0(t3)
	jal			x0,.main_22
.main_15:
	addiw		t1,fp,-1
	jal			x0,.main_25
.main_16:
	addiw		a0,x0,184
	call		_sysy_stoptime
	slli		t0,x0,2
	addi		t1,sp,0
	add			a1,t1,t0
	addiw		a0,x0,5
	call		putarray
	addiw		a0,x0,0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.main_18:
2:
	auipc		t1,%pcrel_hi(state)
	lw			t1,%pcrel_lo(2b)(t1)
3:
	auipc		t2,%pcrel_hi(state)
	lw			t2,%pcrel_lo(3b)(t2)
	lui			t3,2
	mulw		t2,t2,t3
	addw		t1,t1,t2
4:
	auipc		t2,%pcrel_hi(state)
	sw			t1,%pcrel_lo(4b)(t2)
5:
	auipc		t1,%pcrel_hi(state)
	lw			t1,%pcrel_lo(5b)(t1)
6:
	auipc		t2,%pcrel_hi(state)
	lw			t2,%pcrel_lo(6b)(t2)
	lui			t3,32
	divw		t2,t2,t3
	addw		t1,t1,t2
7:
	auipc		t2,%pcrel_hi(state)
	sw			t1,%pcrel_lo(7b)(t2)
8:
	auipc		t1,%pcrel_hi(state)
	lw			t1,%pcrel_lo(8b)(t1)
9:
	auipc		t2,%pcrel_hi(state)
	lw			t2,%pcrel_lo(9b)(t2)
	slliw		t2,t2,5
	addw		t1,t1,t2
10:
	auipc		t2,%pcrel_hi(state)
	sw			t1,%pcrel_lo(10b)(t2)
11:
	auipc		t1,%pcrel_hi(state)
	lw			t1,%pcrel_lo(11b)(t1)
	addiw		t2,x0,256
	remw		t1,t1,t2
	addi		t2,t0,0
	slli		t2,t2,2
	la			t3,buffer
	add			t2,t2,t3
	sw			t1,0(t2)
	addiw		t2,t0,1
	add			t0,t2,x0
	add			t1,t2,x0
	lui			t3,8
	addiw		t3,t3,-768
	blt			t2,t3,.main_6
	jal			x0,.main_10
.main_22:
	addw		t4,t2,t3
	subw		t2,t2,t4
	addw		t2,t2,t3
	jal			x0,.main_24
.main_24:
	subw		t2,t2,t4
	sw			t2,0(t1)
	addiw		t2,t0,1
	add			t0,t2,x0
	add			t1,t2,x0
	addiw		t3,x0,5
	blt			t2,t3,.main_11
	jal			x0,.main_15
.main_25:
	add			t0,t1,x0
	add			fp,t1,x0
	addiw		t2,x0,0
	bgt			t1,t2,.main_2
	jal			x0,.main_16
.main_31:
	jal			x0,.main_2
pseudo_sha1:
.pseudo_sha1_0:
	sd			s11,-104(sp)
	sd			s10,-96(sp)
	sd			s9,-88(sp)
	sd			s8,-80(sp)
	sd			s7,-72(sp)
	sd			s6,-64(sp)
	sd			s5,-56(sp)
	sd			s4,-48(sp)
	sd			s3,-40(sp)
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-432
	add			fp,a2,x0
	add			s1,a0,x0
	jal			x0,.pseudo_sha1_1
.pseudo_sha1_1:
	addi		t0,a1,0
	slli		t0,t0,2
	add			t0,s1,t0
	addiw		t1,x0,128
	sw			t1,0(t0)
	addiw		t0,a1,1
	addiw		t1,x0,64
	remw		t1,t0,t1
	add			t2,t0,x0
	addiw		t3,x0,60
	bne			t1,t3,.pseudo_sha1_2
	jal			x0,.pseudo_sha1_6
.pseudo_sha1_2:
	addiw		t0,x0,64
	remw		t0,t2,t0
	addi		t0,t2,0
	slli		t0,t0,2
	add			t0,s1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,t2,1
	addiw		t1,x0,64
	remw		t1,t0,t1
	add			t2,t0,x0
	addiw		t3,x0,60
	bne			t1,t3,.pseudo_sha1_2
	jal			x0,.pseudo_sha1_6
.pseudo_sha1_6:
	lui			t1,4096
	divw		t1,a1,t1
	addiw		t2,x0,256
	remw		t1,t1,t2
	addi		t2,t0,0
	slli		t2,t2,2
	add			t2,s1,t2
	sw			t1,0(t2)
	lui			t1,16
	divw		t1,a1,t1
	addiw		t2,x0,256
	remw		t1,t1,t2
	addiw		t2,t0,1
	slli		t2,t2,2
	add			t2,s1,t2
	sw			t1,0(t2)
	addiw		t1,x0,256
	divw		t1,a1,t1
	addiw		t2,x0,256
	remw		t1,t1,t2
	addiw		t2,t0,2
	slli		t2,t2,2
	add			t2,s1,t2
	sw			t1,0(t2)
	addiw		t1,x0,256
	remw		t1,a1,t1
	addiw		t2,t0,3
	slli		t2,t2,2
	add			t2,s1,t2
	sw			t1,0(t2)
	addiw		s2,t0,4
	addi		a0,sp,0
	addiw		a1,x0,0
	addiw		a2,x0,320
	call		memset
	slli		t0,x0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	lui			t0,422994
	addiw		t0,t0,769
	lui			t1,982235
	addiw		t1,t1,-1143
	lui			t2,625582
	addiw		t2,t2,-770
	lui			t3,66341
	addiw		t3,t3,1142
	lui			t4,802094
	addiw		t4,t4,496
	addiw		t5,x0,0
	addiw		t6,x0,0
	addiw		a0,x0,0
	jal			x0,.pseudo_sha1_7
.pseudo_sha1_7:
	blt			a0,s2,.pseudo_sha1_158
	jal			x0,.pseudo_sha1_49
.pseudo_sha1_11:
	slliw		a2,a1,2
	addw		a2,a0,a2
	addi		a3,a2,0
	slli		a3,a3,2
	add			a3,s1,a3
	lw			a3,0(a3)
	lui			a4,4096
	mulw		a3,a3,a4
	addiw		a4,a2,1
	slli		a4,a4,2
	add			a4,s1,a4
	lw			a4,0(a4)
	lui			a5,16
	mulw		a4,a4,a5
	addw		a3,a3,a4
	addiw		a4,a2,2
	slli		a4,a4,2
	add			a4,s1,a4
	lw			a4,0(a4)
	slliw		a4,a4,8
	addw		a3,a3,a4
	addiw		a2,a2,3
	slli		a2,a2,2
	add			a2,s1,a2
	lw			a2,0(a2)
	addw		a2,a3,a2
	addi		a3,a1,0
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	sw			a2,0(a3)
	addiw		a3,a1,1
	add			a1,a3,x0
	add			a2,a3,x0
	addiw		a4,x0,16
	blt			a3,a4,.pseudo_sha1_11
	jal			x0,.pseudo_sha1_159
.pseudo_sha1_16:
	addiw		a2,a1,-3
	slli		a2,a2,2
	addi		a3,sp,0
	add			a2,a3,a2
	lw			a2,0(a2)
	addiw		a3,a1,-8
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	lw			a3,0(a3)
	addw		a4,a2,a3
	subw		a2,a2,a4
	addw		a2,a2,a3
	subw		a2,a2,a4
	addiw		a3,a1,-14
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	lw			a3,0(a3)
	addw		a4,a2,a3
	subw		a2,a2,a4
	addw		a2,a2,a3
	subw		a2,a2,a4
	addiw		a3,a1,-16
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	lw			a3,0(a3)
	addw		a4,a2,a3
	subw		a2,a2,a4
	addw		a2,a2,a3
	subw		a2,a2,a4
	slliw		a3,a2,1
	addiw		a4,x0,2
	remw		a2,a2,a4
	addw		a2,a3,a2
	addi		a3,a1,0
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	sw			a2,0(a3)
	addiw		a2,a1,1
	add			a1,a2,x0
	add			a3,a2,x0
	addiw		a4,x0,80
	blt			a2,a4,.pseudo_sha1_16
	jal			x0,.pseudo_sha1_160
.pseudo_sha1_21:
	add			s5,a1,x0
	add			s7,a3,x0
	add			s8,a4,x0
	addiw		t5,x0,20
	blt			s3,t5,.pseudo_sha1_53
	jal			x0,.pseudo_sha1_30
.pseudo_sha1_30:
	addiw		t5,x0,40
	blt			s3,t5,.pseudo_sha1_67
	jal			x0,.pseudo_sha1_35
.pseudo_sha1_35:
	addiw		t5,x0,60
	blt			s3,t5,.pseudo_sha1_57
	jal			x0,.pseudo_sha1_40
.pseudo_sha1_40:
	addiw		t5,x0,80
	blt			s3,t5,.pseudo_sha1_71
	jal			x0,.pseudo_sha1_44
.pseudo_sha1_44:
	jal			x0,.pseudo_sha1_45
.pseudo_sha1_45:
	jal			x0,.pseudo_sha1_46
.pseudo_sha1_46:
	add			t5,a6,x0
	add			t6,a7,x0
	jal			x0,.pseudo_sha1_47
.pseudo_sha1_47:
	jal			x0,.pseudo_sha1_61
.pseudo_sha1_48:
	addw		t0,t0,s4
	addw		t1,t1,s5
	addw		t2,t2,s6
	addw		t3,t3,s7
	addw		t4,t4,s8
	addiw		a0,a0,64
	jal			x0,.pseudo_sha1_7
.pseudo_sha1_49:
	slli		t5,x0,2
	add			t5,fp,t5
	sw			t0,0(t5)
	addiw		t0,x0,1
	addiw		t5,x0,1
	mul			t0,t5,t0
	slli		t0,t0,2
	add			t0,fp,t0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,fp,t0
	sw			t2,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,3
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,fp,t0
	sw			t3,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,4
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,fp,t0
	sw			t4,0(t0)
	addi		sp,sp,432
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	ld			s3,-40(sp)
	ld			s4,-48(sp)
	ld			s5,-56(sp)
	ld			s6,-64(sp)
	ld			s7,-72(sp)
	ld			s8,-80(sp)
	ld			s9,-88(sp)
	ld			s10,-96(sp)
	ld			s11,-104(sp)
	jalr		x0,ra,0
.pseudo_sha1_53:
	addw		t5,a2,s7
	jal			x0,.pseudo_sha1_65
.pseudo_sha1_57:
	addw		t5,a2,s7
	jal			x0,.pseudo_sha1_69
.pseudo_sha1_61:
	slliw		a1,s5,5
	addiw		a3,x0,32
	remw		a3,s5,a3
	addw		a1,a1,a3
	addw		a1,a1,t5
	addw		a1,a1,a5
	addw		a1,a1,t6
	addi		a3,s3,0
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	lw			a3,0(a3)
	addw		s4,a1,a3
	jal			x0,.pseudo_sha1_73
.pseudo_sha1_65:
	addiw		t6,x0,-1
	subw		t6,t6,a2
	jal			x0,.pseudo_sha1_77
.pseudo_sha1_67:
	addw		t5,a2,s7
	subw		t6,a2,t5
	addw		t6,t6,s7
	jal			x0,.pseudo_sha1_79
.pseudo_sha1_69:
	addw		t6,a2,s8
	jal			x0,.pseudo_sha1_101
.pseudo_sha1_71:
	addw		t5,a2,s7
	subw		t6,a2,t5
	addw		t6,t6,s7
	jal			x0,.pseudo_sha1_83
.pseudo_sha1_73:
	lui			a1,262144
	mulw		a1,a2,a1
	lui			a3,262144
	remw		a2,a2,a3
	addw		s6,a1,a2
	addiw		s10,s3,1
	add			a1,s4,x0
	add			a2,s5,x0
	add			a3,s6,x0
	add			a4,s7,x0
	add			a5,s8,x0
	add			a6,t5,x0
	add			a7,t6,x0
	add			s3,s10,x0
	add			s9,s10,x0
	addiw		s11,x0,80
	blt			s10,s11,.pseudo_sha1_21
	jal			x0,.pseudo_sha1_48
.pseudo_sha1_77:
	addw		t6,t6,s8
	jal			x0,.pseudo_sha1_107
.pseudo_sha1_79:
	subw		t5,t6,t5
	jal			x0,.pseudo_sha1_99
.pseudo_sha1_83:
	subw		t5,t6,t5
	jal			x0,.pseudo_sha1_103
.pseudo_sha1_99:
	addw		t6,t5,s8
	subw		t5,t5,t6
	addw		t5,t5,s8
	jal			x0,.pseudo_sha1_109
.pseudo_sha1_101:
	addw		a1,t5,t6
	subw		t5,t5,a1
	addw		t5,t5,t6
	jal			x0,.pseudo_sha1_111
.pseudo_sha1_103:
	addw		t6,t5,s8
	subw		t5,t5,t6
	addw		t5,t5,s8
	jal			x0,.pseudo_sha1_113
.pseudo_sha1_107:
	addw		a1,t5,t6
	subw		t5,t5,a1
	addw		t5,t5,t6
	jal			x0,.pseudo_sha1_117
.pseudo_sha1_109:
	subw		a6,t5,t6
	lui			t5,454047
	addiw		a7,t5,-1151
	jal			x0,.pseudo_sha1_46
.pseudo_sha1_111:
	subw		t5,t5,a1
	jal			x0,.pseudo_sha1_131
.pseudo_sha1_113:
	subw		a6,t5,t6
	lui			t5,828972
	addiw		a7,t5,262
	jal			x0,.pseudo_sha1_44
.pseudo_sha1_117:
	subw		t5,t5,a1
	jal			x0,.pseudo_sha1_135
.pseudo_sha1_131:
	addw		t6,t5,a1
	subw		t5,t5,t6
	addw		t5,t5,a1
	jal			x0,.pseudo_sha1_137
.pseudo_sha1_135:
	addw		t6,t5,a1
	subw		t5,t5,t6
	addw		t5,t5,a1
	jal			x0,.pseudo_sha1_139
.pseudo_sha1_137:
	subw		t5,t5,t6
	jal			x0,.pseudo_sha1_141
.pseudo_sha1_139:
	subw		t5,t5,t6
	lui			t6,370728
	addiw		t6,t6,-1639
	jal			x0,.pseudo_sha1_47
.pseudo_sha1_141:
	addw		t6,s7,s8
	jal			x0,.pseudo_sha1_147
.pseudo_sha1_147:
	addw		a1,t5,t6
	subw		t5,t5,a1
	addw		t5,t5,t6
	jal			x0,.pseudo_sha1_149
.pseudo_sha1_149:
	subw		t5,t5,a1
	jal			x0,.pseudo_sha1_155
.pseudo_sha1_155:
	addw		t6,t5,a1
	subw		t5,t5,t6
	addw		t5,t5,a1
	jal			x0,.pseudo_sha1_157
.pseudo_sha1_157:
	subw		a6,t5,t6
	lui			t5,586172
	addiw		a7,t5,-804
	jal			x0,.pseudo_sha1_45
.pseudo_sha1_158:
	addiw		a1,x0,0
	addiw		a2,x0,0
	addiw		a3,x0,0
	addiw		a4,x0,16
	blt			a3,a4,.pseudo_sha1_11
	jal			x0,.pseudo_sha1_159
.pseudo_sha1_159:
	add			a1,a2,x0
	add			a3,a2,x0
	addiw		a4,x0,80
	blt			a2,a4,.pseudo_sha1_16
	jal			x0,.pseudo_sha1_160
.pseudo_sha1_160:
	add			a1,t0,x0
	add			a2,t1,x0
	add			a3,t2,x0
	add			a4,t3,x0
	add			a5,t4,x0
	add			a6,t5,x0
	add			a7,t6,x0
	addiw		s3,x0,0
	add			s4,t0,x0
	add			s5,t1,x0
	add			s6,t2,x0
	add			s7,t3,x0
	add			s8,t4,x0
	addiw		s9,x0,0
	addiw		s10,x0,0
	addiw		s11,x0,80
	blt			s10,s11,.pseudo_sha1_21
	jal			x0,.pseudo_sha1_48
	.data
state:
	.word	19260817
buffer:
	.zero	131072
