	.text
	.globl main
	.attribute arch, "rv64gc"
main:
.main_0:
	sd			ra,-8(sp)
	li			t0,32784
	sub			sp,sp,t0
	jal			x0,.main_1
.main_1:
	li			t0,16384
	add			a0,sp,t0
	addiw		a1,x0,0
	lui			t0,4
	addiw		a2,t0,0
	call		memset
	addiw		t0,x0,1024 ;t0=1024
	addiw		t1,x0,1    ;t1=1
	mul			t0,t1,t0   ;t0=1024
	slli		t0,t0,2    ;t0=4096
	li			t1,16384   ;t1=16384
	add			t1,sp,t1   ;t1=sp+16384
	add			t0,t1,t0   ;t0=sp+16384+4096
	addiw		t1,x0,1    ;t1=1
	sw			t1,0(t0)   ;sp[16384+4096]=1
	addiw		t0,x0,1024 ;t0=1024
	addiw		t1,x0,2    ;t1=2
	mul			t0,t1,t0   ;t0=2048
	slli		t0,t0,2    ;t0=8192
	li			t1,16384   ;t1=16384
	add			t1,sp,t1   ;t1=sp+16384
	add			t0,t1,t0   ;t0=sp+16384+8192
	addiw		t1,x0,2    ;t1=2
	sw			t1,0(t0)   ;sp[16384+8192]=2
	addiw		t0,x0,1024 ;t0=1024
	addiw		t1,x0,2    ;t1=2
	mul			t0,t1,t0   ;t0=2048
	addiw		t1,x0,1    ;t1=1
	addiw		t2,x0,1    ;t2=1
	mul			t1,t2,t1   ;t1=2
	add			t0,t1,t0   ;t0=2048+2=2050
	slli		t0,t0,2    ;t0=8200
	li			t1,16384   ;t1=16384
	add			t1,sp,t1   ;t1=sp+16384
	add			t0,t1,t0   ;t0=sp+16384+8200
	addiw		t1,x0,3    ;t1=3
	sw			t1,0(t0)   ;sp[16384+8200]=3
	addiw		t0,x0,1024 ;t0=1024
	addiw		t1,x0,3    ;t1=3
	mul			t0,t1,t0   ;t0=3072
	slli		t0,t0,2    ;t0=12288
	li			t1,16384   ;t1=16384
	add			t1,sp,t1   ;t1=sp+16384
	add			t0,t1,t0   ;t0=sp+16384+12288
	addiw		t1,x0,4    ;t1=4
	sw			t1,0(t0)   ;sp[16384+12288]=4
	addiw		t0,x0,1024 ;t0=1024
	addiw		t1,x0,3    ;t1=3
	mul			t0,t1,t0   ;t0=3072
	addiw		t1,x0,1    ;t1=1
	addiw		t2,x0,1    ;t2=1
	mul			t1,t2,t1   ;t1=3
	add			t0,t1,t0   ;t0=3072+3=3075
	slli		t0,t0,2    ;t0=12300
	li			t1,16384   ;t1=16384
	add			t1,sp,t1   ;t1=sp+16384
	add			t0,t1,t0   ;t0=sp+16384+12300
	addiw		t1,x0,5    ;t1=5
	sw			t1,0(t0)   ;sp[16384+12300]=5
	addiw		t0,x0,1024 ;t0=1024
	addiw		t1,x0,3    ;t1=3
	mul			t0,t1,t0   ;t0=3072
	addiw		t1,x0,1    ;t1=1
	addiw		t2,x0,2    ;t2=2
	mul			t1,t2,t1   ;t1=6
	add			t0,t1,t0   ;t0=3072+6=3078
	slli		t0,t0,2    ;t0=12312
	li			t1,16384   ;t1=16384
	add			t1,sp,t1   ;t1=sp+16384
	add			t0,t1,t0   ;t0=sp+16384+12312
	addiw		t1,x0,6    ;t1=6
	sw			t1,0(t0)   ;sp[16384+12312]=6
	li			t0,0       ;t0=0
	add			a0,sp,t0   ;a0=sp+0
	addiw		a1,x0,0    ;a1=0
	lui			t0,4	   ;t0=4
	addiw		a2,t0,0    ;a2=4
	call		memset
	slli		t0,x0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,1
	sw			t1,0(t0)
	addiw		t0,x0,1
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,2
	sw			t1,0(t0)
	addiw		t0,x0,4
	addiw		t1,x0,1
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,3
	sw			t1,0(t0)
	addiw		t0,x0,4
	addiw		t1,x0,1
	mul			t0,t1,t0
	addiw		t1,x0,1
	addiw		t2,x0,1
	mul			t1,t2,t1
	add			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t1,x0,4
	sw			t1,0(t0)
	slli		t0,x0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	addiw		t0,x0,1
	addiw		t1,x0,5
	mul			t0,t1,t0
	slli		t0,t0,2
	la			t1,a
	add			t0,t0,t1
	lui			t1,1
	addiw		t1,t1,-96
	sw			t1,0(t0)
	addiw		t1,x0,1
	lui			t2,1
	addiw		t2,t2,-96
	mul			t1,t2,t1
	slli		t1,t1,2
	la			t2,a
	add			t1,t1,t2
	addiw		t2,x0,3
	sw			t2,0(t1)
	addiw		t1,x0,1
	lui			t2,1
	addiw		t2,t2,-1
	mul			t1,t2,t1
	slli		t1,t1,2
	la			t2,a
	add			t1,t1,t2
	addiw		t2,x0,7
	sw			t2,0(t1)
	addiw		t2,x0,1
	lui			t3,1
	addiw		t3,t3,-1880
	mul			t2,t3,t2
	slli		t2,t2,2
	la			t3,a
	add			t2,t2,t3
	lw			t2,0(t2)
	addiw		t2,t2,9
	lw			t1,0(t1)
	slli		t1,t1,2
	li			t3,0
	add			t3,sp,t3
	add			t1,t3,t1
	sw			t2,0(t1)
	lw			t0,0(t0)
	slli		t0,t0,2
	la			t1,a
	add			t0,t0,t1
	lw			t0,0(t0)
	addiw		a0,t0,0
	call		putint
	addiw		a0,x0,10
	call		putch
	addiw		t0,x0,4
	addiw		t1,x0,2
	mul			t0,t1,t0
	slli		t0,t0,2
	li			t1,0
	add			t1,sp,t1
	add			t0,t1,t0
	lw			a0,0(t0)
	li			t0,32784
	add			sp,sp,t0
	ld			ra,-8(sp)
	jalr		x0,ra,0
	.data
a:
	.zero	16384
