	.text
	.globl main
	.attribute arch, "rv64gc"
KMP:
.KMP_0:
	li			t0,16400
	sub			sp,sp,t0
	jal			x0,.KMP_1
.KMP_1:
	slli		t0,x0,2
	add			t0,a0,t0
	slli		t0,x0,2
	addi		t1,sp,0
	add			t0,t1,t0
	slli		t0,x0,2
	addi		t1,sp,0
	add			t0,t1,t0
	addiw		t1,x0,-1
	sw			t1,0(t0)
	addiw		t0,x0,0
	addiw		t1,x0,-1
	jal			x0,.KMP_26
.KMP_2:
	slli		t0,t5,2
	add			t0,a1,t0
	lw			t1,0(t0)
	addiw		t2,x0,-1
	addiw		t3,x0,0
	bne			t1,t3,.KMP_6
	jal			x0,.KMP_23
.KMP_6:
	slli		t1,t4,2
	add			t1,a0,t1
	lw			t1,0(t1)
	lw			t0,0(t0)
	beq			t1,t0,.KMP_9
	jal			x0,.KMP_15
.KMP_9:
	addiw		t0,t4,1
	addiw		t2,t5,1
	slli		t1,t0,2
	add			t1,a0,t1
	lw			t1,0(t1)
	addiw		t1,t1,0
	sltiu		t1,t1,1
	add			t3,t2,x0
	addiw		t4,x0,0
	bne			t1,t4,.KMP_23
	jal			x0,.KMP_21
.KMP_15:
	slli		t0,t4,2
	addi		t1,sp,0
	add			t0,t1,t0
	lw			t1,0(t0)
	add			t0,t1,x0
	add			t3,t5,x0
	addiw		t2,x0,-1
	beq			t1,t2,.KMP_19
	jal			x0,.KMP_20
.KMP_19:
	addiw		t0,t1,1
	addiw		t3,t5,1
	jal			x0,.KMP_20
.KMP_20:
	jal			x0,.KMP_21
.KMP_21:
	add			t4,t0,x0
	add			t5,t3,x0
	jal			x0,.KMP_2
.KMP_23:
	add			a0,t2,x0
	li			t0,16400
	add			sp,sp,t0
	jalr		x0,ra,0
.KMP_26:
	slli		t2,t0,2
	add			t2,a0,t2
	lw			t3,0(t2)
	addiw		t4,x0,0
	addiw		t5,x0,0
	addiw		t6,x0,0
	bne			t3,t6,.KMP_28
	jal			x0,.KMP_2
.KMP_27:
	jal			x0,.KMP_26
.KMP_28:
	addiw		t3,x0,-1
	beq			t1,t3,.KMP_30
	jal			x0,.KMP_31
.KMP_30:
	addiw		t1,t1,1
	addiw		t0,t0,1
	slli		t2,t0,2
	addi		t3,sp,0
	add			t2,t3,t2
	sw			t1,0(t2)
	jal			x0,.KMP_27
.KMP_31:
	lw			t2,0(t2)
	slli		t3,t1,2
	add			t3,a0,t3
	lw			t3,0(t3)
	beq			t2,t3,.KMP_30
	jal			x0,.KMP_32
.KMP_32:
	slli		t1,t1,2
	addi		t2,sp,0
	add			t1,t2,t1
	lw			t1,0(t1)
	jal			x0,.KMP_27
main:
.main_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	li			t0,32800
	sub			sp,sp,t0
	jal			x0,.main_1
.main_1:
	slli		t0,x0,2
	li			t1,16384
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		fp,x0,0
	jal			x0,.main_3
.main_3:
	call		getch
	slli		t0,fp,2
	li			t1,16384
	add			t1,sp,t1
	add			t0,t1,t0
	sw			a0,0(t0)
	lw			t1,0(t0)
	addiw		t2,x0,10
	beq			t1,t2,.main_7
	jal			x0,.main_4
.main_4:
	addiw		fp,fp,1
	jal			x0,.main_3
.main_7:
	addiw		t1,x0,0
	sw			t1,0(t0)
	slli		t0,x0,2
	addi		t1,sp,0
	add			fp,t1,t0
	addiw		s1,x0,0
	jal			x0,.main_9
.main_9:
	jal			x0,.main_11
.main_10:
	addiw		s1,s1,1
	jal			x0,.main_9
.main_11:
	call		getch
	slli		t0,s1,2
	addi		t1,sp,0
	add			t0,t1,t0
	sw			a0,0(t0)
	jal			x0,.main_12
.main_12:
	lw			t1,0(t0)
	addiw		t2,x0,10
	beq			t1,t2,.main_13
	jal			x0,.main_10
.main_13:
	addiw		t1,x0,0
	sw			t1,0(t0)
	slli		t0,x0,2
	li			t1,16384
	add			t1,sp,t1
	add			a0,t1,t0
	add			a1,fp,x0
	call		KMP
	call		putint
	addiw		a0,x0,10
	call		putch
	addiw		a0,x0,0
	li			t0,32800
	add			sp,sp,t0
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
	.data
