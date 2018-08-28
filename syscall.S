.section .text
.globl syscall_func
.globl syscall_func_end
.type syscall_func, @function
syscall_func:
	movq %rdi, %rax		/* Syscall number -> rax.  */
	movq %rsi, %rdi		/* shift arg1 - arg5.  */
	movq %rdx, %rsi
	movq %rcx, %rdx
	movq %r8, %r10
	movq %r9, %r8
	movq 8(%rsp),%r9	/* arg6 is on the stack.  */
	syscall			/* Do the system call.  */
	ret			/* Return to caller.  */
syscall_func_end:
