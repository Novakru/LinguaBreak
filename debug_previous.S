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
	sw			x0,20(sp)
	sw			x0,4(t0)
	sw			x0,4(t0)
	sw			x0,4(t0)
	sw			x0,4(t0)
	call		getint
1:
	auipc		t0,%pcrel_hi(state)
	sw			a0,%pcrel_lo(1b)(t0)
	call		getint
	addw		fp,a0,x0
	addiw		a0,x0,147
	call		_sysy_starttime
	sw			x0,0(sp)
	sw			x0,4(t0)
	sw			x0,4(t0)
	sw			x0,4(t0)
	sw			x0,4(t0)
	jal			x0,.main_2
.main_2:
	addiw		t0,x0,0
	bgt			fp,t0,.main_24
	jal			x0,.main_16
.main_6:
	lui			t2,8
	addiw		t2,t2,-768
	blt			t1,t2,.main_18
	jal			x0,.main_10
.main_10:
	slli		t0,x0,2
	la			t1,buffer
	add			a0,t0,t1
	addi		a2,sp,20
	lui			t0,8
	addiw		a1,t0,-768
	call		pseudo_sha1
	addi		t0,sp,0
	addi		t1,sp,20
	addiw		t2,x0,0
	jal			x0,.main_11
.main_11:
	addiw		t3,x0,5
	blt			t2,t3,.main_14
	jal			x0,.main_15
.main_14:
	lw			t3,0(t0)
	lw			t4,0(t1)
	addw		t5,t3,t4
	subw		t3,t3,t5
	addw		t3,t3,t4
	subw		t3,t3,t5
	sw			t3,0(t0)
	addiw		t2,t2,1
	addi		t0,t0,4
	addi		t1,t1,4
	jal			x0,.main_11
.main_15:
	addiw		fp,fp,-1
	jal			x0,.main_2
.main_16:
	addiw		a0,x0,170
	call		_sysy_stoptime
	addi		a1,sp,0
	addiw		a0,x0,5
	call		putarray
	addiw		a0,x0,0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.main_18:
2:
	auipc		t2,%pcrel_hi(state)
	lw			t2,%pcrel_lo(2b)(t2)
	slliw		t3,t2,13
	addw		t2,t2,t3
3:
	auipc		t3,%pcrel_hi(state)
	sw			t2,%pcrel_lo(3b)(t3)
	lui			t3,32
	divw		t3,t2,t3
	addw		t2,t2,t3
4:
	auipc		t3,%pcrel_hi(state)
	sw			t2,%pcrel_lo(4b)(t3)
	slliw		t3,t2,5
	addw		t2,t2,t3
5:
	auipc		t3,%pcrel_hi(state)
	sw			t2,%pcrel_lo(5b)(t3)
	addiw		t3,x0,256
	remw		t2,t2,t3
	sw			t2,0(t0)
	addiw		t1,t1,1
	addi		t0,t0,4
	jal			x0,.main_6
.main_24:
	slli		t0,x0,2
	la			t1,buffer
	add			t0,t0,t1
	addiw		t1,x0,0
	jal			x0,.main_6
pseudo_sha1:
.pseudo_sha1_0:
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
	addi		sp,sp,-400
	add			fp,a2,x0
	add			s1,a0,x0
	jal			x0,.pseudo_sha1_1
.pseudo_sha1_1:
	slli		t0,a1,2
	add			t0,s1,t0
	addiw		t1,x0,128
	sw			t1,0(t0)
	addiw		t0,a1,1
	slli		t1,t0,2
	add			t1,s1,t1
	jal			x0,.pseudo_sha1_2
.pseudo_sha1_2:
	addiw		t2,x0,64
	remw		t2,t0,t2
	addiw		t3,x0,60
	bne			t2,t3,.pseudo_sha1_5
	jal			x0,.pseudo_sha1_6
.pseudo_sha1_5:
	sw			x0,0(t1)
	addiw		t0,t0,1
	addi		t1,t1,4
	jal			x0,.pseudo_sha1_2
