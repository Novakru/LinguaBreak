	.text
	.globl main
	.attribute arch, "rv64gc"
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
	addw		s1,a0,x0
	addiw		s2,x0,1
	addiw		t0,x0,1
	addiw		t1,x0,1
	ble			t1,fp,.main_2
	jal			x0,.main_33
.main_2:
	call		getint
	addiw		t0,x0,2
	remw		t0,a0,t0
	slli		t1,s2,1
	add			t0,t0,t1
	slli		t0,t0,2
	la			t1,t
	add			t0,t0,t1
	addiw		t1,x0,1
	sw			t1,0(t0)
	addiw		t0,s2,-1
	addiw		t1,x0,35
	mul			t0,t0,t1
	slli		t0,t0,2
	la			t1,dp
	add			t0,t0,t1
	lw			t0,0(t0)
	slli		t1,s2,1
	addiw		t2,x0,1
	addiw		t3,x0,1
	mul			t2,t3,t2
	add			t1,t2,t1
	slli		t1,t1,2
	la			t2,t
	add			t1,t1,t2
	lw			t1,0(t1)
	addw		t0,t0,t1
	addiw		t1,x0,35
	mul			t1,s2,t1
	slli		t1,t1,2
	la			t2,dp
	add			t1,t1,t2
	sw			t0,0(t1)
	addiw		t1,s2,1
	add			s2,t1,x0
	add			t0,t1,x0
	ble			t1,fp,.main_2
	jal			x0,.main_33
.main_7:
	addiw		t1,x0,1
	jal			x0,.main_11
.main_11:
	add			t2,t1,x0
	ble			t2,s1,.main_15
	jal			x0,.main_21
.main_15:
	addiw		t3,t0,-1
	addiw		t1,x0,35
	mul			t1,t3,t1
	addi		t4,t2,0
	add			t1,t4,t1
	slli		t1,t1,2
	la			t4,dp
	add			t4,t1,t4
	lw			t5,0(t4)
	addiw		t1,t2,1
	addiw		t6,x0,2
	remw		t6,t1,t6
	slli		a0,t0,1
	add			t6,t6,a0
	slli		t6,t6,2
	la			a0,t
	add			t6,t6,a0
	lw			a0,0(t6)
	addw		t5,t5,a0
	addiw		a0,t2,-1
	addiw		a1,x0,35
	mul			t3,t3,a1
	add			t3,a0,t3
	slli		t3,t3,2
	la			a0,dp
	add			t3,t3,a0
	lw			a0,0(t3)
	lw			a1,0(t6)
	addw		a0,a0,a1
	bgt			t5,a0,.main_18
	jal			x0,.main_19
.main_18:
	lw			t3,0(t4)
	lw			t4,0(t6)
	addw		t3,t3,t4
	addiw		t4,x0,35
	mul			t4,t0,t4
	add			t2,t2,t4
	slli		t2,t2,2
	la			t4,dp
	add			t2,t2,t4
	sw			t3,0(t2)
	jal			x0,.main_20
.main_19:
	lw			t3,0(t3)
	lw			t4,0(t6)
	addw		t3,t3,t4
	addiw		t4,x0,35
	mul			t4,t0,t4
	add			t2,t2,t4
	slli		t2,t2,2
	la			t4,dp
	add			t2,t2,t4
	sw			t3,0(t2)
	jal			x0,.main_20
.main_20:
	jal			x0,.main_11
.main_21:
	addiw		t2,t0,1
	add			t0,t2,x0
	add			t1,t2,x0
	ble			t2,fp,.main_7
	jal			x0,.main_36
.main_23:
	ble			t0,s1,.main_27
	jal			x0,.main_32
.main_27:
	addiw		t1,x0,35
	mul			t1,fp,t1
	addi		t2,t0,0
	add			t1,t2,t1
	slli		t1,t1,2
	la			t2,dp
	add			t1,t1,t2
	lw			t2,0(t1)
	add			t3,a0,x0
	blt			a0,t2,.main_30
	jal			x0,.main_31
.main_30:
	lw			t3,0(t1)
	jal			x0,.main_31
.main_31:
	add			a0,t3,x0
	addiw		t0,t0,1
	jal			x0,.main_23
.main_32:
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
.main_33:
	addiw		t0,x0,1
	addiw		t1,x0,1
	addiw		t2,x0,1
	ble			t2,fp,.main_7
	jal			x0,.main_36
.main_36:
	addiw		t0,x0,0
	addiw		a0,x0,0
	jal			x0,.main_23
	.data
t:
	.zero	8040
dp:
	.zero	140700
