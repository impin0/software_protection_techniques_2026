# 2. Memory Safety Vulnerabilities

### Contents
1. Memory layout refresh
2. 

## 2.1 Memory Layout Refresh
**How memory is organized? What are the main sections?**  
32 bit architecture (Intel) -> 4GB of address space
In linux 1GB is reserved for the OS (0xc0000000 to 0xffffffff) -> 3GB usable address space (0x00000000 - 0xbfffffff)

- *Text / Code* section: is where the assembled machine language instruction of the program are located. Because the execution of instructions is nonlinear (jumps are performed) as the program execute the EIP (Register Instruction Pointer) is set to the fist instruction in the text segment. The processor follow this execution loop (1) Read the instruction that the EIP is pointing to. (2) Adds the byte length of the instruction to EIP. (3) Execute the instruction read in step 1. (4) Go back to step 1. (the processor it's expecting the execution non linear, so if there is a jump doesn't matter).  
The text segment doesn't have write permission, any attempt to write the text segment will send an alert and kill the process. (in this way we can also share the memory, thus have more code instances without occupy to much memory). It also have a fixed size
- Data: is filled with the initialized global and static variables, is writable but with a fixed size
- Bss: static memory, filled with the uninitialized variables, is writable but with a fixed size
- Heap: dynamic memory, portion of memory where the programmer has the control (malloc, calloc, free). Grow from higher memory address towards lower memory addresses
- Stack: dynamic memory, portion of memory used by the program to execute functions etc. Grow from lower memory address towards higher memory addresses
- command line + enviroment

**How the stack works?**  
The OS set a maximum size for the stack. 
SP: Stack Pointer, an hardware register that store the higher not used (empty) address of the stack
Operation permmitted: PUSH & POP
In the stack there isn't a deletion of the data when the pop is used.It just change the value stored in the SP register

CALL:
- save the address of the instruction pointer
- push addr

PUSH:

POP:

RET: 
- set the IP as SP (previously saved IP)

Into the stack is saved:
- function parameters
- local variables
- the return address (next memory address of the Instruction Pointer 
The stack frame is the whole function arguments saved for a function

## 2.2 Buffer Overflow Stack-based
**What is the basic idea that an attacker can utilize?**

**What is the objective of the attacker?**  
1. To overwrite the return address, in a useful way, by inserting a different return address. By performing this operation we can modify the flow of the program (Control Flow Hijacking)
2. Create a program to inject in another program (known as shell code)
3. Create the injection code, to insert the program written in the previous phase
4. Modify the instruction pointer, by modifying the return address

Injection Vector, is the input sent to the program


``` c
void function(char *input, int value){
    char buffer[20];

    strcpy(buffer, input);

    return;
}
```
**How the injection vector is written?**  
The sum of the shell code + padding must be equal to the size of the buffer. After the shell code + padding we put the address of the shellcode