.pseudo_sha1_6:
	lui			t1,4096
	divw		t1,a1,t1
	addiw		t2,x0,256
	remw		t1,t1,t2
	slli		t2,t0,2
	add			t2,s1,t2
	sw			t1,0(t2)
	lui			t1,16
	divw		t1,a1,t1
	addiw		t3,x0,256
	remw		t1,t1,t3
	sw			t1,4(t2)
	addiw		t1,x0,256
	divw		t1,a1,t1
	addiw		t3,x0,256
	remw		t1,t1,t3
	sw			t1,8(t2)
	addiw		t1,x0,256
	remw		t1,a1,t1
	sw			t1,12(t2)
	addiw		s2,t0,4
	addi		a0,sp,0
	addiw		a1,x0,0
	addiw		a2,x0,320
	call		memset
	sw			x0,0(sp)
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
	blt			a0,s2,.pseudo_sha1_132
	jal			x0,.pseudo_sha1_49
.pseudo_sha1_11:
	addiw		s3,x0,16
	blt			a7,s3,.pseudo_sha1_14
	jal			x0,.pseudo_sha1_133
.pseudo_sha1_14:
	lw			s3,0(a1)
	slliw		s3,s3,24
	lw			s4,0(a2)
	slliw		s4,s4,16
	addw		s3,s3,s4
	lw			s4,0(a3)
	slliw		s4,s4,8
	addw		s3,s3,s4
	lw			s4,0(a4)
	addw		s3,s3,s4
	sw			s3,0(a6)
	addiw		a7,a7,1
	addi		a1,a1,16
	addi		a2,a2,16
	addi		a3,a3,16
	addi		a4,a4,16
	addi		a6,a6,4
	jal			x0,.pseudo_sha1_11
.pseudo_sha1_16:
	add			a2,a5,x0
	add			a3,t0,x0
	add			a4,t1,x0
	add			a6,t2,x0
	add			s3,t3,x0
	add			s4,t4,x0
	add			s5,t5,x0
	add			s6,t6,x0
	addiw		s7,x0,0
	addiw		s8,x0,80
	blt			a7,s8,.pseudo_sha1_19
	jal			x0,.pseudo_sha1_21
.pseudo_sha1_19:
	addiw		a2,a7,-3
	slli		a2,a2,2
	addi		a3,sp,0
	add			a2,a3,a2
	lw			a2,0(a2)
	addiw		a3,a7,-8
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	lw			a3,0(a3)
	addw		a4,a2,a3
	subw		a2,a2,a4
	addw		a2,a2,a3
	subw		a2,a2,a4
	addiw		a3,a7,-14
	slli		a3,a3,2
	addi		a4,sp,0
	add			a3,a4,a3
	lw			a3,0(a3)
	addw		a4,a2,a3
	subw		a2,a2,a4
	addw		a2,a2,a3
	subw		a2,a2,a4
	addiw		a3,a7,-16
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
	sw			a2,0(a1)
	addiw		a7,a7,1
	addi		a1,a1,4
	jal			x0,.pseudo_sha1_16
.pseudo_sha1_21:
	add			a1,a3,x0
	add			a5,a6,x0
	add			a7,s3,x0
	add			t5,s5,x0
	add			t6,s6,x0
	addiw		a3,x0,80
	blt			s7,a3,.pseudo_sha1_25
	jal			x0,.pseudo_sha1_48
.pseudo_sha1_25:
	addiw		a3,x0,20
	blt			s7,a3,.pseudo_sha1_53
	jal			x0,.pseudo_sha1_30
.pseudo_sha1_30:
	addiw		a3,x0,40
	blt			s7,a3,.pseudo_sha1_67
	jal			x0,.pseudo_sha1_35
.pseudo_sha1_35:
	addiw		a3,x0,60
	blt			s7,a3,.pseudo_sha1_57
	jal			x0,.pseudo_sha1_40
.pseudo_sha1_40:
	addiw		a3,x0,80
	blt			s7,a3,.pseudo_sha1_71
	jal			x0,.pseudo_sha1_44
.pseudo_sha1_44:
	jal			x0,.pseudo_sha1_45
.pseudo_sha1_45:
	jal			x0,.pseudo_sha1_46
