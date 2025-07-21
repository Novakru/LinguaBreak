	.text
	.globl main
	.attribute arch, "rv64gc"
quick_read:
.quick_read_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	jal			x0,.quick_read_1
.quick_read_1:
	call		getch
	addw		a0,a0,x0
	add			a0,a0,x0
	addiw		fp,x0,0
	add			fp,fp,x0
	jal			x0,.quick_read_2
.quick_read_2:
	add			a0,a0,x0
	add			fp,fp,x0
	addiw		t0,x0,48
	blt			a0,t0,.quick_read_7
	jal			x0,.quick_read_5
.quick_read_5:
	add			t0,a0,x0
	addiw		s1,x0,0
	add			s1,s1,x0
	addiw		t1,x0,57
	bgt			a0,t1,.quick_read_7
	jal			x0,.quick_read_13
.quick_read_7:
	add			fp,fp,x0
	addiw		t0,x0,45
	beq			a0,t0,.quick_read_10
	jal			x0,.quick_read_11
.quick_read_10:
	addiw		fp,x0,1
	add			fp,fp,x0
	jal			x0,.quick_read_11
.quick_read_11:
	add			fp,fp,x0
	call		getch
	addw		a0,a0,x0
	add			a0,a0,x0
	add			fp,fp,x0
	jal			x0,.quick_read_2
.quick_read_13:
	add			t0,t0,x0
	add			s1,s1,x0
	addiw		t1,x0,48
	bge			t0,t1,.quick_read_16
	jal			x0,.quick_read_19
.quick_read_16:
	addiw		t1,x0,57
	ble			t0,t1,.quick_read_17
	jal			x0,.quick_read_19
.quick_read_17:
	addiw		t1,x0,10
	mulw		t1,s1,t1
	addw		t0,t1,t0
	addiw		t1,x0,48
	subw		s1,t0,t1
	call		getch
	addw		t0,a0,x0
	add			t0,t0,x0
	add			s1,s1,x0
	jal			x0,.quick_read_13
.quick_read_19:
	add			a0,s1,x0
	addiw		t0,x0,0
	bne			fp,t0,.quick_read_22
	jal			x0,.quick_read_25
.quick_read_22:
	addiw		t0,x0,0
	subw		a0,t0,s1
	add			a0,a0,x0
	jal			x0,.quick_read_25
.quick_read_25:
	add			a0,a0,x0
	add			a0,a0,x0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
find:
.find_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	add			a0,a0,x0
	jal			x0,.find_2
.find_2:
	addiw		t0,x0,1
	mul			t0,a0,t0
	slli		t0,t0,2
	la			t1,fa
	add			fp,t0,t1
	lw			t0,0(fp)
	add			t1,a0,x0
	beq			t0,a0,.find_8
	jal			x0,.find_6
.find_6:
	lw			a0,0(fp)
	add			a0,a0,x0
	call		find
	addw		t1,a0,x0
	sw			t1,0(fp)
	add			t1,t1,x0
	jal			x0,.find_8
.find_8:
	add			t1,t1,x0
	add			a0,t1,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
same:
.same_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	add			fp,a1,x0
	add			a0,a0,x0
	jal			x0,.same_2
.same_2:
	add			a0,a0,x0
	call		find
	addw		s1,a0,x0
	add			a0,fp,x0
	call		find
	addw		a0,a0,x0
	addiw		t0,x0,1
	add			t0,t0,x0
	beq			s1,a0,.same_7
	jal			x0,.same_6
.same_6:
	addiw		t0,x0,0
	add			t0,t0,x0
	jal			x0,.same_7
.same_7:
	add			t0,t0,x0
	add			a0,t0,x0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
main:
.main_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	jal			x0,.main_1
.main_1:
	call		quick_read
	addw		a0,a0,x0
	la			t0,n
	sw			a0,0(t0)
	call		quick_read
	addw		a0,a0,x0
	la			t0,m
	sw			a0,0(t0)
	la			t0,n
	lw			t0,0(t0)
	addiw		t1,x0,1
	add			t1,t1,x0
	addiw		t2,x0,1
	ble			t2,t0,.main_24
	jal			x0,.main_23
.main_2:
	la			t0,m
	lw			t0,0(t0)
	addiw		t1,x0,0
	bne			t0,t1,.main_5
	jal			x0,.main_18
.main_5:
	call		getch
	addw		a0,a0,x0
	add			a0,a0,x0
	jal			x0,.main_6
.main_6:
	add			a0,a0,x0
	addiw		t0,x0,81
	bne			a0,t0,.main_9
	jal			x0,.main_12
.main_9:
	addiw		t0,x0,85
	bne			a0,t0,.main_10
	jal			x0,.main_12
.main_10:
	call		getch
	addw		a0,a0,x0
	add			a0,a0,x0
	jal			x0,.main_6
.main_12:
	addiw		t0,x0,81
	beq			a0,t0,.main_15
	jal			x0,.main_16
.main_15:
	call		quick_read
	addw		fp,a0,x0
	call		quick_read
	addw		a1,a0,x0
	add			a0,fp,x0
	add			a1,a1,x0
	call		same
	addw		a0,a0,x0
	add			a0,a0,x0
	call		putint
	addiw		a0,x0,10
	call		putch
	jal			x0,.main_17
.main_16:
	call		quick_read
	addw		a0,a0,x0
	add			a0,a0,x0
	call		find
	addw		fp,a0,x0
	call		quick_read
	addw		a0,a0,x0
	add			a0,a0,x0
	call		find
	addw		a0,a0,x0
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,fa
	add			t0,t0,t1
	sw			a0,0(t0)
	jal			x0,.main_17
.main_17:
	la			t0,m
	lw			t0,0(t0)
	addiw		t1,x0,1
	subw		t0,t0,t1
	la			t1,m
	sw			t0,0(t1)
	jal			x0,.main_2
.main_18:
	addiw		a0,x0,0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.main_20:
	add			t2,t2,x0
	addiw		t1,x0,1
	mul			t1,t2,t1
	slli		t1,t1,2
	la			t3,fa
	add			t1,t1,t3
	sw			t2,0(t1)
	addiw		t1,x0,1
	addw		t3,t2,t1
	add			t1,t3,x0
	add			t2,t3,x0
	ble			t3,t0,.main_20
	jal			x0,.main_23
.main_23:
	add			t1,t1,x0
	jal			x0,.main_2
.main_24:
	la			t0,n
	lw			t0,0(t0)
	la			t1,n
	lw			t1,0(t1)
	addiw		t2,x0,1
	add			t2,t2,x0
	jal			x0,.main_20
	.data
n:
	.word	0
m:
	.word	0
fa:
	.zero	400020
