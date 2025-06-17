	.text
	.globl main
	.attribute arch, "rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0"
search:
.search_0:
	addi		sp,sp,-16
	add			t0,a1,x0
	add			t1,a0,x0
	add			t1,t1,x0
	add			t0,t0,x0
	jal			x0,.search_1
.search_1:
	add			t1,t1,x0
	add			t0,t0,x0
	jal			x0,.search_2
.search_2:
	addiw		t2,x0,-1
	beq			t1,t2,.search_6
	jal			x0,.search_5
.search_5:
	addiw		t2,x0,1
	mul			t2,t1,t2
	slli		t2,t2,2
	la			t3,value
	add			t2,t2,t3
	lw			t2,0(t2)
	beq			t2,t0,.search_6
	jal			x0,.search_8
.search_6:
	add			a0,t1,x0
	addi		sp,sp,16
	jalr		x0,ra,0
.search_8:
	addiw		t2,x0,1
	mul			t2,t1,t2
	slli		t2,t2,2
	la			t3,value
	add			t2,t2,t3
	lw			t2,0(t2)
	bgt			t0,t2,.search_11
	jal			x0,.search_12
.search_11:
	addiw		t2,x0,1
	mul			t1,t1,t2
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	lw			t1,0(t1)
	add			t1,t1,x0
	add			t0,t0,x0
	jal			x0,.search_1
.search_12:
	addiw		t2,x0,1
	mul			t1,t1,t2
	slli		t1,t1,2
	la			t2,left_child
	add			t1,t1,t2
	lw			t1,0(t1)
	add			t1,t1,x0
	add			t0,t0,x0
	jal			x0,.search_1
find_minimum:
.find_minimum_0:
	addi		sp,sp,-16
	add			t0,a0,x0
	add			t0,t0,x0
	jal			x0,.find_minimum_1
.find_minimum_1:
	add			t0,t0,x0
	jal			x0,.find_minimum_2
.find_minimum_2:
	addiw		t1,x0,-1
	beq			t0,t1,.find_minimum_5
	jal			x0,.find_minimum_7
.find_minimum_5:
	addiw		a0,x0,-1
	addi		sp,sp,16
	jalr		x0,ra,0
.find_minimum_7:
	addiw		t1,x0,1
	mul			t1,t0,t1
	slli		t1,t1,2
	la			t2,left_child
	add			t1,t1,t2
	lw			t1,0(t1)
	addiw		t2,x0,-1
	bne			t1,t2,.find_minimum_10
	jal			x0,.find_minimum_12
.find_minimum_10:
	addiw		t1,x0,1
	mul			t0,t0,t1
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	lw			t0,0(t0)
	add			t0,t0,x0
	jal			x0,.find_minimum_1
.find_minimum_12:
	add			a0,t0,x0
	addi		sp,sp,16
	jalr		x0,ra,0
new_node:
.new_node_0:
	addi		sp,sp,-16
	add			t0,a0,x0
	jal			x0,.new_node_1
.new_node_1:
	la			t1,now
	lw			t1,0(t1)
	addiw		t2,x0,1
	mul			t1,t1,t2
	slli		t1,t1,2
	la			t2,value
	add			t1,t1,t2
	sw			t0,0(t1)
	la			t0,now
	lw			t0,0(t0)
	addiw		t1,x0,1
	mul			t0,t0,t1
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	addiw		t1,x0,-1
	sw			t1,0(t0)
	la			t0,now
	lw			t0,0(t0)
	addiw		t1,x0,1
	mul			t0,t0,t1
	slli		t0,t0,2
	la			t1,right_child
	add			t0,t0,t1
	addiw		t1,x0,-1
	sw			t1,0(t0)
	la			t0,now
	lw			t0,0(t0)
	addiw		t1,x0,1
	addw		t0,t0,t1
	la			t1,now
	sw			t0,0(t1)
	la			t0,now
	lw			t0,0(t0)
	addiw		t1,x0,1
	subw		t0,t0,t1
	add			a0,t0,x0
	addi		sp,sp,16
	jalr		x0,ra,0
