section .data
buff db '0'
section .text
global _start
_start:
call program
mov rax, 60
xor rdi, rdi
syscall
print:
push rcx
call printnum
mov sil, 10
mov [buff], sil
call printc
pop rcx
ret
printnum:
push rax
push rbx
push rcx
push rdx
cmp rax, 0
jge .check_zero
mov sil, '-'
mov [buff], sil
call printc
neg rax
.check_zero:
mov rbx, 10
xor rdx, rdx
div rbx
cmp rax, 0
je .print_digit
push rdx
call printnum
pop rdx
.print_digit:
add dl, '0'
mov [buff], dl
call printc
pop rdx
pop rcx
pop rbx
pop rax
ret
printc:
push rdi
push rsi
push rdx
push rax
mov rdi, 1
mov rsi, buff
mov rdx, 1
mov rax, 1
syscall
pop rax
pop rdx
pop rsi
pop rdi
ret
program:
mov eax, 23
sub rsp, 4
mov dword [rsp], eax
sub rsp, 4
mov dword [rsp], ebx
mov eax, 20
mov ebx, eax
mov eax, [rsp + 4]
sub eax, ebx
mov ebx, [rsp]
add rsp, 4
sub rsp, 4
mov dword [rsp], eax
mov eax, [rsp + 0]
mov rcx, rsp
and rsp, 0xFFFFFFFFFFFFFFF0
call print
mov rsp, rcx
sub rsp, 4
mov dword [rsp], ebx
mov eax, 2
mov ebx, eax
mov eax, [rsp + 4]
sub eax, ebx
mov ebx, [rsp]
add rsp, 4
mov [rsp + 4], eax
mov eax, [rsp + 4]
mov rcx, rsp
and rsp, 0xFFFFFFFFFFFFFFF0
call print
mov rsp, rcx
add rsp, 8
ret
