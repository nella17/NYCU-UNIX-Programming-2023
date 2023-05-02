sort:
	movsxd rsi,esi
	lea    rsi,[rdi+rsi*8-0x8]
	jmp    impl
impl:
	push   rbp
	mov    rbp,rsi
	push   rbx
	push   rdx
	mov    rax,QWORD PTR [rdi]
	mov    rdx,QWORD PTR [rbp+0x0]
	mov    rbx,rbp
	mov    QWORD PTR [rbp+0x0],rax
	lea    rax,[rdi+0x8]
	cmp    rax,rbx
	ja     impl+0x54
	cmp    QWORD PTR [rax],rdx
	jg     impl+0x2d
	add    rax,0x8
	cmp    rbx,rax
	jae    impl+0x1d
	jmp    impl+0x18
	mov    rsi,rbx
	mov    rcx,QWORD PTR [rsi]
	mov    rbx,rsi
	cmp    rsi,rax
	jb     impl+0x18
	lea    rsi,[rsi-0x8]
	cmp    rdx,rcx
	jl     impl+0x30
	cmp    rbx,rax
	jbe    impl+0x18
	mov    rsi,QWORD PTR [rax]
	mov    QWORD PTR [rax],rcx
	mov    QWORD PTR [rbx],rsi
	jmp    impl+0x18
	mov    rax,QWORD PTR [rbx]
	lea    rsi,[rbx-0x8]
	mov    QWORD PTR [rdi],rax
	mov    QWORD PTR [rbx],rdx
	cmp    rsi,rdi
	jbe    impl+0x6b
	call   impl
	lea    rdi,[rbx+0x8]
	cmp    rdi,rbp
	jb     impl+0x6
	pop    rax
	pop    rbx
	pop    rbp
	ret    
