	.text
	.globl main
	.attribute arch, "rv64gc"
getMaxNum:
.getMaxNum_0:
	addi		sp,sp,-16
	addiw		t0,x0,0
	addiw		t1,x0,0
	jal			x0,.getMaxNum_2
.getMaxNum_2:
	blt			t1,a0,.getMaxNum_6
	jal			x0,.getMaxNum_11
.getMaxNum_6:
	addi		t2,t1,0
	slli		t2,t2,2
	add			t2,a1,t2
	lw			t3,0(t2)
	add			t4,t0,x0
	bgt			t3,t0,.getMaxNum_9
	jal			x0,.getMaxNum_10
.getMaxNum_9:
	lw			t4,0(t2)
	jal			x0,.getMaxNum_10
.getMaxNum_10:
	add			t0,t4,x0
	addiw		t1,t1,1
	jal			x0,.getMaxNum_2
.getMaxNum_11:
	add			a0,t0,x0
	addi		sp,sp,16
	jalr		x0,ra,0
radixSort:
.radixSort_0:
	sd			s3,-40(sp)
	sd			s2,-32(sp)
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-240
	add			fp,a3,x0
	add			s1,a2,x0
	add			s2,a1,x0
	add			s3,a0,x0
	jal			x0,.radixSort_1
.radixSort_1:
	li			t0,128
	add			a0,sp,t0
	addiw		a1,x0,0
	addiw		a2,x0,64
	call		memset
	li			t0,64
	add			a0,sp,t0
	addiw		a1,x0,0
	addiw		a2,x0,64
	call		memset
	li			t0,0
	add			a0,sp,t0
	addiw		a1,x0,0
	addiw		a2,x0,64
	call		memset
	addiw		t0,x0,-1
	beq			s3,t0,.radixSort_6
	jal			x0,.radixSort_5
.radixSort_5:
	addiw		t0,s1,1
	bge			t0,fp,.radixSort_6
	jal			x0,.radixSort_76
.radixSort_6:
	jal			x0,.radixSort_43
.radixSort_8:
	jal			x0,.radixSort_11
.radixSort_11:
	addi		t0,t1,0
	slli		t0,t0,2
	add			t0,s2,t0
	lw			t2,0(t0)
	add			t3,t2,x0
	addiw		t4,x0,0
	addiw		t5,x0,0
	blt			t5,s3,.radixSort_80
	jal			x0,.radixSort_47
.radixSort_12:
	slli		t0,x0,2
	li			t1,128
	add			t1,sp,t1
	add			t0,t1,t0
	sw			s1,0(t0)
	slli		t0,x0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	lw			t0,0(t0)
	addw		t0,s1,t0
	slli		t1,x0,2
	li			t2,64
	add			t2,sp,t2
	add			t1,t2,t1
	sw			t0,0(t1)
1:
	auipc		t0,%pcrel_hi(base)
	lw			t0,%pcrel_lo(1b)(t0)
	addiw		t1,x0,1
	addiw		t2,x0,1
	blt			t2,t0,.radixSort_84
	jal			x0,.radixSort_77
.radixSort_13:
	jal			x0,.radixSort_16
.radixSort_16:
	addiw		t1,x0,1
	subw		t1,t2,t1
	slli		t1,t1,2
	li			t3,64
	add			t3,sp,t3
	add			t1,t3,t1
	lw			t1,0(t1)
	addi		t3,t2,0
	slli		t3,t3,2
	li			t4,128
	add			t4,sp,t4
	add			t3,t4,t3
	sw			t1,0(t3)
	lw			t1,0(t3)
	addi		t3,t2,0
	slli		t3,t3,2
	li			t4,0
	add			t4,sp,t4
	add			t3,t4,t3
	lw			t3,0(t3)
	addw		t1,t1,t3
	addi		t3,t2,0
	slli		t3,t3,2
	li			t4,64
	add			t4,sp,t4
	add			t3,t4,t3
	sw			t1,0(t3)
	addiw		t3,t2,1
	add			t1,t3,x0
	add			t2,t3,x0
	blt			t3,t0,.radixSort_13
	jal			x0,.radixSort_77
.radixSort_18:
	blt			t1,t0,.radixSort_97
	jal			x0,.radixSort_32
.radixSort_22:
	lw			t4,0(t3)
	blt			t4,t2,.radixSort_25
	jal			x0,.radixSort_31
.radixSort_25:
	lw			t4,0(t3)
	slli		t4,t4,2
	add			t4,s2,t4
	lw			t4,0(t4)
	jal			x0,.radixSort_26
.radixSort_26:
	add			t5,t4,x0
	add			t4,t5,x0
	addiw		t6,x0,0
	addiw		a0,x0,0
	blt			a0,s3,.radixSort_86
	jal			x0,.radixSort_51
