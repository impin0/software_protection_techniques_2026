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

**What is contained in the text/code segment?**  
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
The heap segment is a segment of memory that a programmer can directly control, using functions that allocate and deallocate blocks of memory of variable sizes (malloc, calloc, realloc, free). It doesn't have a fixed size, and the growth of the heap moves upwards toward higher memory addresses

**What the stack segment contains?**  
The stack segment also has a variable size (but with a size limit) and is used as a temporary scratch pad to store local function variables and context during function calls.  
The function's code will be at a different memory location in the text segment, since the context and RIP must change when a function is called the stack is used to remember all of the passed variables, the location the RIP should return to after the function is finished and all the local variables used by that function. All this information is stored together on the stack in what is called the stack frame. The stack contains many stack frames.  And it grow downward toward lower memory addresses.

**What are the assembly instructions critical to understand the function call mechanism?** 
The stack work primarily with two registers: the RSP (Register Stack Pointer), known as stack pointer, is used to keep track of the address of the end of the stack, which changes as items are pushed and popped. And the RBP (Register Base Pointer), also called as the frame pointer, provide a fixed reference point within the current function's stack frame, set at the beginning of the function and remains unchainged during xecution, allowing access to local variables and parameters through constant offsets
- PUSH: decrees the stack pointer and write the value at the top of the stack
- POP: read the value at the top of the stack and increment the stack pointer (no deletion occurs)
- CALL: perform a PUSH of the return address and jump to the beginning of the function called
- RET: perform a POP into RIP, restoring the return address from the top of the stack

**What is contained into a stack frame?**  
- the local variables of the function
- the saved frame pointer, is used to restore RBP to its previous value
- the return address, is used to restore RIP to the next instruction found after the function call, restoring the functional context of the previous stack frame
- the argument pushed to the function in reverse order of code (LIFO)

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
When the program is run, the main() function is called:
1. The first few instructions of each function are called *function prologue*, they set up the stack frame by saving the FP on the stack and save stack memory for the local function variables.    
The main() function simply calls test_function()  
2. When the `test_function()` is called from the `main()` function, the various values are pushed to the stack to create the start of the stack frame, the function arguments are pushed onto the stack in reverse order.  
3. Next when the assembly `call` instruction is executed the return address is pushed onto the stack and the execution flow jumps to the start of the `test_function()` at `0x00..001171`, the return address value will be the location of the instruction following the current RIP, in this case the `mov` instruction in `main()`. The `call` also jumps to the first instruction of `test_function()` where the function prologue will execute and finish building the stack frame. In this step, the current value of RBP is pushed to the stack, this value called SFP is later used to restore RBP back to its original state. The current value of the RSP is then copied into RBP to set the new frame pointer
4. The stack frame look like:
```
high addresses  +-----------------------------------+
                |    d                              |
                |    c                              |
                |    b                             |
                |    a                              |
                |    Return Address (ret)           |
                |    Saved Frame Pointer (SFP / BP) |
                |    flag                           |
                |    buffer                         |
                +-----------------------------------+
low  addresses
```
5. After the execution finishes, the entire stack frame is popped off the stack, the RIP set to the return address so the program continue execution

## 2.2 Buffer Overflow Stack-based
**What is exploiting?**  
Exploiting a program is a clever way of getting the computer to do what you want it to do, even if the currently running program was designed to prevent that action, leveraging some holes created by the programmer

**What is the objective of the attacker?**  
The ultimate goal is to take control of the target program's execution flow by tricking into running a piece of malicious code that has been smuggled into memory

**What are buffer overflow vulnerabilities?**  
C assumes the programmer is responsible for data integrity, this increase the programmer's control and the efficiency of the programs, but also result in programs that are vulnerable to buffer overflows and memory leaks, this means that once a variable is allocated memory, there are no built in safeguards to ensure that the contents of a variable fit into the allocated memory space.  
Programs crashes are annoying, but in the hands of a hacker they can become dangerous, he can take control of the program as it crashes. 

**How a stack-based buffer overflow vulnerabilities works?**  
Programs are exploited by corrupting memory to control execution flow. In all C programs there is an execution control point, that is located after all the stack variables, the saved return address. Modifying the return address we can change the execution flow of a program!

**How can we determine where the return address is?**  
By understanding how the stack frame is created and experimenting with the program.  
Another option is to repeat the return address multiple times.

### 2.2.1 BASH & Perl
{
**How can we experiment quickly?**  
By using Perl in combination with the BASH:
Perl is a programming language with a `print` command that can generate long sequence of characters + it can be used to execute instruction on the command line by using the `-e` switch like this: `perl -e 'print "A" x 20;'`.  
- We can print any character (even non-printable ones) by using `\x##` where `##` is the hexadecimal value of the character `perl -e 'print "\x41" x 20;'` is the same as the one above. 
- We can perform string concatenation in Perl using the period `.`: `perl -e 'print "A"x20 . "BCD" . "\x61\x66\x67\x69"x2 . "Z";'` -> `AAAAAAAAAAAAAAAAAAAABCDafgiafgiZ`

An entire shell command can be executed like a function, by surrounding the command with parentheses and prefixing a dollar sign: `$(perl -e 'print "uname";')` or `una$(perl -e 'print "m";')e`. In each cases the output of the command is substituted for the command, and the command executed
}

**What is the next step after we know the distance between the input variable and the return address?**  
By knowing the distance between the variable that we can insert our input and the variable that contains the return address we can insert any data that we want in there. Remember that if you are working on a little endian architecture you must write those bytes into memory in reverse order: we want `oxdeadbeef` we type `"x\efx\be\xad\xde"`. This technique can be applied to overwrite the return address with an exact value, this will limit us to use instructions that exist in the original program

**How can we execute instruction that are not in the original program?**  
We need to use a shellcode, are instructions that are injected into memory and then return execution there, those instruction tell the program to restore privileges and open a shell prompt

**What is another problem of performing a shellcode?**  
We must known in advance the actual memory address of the shellcode, this can be difficult in a dynamic environment like the stack. Fortunately there is an hacking technique called NOP sled that can help us. NOP is an assembly instruction (`0x90` in x86 archetecture)that is short for No Operation, and does absolutely nothing. We can use those NOP instruction by creating an array of NOP and place it before the shellcode, then the RIP will point to any address found in the NOP sled, execute each instruction (increment the RIP) until it reaches the shelcode
```
+-------------------------------------------------+
| NOP sled | shell code | repeated return address |
+-------------------------------------------------+
```
