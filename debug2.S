	.text
	.globl main
	.attribute arch, "rv64gc"
my_cos:
.my_cos_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.my_cos_1
.my_cos_1:
	la			t0,PI
	flw			ft0,0(t0)
	lui			t0,262144
	fmv.w.x		ft1,t0
	fdiv.s		ft0,ft0,ft1
	fadd.s		fa0,fa0,ft0
	call		my_sin
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
main:
.main_0:
	sd			s4,-48(sp)
	sd			s3,-40(sp)
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-48
	jal			x0,.main_1
.main_1:
	call		getint
	addw		fp,a0,x0
	call		getint
	addw		s1,a0,x0
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_32
	jal			x0,.main_11
.main_2:
	addiw		s3,x0,0
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,s1,.main_6
	jal			x0,.main_10
.main_6:
	jal			x0,.main_9
.main_9:
	call		getfloat
	addiw		t0,x0,8
	mul			t0,s2,t0
	addi		t1,s3,0
	add			t0,t1,t0
	slli		t0,t0,2
	la			t1,test_block
	add			t0,t0,t1
	fsw			fa0,0(t0)
	addiw		t0,x0,1
	addw		t1,s3,t0
	add			s3,t1,x0
	add			t0,t1,x0
	blt			t1,s1,.main_6
	jal			x0,.main_10
.main_10:
	addiw		t0,x0,1
	addw		t1,s2,t0
	jal			x0,.main_29
.main_11:
	slli		t0,x0,2
	la			t1,test_dct
	add			s2,t0,t1
	slli		t0,x0,2
	la			t1,test_block
	add			a1,t0,t1
	add			a0,s2,x0
	add			a2,fp,x0
	add			a3,s1,x0
	call		dct
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_36
	jal			x0,.main_16
.main_13:
	addiw		t0,x0,8
	mul			t0,s3,t0
	slli		t0,t0,2
	la			t1,test_dct
	add			t0,t0,t1
	flw			fa0,0(t0)
	call		putfloat
	addiw		t0,x0,1
	addiw		s4,x0,1
	addiw		t1,x0,1
	blt			t1,s1,.main_17
	jal			x0,.main_14
.main_14:
	addiw		a0,x0,10
	call		putch
	addiw		t0,x0,1
	addw		t1,s3,t0
	jal			x0,.main_26
.main_16:
	addiw		a0,x0,10
	call		putch
	slli		t0,x0,2
	la			t1,test_idct
	add			a0,t0,t1
	add			a1,s2,x0
	add			a2,fp,x0
	add			a3,s1,x0
	call		idct
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.main_40
	jal			x0,.main_23
.main_17:
	jal			x0,.main_18
.main_18:
	addiw		a0,x0,32
	call		putch
	addiw		t0,x0,8
	mul			t0,s3,t0
	addi		t1,s4,0
	add			t0,t1,t0
	slli		t0,t0,2
	la			t1,test_dct
	add			t0,t0,t1
	flw			fa0,0(t0)
	call		putfloat
	addiw		t0,x0,1
	addw		t1,s4,t0
	add			t0,t1,x0
	add			s4,t1,x0
	blt			t1,s1,.main_17
	jal			x0,.main_14
.main_20:
	addiw		t0,x0,8
	mul			t0,s2,t0
	slli		t0,t0,2
	la			t1,test_idct
	add			t0,t0,t1
	flw			fa0,0(t0)
	call		putfloat
	addiw		t0,x0,1
	addiw		s3,x0,1
	addiw		t1,x0,1
	blt			t1,s1,.main_24
	jal			x0,.main_21
.main_21:
	addiw		a0,x0,10
	call		putch
	addiw		t0,x0,1
	addw		t1,s2,t0
	jal			x0,.main_27
.main_23:
	addiw		a0,x0,10
	call		putch
	addiw		a0,x0,0
	addi		sp,sp,48
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	ld			s3,-40(sp)
	ld			s4,-48(sp)
	jalr		x0,ra,0
.main_24:
	jal			x0,.main_25
.main_25:
	addiw		a0,x0,32
	call		putch
	addiw		t0,x0,8
	mul			t0,s2,t0
	addi		t1,s3,0
	add			t0,t1,t0
	slli		t0,t0,2
	la			t1,test_idct
	add			t0,t0,t1
	flw			fa0,0(t0)
	call		putfloat
	addiw		t0,x0,1
	addw		t1,s3,t0
	add			t0,t1,x0
	add			s3,t1,x0
	blt			t1,s1,.main_24
	jal			x0,.main_21
.main_26:
	add			t0,t1,x0
	add			s3,t1,x0
	blt			t1,fp,.main_13
	jal			x0,.main_16
