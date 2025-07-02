	.text
	.globl main
	.attribute arch, "rv64gc"
swap:
.swap_0:
	addi		sp,sp,-16
	add			t0,a2,x0
	add			t1,a1,x0
	add			t2,a0,x0
	jal			x0,.swap_1
.swap_1:
	addiw		t3,x0,1
	mul			t3,t1,t3
	slli		t3,t3,2
	add			t3,t2,t3
	lw			t3,0(t3)
	addiw		t4,x0,1
	mul			t4,t0,t4
	slli		t4,t4,2
	add			t4,t2,t4
	lw			t4,0(t4)
	addiw		t5,x0,1
	mul			t1,t1,t5
	slli		t1,t1,2
	add			t1,t2,t1
	sw			t4,0(t1)
	addiw		t1,x0,1
	mul			t0,t0,t1
	slli		t0,t0,2
	add			t0,t2,t0
	sw			t3,0(t0)
	addiw		a0,x0,0
	addi		sp,sp,16
	jalr		x0,ra,0
heap_ajust:
.heap_ajust_0:
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	add			fp,a2,x0
	add			t0,a1,x0
	add			s1,a0,x0
	jal			x0,.heap_ajust_1
.heap_ajust_1:
	addiw		t1,x0,2
	mulw		t1,t0,t1
	addiw		t2,x0,1
	addw		t1,t1,t2
	add			t0,t0,x0
	add			t1,t1,x0
	jal			x0,.heap_ajust_2
.heap_ajust_2:
	add			t0,t0,x0
	add			t1,t1,x0
	addiw		t2,x0,1
	addw		t2,fp,t2
	blt			t1,t2,.heap_ajust_6
	jal			x0,.heap_ajust_19
.heap_ajust_6:
	blt			t1,fp,.heap_ajust_9
	jal			x0,.heap_ajust_8
.heap_ajust_8:
	add			t1,t1,x0
	jal			x0,.heap_ajust_11
.heap_ajust_9:
	addiw		t2,x0,1
	mul			t2,t1,t2
	slli		t2,t2,2
	add			t2,s1,t2
	lw			t2,0(t2)
	addiw		t3,x0,1
	addw		t3,t1,t3
	addiw		t4,x0,1
	mul			t3,t3,t4
	slli		t3,t3,2
	add			t3,s1,t3
	lw			t3,0(t3)
	blt			t2,t3,.heap_ajust_10
	jal			x0,.heap_ajust_8
.heap_ajust_10:
	addiw		t2,x0,1
	addw		t1,t1,t2
	add			t1,t1,x0
	jal			x0,.heap_ajust_11
.heap_ajust_11:
	add			s2,t1,x0
	jal			x0,.heap_ajust_12
.heap_ajust_12:
	addiw		t1,x0,1
	mul			t1,t0,t1
	slli		t1,t1,2
	add			t1,s1,t1
	lw			t1,0(t1)
	addiw		t2,x0,1
	mul			t2,s2,t2
	slli		t2,t2,2
	add			t2,s1,t2
	lw			t2,0(t2)
	bgt			t1,t2,.heap_ajust_19
	jal			x0,.heap_ajust_16
.heap_ajust_16:
	slli		t1,x0,2
	add			t1,s1,t1
	add			a0,t1,x0
	add			a1,t0,x0
	add			a2,s2,x0
	call		swap
	addw		t0,a0,x0
	addiw		t0,x0,2
	mulw		t0,s2,t0
	addiw		t1,x0,1
	addw		t1,t0,t1
	add			t0,s2,x0
	add			t1,t1,x0
	jal			x0,.heap_ajust_2
.heap_ajust_19:
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
heap_sort:
.heap_sort_0:
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	add			fp,a1,x0
	add			s1,a0,x0
	jal			x0,.heap_sort_1
.heap_sort_1:
	addiw		t0,x0,2
	divw		t0,fp,t0
	addiw		t1,x0,1
	subw		t0,t0,t1
	add			t0,t0,x0
	jal			x0,.heap_sort_2
.heap_sort_2:
	add			s2,t0,x0
	addiw		t0,x0,-1
	bgt			s2,t0,.heap_sort_5
	jal			x0,.heap_sort_6
.heap_sort_5:
	addiw		t0,x0,1
	subw		t0,fp,t0
	slli		t1,x0,2
	add			t1,s1,t1
	add			a0,t1,x0
	add			a1,s2,x0
	add			a2,t0,x0
	call		heap_ajust
	addw		t0,a0,x0
	addiw		t0,x0,1
	subw		t0,s2,t0
	add			t0,t0,x0
	jal			x0,.heap_sort_2
.heap_sort_6:
	addiw		t0,x0,1
	subw		t0,fp,t0
	add			t0,t0,x0
	jal			x0,.heap_sort_7
.heap_sort_7:
	add			fp,t0,x0
	addiw		t0,x0,0
	bgt			fp,t0,.heap_sort_10
	jal			x0,.heap_sort_11
.heap_sort_10:
	slli		t0,x0,2
	add			t0,s1,t0
	add			a0,t0,x0
	addiw		a1,x0,0
	add			a2,fp,x0
	call		swap
	addw		t0,a0,x0
	addiw		t0,x0,1
	subw		t0,fp,t0
	slli		t1,x0,2
	add			t1,s1,t1
	add			a0,t1,x0
	addiw		a1,x0,0
	add			a2,t0,x0
	call		heap_ajust
	addw		t0,a0,x0
	addiw		t0,x0,1
	subw		t0,fp,t0
	add			t0,t0,x0
	jal			x0,.heap_sort_7
.heap_sort_11:
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
main:
.main_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-64
	jal			x0,.main_1
.main_1:
	addiw		t0,x0,10
	la			t1,n
	sw			t0,0(t1)
	slli		t0,x0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,4
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,3
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,9
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,3
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,2
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,4
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,0
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,5
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,1
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,6
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,6
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,7
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,5
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,8
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,7
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,9
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,8
	sw			t1,0(t0)
	slli		t0,x0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	la			t1,n
	lw			t1,0(t1)
	add			a0,t0,x0
	add			a1,t1,x0
	call		heap_sort
	addw		t0,a0,x0
	add			t0,t0,x0
	jal			x0,.main_2
.main_2:
	add			fp,t0,x0
	la			t0,n
	lw			t0,0(t0)
	blt			fp,t0,.main_5
	jal			x0,.main_6
.main_5:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	lw			t0,0(t0)
	add			a0,t0,x0
	call		putint
	addiw		a0,x0,10
	call		putch
	addiw		t0,x0,1
	addw		t0,fp,t0
	add			t0,t0,x0
	jal			x0,.main_2
.main_6:
	addiw		a0,x0,0
	addi		sp,sp,64
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
	.data
n:
	.word	0
