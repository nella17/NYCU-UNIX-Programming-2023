sort:
	push   rbp
	mov    rbp,rsp
	movsxd rsi,esi
	shl    rsi,0x3
	lea    rax,[rsi+0xf]
	mov    rcx,rax
	and    rcx,0xfffffffffffffff0
	and    rax,0xfffffffffffff000
	mov    rdx,rsp
	sub    rdx,rax
	cmp    rsp,rdx
	je     sort+0x39
	sub    rsp,0x1000
	or     QWORD PTR [rsp+0xff8],0x0
	jmp    sort+0x22
	mov    rax,rcx
	and    eax,0xfff
	sub    rsp,rax
	test   rax,rax
	je     sort+0x4f
	or     QWORD PTR [rsp+rax*1-0x8],0x0
	mov    rdx,rsp
	add    rsi,rdi
	call   merge
	leave  
	ret    

merge:
	push   r14
	push   r13
	push   r12
	push   rbp
	push   rbx
	mov    r14,rsi
	sub    r14,rdi
	sar    r14,0x3
	cmp    r14d,0x1
	jle    merge+0x103
	mov    r12,rdi
	mov    rbp,rsi
	mov    r13,rdx
	mov    eax,r14d
	shr    eax,0x1f
	add    eax,r14d
	sar    eax,1
	cdqe   
	lea    rbx,[rdi+rax*8]
	mov    rsi,rbx
	call   merge
	mov    rdx,r13
	mov    rsi,rbp
	mov    rdi,rbx
	call   merge
	cmp    r12,rbx
	jae    merge+0x92
	cmp    rbp,rbx
	jbe    merge+0x92
	lea    rcx,[r13+0x8]
	mov    rax,rbx
	mov    rdi,r12
	jmp    merge+0x7e
	mov    rdx,rcx
	add    rax,0x8
	mov    rsi,r8
	mov    QWORD PTR [rcx-0x8],rsi
	add    rcx,0x8
	cmp    rdi,rbx
	jae    merge+0x9b
	cmp    rax,rbp
	jae    merge+0x9b
	mov    rsi,QWORD PTR [rdi]
	mov    r8,QWORD PTR [rax]
	cmp    rsi,r8
	jge    merge+0x62
	mov    rdx,rcx
	add    rdi,0x8
	jmp    merge+0x6c
	mov    rdx,r13
	mov    rax,rbx
	mov    rdi,r12
	cmp    rbx,rdi
	jbe    merge+0xcb
	mov    rsi,rdx
	mov    rcx,rdi
	add    rcx,0x8
	mov    r8,QWORD PTR [rcx-0x8]
	add    rsi,0x8
	mov    QWORD PTR [rsi-0x8],r8
	cmp    rbx,rcx
	ja     merge+0xa6
	sub    rbx,0x1
	sub    rbx,rdi
	shr    rbx,0x3
	lea    rdx,[rdx+rbx*8+0x8]
	cmp    rbp,rax
	jbe    merge+0xe5
	add    rax,0x8
	mov    rcx,QWORD PTR [rax-0x8]
	add    rdx,0x8
	mov    QWORD PTR [rdx-0x8],rcx
	cmp    rbp,rax
	ja     merge+0xd0
	lea    ecx,[r14-0x1]
	mov    eax,0x0
	mov    rdx,QWORD PTR [r13+rax*8+0x0]
	mov    QWORD PTR [r12+rax*8],rdx
	mov    rdx,rax
	add    rax,0x1
	cmp    rcx,rdx
	jne    merge+0xee
	pop    rbx
	pop    rbp
	pop    r12
	pop    r13
	pop    r14
	ret    