.main_27:
	add			t0,t1,x0
	add			s2,t1,x0
	blt			t1,fp,.main_20
	jal			x0,.main_23
.main_29:
	add			t0,t1,x0
	add			s2,t1,x0
	blt			t1,fp,.main_2
	jal			x0,.main_11
.main_32:
	addiw		s2,x0,0
	jal			x0,.main_2
.main_36:
	addiw		s3,x0,0
	jal			x0,.main_13
.main_40:
	addiw		s2,x0,0
	jal			x0,.main_20
my_sin_impl:
.my_sin_impl_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.my_sin_impl_9
.my_sin_impl_6:
	lui			t0,263168
	fmv.w.x		ft0,t0
	fdiv.s		fa0,fa0,ft0
	call		my_sin_impl
	lui			t0,263168
	fmv.w.x		ft0,t0
	fmul.s		ft0,ft0,fa0
	lui			t0,264192
	fmv.w.x		ft1,t0
	fmul.s		ft1,ft1,fa0
	fmul.s		ft1,ft1,fa0
	fmul.s		ft1,ft1,fa0
	fsub.s		ft0,ft0,ft1
	fmv.w.x		ft1,x0
	fadd.s		ft2,ft0,ft1
	jal			x0,.my_sin_impl_7
.my_sin_impl_7:
	fmv.s		fa0,ft2
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
.my_sin_impl_9:
	fmv.s		ft0,fa0
	fmv.w.x		ft1,x0
	fle.s		t0,fa0,ft1
	beq			t0,x0,.my_sin_impl_10
	jal			x0,.my_sin_impl_11
.my_sin_impl_10:
	fmv.w.x		ft1,x0
	fadd.s		ft0,ft0,ft1
	la			t0,EPSILON
	flw			ft1,0(t0)
	fmv.s		ft2,fa0
	fle.s		t0,ft0,ft1
	bne			t0,x0,.my_sin_impl_7
	jal			x0,.my_sin_impl_6
.my_sin_impl_11:
	fmv.w.x		ft0,x0
	fsub.s		ft0,ft0,fa0
	jal			x0,.my_sin_impl_10
dct:
.dct_0:
	fsd			fs4,-120(sp)
	fsd			fs3,-112(sp)
	fsd			fs2,-104(sp)
	fsd			fs1,-96(sp)
	fsd			fs0,-88(sp)
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
	addi		sp,sp,-128
	add			fp,a3,x0
	add			s1,a2,x0
	add			s2,a1,x0
	add			s3,a0,x0
	addiw		s4,x0,0
	jal			x0,.dct_2
.dct_2:
	addiw		s5,x0,0
	blt			s4,s1,.dct_6
	jal			x0,.dct_21
.dct_6:
	blt			s5,fp,.dct_9
	jal			x0,.dct_20
.dct_9:
	addiw		t0,x0,8
	mul			t0,s4,t0
	addi		t1,s5,0
	add			t0,t1,t0
	slli		t0,t0,2
	add			s6,s3,t0
	fmv.w.x		ft0,x0
	fsw			ft0,0(s6)
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,s1,.dct_30
	jal			x0,.dct_19
.dct_10:
	addiw		t0,x0,0
	addiw		t1,x0,0
	blt			t1,fp,.dct_28
	jal			x0,.dct_18
.dct_14:
	jal			x0,.dct_17
.dct_17:
	flw			fs3,0(s6)
	addiw		t0,x0,8
	mul			t0,s7,t0
	addi		t1,s8,0
	add			t0,t1,t0
	slli		t0,t0,2
	add			t0,s2,t0
	flw			ft0,0(t0)
	fmul.s		fs4,ft0,fs2
	fcvt.s.w		ft0,s8
	lui			t0,258048
	fmv.w.x		ft1,t0
	fadd.s		ft0,ft0,ft1
	fmul.s		ft0,fs1,ft0
	fmul.s		fa0,ft0,fs0
	call		my_cos
	fmul.s		ft0,fs4,fa0
	fadd.s		ft0,fs3,ft0
	fsw			ft0,0(s6)
	addiw		t0,x0,1
	addw		t1,s8,t0
	add			t0,t1,x0
	add			s8,t1,x0
	blt			t1,fp,.dct_14
	jal			x0,.dct_18
.dct_18:
	addiw		t0,x0,1
	addw		t1,s7,t0
	jal			x0,.dct_23
.dct_19:
	addiw		t0,x0,1
	addw		s5,s5,t0
	jal			x0,.dct_6
.dct_20:
	addiw		t0,x0,1
	addw		s4,s4,t0
	jal			x0,.dct_2
.dct_21:
	addi		sp,sp,128
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
	fld			fs0,-88(sp)
	fld			fs1,-96(sp)
	fld			fs2,-104(sp)
	fld			fs3,-112(sp)
	fld			fs4,-120(sp)
	jalr		x0,ra,0
