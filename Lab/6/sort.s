sort:
	movsxd rsi,esi
	lea    rsi,[rdi+rsi*8-0x8]
	jmp    impl
impl:
	push   rbp
	mov    rbp,rsi
	push   rbx
	push   rdx
	cmp    rdi,rbp
	jae    impl+0x69
	mov    rdx,QWORD PTR [rdi]
	lea    rax,[rdi+0x8]
	mov    rbx,rbp
	cmp    rax,rbx
	ja     impl+0x51
	cmp    QWORD PTR [rax],rdx
	jg     impl+0x2a
	add    rax,0x8
	cmp    rbx,rax
	jae    impl+0x1a
	jmp    impl+0x15
	mov    rsi,rbx
	mov    rcx,QWORD PTR [rsi]
	mov    rbx,rsi
	cmp    rsi,rax
	jb     impl+0x15
	lea    rsi,[rsi-0x8]
	cmp    rdx,rcx
	jl     impl+0x2d
	cmp    rbx,rax
	jbe    impl+0x15
	mov    rsi,QWORD PTR [rax]
	mov    QWORD PTR [rax],rcx
	mov    QWORD PTR [rbx],rsi
	jmp    impl+0x15
	mov    rax,QWORD PTR [rbx]
	lea    rsi,[rbx-0x8]
	mov    QWORD PTR [rdi],rax
	mov    QWORD PTR [rbx],rdx
	call   impl
	lea    rdi,[rbx+0x8]
	jmp    impl+0x6
	pop    rax
	pop    rbx
	pop    rbp
	ret    
