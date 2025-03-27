	.text
	.attribute	4, 16
	.attribute	5, "rv64i2p0_m2p0_a2p0_c2p0"
	.file	"temp.out.ll"
	.globl	main
	.p2align	1
	.type	main,@function
main:
	.cfi_startproc
	addi	sp, sp, -16
	.cfi_def_cfa_offset 16
	j	.LBB0_1
.LBB0_1:
	li	a0, 0
	sw	a0, 12(sp)
	addi	sp, sp, 16
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc

	.section	".note.GNU-stack","",@progbits
	.addrsig