insert:
.insert_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	add			t0,a1,x0
	add			fp,a0,x0
	jal			x0,.insert_2
.insert_2:
	addiw		t1,x0,-1
	beq			fp,t1,.insert_5
	jal			x0,.insert_7
.insert_5:
	add			a0,t0,x0
	call		new_node
	addw		t0,a0,x0
	add			a0,t0,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.insert_7:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,value
	add			t1,t1,t2
	lw			t1,0(t1)
	bgt			t0,t1,.insert_10
	jal			x0,.insert_11
.insert_10:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	lw			t1,0(t1)
	add			a0,t1,x0
	add			a1,t0,x0
	call		insert
	addw		t0,a0,x0
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	sw			t0,0(t1)
	jal			x0,.insert_13
.insert_11:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,left_child
	add			t1,t1,t2
	lw			t1,0(t1)
	add			a0,t1,x0
	add			a1,t0,x0
	call		insert
	addw		t0,a0,x0
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,left_child
	add			t1,t1,t2
	sw			t0,0(t1)
	jal			x0,.insert_13
.insert_13:
	add			a0,fp,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
delete:
.delete_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	add			t0,a1,x0
	add			fp,a0,x0
	jal			x0,.delete_2
.delete_2:
	addiw		t1,x0,-1
	beq			fp,t1,.delete_5
	jal			x0,.delete_7
.delete_5:
	addiw		a0,x0,-1
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.delete_7:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,value
	add			t1,t1,t2
	lw			t1,0(t1)
	bgt			t0,t1,.delete_10
	jal			x0,.delete_12
.delete_10:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	lw			t1,0(t1)
	add			a0,t1,x0
	add			a1,t0,x0
	call		delete
	addw		t0,a0,x0
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	sw			t0,0(t1)
	jal			x0,.delete_38
.delete_12:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,value
	add			t1,t1,t2
	lw			t1,0(t1)
	blt			t0,t1,.delete_15
	jal			x0,.delete_17
.delete_15:
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,left_child
	add			t1,t1,t2
	lw			t1,0(t1)
	add			a0,t1,x0
	add			a1,t0,x0
	call		delete
	addw		t0,a0,x0
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,left_child
	add			t1,t1,t2
	sw			t0,0(t1)
	jal			x0,.delete_38
.delete_17:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	lw			t0,0(t0)
	addiw		t1,x0,-1
	beq			t0,t1,.delete_20
	jal			x0,.delete_23
.delete_20:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,right_child
	add			t0,t0,t1
	lw			t0,0(t0)
	addiw		t1,x0,-1
	beq			t0,t1,.delete_21
	jal			x0,.delete_23
.delete_21:
	addiw		a0,x0,-1
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.delete_23:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	lw			t0,0(t0)
	addiw		t1,x0,-1
	beq			t0,t1,.delete_28
	jal			x0,.delete_26
.delete_26:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,right_child
	add			t0,t0,t1
	lw			t0,0(t0)
	addiw		t1,x0,-1
	beq			t0,t1,.delete_28
	jal			x0,.delete_34
.delete_28:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	lw			t0,0(t0)
	addiw		t1,x0,-1
	beq			t0,t1,.delete_31
	jal			x0,.delete_32
.delete_31:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,right_child
	add			t0,t0,t1
	lw			t0,0(t0)
	add			a0,t0,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.delete_32:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	lw			t0,0(t0)
	add			a0,t0,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
.delete_34:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,right_child
	add			t0,t0,t1
	lw			t0,0(t0)
	add			a0,t0,x0
	call		find_minimum
	addw		t0,a0,x0
	addiw		t1,x0,1
	mul			t1,t0,t1
	slli		t1,t1,2
	la			t2,value
	add			t1,t1,t2
	lw			t1,0(t1)
	addiw		t2,x0,1
	mul			t2,fp,t2
	slli		t2,t2,2
	la			t3,value
	add			t2,t2,t3
	sw			t1,0(t2)
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	lw			t1,0(t1)
	addiw		t2,x0,1
	mul			t0,t0,t2
	slli		t0,t0,2
	la			t2,value
	add			t0,t0,t2
	lw			t0,0(t0)
	add			a0,t1,x0
	add			a1,t0,x0
	call		delete
	addw		t0,a0,x0
	addiw		t1,x0,1
	mul			t1,fp,t1
	slli		t1,t1,2
	la			t2,right_child
	add			t1,t1,t2
	sw			t0,0(t1)
	jal			x0,.delete_38
