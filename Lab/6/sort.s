sort:
	push   rbp
	mov    rbp,rsp
	movsxd rsi,esi
	lea    rdx,[rsi*8+0x0]
	sub    rsp,rdx
	mov    rdx,rsp
	lea    rsi,[rdi+rsi*8]
	call   merge
	leave  
	ret    
merge:
	mov    rax,rsi
	sub    rax,rdi
	sar    rax,0x3
	cmp    eax,0x1
	jle    merge+0xe2
	push   r14
	push   r13
	mov    r13,rdx
	push   r12
	mov    r12,rdi
	push   rbp
	mov    ebp,eax
	sar    eax,1
	cdqe   
	push   rbx
	mov    rbx,rsi
	lea    r14,[rdi+rax*8]
	mov    rsi,r14
	call   merge
	mov    rdx,r13
	mov    rsi,rbx
	mov    rdi,r14
	call   merge
	mov    rdx,r13
	mov    rsi,r14
	mov    rax,r12
	cmp    rax,r14
	jae    merge+0x79
	cmp    rsi,rbx
	jae    merge+0x79
	mov    rdi,QWORD PTR [rax]
	mov    rcx,QWORD PTR [rsi]
	add    rdx,0x8
	cmp    rdi,rcx
	jge    merge+0x6f
	add    rax,0x8
	mov    rcx,rdi
	jmp    merge+0x73
	add    rsi,0x8
	mov    QWORD PTR [rdx-0x8],rcx
	jmp    merge+0x4d
	xor    ecx,ecx
	lea    rdi,[rcx+rax*1]
	cmp    r14,rdi
	jbe    merge+0x92
	mov    rdi,QWORD PTR [rax+rcx*1]
	mov    QWORD PTR [rdx+rcx*1],rdi
	add    rcx,0x8
	jmp    merge+0x7b
	lea    rcx,[r14+0x7]
	sub    rcx,rax
	sub    rax,0x7
	and    rcx,0xfffffffffffffff8
	cmp    r14,rax
	mov    eax,0x0
	cmovb  rcx,rax
	add    rdx,rcx
	lea    rcx,[rsi+rax*1]
	cmp    rbx,rcx
	jbe    merge+0xc7
	mov    rcx,QWORD PTR [rsi+rax*1]
	mov    QWORD PTR [rdx+rax*1],rcx
	add    rax,0x8
	jmp    merge+0xb0
	xor    eax,eax
	mov    rdx,QWORD PTR [r13+rax*8+0x0]
	mov    QWORD PTR [r12+rax*8],rdx
	inc    rax
	cmp    ebp,eax
	jg     merge+0xc9
	pop    rbx
	pop    rbp
	pop    r12
	pop    r13
	pop    r14
	ret    
	ret    
