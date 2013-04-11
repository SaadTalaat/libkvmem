# Libkvmem #
## Libkvmem, a port from BSD libkvm ##
---

Libkvmem provides an access to kernel virtual memory using kernel symbols

---

### Info ###

Providing access to kernel memory from kernel /dev/mem port and providing a method to get symbol addresses by name grant you an easier way to patch kernel from user space and enable the existence of user mode drivers despite the monolithic nature of linux kernel. Also, It provides a more sophisticated way to check on kernel integrity against rootkits that tend to hook linux main structures.

---

### Installation ###
* Building
    - `$ git clone https://github.com/SaadTalaat/Libkvmem.git`
    Download source package
    - `$ aclocal`
    Build local macros from configure.ac
    - `$ autoconf`
    Generate configuration files
    - `$ automake -a`
    Generate Makefile.in
    - `$ ./configure`
    execute the configure script
    - `$ make`
    Build library
    - `# make install`
    Moves library into system default library path for execution


---

### Notes ###

Library is still under development


---

## Author ##
Saad Talaat

[Mail][]

[Twitter][]

[Linkedin][]

[Mail]: mailto:saadtalaat@gmail.com
[Twitter]: http://twitter.com/sa3dtalaat
[Linkedin]: http://www.linkedin.com/in/saadtalaat
