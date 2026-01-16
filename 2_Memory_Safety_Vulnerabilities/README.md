# 2. Memory Safety Vulnerabilities

### Contents
1. Memory layout refresh
2. Buffer Overflow Stack-based

## 2.1 Memory Layout Refresh
**How the memory of a compiled program is organized?**  
In linux 1GB is always reserved for the OS (0xc0000000 to 0xffffffff) -> 3GB usable address space (0x00000000 - 0xbfffffff) for compiled programs.  
A compiled program's memory is divided into five segments:
- Text or Code segment
- Data segment
- BSS segment
- Heap segment
- Stack segment

**What the text/code segment contains?**  
The *Text / Code* section is where the assembled machine language instruction of the program are located.  
Because the execution of instructions is nonlinear (jumps are performed), as the program execute the RIP (Instruction Pointer Register) is set to the fist instruction in the text segment.  
The processor follow this execution loop:
1. Read the instruction that the RIP is pointing to 
2. Adds the byte length of the instruction to RIP 
3. Execute the instruction read in step 1
4. Go back to step 1.  

The text segment doesn't have write permission and has a fixed size, any attempt to write the text segment will send an alert and kill the process. (in this way we can also share the memory, thus have more code instances without occupy to much memory).  

**What the data segment contains?**  
The data segment is filled with the initialized global and static variables, is writable and with a fixed size.

**What the BSS segment contains?**  
The BSS segment is filled with the uninitialized variables, is writable and with a fixed size.

**What the heap segment contains?**  
The heap segment is a segment of memory that a programmer can directly control, using functions that allocate and deallocate blocks of memory of variable sizes (malloc, calloc, realloc, free). It isn't fixed, and the growth of the heap moves upwards toward higher memory addresses

**What the stack segment contains?**  
The stack segment also has a variable size (but with a size limit) and is used as a temporary scratch pad to store local function variables and context during function calls.  
The function's code will be at a different memory location in the text segment, since the context and RIP must change when a function is called the stack is used to remember all of the passed variables, the location the RIP should return to after the function is finished and all the local variables used by that function. All this information is stored together on the stack in what is called the stack frame. The stack contains many stack frames.  
It grow downward toward lower memory addresses.

ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
**What are the assembly instructions critical to understand the function call mechanism?** 
The stack work primarily with two registers: the RSP (Register Stack Pointer), known as stack pointer, is used to keep track of the address of the end of the stack, which changes as items are pushed and popped. And the RBP (Register Base Pointer), also called as the frame pointer, is used to reference local function variables in the current stack frame
- CALL: save the address of the instruction pointer and push addr
- PUSH:
- POP: in the stack there isn't a deletion of the data when the pop is used.It just change the value stored in the SP register
- RET: set the IP as SP (previously saved IP)

**What is contained into a stack frame?**  
- the local variables of the function, in the same order as they appear in the code (The local variable allocation is ultimately up to the compiler: Variables could be allocated in any order, or not allocated at all and stored only in registers, depending on the optimization level used.)
- the saved frame pointer, is used to restore RBP to its previous value
- the return address, is used to restore RIP to the next instruction found after the function call, restoring the functional context of the previous stack frame
- the argument pushed to the function in reverse order of ode (LIFO)

**Explain what happens in this code snippet:**  
```c
void test_function(int a, int b, int c, int d){
    int flag;
    char buffer[10];

    flag = 31337;
    buffer[0] = 'A';

    return;
}
int main(){
    test_function(1, 2, 3, 4);

    return EXIT_SUCCCESS;
}
```
```
Dump of assembler code for function main:
   0x0000000000001149 <+0>:	endbr64                 
   0x000000000000114d <+4>:	push   rbp              ;prologue
   0x000000000000114e <+5>:	mov    rbp,rsp          ;prologue
   0x0000000000001151 <+8>:	mov    ecx,0x4
   0x0000000000001156 <+13>:	mov    edx,0x3
   0x000000000000115b <+18>:	mov    esi,0x2
   0x0000000000001160 <+23>:	mov    edi,0x1
   0x0000000000001165 <+28>:	call   0x1171 <test_function>
   0x000000000000116a <+33>:	mov    eax,0x0
   0x000000000000116f <+38>:	pop    rbp
   0x0000000000001170 <+39>:	ret
Dump of assembler code for function test_function:
   0x0000000000001171 <+0>:	endbr64 
   0x0000000000001175 <+4>:	push   rbp              ;prologue
   0x0000000000001176 <+5>:	mov    rbp,rsp          ;prologue
   0x0000000000001179 <+8>:	sub    rsp,0x30         ;prologue
   0x000000000000117d <+12>:	mov    DWORD PTR [rbp-0x24],edi
   0x0000000000001180 <+15>:	mov    DWORD PTR [rbp-0x28],esi
   0x0000000000001183 <+18>:	mov    DWORD PTR [rbp-0x2c],edx
   0x0000000000001186 <+21>:	mov    DWORD PTR [rbp-0x30],ecx
   0x0000000000001189 <+24>:	mov    rax,QWORD PTR fs:0x28
   0x0000000000001192 <+33>:	mov    QWORD PTR [rbp-0x8],rax
   0x0000000000001196 <+37>:	xor    eax,eax
   0x0000000000001198 <+39>:	mov    DWORD PTR [rbp-0x18],0x7a69
   0x000000000000119f <+46>:	mov    BYTE PTR [rbp-0x12],0x41
   0x00000000000011a3 <+50>:	nop
   0x00000000000011a4 <+51>:	mov    rax,QWORD PTR [rbp-0x8]
   0x00000000000011a8 <+55>:	sub    rax,QWORD PTR fs:0x28
   0x00000000000011b1 <+64>:	je     0x11b8 <test_function+71>
   0x00000000000011b3 <+66>:	call   0x1050 <__stack_chk_fail@plt>
   0x00000000000011b8 <+71>:	leave  
   0x00000000000011b9 <+72>:	ret
```
When the program is run, the main() function is called
The first few instructions of each function set up the stack frame and are called *function prologue*  
The main() function simply calls test_function()
Then the various values are pushed to the stack to create the start of the stack frame as follow:
- function arguments are pushed to the stack in reverse order (since FILO) 

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