.radixSort_30:
	addi		t4,t1,0
	slli		t4,t4,2
	li			t6,128
	add			t6,sp,t6
	add			t4,t6,t4
	lw			t6,0(t4)
	slli		t6,t6,2
	add			t6,s2,t6
	sw			t5,0(t6)
	lw			t5,0(t4)
	addiw		t5,t5,1
	sw			t5,0(t4)
	jal			x0,.radixSort_22
.radixSort_31:
	addiw		t1,t1,1
	jal			x0,.radixSort_18
.radixSort_32:
	slli		t0,x0,2
	li			t1,128
	add			t1,sp,t1
	add			t0,t1,t0
	sw			s1,0(t0)
	slli		t0,x0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	lw			t0,0(t0)
	addw		t0,s1,t0
	slli		t1,x0,2
	li			t2,64
	add			t2,sp,t2
	add			t1,t2,t1
	sw			t0,0(t1)
3:
	auipc		t0,%pcrel_hi(base)
	lw			fp,%pcrel_lo(3b)(t0)
	addiw		s1,x0,0
	jal			x0,.radixSort_33
.radixSort_33:
	blt			s1,fp,.radixSort_37
	jal			x0,.radixSort_43
.radixSort_37:
	addiw		t0,x0,0
	bgt			s1,t0,.radixSort_40
	jal			x0,.radixSort_41
.radixSort_40:
	addiw		t0,x0,1
	subw		t0,s1,t0
	slli		t0,t0,2
	li			t1,64
	add			t1,sp,t1
	add			t0,t1,t0
	lw			t0,0(t0)
	addi		t1,s1,0
	slli		t1,t1,2
	li			t2,128
	add			t2,sp,t2
	add			t1,t2,t1
	sw			t0,0(t1)
	lw			t0,0(t1)
	addi		t1,s1,0
	slli		t1,t1,2
	li			t2,0
	add			t2,sp,t2
	add			t1,t2,t1
	lw			t1,0(t1)
	addw		t0,t0,t1
	addi		t1,s1,0
	slli		t1,t1,2
	li			t2,64
	add			t2,sp,t2
	add			t1,t2,t1
	sw			t0,0(t1)
	jal			x0,.radixSort_41
.radixSort_41:
	addiw		t0,x0,1
	subw		a0,s3,t0
	slli		t0,x0,2
	add			a1,s2,t0
	addi		t0,s1,0
	slli		t0,t0,2
	li			t1,128
	add			t1,sp,t1
	add			t0,t1,t0
	lw			a2,0(t0)
	addi		t0,s1,0
	slli		t0,t0,2
	li			t1,64
	add			t1,sp,t1
	add			t0,t1,t0
	lw			a3,0(t0)
	call		radixSort
	addiw		s1,s1,1
	jal			x0,.radixSort_33
.radixSort_43:
	addi		sp,sp,240
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	ld			s3,-40(sp)
	jalr		x0,ra,0
.radixSort_45:
	jal			x0,.radixSort_46
.radixSort_46:
	divw		t2,t2,t5
	addiw		a0,t6,1
	add			t3,t2,x0
	add			t4,a0,x0
	add			t6,a0,x0
	blt			a0,s3,.radixSort_45
	jal			x0,.radixSort_47
.radixSort_47:
16:
	auipc		t2,%pcrel_hi(base)
	lw			t2,%pcrel_lo(16b)(t2)
	remw		t2,t3,t2
	addiw		t2,t2,0
	slli		t2,t2,2
	li			t3,0
	add			t3,sp,t3
	add			t2,t3,t2
	lw			t2,0(t2)
	addiw		t2,t2,1
	lw			t0,0(t0)
	add			t3,t0,x0
	addiw		t4,x0,0
	addiw		t5,x0,0
	blt			t5,s3,.radixSort_78
	jal			x0,.radixSort_59
.radixSort_49:
	jal			x0,.radixSort_50
.radixSort_50:
	divw		a1,a1,a0
	addiw		a3,a2,1
	add			t4,a1,x0
	add			t6,a3,x0
	add			a2,a3,x0
	blt			a3,s3,.radixSort_49
	jal			x0,.radixSort_51
.radixSort_51:
4:
	auipc		t6,%pcrel_hi(base)
	lw			t6,%pcrel_lo(4b)(t6)
	remw		t4,t4,t6
	addiw		t4,t4,0
	bne			t4,t1,.radixSort_73
	jal			x0,.radixSort_30
.radixSort_53:
	jal			x0,.radixSort_54
.radixSort_54:
	divw		a1,a1,a0
	addiw		a3,a2,1
	add			t4,a1,x0
	add			t6,a3,x0
	add			a2,a3,x0
	blt			a3,s3,.radixSort_53
	jal			x0,.radixSort_55
