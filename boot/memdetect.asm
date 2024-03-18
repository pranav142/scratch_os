[bits 16]

struc	MemoryMapEntry
	.baseAddress	resq	1	
	.length		resq	1	
  .type		resd	1	
	.acpi_null	resd	1	
endstruc

; 8000 tells where number of entries goess and addres 8004 is where entries start
; whenevr you change address you need to change it in the mem.c 
mmap_count equ 0x8000
do_e820:
	pushad
	xor	ebx, ebx
	xor	bp, bp			
  xor eax, eax
	mov	edx, 'PAMS'		
  mov edi, 0x8004
	mov	eax, 0xe820
	mov	ecx, 24			
	int	0x15			
	jc	.error	
	cmp	eax, 'PAMS'		
	jne	.error
	test	ebx, ebx		
	je	.error
	jmp	.start
.next_entry:
	mov	edx, 'PAMS'		
	mov	ecx, 24			
	mov	eax, 0xe820
	int	0x15			
.start:
	jcxz	.skip_entry		
.notext:
	mov	ecx, [es:di + MemoryMapEntry.length]	
	test	ecx, ecx		
	jne	short .good_entry
	mov	ecx, [es:di + MemoryMapEntry.length + 4]
	jecxz	.skip_entry		
.good_entry:
	inc	bp		
	add	di, 24			
.skip_entry:
	cmp	ebx, 0			
	jne	.next_entry		
	jmp	.done
.error:
	stc
.done:
  mov [mmap_count], bp
	popad
	ret
