	.text
	.globl main
	.attribute arch, "rv64gc"
spmv:
.spmv_0:
	addi		sp,sp,-16
	addiw		t0,x0,0
	jal			x0,.spmv_2
.spmv_2:
	addiw		t1,x0,0
	blt			t0,a0,.spmv_5
	jal			x0,.spmv_7
.spmv_5:
	addi		t1,t0,0
	slli		t1,t1,2
	add			t1,a5,t1
	addiw		t2,x0,0
	sw			t2,0(t1)
	addiw		t0,t0,1
	jal			x0,.spmv_2
.spmv_7:
	add			t0,t1,x0
	blt			t0,a0,.spmv_10
	jal			x0,.spmv_21
.spmv_10:
	addi		t1,t0,0    ; ptr: a1; index: t0
	slli		t1,t1,2
	add			t1,a1,t1
	lw			t2,0(t1)

	addiw		t3,t0,1
 
	addi		t4,t3,0    ; ptr: a1; index: t3(t0+1)
	slli		t4,t4,2
	add			t4,a1,t4
	lw			t5,0(t4)
	jal			x0,.spmv_11
.spmv_11:
	blt			t2,t5,.spmv_14
	jal			x0,.spmv_15
.spmv_14:
	addi		t6,t2,0    ; ptr: a2; index: t2
	slli		t6,t6,2
	add			t6,a2,t6
	lw			a6,0(t6)
	slli		a6,a6,2
	add			a6,a5,a6
	lw			a6,0(a6)

	addi		a7,t2,0    ; ptr: a3; index: t2+a3(不完全是)
	slli		a7,a7,2    
	add			a7,a3,a7
	lw			a7,0(a7)
	addw		a6,a6,a7
	lw			t6,0(t6)
	slli		t6,t6,2
	add			t6,a5,t6   ; ptr: a5; index: tx
	sw			a6,0(t6)
	addiw		t2,t2,1
	jal			x0,.spmv_11
.spmv_15:
	lw			t2,0(t1)
	lw			t4,0(t4)
	jal			x0,.spmv_16
.spmv_16:
	add			t1,t3,x0
	blt			t2,t4,.spmv_19
	jal			x0,.spmv_7
.spmv_19:
	addi		t1,t2,0
	slli		t1,t1,2
	add			t1,a2,t1
	lw			t5,0(t1)
	slli		t5,t5,2
	add			t5,a5,t5
	lw			t5,0(t5)
	addi		t6,t2,0
	slli		t6,t6,2
	add			t6,a3,t6
	lw			t6,0(t6)
	addi		a6,t0,0
	slli		a6,a6,2
	add			a6,a4,a6
	lw			a6,0(a6)
	addiw		a6,a6,-1
	mulw		t6,t6,a6
	addw		t5,t5,t6
	lw			t1,0(t1)
	slli		t1,t1,2
	add			t1,a5,t1
	sw			t5,0(t1)
	addiw		t2,t2,1
	jal			x0,.spmv_16
.spmv_21:
	addi		sp,sp,16
	jalr		x0,ra,0
main:
.main_0:
	sd			s6,-64(sp)
	sd			s5,-56(sp)
	sd			s4,-48(sp)
	sd			s3,-40(sp)
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-64
	jal			x0,.main_1
.main_1:
	slli		t0,x0,2
	la			t1,x
	add			fp,t0,t1
	add			a0,fp,x0
	call		getarray
	addiw		s1,a0,-1
	slli		t0,x0,2
	la			t1,y
	add			s2,t0,t1
	add			a0,s2,x0
	call		getarray
	slli		t0,x0,2
	la			t1,v
	add			s3,t0,t1
	add			a0,s3,x0
	call		getarray
	slli		t0,x0,2
	la			t1,a
	add			s4,t0,t1
	add			a0,s4,x0
	call		getarray
	addiw		a0,x0,39
	call		_sysy_starttime
	addiw		t0,x0,0
	addiw		t1,x0,0
	addiw		t2,x0,5
	blt			t1,t2,.main_7
	jal			x0,.main_6
.main_2:
	add			a0,s1,x0
	add			a1,fp,x0
	add			a2,s2,x0
	add			a3,s3,x0
	add			a4,s4,x0
	add			a5,s5,x0
	call		spmv
	add			a0,s1,x0
	add			a1,fp,x0
	add			a2,s2,x0
	add			a3,s3,x0
	add			a4,s5,x0
	add			a5,s4,x0
	call		spmv
	addiw		t1,s6,1
	add			t0,t1,x0
	add			s6,t1,x0
	addiw		t2,x0,5
	blt			t1,t2,.main_2
	jal			x0,.main_6
.main_6:
	addiw		a0,x0,47
	call		_sysy_stoptime
	slli		t0,x0,2
	la			t1,b
	add			a1,t0,t1
	add			a0,s1,x0
	call		putarray
	addiw		a0,x0,0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	ld			s3,-40(sp)
	ld			s4,-48(sp)
	ld			s5,-56(sp)
	ld			s6,-64(sp)
	jalr		x0,ra,0
.main_7:
	slli		t0,x0,2
	la			t1,b
	add			s5,t0,t1
	addiw		s6,x0,0
	jal			x0,.main_2
	.data
N:
	.word	100010
M:
	.word	3000000
x:
	.zero	400040
y:
	.zero	12000000
v:
	.zero	12000000
a:
	.zero	400040
b:
	.zero	400040
c:
	.zero	400040