.radixSort_55:
5:
	auipc		t6,%pcrel_hi(base)
	lw			t6,%pcrel_lo(5b)(t6)
	remw		t4,t4,t6
	addiw		t4,t4,0
	slli		t4,t4,2
	li			t6,128
	add			t6,sp,t6
	add			t4,t6,t4
	lw			t4,0(t4)
	slli		t4,t4,2
	add			t4,s2,t4
	lw			t4,0(t4)
	add			t6,t5,x0
	addiw		a0,x0,0
	addiw		a1,x0,0
	blt			a1,s3,.radixSort_88
	jal			x0,.radixSort_63
.radixSort_57:
	jal			x0,.radixSort_58
.radixSort_58:
	divw		t0,t0,t5
	addiw		a0,t6,1
	add			t3,t0,x0
	add			t4,a0,x0
	add			t6,a0,x0
	blt			a0,s3,.radixSort_57
	jal			x0,.radixSort_59
.radixSort_59:
17:
	auipc		t0,%pcrel_hi(base)
	lw			t0,%pcrel_lo(17b)(t0)
	remw		t0,t3,t0
	addiw		t0,t0,0
	slli		t0,t0,2
	li			t3,0
	add			t3,sp,t3
	add			t0,t3,t0
	sw			t2,0(t0)
	addiw		t2,t1,1
	jal			x0,.radixSort_75
.radixSort_61:
	jal			x0,.radixSort_62
.radixSort_62:
	divw		a2,a2,a1
	addiw		a4,a3,1
	add			t6,a2,x0
	add			a0,a4,x0
	add			a3,a4,x0
	blt			a4,s3,.radixSort_61
	jal			x0,.radixSort_63
.radixSort_63:
6:
	auipc		a0,%pcrel_hi(base)
	lw			a0,%pcrel_lo(6b)(a0)
	remw		t6,t6,a0
	addiw		t6,t6,0
	slli		t6,t6,2
	li			a0,128
	add			a0,sp,a0
	add			t6,a0,t6
	lw			t6,0(t6)
	slli		t6,t6,2
	add			t6,s2,t6
	sw			t5,0(t6)
	add			t6,t5,x0
	addiw		a0,x0,0
	addiw		a1,x0,0
	blt			a1,s3,.radixSort_92
	jal			x0,.radixSort_67
.radixSort_65:
	jal			x0,.radixSort_66
.radixSort_66:
	divw		a2,a2,a1
	addiw		a4,a3,1
	add			t6,a2,x0
	add			a0,a4,x0
	add			a3,a4,x0
	blt			a4,s3,.radixSort_65
	jal			x0,.radixSort_67
.radixSort_67:
7:
	auipc		a0,%pcrel_hi(base)
	lw			a0,%pcrel_lo(7b)(a0)
	remw		t6,t6,a0
	addiw		t6,t6,0
	slli		t6,t6,2
	li			a0,128
	add			a0,sp,a0
	add			t6,a0,t6
	lw			t6,0(t6)
	addiw		t6,t6,1
	add			a0,t5,x0
	addiw		a1,x0,0
	addiw		a2,x0,0
	blt			a2,s3,.radixSort_90
	jal			x0,.radixSort_71
.radixSort_69:
	jal			x0,.radixSort_70
.radixSort_70:
	divw		t5,t5,a2
	addiw		a4,a3,1
	add			a0,t5,x0
	add			a1,a4,x0
	add			a3,a4,x0
	blt			a4,s3,.radixSort_69
	jal			x0,.radixSort_71
.radixSort_71:
8:
	auipc		t5,%pcrel_hi(base)
	lw			t5,%pcrel_lo(8b)(t5)
	remw		t5,a0,t5
	addiw		t5,t5,0
	slli		t5,t5,2
	li			a0,128
	add			a0,sp,a0
	add			t5,a0,t5
	sw			t6,0(t5)
	jal			x0,.radixSort_26
.radixSort_73:
	add			t4,t5,x0
	addiw		t6,x0,0
	addiw		a0,x0,0
	blt			a0,s3,.radixSort_94
	jal			x0,.radixSort_55
.radixSort_75:
	add			t0,t2,x0
	add			t1,t2,x0
	blt			t2,fp,.radixSort_8
	jal			x0,.radixSort_12
.radixSort_76:
	add			t0,s1,x0
	blt			s1,fp,.radixSort_82
	jal			x0,.radixSort_12
.radixSort_77:
2:
	auipc		t0,%pcrel_hi(base)
	lw			t0,%pcrel_lo(2b)(t0)
	addiw		t1,x0,0
	jal			x0,.radixSort_18
