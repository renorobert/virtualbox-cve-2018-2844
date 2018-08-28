exploit: helpers.c shellcode.c exploit.c stage_one.S syscall.S
	gcc -Wall -ggdb -o exploit syscall.S stage_one.S shellcode.c helpers.c exploit.c -lpthread
clean:
	rm exploit
