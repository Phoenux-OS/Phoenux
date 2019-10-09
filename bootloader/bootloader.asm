org 0x7C00						; BIOS loads us here (0000:7C00)
bits 16							; 16-bit real mode code

jmp short code_start			; Jump to the start of the code

times 64-($-$$) db 0x00			; Pad some space for the echfs header

; Start of main bootloader code

code_start:

cli
jmp 0x0000:initialise_cs		; Initialise CS to 0x0000 with a long jump
initialise_cs:
xor ax, ax
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax
mov sp, 0x7BF0
sti

mov byte [drive_number], dl		; Save boot drive in memory

mov si, LoadingMsg				; Print loading message using simple print (BIOS)
call simple_print

; ****************** Load stage 2 ******************

mov si, Stage2Msg				; Print loading stage 2 message
call simple_print

mov eax, 1						; Start from LBA sector 1
mov ebx, 0x7E00					; Load to offset 0x7E00
mov ecx, 7						; Load 7 sectors
call read_sectors

jc err							; Catch any error

mov si, DoneMsg
call simple_print				; Display done message

jmp 0x7E00						; Jump to stage 2

err:
mov si, ErrMsg
call simple_print

halt:
hlt
jmp halt

;Data

LoadingMsg		db 0x0D, 0x0A, 'Loading...', 0x0D, 0x0A, 0x0A, 0x00
Stage2Msg		db 'Loading Stage 2...', 0x00
ErrMsg			db 0x0D, 0x0A, 'Error, system halted.', 0x00
DoneMsg			db '  DONE', 0x0D, 0x0A, 0x00

;Includes

%include 'bootloader/includes/simple_print.inc'
%include 'bootloader/includes/disk.inc'

drive_number				db 0x00				; Drive number

; Add a fake MBR because some motherboards won't boot otherwise

times 0x1b8-($-$$) db 0
mbr:
    .signature: dd 0xdeadbeef
    times 2 db 0
    .p1:
        db 0x80         ; status (active)
        db 0x20, 0x21, 0x00    ; CHS start
        db 0x83         ; partition type (Linux)
        db 0xb6, 0x25, 0x51    ; CHS end
        dd 1024*1024   ; LBA start
        dd 1024*1024    ; size in sectors
    .p2:
        db 0x00         ; status (invalid)
        times 3 db 0    ; CHS start
        db 0x00         ; partition type
        times 3 db 0    ; CHS end
        dd 00           ; LBA start
        dd 00           ; size in sectors
    .p3:
        db 0x00         ; status (invalid)
        times 3 db 0    ; CHS start
        db 0x00         ; partition type
        times 3 db 0    ; CHS end
        dd 00           ; LBA start
        dd 00           ; size in sectors
    .p4:
        db 0x00         ; status (invalid)
        times 3 db 0    ; CHS start
        db 0x00         ; partition type
        times 3 db 0    ; CHS end
        dd 00           ; LBA start
        dd 00           ; size in sectors

times 510-($-$$) db 0x00
dw 0xaa55

; ************************* STAGE 2 ************************

; ***** A20 *****

mov si, A20Msg					; Display A20 message
call simple_print

call enable_a20					; Enable the A20 address line to access the full memory
jc err							; If it fails, print an error and halt

mov si, DoneMsg
call simple_print				; Display done message

; ***** Unreal Mode *****

mov si, UnrealMsg				; Display unreal message
call simple_print

lgdt [GDT]						; Load the GDT

%include 'bootloader/includes/enter_unreal.inc'		; Enter Unreal Mode

mov si, DoneMsg
call simple_print				; Display done message

; ***** Kernel *****

; Load the kernel to 0x100000 (1 MiB)

mov si, KernelMsg				; Show loading kernel message
call simple_print

mov esi, kernel_name
mov ebx, 0x100000				; Load to offset 0x100000
call load_file

jc err							; Catch any error

mov si, DoneMsg
call simple_print				; Display done message

; *** Setup registers ***

cli
mov eax, cr0
or al, 1
mov cr0, eax
jmp 0x18:pmode32
bits 32
pmode32:
mov ax, 0x20
mov ds, ax
mov es, ax
mov ss, ax
mov fs, ax
mov gs, ax
mov esp, 0xeffff0

jmp 0x100000
bits 16

;Data

kernel_name		db 'phoenux.bin', 0x00
A20Msg			db 'Enabling A20 line...', 0x00
UnrealMsg		db 'Entering Unreal Mode...', 0x00
KernelMsg		db 'Loading kernel...', 0x00

;Includes

%include 'bootloader/includes/echfs.inc'
%include 'bootloader/includes/disk2.inc'
%include 'bootloader/includes/a20_enabler.inc'
%include 'bootloader/includes/gdt.inc'

times 4096-($-$$)			db 0x00				; Padding
