# Introduction To Software Security

**What is the definition of Software Security?**  
Software security is a subset of computer security, that focuses on the secure design and implementation of software. 

**What differentiate it from other subset of computer security?**  
In SS the focus of the study is *the code*, rather than treating the code as a black box. While other subset of computer security, such as OS security, network security or device security tools that focus on external controls and policies

**What is the definition of OS security?**  
As we know Operating Systems mediate a program's actions through *system calls*, OS security enforces policies that control the actions the OS performs on behalf of programs, for example ensuring that programs run by Alice cannot read files owned by Bob etc..  

**What are the limitations of OS security?**  
OS security cannot:
- enforce application-specific policies (only a bank manager can approve a loan)
- enforce information flow policies (preventing data from being leaked indirectly through covert channels)
- protect against vulnerabilities within application code itself

**What about device security, can it be sufficient?**  
Firewalls (coarse-grained filtering) and IDSs (fine-grained filtering) observe, block and filter messages exchanged by programs based on certain parameters. This defence systems are not perfect and an attacker can still find a workaround to bypass them.  
Antivirus looks for sign of malicious behavior in local files, but in practice are frequently bypassed

**Give and example that sustain what said until now:**  
Heartbleed is a bug in the OpenSSL implementation of SSL/TLS (core protocol for encrypted communications used by the web). A carefully crafted packet causes OpenSSL to read and return portion of a vulnerable server's memory (leaking passwords, keys, etc). In this case we can notice that black box security is incomplete against Earthbleed exploits, and SS methods attack the source of the problem: the buggy code
