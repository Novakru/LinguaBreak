	.text
	.globl main
	.attribute arch, "rv64gc"
kernel_nussinov:
.kernel_nussinov_0:
	addi		sp,sp,-16
	jal			x0,.kernel_nussinov_1
.kernel_nussinov_1:
	addiw		t0,a0,-1
	jal			x0,.kernel_nussinov_2
.kernel_nussinov_2:
	addiw		t1,x0,0
	addiw		t2,x0,0
	bge			t0,t2,.kernel_nussinov_5
	jal			x0,.kernel_nussinov_69
.kernel_nussinov_5:
	addiw		t1,t0,1
	addiw		t2,x0,1400
	mulw		t2,t0,t2
	addw		t2,t2,t1
	slli		t2,t2,2
	add			t2,a2,t2
	slli		t3,t1,2
	add			t3,a1,t3
	addiw		t4,x0,1400
	mulw		t4,t1,t4
	addw		t4,t4,t1
	slli		t4,t4,2
	add			t4,a2,t4
	add			t5,t1,x0
	jal			x0,.kernel_nussinov_6
.kernel_nussinov_6:
	blt			t5,a0,.kernel_nussinov_10
	jal			x0,.kernel_nussinov_67
.kernel_nussinov_10:
	addiw		t6,t5,-1
	addiw		a3,x0,0
	bge			t6,a3,.kernel_nussinov_14
	jal			x0,.kernel_nussinov_20
.kernel_nussinov_14:
	lw			a3,0(t2)
	addiw		a4,x0,1400
	mul			a4,t0,a4
	add			a4,t6,a4
	slli		a4,a4,2
	add			a4,a2,a4
	lw			a5,0(a4)
	blt			a3,a5,.kernel_nussinov_17
	jal			x0,.kernel_nussinov_20
.kernel_nussinov_17:
	lw			a3,0(a4)
	sw			a3,0(t2)
	jal			x0,.kernel_nussinov_20
.kernel_nussinov_20:
	blt			t1,a0,.kernel_nussinov_24
	jal			x0,.kernel_nussinov_30
.kernel_nussinov_24:
	lw			a3,0(t2)
	lw			a4,0(t4)
	blt			a3,a4,.kernel_nussinov_27
	jal			x0,.kernel_nussinov_30
.kernel_nussinov_27:
	lw			a3,0(t4)
	sw			a3,0(t2)
	jal			x0,.kernel_nussinov_30
.kernel_nussinov_30:
	addiw		a3,x0,0
	bge			t6,a3,.kernel_nussinov_33
	jal			x0,.kernel_nussinov_56
.kernel_nussinov_33:
	blt			t1,a0,.kernel_nussinov_35
	jal			x0,.kernel_nussinov_56
.kernel_nussinov_35:
	blt			t0,t6,.kernel_nussinov_39
	jal			x0,.kernel_nussinov_50
.kernel_nussinov_39:
	slli		a3,t0,2
	add			a3,a1,a3
	lw			a3,0(a3)
	lw			a4,0(t3)
	addw		a3,a3,a4
	addiw		a4,x0,0
	addiw		a5,x0,3
	beq			a3,a5,.kernel_nussinov_42
	jal			x0,.kernel_nussinov_43
.kernel_nussinov_42:
	addiw		a4,x0,1
	jal			x0,.kernel_nussinov_43
.kernel_nussinov_43:
	lw			a3,0(t2)
	addiw		a5,x0,1400
	mul			a5,t1,a5
	add			t6,t6,a5
	slli		t6,t6,2
	add			t6,a2,t6
	lw			a5,0(t6)
	addw		a5,a5,a4
	blt			a3,a5,.kernel_nussinov_47
	jal			x0,.kernel_nussinov_56
.kernel_nussinov_47:
	lw			t6,0(t6)
	addw		t6,t6,a4
	sw			t6,0(t2)
	jal			x0,.kernel_nussinov_56
.kernel_nussinov_50:
	lw			a3,0(t2)
	addiw		a4,x0,1400
	mul			a4,t1,a4
	add			t6,t6,a4
	slli		t6,t6,2
	add			t6,a2,t6
	lw			a4,0(t6)
	blt			a3,a4,.kernel_nussinov_53
	jal			x0,.kernel_nussinov_56
.kernel_nussinov_53:
	lw			t6,0(t6)
	sw			t6,0(t2)
	jal			x0,.kernel_nussinov_56
