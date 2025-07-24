	.text
	.globl main
	.attribute arch, "rv64gc"
trsm:
.trsm_0:
	addi		sp,sp,-16
	addiw		t0,x0,0
	jal			x0,.trsm_2
.trsm_2:
	blt			t0,a0,.trsm_19
	jal			x0,.trsm_16
.trsm_6:
	slli		t2,t1,10
	addi		t3,t0,0
	add			t2,t3,t2
	slli		t2,t2,2
	add			t2,a2,t2
	flw			ft0,0(t2)
	slli		t3,t1,10
	addi		t4,t1,0
	add			t3,t4,t3
	slli		t3,t3,2
	add			t3,a1,t3
	flw			ft1,0(t3)
	fdiv.s		ft0,ft0,ft1
	fsw			ft0,0(t2)
	addiw		t3,t1,1
	add			t4,t3,x0
	add			t5,t3,x0
	blt			t3,a0,.trsm_10
	jal			x0,.trsm_17
.trsm_10:
	jal			x0,.trsm_13
.trsm_13:
	slli		t5,t4,10
	addi		t6,t0,0
	add			t5,t6,t5
	slli		t5,t5,2
	add			t5,a2,t5
	flw			ft0,0(t5)
	slli		t6,t4,10
	addi		a3,t1,0
	add			t6,a3,t6
	slli		t6,t6,2
	add			t6,a1,t6
	flw			ft1,0(t6)
	flw			ft2,0(t2)
	fmul.s		ft1,ft1,ft2
	fsub.s		ft0,ft0,ft1
	fsw			ft0,0(t5)
	addiw		t6,t4,1
	add			t4,t6,x0
	add			t5,t6,x0
	blt			t6,a0,.trsm_10
	jal			x0,.trsm_17
.trsm_15:
	addiw		t0,t0,1
	jal			x0,.trsm_2
.trsm_16:
	addi		sp,sp,16
	jalr		x0,ra,0
.trsm_17:
	jal			x0,.trsm_18
.trsm_18:
	add			t1,t3,x0
	add			t2,t3,x0
	blt			t3,a0,.trsm_6
	jal			x0,.trsm_15
.trsm_19:
	addiw		t1,x0,0
	addiw		t2,x0,0
	addiw		t3,x0,0
	blt			t3,a0,.trsm_6
	jal			x0,.trsm_15
main:
.main_0:
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	jal			x0,.main_1
.main_1:
	call		getint
	addw		fp,a0,x0
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_60
	jal			x0,.main_53
.main_2:
	addiw		s2,x0,0
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_6
	jal			x0,.main_10
.main_6:
	jal			x0,.main_9
.main_9:
	call		getfloat
	slli		t0,s1,10
	addi		t1,s2,0
	add			t0,t1,t0
	slli		t0,t0,2
	la			t1,A
	add			t0,t0,t1
	fsw			fa0,0(t0)
	addiw		t1,s2,1
	add			s2,t1,x0
	add			t0,t1,x0
	blt			t1,fp,.main_6
	jal			x0,.main_10
.main_10:
	addiw		t1,s1,1
	jal			x0,.main_55
.main_12:
	addiw		s2,x0,0
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_16
	jal			x0,.main_20
.main_16:
	jal			x0,.main_19
.main_19:
	call		getfloat
	slli		t0,s1,10
	addi		t1,s2,0
	add			t0,t1,t0
	slli		t0,t0,2
	la			t1,C
	add			t0,t0,t1
	fsw			fa0,0(t0)
	addiw		t1,s2,1
	add			s2,t1,x0
	add			t0,t1,x0
	blt			t1,fp,.main_16
	jal			x0,.main_20
.main_20:
	addiw		t1,s1,1
	jal			x0,.main_52
.main_21:
	addiw		a0,x0,52
	call		_sysy_starttime
	addiw		s1,x0,0
	jal			x0,.main_22
.main_22:
	addiw		t0,x0,5
	blt			s1,t0,.main_49
	jal			x0,.main_36
.main_26:
	addiw		t0,x0,0
	addiw		t2,x0,0
	addiw		t3,x0,0
	blt			t3,fp,.main_30
	jal			x0,.main_34
.main_30:
	jal			x0,.main_33
.main_33:
	slli		t2,t1,10
	addi		t3,t0,0
	add			t2,t3,t2
	slli		t2,t2,2
	la			t3,C
	add			t2,t2,t3
	flw			ft0,0(t2)
	slli		t2,t1,10
	addi		t3,t0,0
	add			t2,t3,t2
	slli		t2,t2,2
	la			t3,B
	add			t2,t2,t3
	fsw			ft0,0(t2)
	addiw		t3,t0,1
	add			t0,t3,x0
	add			t2,t3,x0
	blt			t3,fp,.main_30
	jal			x0,.main_34
.main_34:
	addiw		t2,t1,1
	jal			x0,.main_48
.main_35:
	slli		t0,x0,2
	la			t1,A
	add			a1,t0,t1
	slli		t0,x0,2
	la			t1,B
	add			a2,t0,t1
	add			a0,fp,x0
	call		trsm
	addiw		s1,s1,1
	jal			x0,.main_22
.main_36:
	addiw		a0,x0,67
	call		_sysy_stoptime
	addiw		t0,x0,0
	fmv.w.x		fa0,x0
	addiw		t1,x0,0
	blt			t1,fp,.main_72
	jal			x0,.main_46
.main_37:
	addiw		t0,x0,0
	fmv.s		ft1,ft0
	addiw		t2,x0,0
	addiw		t3,x0,0
	blt			t3,fp,.main_41
	jal			x0,.main_45
.main_41:
	jal			x0,.main_44
.main_44:
	slli		t2,t1,10
	addi		t3,t0,0
	add			t2,t3,t2
	slli		t2,t2,2
	la			t3,B
	add			t2,t2,t3
	flw			ft0,0(t2)
	fadd.s		ft0,ft1,ft0
	addiw		t3,t0,1
	add			t0,t3,x0
	fmv.s		ft1,ft0
	add			t2,t3,x0
	blt			t3,fp,.main_41
	jal			x0,.main_45
.main_45:
	addiw		t2,t1,1
	jal			x0,.main_57
.main_46:
	call		putfloat
	addiw		a0,x0,10
	call		putch
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
.main_48:
	add			t0,t2,x0
	add			t1,t2,x0
	blt			t2,fp,.main_26
	jal			x0,.main_35
.main_49:
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_68
	jal			x0,.main_35
.main_52:
	add			t0,t1,x0
	add			s1,t1,x0
	blt			t1,fp,.main_12
	jal			x0,.main_21
.main_53:
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_64
	jal			x0,.main_21
.main_55:
	add			t0,t1,x0
	add			s1,t1,x0
	blt			t1,fp,.main_2
	jal			x0,.main_53
.main_57:
	add			t0,t2,x0
	fmv.s		fa0,ft0
	add			t1,t2,x0
	blt			t2,fp,.main_37
	jal			x0,.main_46
.main_60:
	addiw		s1,x0,0
	jal			x0,.main_2
.main_64:
	addiw		s1,x0,0
	jal			x0,.main_12
.main_68:
	addiw		t1,x0,0
	jal			x0,.main_26
.main_72:
	addiw		t1,x0,0
	fmv.w.x		ft0,x0
	jal			x0,.main_37
	.data
N:
	.word	1024
A:
	.zero	4194304
B:
	.zero	4194304
C:
	.zero	4194304