.radixSort_78:
18:
	auipc		t3,%pcrel_hi(base)
	lw			t5,%pcrel_lo(18b)(t3)
	addiw		t6,x0,0
	jal			x0,.radixSort_57
.radixSort_80:
19:
	auipc		t3,%pcrel_hi(base)
	lw			t5,%pcrel_lo(19b)(t3)
	addiw		t6,x0,0
	jal			x0,.radixSort_45
.radixSort_82:
	add			t1,s1,x0
	jal			x0,.radixSort_8
.radixSort_84:
14:
	auipc		t0,%pcrel_hi(base)
	lw			t0,%pcrel_lo(14b)(t0)
15:
	auipc		t1,%pcrel_hi(base)
	lw			t1,%pcrel_lo(15b)(t1)
	addiw		t2,x0,1
	jal			x0,.radixSort_13
.radixSort_86:
13:
	auipc		t4,%pcrel_hi(base)
	lw			a0,%pcrel_lo(13b)(t4)
	add			a1,t5,x0
	addiw		a2,x0,0
	jal			x0,.radixSort_49
.radixSort_88:
11:
	auipc		t6,%pcrel_hi(base)
	lw			a1,%pcrel_lo(11b)(t6)
	add			a2,t5,x0
	addiw		a3,x0,0
	jal			x0,.radixSort_61
.radixSort_90:
9:
	auipc		a0,%pcrel_hi(base)
	lw			a2,%pcrel_lo(9b)(a0)
	addiw		a3,x0,0
	jal			x0,.radixSort_69
.radixSort_92:
10:
	auipc		t6,%pcrel_hi(base)
	lw			a1,%pcrel_lo(10b)(t6)
	add			a2,t5,x0
	addiw		a3,x0,0
	jal			x0,.radixSort_65
.radixSort_94:
12:
	auipc		t4,%pcrel_hi(base)
	lw			a0,%pcrel_lo(12b)(t4)
	add			a1,t5,x0
	addiw		a2,x0,0
	jal			x0,.radixSort_53
.radixSort_97:
	addi		t2,t1,0
	slli		t2,t2,2
	li			t3,64
	add			t3,sp,t3
	add			t2,t3,t2
	lw			t2,0(t2)
	addi		t3,t1,0
	slli		t3,t3,2
	li			t4,128
	add			t4,sp,t4
	add			t3,t4,t3
	jal			x0,.radixSort_22
main:
.main_0:
	sd			s1,-24(sp)
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-32
	jal			x0,.main_1
.main_1:
	slli		t0,x0,2
	la			t1,a
	add			fp,t0,t1
	add			a0,fp,x0
	call		getarray
	addw		s1,a0,x0
	addiw		a0,x0,90
	call		_sysy_starttime
	addiw		a0,x0,8
	add			a1,fp,x0
	addiw		a2,x0,0
	add			a3,s1,x0
	call		radixSort
	addiw		t0,x0,0
	addiw		t1,x0,0
	addiw		t2,x0,0
	blt			t2,s1,.main_2
	jal			x0,.main_7
.main_2:
20:
	auipc		t1,%pcrel_hi(ans)
	lw			t1,%pcrel_lo(20b)(t1)
	addi		t2,t0,0
	slli		t2,t2,2
	la			t3,a
	add			t2,t2,t3
	lw			t2,0(t2)
	addiw		t3,x0,2
	addw		t3,t3,t0
	remw		t2,t2,t3
	mulw		t2,t0,t2
	addw		t1,t1,t2
21:
	auipc		t2,%pcrel_hi(ans)
	sw			t1,%pcrel_lo(21b)(t2)
	addiw		t2,t0,1
	add			t0,t2,x0
	add			t1,t2,x0
	blt			t2,s1,.main_2
	jal			x0,.main_7
.main_7:
22:
	auipc		t0,%pcrel_hi(ans)
	lw			t0,%pcrel_lo(22b)(t0)
	addiw		t1,x0,0
	blt			t0,t1,.main_10
	jal			x0,.main_11
.main_10:
23:
	auipc		t0,%pcrel_hi(ans)
	lw			t0,%pcrel_lo(23b)(t0)
	addiw		t1,x0,0
	subw		t0,t1,t0
24:
	auipc		t1,%pcrel_hi(ans)
	sw			t0,%pcrel_lo(24b)(t1)
	jal			x0,.main_11
.main_11:
	addiw		a0,x0,102
	call		_sysy_stoptime
25:
	auipc		t0,%pcrel_hi(ans)
	lw			a0,%pcrel_lo(25b)(t0)
	call		putint
	addiw		a0,x0,10
	call		putch
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	jalr		x0,ra,0
	.data
base:
	.word	16
a:
	.zero	120000040
ans:
	.word	0