.dct_23:
	add			t0,t1,x0
	add			s7,t1,x0
	blt			t1,s1,.dct_10
	jal			x0,.dct_19
.dct_28:
	fcvt.s.w		fs0,s5
	fcvt.s.w		ft0,fp
	la			t0,PI
	flw			ft1,0(t0)
	fdiv.s		fs1,ft1,ft0
	fcvt.s.w		ft0,s4
	fcvt.s.w		ft1,s7
	lui			t0,258048
	fmv.w.x		ft2,t0
	fadd.s		ft1,ft1,ft2
	fcvt.s.w		ft2,s1
	la			t0,PI
	flw			ft3,0(t0)
	fdiv.s		ft2,ft3,ft2
	fmul.s		ft1,ft2,ft1
	fmul.s		fa0,ft1,ft0
	call		my_cos
	fmv.s		fs2,fa0
	addiw		s8,x0,0
	jal			x0,.dct_14
.dct_30:
	addiw		s7,x0,0
	jal			x0,.dct_10
idct:
.idct_0:
	fsd			fs3,-112(sp)
	fsd			fs2,-104(sp)
	fsd			fs1,-96(sp)
	fsd			fs0,-88(sp)
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
	addi		sp,sp,-112
	add			fp,a3,x0
	add			s1,a2,x0
	add			s2,a1,x0
	add			s3,a0,x0
	addiw		s4,x0,0
	jal			x0,.idct_2
.idct_2:
	addiw		s5,x0,0
	blt			s4,s1,.idct_6
	jal			x0,.idct_31
.idct_6:
	blt			s5,fp,.idct_9
	jal			x0,.idct_30
.idct_9:
	slli		t0,x0,2
	add			t0,s2,t0
	flw			ft0,0(t0)
	lui			t0,256000
	fmv.w.x		ft1,t0
	fmul.s		ft0,ft1,ft0
	addiw		t0,x0,8
	mul			t0,s4,t0
	addi		t1,s5,0
	add			t0,t1,t0
	slli		t0,t0,2
	add			s6,s3,t0
	fsw			ft0,0(s6)
	addiw		t0,x0,1
	addiw		t1,x0,1
	addiw		t2,x0,1
	blt			t2,s1,.idct_10
	jal			x0,.idct_36
.idct_10:
	flw			ft0,0(s6)
	addiw		t1,x0,8
	mul			t1,t0,t1
	slli		t1,t1,2
	add			t1,s2,t1
	flw			ft1,0(t1)
	lui			t1,258048
	fmv.w.x		ft2,t1
	fmul.s		ft1,ft2,ft1
	fadd.s		ft0,ft0,ft1
	fsw			ft0,0(s6)
	addiw		t1,x0,1
	addw		t2,t0,t1
	add			t0,t2,x0
	add			t1,t2,x0
	blt			t2,s1,.idct_10
	jal			x0,.idct_36
.idct_15:
	flw			ft0,0(s6)
	addi		t1,t0,0
	slli		t1,t1,2
	add			t1,s2,t1
	flw			ft1,0(t1)
	lui			t1,258048
	fmv.w.x		ft2,t1
	fmul.s		ft1,ft2,ft1
	fadd.s		ft0,ft0,ft1
	fsw			ft0,0(s6)
	addiw		t1,x0,1
	addw		t2,t0,t1
	add			t0,t2,x0
	add			t1,t2,x0
	blt			t2,fp,.idct_15
	jal			x0,.idct_34
.idct_20:
	addiw		t0,x0,1
	addiw		t1,x0,1
	blt			t1,fp,.idct_44
	jal			x0,.idct_28
.idct_24:
	jal			x0,.idct_27
.idct_27:
	flw			fs2,0(s6)
	addiw		t0,x0,8
	mul			t0,s7,t0
	addi		t1,s8,0
	add			t0,t1,t0
	slli		t0,t0,2
	add			t0,s2,t0
	flw			ft0,0(t0)
	fmul.s		fs3,ft0,fs1
	fcvt.s.w		ft0,s8
	fmul.s		fa0,fs0,ft0
	call		my_cos
	fmul.s		ft0,fs3,fa0
	fadd.s		ft0,fs2,ft0
	fsw			ft0,0(s6)
	addiw		t0,x0,1
	addw		t1,s8,t0
	add			t0,t1,x0
	add			s8,t1,x0
	blt			t1,fp,.idct_24
	jal			x0,.idct_28
.idct_28:
	addiw		t0,x0,1
	addw		t1,s7,t0
	jal			x0,.idct_33
