	.text
	.globl main
	.attribute arch, "rv64gc"
fib:
.fib_0:
	sd			s3,-40(sp)
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-48
	add			fp,a2,x0
	add			s1,a1,x0
	add			s2,a0,x0
	jal			x0,.fib_2
.fib_2:
	addiw		t0,x0,0
	beq			s1,t0,.fib_6
	jal			x0,.fib_5
.fib_5:
	addiw		t0,x0,1
	beq			s1,t0,.fib_6
	jal			x0,.fib_7
.fib_6:
	addiw		t0,s2,1
	sraiw		t0,t0,1
	slliw		t1,fp,1
	addiw		t2,x0,3
	remw		t1,t1,t2
	addw		a0,t0,t1
	jal			x0,.fib_8
.fib_7:
	addiw		a0,s2,1
	addiw		a1,s1,-1
	addiw		t0,fp,1
	sraiw		a2,t0,1
	call		fib
	addw		s3,a0,x0
	addiw		t0,s2,-2
	sraiw		a0,t0,1
	addiw		a1,s1,-2
	addiw		t0,fp,-3
	addiw		t1,x0,2
	remw		a2,t0,t1
	call		fib
	addw		a0,s3,a0
	jal			x0,.fib_8
.fib_8:
	addi		sp,sp,48
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	ld			s3,-40(sp)
	jalr		x0,ra,0
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
	call		getint
1:
	auipc		t0,%pcrel_hi(seed)
	sw			a0,%pcrel_lo(1b)(t0)
	addiw		a0,x0,23
	call		_sysy_starttime
	addiw		s1,x0,0
	addiw		s2,x0,0
	jal			x0,.main_2
.main_2:
	blt			s2,fp,.main_6
	jal			x0,.main_12
.main_6:
	addiw		t0,x0,2
	remw		t0,s2,t0
	addiw		t1,x0,0
	beq			t0,t1,.main_14
	jal			x0,.main_19
.main_11:
	addiw		t1,x0,256
	remw		s1,t0,t1
	add			a0,s1,x0
	call		putint
	addiw		a0,x0,10
	call		putch
	addiw		s2,s2,1
	jal			x0,.main_2
.main_12:
	addiw		a0,x0,35
	call		_sysy_stoptime
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
.main_14:
2:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(2b)(t0)
	lui			t1,4878
	addiw		t1,t1,-158
	mulw		t0,t0,t1
	lui			t1,6
	addiw		t1,t1,-1243
	addw		t0,t0,t1
	lui			t1,24414
	addiw		t1,t1,263
	remw		t0,t0,t1
3:
	auipc		t1,%pcrel_hi(seed)
	sw			t0,%pcrel_lo(3b)(t1)
4:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(4b)(t0)
	addiw		t1,x0,0
	blt			t0,t1,.main_16
	jal			x0,.main_17
.main_16:
5:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(5b)(t0)
	lui			t1,24414
	addiw		t1,t1,263
	addw		t0,t0,t1
6:
	auipc		t1,%pcrel_hi(seed)
	sw			t0,%pcrel_lo(6b)(t1)
	jal			x0,.main_17
.main_17:
7:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(7b)(t0)
	addiw		t0,t0,0
	lui			t1,2
	addiw		t1,t1,1815
	divw		a0,t0,t1
	add			a1,s2,x0
	add			a2,s1,x0
	call		fib
	addiw		t0,x0,256
	remw		t0,a0,t0
	subw		t0,s1,t0
	jal			x0,.main_11
.main_19:
8:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(8b)(t0)
	lui			t1,4878
	addiw		t1,t1,-158
	mulw		t0,t0,t1
	lui			t1,6
	addiw		t1,t1,-1243
	addw		t0,t0,t1
	lui			t1,24414
	addiw		t1,t1,263
	remw		t0,t0,t1
9:
	auipc		t1,%pcrel_hi(seed)
	sw			t0,%pcrel_lo(9b)(t1)
10:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(10b)(t0)
	addiw		t1,x0,0
	blt			t0,t1,.main_21
	jal			x0,.main_22
.main_21:
11:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(11b)(t0)
	lui			t1,24414
	addiw		t1,t1,263
	addw		t0,t0,t1
12:
	auipc		t1,%pcrel_hi(seed)
	sw			t0,%pcrel_lo(12b)(t1)
	jal			x0,.main_22
.main_22:
13:
	auipc		t0,%pcrel_hi(seed)
	lw			t0,%pcrel_lo(13b)(t0)
	addiw		t0,t0,0
	lui			t1,1048574
	addiw		t1,t1,-1815
	remw		a0,t0,t1
	add			a1,s2,x0
	add			a2,s2,x0
	call		fib
	addiw		t0,x0,256
	remw		t0,a0,t0
	addw		t0,s1,t0
	jal			x0,.main_11
	.data
seed:
	.word	0
