import os

ASM_FILE = "./kernel/isr.inc"
C_FILE = "./kernel/isr_gen.c"

interrupts_with_error = {8, 10, 11, 12, 13, 14, 17}

def define_assembly_interrupts(output_file, interrupts_with_error):
    with open(output_file, "w") as file:
        file.write(f"; This is a Automatically Generated File\n")
        for i in range(256): 
            if i in interrupts_with_error: 
                file.write(f"ISR_ERRORCODE {i}\n")
            else:
                file.write(f"ISR_NOERRORCODE {i}\n")

def define_c_initialization_interrupts(output_file):
    with open(output_file, "w") as file:
        file.write(f"// This is a Automatically Generated File\n")
        file.write(f'#include "idt.h"\n\n')

        for i in range(256):
            file.write(f"extern void __attribute__((cdecl)) ISR{i}();\n")
        file.write("\n")
        file.write("void ISR_InitializeGates() {\n")
        for i in range(256):
            file.write(f"\tIDT_SetGate({i}, ISR{i}, CODE_SEGMENT, IDT_FLAG_GATE_32_INT | IDT_FLAG_RING_0);\n")
        file.write("}")
    
def main():
    define_assembly_interrupts(ASM_FILE, interrupts_with_error)
    define_c_initialization_interrupts(C_FILE)


if __name__ == "__main__":
    main()
    