.kernel_nussinov_56:
	addiw		t6,x0,1400
	mulw		t6,t0,t6
	addw		t6,t6,t1
	slli		t6,t6,2
	add			t6,a2,t6
	addiw		a3,t1,1
	addiw		a4,x0,1400
	mulw		a3,a3,a4
	addw		a3,t5,a3
	slli		a3,a3,2
	add			a3,a2,a3
	add			a4,t1,x0
	jal			x0,.kernel_nussinov_57
.kernel_nussinov_57:
	blt			a4,t5,.kernel_nussinov_61
	jal			x0,.kernel_nussinov_66
.kernel_nussinov_61:
	lw			a5,0(t2)
	lw			a6,0(t6)
	addiw		a4,a4,1
	lw			a7,0(a3)
	addw		a6,a6,a7
	blt			a5,a6,.kernel_nussinov_64
	jal			x0,.kernel_nussinov_65
.kernel_nussinov_64:
	lw			a5,0(t6)
	lw			a6,0(a3)
	addw		a5,a5,a6
	sw			a5,0(t2)
	jal			x0,.kernel_nussinov_65
.kernel_nussinov_65:
	addiw		a5,x0,1
	addiw		a6,x0,1
	mul			a5,a6,a5
	slli		a5,a5,2
	add			t6,t6,a5
	addiw		a5,x0,1
	addiw		a6,x0,1400
	mul			a5,a6,a5
	slli		a5,a5,2
	add			a3,a3,a5
	jal			x0,.kernel_nussinov_57
.kernel_nussinov_66:
	addiw		t5,t5,1
	addiw		t6,x0,1
	addiw		a3,x0,1
	mul			t6,a3,t6
	slli		t6,t6,2
	add			t2,t2,t6
	addiw		t6,x0,1
	addiw		a3,x0,1
	mul			t6,a3,t6
	slli		t6,t6,2
	add			t3,t3,t6
	addiw		t6,x0,1
	addiw		a3,x0,1
	mul			t6,a3,t6
	slli		t6,t6,2
	add			t4,t4,t6
	jal			x0,.kernel_nussinov_6
.kernel_nussinov_67:
	addiw		t0,t0,-1
	jal			x0,.kernel_nussinov_2
.kernel_nussinov_69:
	blt			t1,a0,.kernel_nussinov_79
	jal			x0,.kernel_nussinov_78
.kernel_nussinov_73:
	blt			t2,a0,.kernel_nussinov_76
	jal			x0,.kernel_nussinov_77
.kernel_nussinov_76:
	lw			t3,0(t0)
	addiw		t4,x0,10
	remw		t3,t3,t4
	sw			t3,0(t0)
	addiw		t2,t2,1
	addiw		t3,x0,1
	addiw		t4,x0,1
	mul			t3,t4,t3
	slli		t3,t3,2
	add			t0,t0,t3
	jal			x0,.kernel_nussinov_73
.kernel_nussinov_77:
	addiw		t1,t1,1
	jal			x0,.kernel_nussinov_69
.kernel_nussinov_78:
	addi		sp,sp,16
	jalr		x0,ra,0
.kernel_nussinov_79:
	addiw		t0,x0,1400
	mulw		t0,t1,t0
	slli		t0,t0,2
	add			t0,a2,t0
	addiw		t2,x0,0
	jal			x0,.kernel_nussinov_73
main:
.main_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	jal			x0,.main_1
.main_1:
	slli		t0,x0,2
	la			t1,seq
	add			fp,t0,t1
	add			a0,fp,x0
	call		getarray
	slli		t0,x0,2
	la			t1,table
	add			s1,t0,t1
	add			a0,s1,x0
	call		getarray
	addiw		a0,x0,81
	call		_sysy_starttime
1:
	auipc		t0,%pcrel_hi(n)
	lw			a0,%pcrel_lo(1b)(t0)
	add			a1,fp,x0
	add			a2,s1,x0
	call		kernel_nussinov
	addiw		a0,x0,83
	call		_sysy_stoptime
2:
	auipc		t0,%pcrel_hi(n)
	lw			t0,%pcrel_lo(2b)(t0)
3:
	auipc		t1,%pcrel_hi(n)
	lw			t1,%pcrel_lo(3b)(t1)
	mulw		a0,t0,t1
	add			a1,s1,x0
	call		putarray
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
	.data
n:
	.word	14
seq:
	.zero	5600
table:
	.zero	7840000
