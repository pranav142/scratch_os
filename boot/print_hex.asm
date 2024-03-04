; a utility function to help debug the OS

print_hex:
    ; load dx with your hex value
    ; We have a global variable HEX_MSG filled with 0's and we fill these values 
    pusha  
    mov bx, HEX_MSG ; load the address of the message into bx            
    mov cl, 12 ; store bit shift by 12

    mov si, bx ; pointer we use to iterate through HEX_MSG
    add si, 2 ; ignores the 0x in the message

    hex_loop:
        mov ax, dx                  ; Move the value from DX to AX so we don't corrupt input data
        shr ax, cl                  ; bit shift the values for the first iteration bit shift by 12
        sub cl, 4                   ; for every next digit bit shift by 4 and 4 less to put desire num at end
        and ax, 0x0F                ; Mask AX to get only the last nibble
        
        cmp ax, 9                   ; if a digit convert to ascii by adding '0'
        jle is_digit
        add ax, 'A' - 10            ; if a letter convert to ascii by adding 'A' - 10
        jmp store_char

        is_digit:
            add ax, '0'
        store_char:
            cmp byte [si], 0        ; if si points to null terminator print the string and stop execution
            je print                
            mov [si], al            ; if not move the value we calculated (in ax but we want lower byte) and store at address in msg
            inc si                  ; inc si to go to the next value in our message
            jmp hex_loop            ; continues loop until we reach terminator of string
        print:
            call print_string
            popa
            ret
    
   
HEX_MSG:
    db '0x0000', 0