.delete_38:
	add			a0,fp,x0
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	jalr		x0,ra,0
inorder:
.inorder_0:
	sd			fp,-16(sp)
	sd			ra,-8(sp)
	addi		sp,sp,-16
	add			fp,a0,x0
	jal			x0,.inorder_2
.inorder_2:
	addiw		t0,x0,-1
	bne			fp,t0,.inorder_5
	jal			x0,.inorder_6
.inorder_5:
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,left_child
	add			t0,t0,t1
	lw			t0,0(t0)
	add			a0,t0,x0
	call		inorder
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,value
	add			t0,t0,t1
	lw			t0,0(t0)
	add			a0,t0,x0
	call		putint
	la			t0,space
	lw			t0,0(t0)
	add			a0,t0,x0
	call		putch
	addiw		t0,x0,1
	mul			t0,fp,t0
	slli		t0,t0,2
	la			t1,right_child
	add			t0,t0,t1
	lw			t0,0(t0)
	add			a0,t0,x0
	call		inorder
	jal			x0,.inorder_6
.inorder_6:
	addi		sp,sp,16
	ld			ra,-8(sp)
	ld			fp,-16(sp)
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
	addiw		t0,x0,0
	la			t1,now
	sw			t0,0(t1)
	call		getint
	addw		fp,a0,x0
	jal			x0,.main_2
.main_2:
	addiw		t0,x0,0
	subw		t0,fp,t0
	sltiu		t0,t0,1
	addiw		t1,x0,0
	bne			t0,t1,.main_5
	jal			x0,.main_6
.main_5:
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
.main_6:
	call		getint
	addw		t0,a0,x0
	add			a0,t0,x0
	call		new_node
	addw		s1,a0,x0
	addiw		t0,x0,1
	add			t0,t0,x0
	jal			x0,.main_7
.main_7:
	add			s2,t0,x0
	blt			s2,fp,.main_10
	jal			x0,.main_11
.main_10:
	call		getint
	addw		t0,a0,x0
	add			a0,s1,x0
	add			a1,t0,x0
	call		insert
	addw		t0,a0,x0
	addiw		t0,x0,1
	addw		t0,s2,t0
	add			t0,t0,x0
	jal			x0,.main_7
.main_11:
	add			a0,s1,x0
	call		inorder
	la			t0,LF
	lw			t0,0(t0)
	add			a0,t0,x0
	call		putch
	call		getint
	addw		fp,a0,x0
	add			t0,s1,x0
	addiw		t1,x0,0
	add			t1,t1,x0
	jal			x0,.main_12
.main_12:
	add			s1,t0,x0
	add			s2,t1,x0
	blt			s2,fp,.main_15
	jal			x0,.main_16
.main_15:
	call		getint
	addw		t0,a0,x0
	add			a0,s1,x0
	add			a1,t0,x0
	call		delete
	addw		t0,a0,x0
	addiw		t1,x0,1
	addw		t1,s2,t1
	add			t0,t0,x0
	add			t1,t1,x0
	jal			x0,.main_12
.main_16:
	add			a0,s1,x0
	call		inorder
	la			t0,LF
	lw			t0,0(t0)
	add			a0,t0,x0
	call		putch
	addiw		a0,x0,0
	addi		sp,sp,32
	ld			ra,-8(sp)
	ld			fp,-16(sp)
	ld			s1,-24(sp)
	ld			s2,-32(sp)
	jalr		x0,ra,0
	.data
space:
	.word	32
LF:
	.word	10
maxNode:
	.word	10000
value:
	.zero	40000
left_child:
	.zero	40000
right_child:
	.zero	40000
now:
	.word	0
