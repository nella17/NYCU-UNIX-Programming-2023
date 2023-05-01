sort:
    movsxd rsi,esi
    lea    rsi,[rdi+rsi*8]
    jmp merge
merge:
    endbr64
	push   rbp
	mov    rbp,rsp
	push   r15
	push   r14
	push   r13
	push   r12
	push   rbx
	mov    rbx,rsi
	sub    rbx,rdi
	push   rax
	sar    rbx,0x3
	cmp    ebx,0x1
	je     merge+0x121
	mov    eax,ebx
	mov    ecx,0x2
	mov    r13,rsi
	mov    r12,rdi
	cdq
	mov    r14,rsp
	idiv   ecx
	cdqe
	lea    r15,[rdi+rax*8]
	mov    rsi,r15
	call   merge
	mov    rsi,r13
	mov    rdi,r15
	call   merge
	movsxd rax,ebx
	mov    rcx,rsp
	lea    rdx,[rax*8+0xf]
	mov    rax,rdx
	and    rdx,0xfffffffffffff000
	and    rax,0xfffffffffffffff0
	sub    rcx,rdx
	cmp    rsp,rcx
	je     merge+0x87
	sub    rsp,0x1000
	or     QWORD PTR [rsp+0xff8],0x0
	jmp    merge+0x70
	and    eax,0xfff
	sub    rsp,rax
	test   rax,rax
	je     merge+0x9a
	or     QWORD PTR [rsp+rax*1-0x8],0x0
	mov    r8,rsp
	mov    r9,r15
	mov    rsi,r12
	mov    r10,r8
	cmp    rsi,r15
	jae    merge+0xd2
	cmp    r9,r13
	jae    merge+0xd2
	mov    rdx,QWORD PTR [rsi]
	mov    rax,QWORD PTR [r9]
	add    r10,0x8
	cmp    rdx,rax
	jge    merge+0xc8
	add    rsi,0x8
	mov    rax,rdx
	jmp    merge+0xcc
	add    r9,0x8
	mov    QWORD PTR [r10-0x8],rax
	jmp    merge+0xa6
	lea    rcx,[r15+0x7]
	lea    rdx,[rsi-0x7]
	xor    eax,eax
	mov    rdi,r10
	sub    rcx,rsi
	and    rcx,0xfffffffffffffff8
	cmp    rdx,r15
	cmova  rcx,rax
	rep movs BYTE PTR es:[rdi],BYTE PTR ds:[rsi]
	lea    rcx,[r13+0x7]
	lea    rsi,[r9-0x7]
	sub    rcx,r9
	and    rcx,0xfffffffffffffff8
	cmp    rsi,r13
	mov    rsi,r9
	cmova  rcx,rax
	rep movs BYTE PTR es:[rdi],BYTE PTR ds:[rsi]
	mov    ecx,ebx
	mov    rdi,r12
	mov    rsi,r8
	shl    rcx,0x3
	test   ebx,ebx
	cmovs  rcx,rax
	rep movs BYTE PTR es:[rdi],BYTE PTR ds:[rsi]
	mov    rsp,r14
	lea    rsp,[rbp-0x28]
	pop    rbx
	pop    r12
	pop    r13
	pop    r14
	pop    r15
	pop    rbp
	ret
