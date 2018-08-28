.section .text
.globl call_shell
.globl call_shell_end
.type call_shell, @function
call_shell:
	lea  0x08(%r12), %rdi		
	lea  0x8e(%r12), %rax	
	call *%rax

	/* for process continuation */
	lea -0x28(%rbp), %rsp
	pop %rbx
	pop %r12
	pop %r13
	pop %r14
	pop %r15
	pop %rbp
	ret	
call_shell_end:
