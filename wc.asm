section         .text

global          _start

_start:
        pop             rax
        cmp             rax, 2
        jne             bad_args

        pop             rax
        pop             rdi
        mov             rax, 2
        xor             rsi, rsi
        xor             rdx, rdx
        syscall
        cmp             rax, 0
        jl              open_fail
        mov             rdi, rax

        xor             rbx, rbx
        xor             r8,  r8
        mov             r9,  1
.read_again:
        xor             rax, rax
        mov             rsi, buf
        mov             rdx, buf_size
        syscall

        lea             rsi, [buf + rax]
        neg             rax
        jz              exit
        jg              read_fail

.next_byte:
        movzx           edx, byte [rsi + rax]
        mov             r8, r9
        xor             r9, r9
        cmp             dl, 9
        jl              .check_new_word
        cmp             dl, 13
        jng             .set_as_white_space
        cmp             dl, 32
        jne             .check_new_word

.set_as_white_space:
        mov             r9, 1

.check_new_word:
        cmp             r8, r9
        jnl             .not_eoln
        inc             rbx

.not_eoln:
        inc             rax
        jnz             .next_byte

        jmp             .read_again

exit:
        mov             rax, 3
        syscall

        mov             rax, rbx
        call            write_number

        mov             rax, 60
        xor             rdi, rdi
        syscall

; rax -- number to print
write_number:
        mov             rbp, rsp
        mov             rdi, rsp
        sub             rsp, 24

        dec             rdi
        mov             byte [rdi], 10

        or              rax, rax
        jz              .write_zero

        mov             ebx, 10
.loop:
        xor             edx, edx
        div             rbx

        add             edx, '0'
        dec             rdi
        mov             byte [rdi], dl

        or              rax, rax
        jnz             .loop
        jmp             .print

.write_zero:
        dec             rdi
        mov             byte [rdi], '0'

.print:
        mov             eax, 1
        mov             rsi, rdi
        mov             rdx, rbp
        sub             rdx, rdi
        mov             edi, eax
        syscall

        mov             rsp, rbp
        ret

bad_args:
        mov             rsi, bad_args_msg
        mov             rdx, bad_args_msg_size
        jmp             print_error_and_quit

open_fail:
        mov             rsi, open_fail_msg
        mov             rdx, open_fail_msg_size
        jmp             print_error_and_quit

read_fail:
        mov             rsi, read_fail_msg
        mov             rdx, read_fail_msg_size
        jmp             print_error_and_quit

write_fail:
        mov             rsi, write_fail_msg
        mov             rdx, write_fail_msg_size
        jmp             print_error_and_quit

print_error_and_quit:
        mov             rax, 1
        mov             rdi, 1
        syscall

        mov             rax, 60
        mov             rdi, 1
        syscall

        section         .rodata
bad_args_msg:   db              "argument number mismatch", 10
bad_args_msg_size: equ $ - bad_args_msg
open_fail_msg:  db              "open failed", 10
open_fail_msg_size: equ $ - open_fail_msg
read_fail_msg:  db              "read failed", 10
read_fail_msg_size: equ $ - read_fail_msg
write_fail_msg: db              "write failed", 10
write_fail_msg_size: equ $ - write_fail_msg

        section         .bss

buf_size:       equ             1024
buf:            resb            buf_size
