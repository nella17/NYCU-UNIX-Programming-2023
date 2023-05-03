sort:
	movsxd rsi,esi
	lea    rsi,[rdi+rsi*8-0x8]
	jmp    impl
impl:
	push   rbp
	mov    rbp,rsi
	push   rbx
	push   rdx
	mov    rdx,QWORD PTR [rdi]
	mov    rax,rdi
	cmp    rdx,QWORD PTR [rbp+0x0]
	mov    rbx,rbp
	cmovl  rax,rbp
	mov    rcx,QWORD PTR [rax]
	mov    QWORD PTR [rax],rdx
	lea    rax,[rdi+0x8]
	cmp    rax,rbx
	ja     impl+0x60
	cmp    QWORD PTR [rax],rcx
	jg     impl+0x36
	add    rax,0x8
	cmp    rbx,rax
	jae    impl+0x26
	jmp    impl+0x4d
	mov    rdx,rbx
	mov    rsi,QWORD PTR [rdx]
	mov    rbx,rdx
	cmp    rdx,rax
	jb     impl+0x4d
	sub    rdx,0x8
	cmp    rcx,rsi
	jl     impl+0x39
	cmp    rax,rbx
	jae    impl+0x21
	mov    rsi,QWORD PTR [rbx]
	mov    rdx,QWORD PTR [rax]
	mov    QWORD PTR [rax],rsi
	mov    QWORD PTR [rbx],rdx
	jmp    impl+0x21
	mov    rax,QWORD PTR [rbx]
	lea    rsi,[rbx-0x8]
	mov    QWORD PTR [rdi],rax
	mov    QWORD PTR [rbx],rcx
	cmp    rsi,rdi
	jbe    impl+0x77
	call   impl
	lea    rdi,[rbx+0x8]
	cmp    rdi,rbp
	jb     impl+0x6
	pop    rax
	pop    rbx
	pop    rbp
	ret    
