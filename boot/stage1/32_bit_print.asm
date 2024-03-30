[ bits 32]

VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f
; prints a null - terminated string pointed to by EDX

print_string_pm:
    pusha
    mov edx , VIDEO_MEMORY ; Set edx to the start of vid mem.

print_string_pm_loop:
    ; ebx stores the pointer to the beggining of the string you want to display

    ; Loading the character from string
    mov al , [ ebx ] ; The character we want to display
    mov ah , WHITE_ON_BLACK ; attrribute of the character
    cmp al , 0 ; if (al == 0) , at end of string , so
    je print_string_pm_done ; jump to done

    ; storing character and attributed in video memory
    mov [edx] , ax ; Store char and attributes into the video memory

    ; incrementing through string and video memory
    add ebx , 1 ; move to the next byte in the string
    add edx , 2 ; move to next character cell by 2 bytes in video memory
    jmp print_string_pm_loop ; loop around to print the next char.

print_string_pm_done:
    popa
    ret ; Return from the function

clear_screen_pm:
    pusha 
    mov edx, VIDEO_MEMORY
    mov ebx, 0     ; counter
    mov ecx, 80*25 ; number of characters 
    mov ax, 0x0720 ; blank space character

clear_screen_loop:
    mov [edx] , ax ; draw blank space character
    add ebx, 1
    add edx, 2

    cmp ebx, ecx 
    je clear_screen_done
    jmp clear_screen_loop
    
clear_screen_done:
    popa 
    ret