.idct_29:
	flw			ft0,0(s6)
	lui			t0,262144
	fmv.w.x		ft1,t0
	fmul.s		ft0,ft0,ft1
	fcvt.s.w		ft1,s1
	fdiv.s		ft0,ft0,ft1
	lui			t0,262144
	fmv.w.x		ft1,t0
	fmul.s		ft0,ft0,ft1
	fcvt.s.w		ft1,fp
	fdiv.s		ft0,ft0,ft1
	fsw			ft0,0(s6)
	addiw		t0,x0,1
	addw		s5,s5,t0
	jal			x0,.idct_6
.idct_30:
	addiw		t0,x0,1
	addw		s4,s4,t0
	jal			x0,.idct_2
.idct_31:
	addi		sp,sp,112
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
	fld			fs0,-88(sp)
	fld			fs1,-96(sp)
	fld			fs2,-104(sp)
	fld			fs3,-112(sp)
	jalr		x0,ra,0
.idct_33:
	add			t0,t1,x0
	add			s7,t1,x0
	blt			t1,s1,.idct_20
	jal			x0,.idct_29
.idct_34:
	addiw		t0,x0,1
	addiw		t1,x0,1
	blt			t1,s1,.idct_46
	jal			x0,.idct_29
.idct_36:
	addiw		t0,x0,1
	addiw		t1,x0,1
	addiw		t2,x0,1
	blt			t2,fp,.idct_15
	jal			x0,.idct_34
.idct_44:
	fcvt.s.w		ft0,s5
	lui			t0,258048
	fmv.w.x		ft1,t0
	fadd.s		ft0,ft0,ft1
	fcvt.s.w		ft1,fp
	la			t0,PI
	flw			ft2,0(t0)
	fdiv.s		ft1,ft2,ft1
	fmul.s		fs0,ft1,ft0
	fcvt.s.w		ft0,s7
	fcvt.s.w		ft1,s4
	lui			t0,258048
	fmv.w.x		ft2,t0
	fadd.s		ft1,ft1,ft2
	fcvt.s.w		ft2,s1
	la			t0,PI
	flw			ft3,0(t0)
	fdiv.s		ft2,ft3,ft2
	fmul.s		ft1,ft2,ft1
	fmul.s		fa0,ft1,ft0
	call		my_cos
	fmv.s		fs1,fa0
	addiw		s8,x0,1
	jal			x0,.idct_24
.idct_46:
	addiw		s7,x0,1
	jal			x0,.idct_20
my_sin:
.my_sin_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.my_sin_2
.my_sin_2:
	la			t0,TWO_PI
	flw			ft0,0(t0)
	fle.s		t0,fa0,ft0
	beq			t0,x0,.my_sin_6
	jal			x0,.my_sin_5
.my_sin_5:
	la			t0,TWO_PI
	flw			ft0,0(t0)
	fmv.w.x		ft1,x0
	fsub.s		ft0,ft1,ft0
	fmv.s		ft1,fa0
	flt.s		t0,fa0,ft0
	bne			t0,x0,.my_sin_6
	jal			x0,.my_sin_7
.my_sin_6:
	la			t0,TWO_PI
	flw			ft0,0(t0)
	fdiv.s		ft0,fa0,ft0
	fcvt.w.s		t0,ft0,rtz
	la			t1,TWO_PI
	flw			ft0,0(t1)
	fcvt.s.w		ft1,t0
	fmul.s		ft0,ft1,ft0
	fsub.s		ft1,fa0,ft0
	jal			x0,.my_sin_7
.my_sin_7:
	la			t0,PI
	flw			ft0,0(t0)
	fmv.s		ft2,ft1
	fle.s		t0,ft1,ft0
	beq			t0,x0,.my_sin_11
	jal			x0,.my_sin_12
.my_sin_11:
	la			t0,TWO_PI
	flw			ft0,0(t0)
	fsub.s		ft2,ft1,ft0
	jal			x0,.my_sin_12
.my_sin_12:
	jal			x0,.my_sin_13
.my_sin_13:
	la			t0,PI
	flw			ft0,0(t0)
	fmv.w.x		ft1,x0
	fsub.s		ft0,ft1,ft0
	fmv.s		fa0,ft2
	flt.s		t0,ft2,ft0
	bne			t0,x0,.my_sin_16
	jal			x0,.my_sin_17
.my_sin_16:
	la			t0,TWO_PI
	flw			ft0,0(t0)
	fadd.s		fa0,ft2,ft0
	jal			x0,.my_sin_17
.my_sin_17:
	call		my_sin_impl
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
	.data
MAX_DIM_X:
	.word	8
MAX_DIM_Y:
	.word	8
test_block:
	.zero	256
test_dct:
	.zero	256
test_idct:
	.zero	256
PI:
	.word	1078530011
TWO_PI:
	.word	1086918619
EPSILON:
	.word	897988541
