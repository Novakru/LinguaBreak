	.text
	.globl main
	.attribute arch, "rv64gc"
add:
.add_0:
	addi		sp,sp,-16
	add			t0,a1,x0
	add			t1,a0,x0
	jal			x0,.add_2
.add_2:
	bgt			t1,t0,.add_5
	jal			x0,.add_6
.add_5:
	addw		t0,t1,t0
	add			t0,t0,x0
	jal			x0,.add_8
.add_6:
	addw		t1,t1,t0
	addw		t0,t1,t0
	add			t0,t0,x0
	jal			x0,.add_8
.add_8:
	add			t0,t0,x0
	add			a0,t0,x0
	addi		sp,sp,16
	jalr		x0,ra,0
main:
.main_0:
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	addiw		a0,x0,3
	addiw		a1,x0,4
	call		add
	addw		t0,a0,x0
	add			a0,t0,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	jalr		x0,ra,0
	.data