.pseudo_sha1_46:
	add			s5,t5,x0
	add			s6,t6,x0
	jal			x0,.pseudo_sha1_47
.pseudo_sha1_47:
	slliw		t5,a1,5
	addiw		t6,x0,32
	remw		t6,a1,t6
	addw		t5,t5,t6
	addw		t5,t5,s5
	addw		t5,t5,s4
	addw		t5,t5,s6
	lw			t6,0(a2)
	addw		a3,t5,t6
	slliw		t5,a4,30
	lui			t6,262144
	remw		t6,a4,t6
	addw		a6,t5,t6
	addiw		s7,s7,1
	addi		a2,a2,4
	add			a4,a1,x0
	add			s3,a5,x0
	add			s4,a7,x0
	jal			x0,.pseudo_sha1_21
.pseudo_sha1_48:
	addw		t0,t0,a1
	addw		t1,t1,a4
	addw		t2,t2,a5
	addw		t3,t3,a7
	addw		t4,t4,s4
	addiw		a0,a0,64
	jal			x0,.pseudo_sha1_7
.pseudo_sha1_49:
	slli		t5,x0,2
	add			t5,fp,t5
	sw			t0,0(t5)
	sw			t1,4(fp)
	sw			t2,8(fp)
	sw			t3,12(fp)
	sw			t4,16(fp)
	addi		sp,sp,400
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
	jalr		x0,ra,0
.pseudo_sha1_53:
	addw		t5,a4,a5
	addiw		t6,x0,-1
	subw		t6,t6,a4
	addw		t6,t6,a7
	addw		a3,t5,t6
	subw		t5,t5,a3
	addw		t5,t5,t6
	subw		t5,t5,a3
	addw		t6,t5,a3
	subw		t5,t5,t6
	addw		t5,t5,a3
	subw		s5,t5,t6
	lui			t5,370728
	addiw		s6,t5,-1639
	jal			x0,.pseudo_sha1_47
.pseudo_sha1_57:
	addw		t5,a4,a5
	addw		t6,a4,a7
	addw		a3,t5,t6
	subw		t5,t5,a3
	addw		t5,t5,t6
	subw		t5,t5,a3
	addw		t6,t5,a3
	subw		t5,t5,t6
	addw		t5,t5,a3
	subw		t5,t5,t6
	addw		t6,a5,a7
	addw		a3,t5,t6
	subw		t5,t5,a3
	addw		t5,t5,t6
	subw		t5,t5,a3
	addw		t6,t5,a3
	subw		t5,t5,t6
	addw		t5,t5,a3
	subw		t5,t5,t6
	lui			t6,586172
	addiw		t6,t6,-804
	jal			x0,.pseudo_sha1_45
.pseudo_sha1_67:
	addw		t5,a4,a5
	subw		t6,a4,t5
	addw		t6,t6,a5
	subw		t5,t6,t5
	addw		t6,t5,a7
	subw		t5,t5,t6
	addw		t5,t5,a7
	subw		t5,t5,t6
	lui			t6,454047
	addiw		t6,t6,-1151
	jal			x0,.pseudo_sha1_46
.pseudo_sha1_71:
	addw		t5,a4,a5
	subw		t6,a4,t5
	addw		t6,t6,a5
	subw		t5,t6,t5
	addw		t6,t5,a7
	subw		t5,t5,t6
	addw		t5,t5,a7
	subw		t5,t5,t6
	lui			t6,828972
	addiw		t6,t6,262
	jal			x0,.pseudo_sha1_44
.pseudo_sha1_132:
	slli		a1,a0,2
	add			a1,s1,a1
	addi		a2,a1,4
	addi		a3,a1,8
	addi		a4,a1,12
	addi		a5,sp,0
	add			a6,a5,x0
	addiw		a7,x0,0
	jal			x0,.pseudo_sha1_11
.pseudo_sha1_133:
	slli		a1,a7,2
	addi		a2,sp,0
	add			a1,a2,a1
	jal			x0,.pseudo_sha1_16
	.data
state:
	.word	19260817
buffer:
	.zero	131072
