sort:
	movsxd rsi,esi
	lea    rsi,[rdi+rsi*8-0x8]
	jmp    impl
impl:
	push   rbp
	mov    rbp,rsi
	push   rbx
	push   rdx
	mov    rax,rbp
	sub    rax,rdi
	cmp    rax,0x48
	jg     impl+0x47
	cmp    rdi,rbp
	ja     impl+0xc2
	lea    rdx,[rdi+0x8]
	mov    rax,rdx
	mov    rcx,QWORD PTR [rdi]
	cmp    rax,rbp
	ja     impl+0x37
	cmp    QWORD PTR [rax],rcx
	cmovl  rdi,rax
	add    rax,0x8
	jmp    impl+0x22
	mov    rax,QWORD PTR [rdx-0x8]
	mov    QWORD PTR [rdx-0x8],rcx
	mov    QWORD PTR [rdi],rax
	mov    rdi,rdx
	jmp    impl+0x12
	mov    rcx,QWORD PTR [rdi]
	mov    rax,rdi
	cmp    rcx,QWORD PTR [rbp+0x0]
	mov    rbx,rbp
	cmovl  rax,rbp
	mov    rdx,QWORD PTR [rax]
	mov    QWORD PTR [rax],rcx
	lea    rax,[rdi+0x8]
	cmp    rax,rbx
	ja     impl+0x9e
	cmp    QWORD PTR [rax],rdx
	jg     impl+0x77
	add    rax,0x8
	cmp    rbx,rax
	jae    impl+0x67
	jmp    impl+0x62
	mov    rsi,rbx
	mov    rcx,QWORD PTR [rsi]
	mov    rbx,rsi
	cmp    rsi,rax
	jb     impl+0x62
	lea    rsi,[rsi-0x8]
	cmp    rcx,rdx
	jg     impl+0x7a
	cmp    rbx,rax
	jbe    impl+0x62
	mov    rsi,QWORD PTR [rax]
	mov    QWORD PTR [rax],rcx
	mov    QWORD PTR [rbx],rsi
	jmp    impl+0x62
	mov    rax,QWORD PTR [rbx]
	lea    rsi,[rbx-0x8]
	mov    QWORD PTR [rdi],rax
	mov    QWORD PTR [rbx],rdx
	cmp    rsi,rdi
	jbe    impl+0xb5
	call   impl
	lea    rdi,[rbx+0x8]
	cmp    rdi,rbp
	jb     impl+0x6
	pop    rax
	pop    rbx
	pop    rbp
	ret    
