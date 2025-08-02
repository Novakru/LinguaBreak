	.text
	.globl main
	.attribute arch, "rv64gc"
tran:
.tran_0:
	addi		sp,sp,-16
	jal			x0,.tran_1
.tran_1:
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a2,t0
	flw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a4,t0
	fsw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a1,t0
	flw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a5,t0
	fsw			ft0,0(t0)
	slli		t0,x0,2
	add			t0,a1,t0
	flw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a3,t0
	fsw			ft0,0(t0)
	slli		t0,x0,2
	add			t0,a2,t0
	flw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a3,t0
	fsw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a0,t0
	flw			ft0,0(t0)
	slli		t0,x0,2
	add			t0,a4,t0
	fsw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a0,t0
	flw			ft0,0(t0)
	slli		t0,x0,2
	add			t0,a5,t0
	fsw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a1,t0
	flw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a4,t0
	fsw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a2,t0
	flw			ft0,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	add			t0,a5,t0
	fsw			ft0,0(t0)
	slli		t0,x0,2
	add			t0,a0,t0
	flw			ft0,0(t0)
	slli		t0,x0,2
	add			t0,a3,t0
	fsw			ft0,0(t0)
	addiw		a0,x0,0
	addi		sp,sp,16
	jalr		x0,ra,0
main:
.main_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-144
	jal			x0,.main_1
.main_1:
	addiw		t0,x0,3
	addiw		t0,t0,0
1:
	auipc		t1,%pcrel_hi(N)
	sw			t0,%pcrel_lo(1b)(t1)
2:
	auipc		t1,%pcrel_hi(M)
	sw			t0,%pcrel_lo(2b)(t1)
3:
	auipc		t1,%pcrel_hi(L)
	sw			t0,%pcrel_lo(3b)(t1)
4:
	auipc		t0,%pcrel_hi(M)
	lw			t0,%pcrel_lo(4b)(t0)
	addiw		t1,x0,0
	addiw		t2,x0,0
	blt			t2,t0,.main_22
	jal			x0,.main_6
.main_2:
	slli		t1,t2,2
	addi		t3,sp,108
	add			t1,t3,t1
	fcvt.s.w		ft0,t2
	fsw			ft0,0(t1)
	slli		t1,t2,2
	addi		t3,sp,96
	add			t1,t3,t1
	fsw			ft0,0(t1)
	slli		t1,t2,2
	addi		t3,sp,84
	add			t1,t3,t1
	fsw			ft0,0(t1)
	slli		t1,t2,2
	addi		t3,sp,72
	add			t1,t3,t1
	fsw			ft0,0(t1)
	slli		t1,t2,2
	addi		t3,sp,60
	add			t1,t3,t1
	fsw			ft0,0(t1)
	slli		t1,t2,2
	addi		t3,sp,48
	add			t1,t3,t1
	fsw			ft0,0(t1)
	addiw		t3,t2,1
	add			t1,t3,x0
	add			t2,t3,x0
	blt			t3,t0,.main_2
	jal			x0,.main_6
.main_6:
	slli		t0,x0,2
	addi		t1,sp,108
	add			a0,t1,t0
	slli		t0,x0,2
	addi		t1,sp,96
	add			a1,t1,t0
	slli		t0,x0,2
	addi		t1,sp,84
	add			a2,t1,t0
	slli		t0,x0,2
	addi		t1,sp,72
	add			t0,t1,t0
	slli		t0,x0,2
	addi		t1,sp,60
	add			t0,t1,t0
	slli		t0,x0,2
	addi		t1,sp,48
	add			t0,t1,t0
	slli		t0,x0,2
	addi		t1,sp,24
	add			a3,t1,t0
	slli		t0,x0,2
	addi		t1,sp,12
	add			a4,t1,t0
	slli		t0,x0,2
	addi		t1,sp,0
	add			a5,t1,t0
	call		tran
	addw		fp,a0,x0
5:
	auipc		t0,%pcrel_hi(N)
	lw			t0,%pcrel_lo(5b)(t0)
	add			t1,fp,x0
	blt			fp,t0,.main_24
	jal			x0,.main_11
.main_7:
	slli		t0,fp,2
	addi		t1,sp,24
	add			t0,t1,t0
	flw			ft0,0(t0)
	fcvt.w.s		a0,ft0,rtz
	call		putint
	addiw		t0,fp,1
	add			t1,t0,x0
	add			fp,t0,x0
	blt			t0,s1,.main_7
	jal			x0,.main_11
.main_11:
	addiw		a0,x0,10
	call		putch
6:
	auipc		t0,%pcrel_hi(N)
	lw			t0,%pcrel_lo(6b)(t0)
	addiw		t1,x0,0
	addiw		t2,x0,0
	blt			t2,t0,.main_26
	jal			x0,.main_16
.main_12:
	slli		t0,s1,2
	addi		t1,sp,12
	add			t0,t1,t0
	flw			ft0,0(t0)
	fcvt.w.s		a0,ft0,rtz
	call		putint
	addiw		t0,s1,1
	add			t1,t0,x0
	add			s1,t0,x0
	blt			t0,fp,.main_12
	jal			x0,.main_16
.main_16:
	addiw		a0,x0,10
	call		putch
7:
	auipc		t0,%pcrel_hi(N)
	lw			t0,%pcrel_lo(7b)(t0)
	addiw		t1,x0,0
	addiw		t2,x0,0
	blt			t2,t0,.main_28
	jal			x0,.main_21
.main_17:
	slli		t0,s1,2
	addi		t1,sp,0
	add			t0,t1,t0
	flw			ft0,0(t0)
	fcvt.w.s		a0,ft0,rtz
	call		putint
	addiw		t0,s1,1
	add			t1,t0,x0
	add			s1,t0,x0
	blt			t0,fp,.main_17
	jal			x0,.main_21
.main_21:
	addiw		a0,x0,10
	call		putch
	addiw		a0,x0,0
	addi		sp,sp,144
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
.main_22:
14:
	auipc		t0,%pcrel_hi(M)
	lw			t0,%pcrel_lo(14b)(t0)
15:
	auipc		t1,%pcrel_hi(M)
	lw			t1,%pcrel_lo(15b)(t1)
	addiw		t2,x0,0
	jal			x0,.main_2
.main_24:
12:
	auipc		t0,%pcrel_hi(N)
	lw			s1,%pcrel_lo(12b)(t0)
13:
	auipc		t0,%pcrel_hi(N)
	lw			t0,%pcrel_lo(13b)(t0)
	jal			x0,.main_7
.main_26:
10:
	auipc		t0,%pcrel_hi(N)
	lw			fp,%pcrel_lo(10b)(t0)
11:
	auipc		t0,%pcrel_hi(N)
	lw			t0,%pcrel_lo(11b)(t0)
	addiw		s1,x0,0
	jal			x0,.main_12
.main_28:
8:
	auipc		t0,%pcrel_hi(N)
	lw			fp,%pcrel_lo(8b)(t0)
9:
	auipc		t0,%pcrel_hi(N)
	lw			t0,%pcrel_lo(9b)(t0)
	addiw		s1,x0,0
	jal			x0,.main_17
	.data
M:
	.word	0
L:
	.word	0
N:
	.word	0
