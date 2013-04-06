# Libkvmem #
## Libkvmem, a port from BSD libkvm ##
---

Libkvmem provides an access to kernel virtual memory using kernel symbols

### Info ###
---

Providing access to kernel memory from kernel /dev/mem port and providing a method to get symbol addresses by name grant you an easier way to patch kernel from user space and enable the existence of user mode drivers despite the monolithic nature of linux kernel. Also, It provides a more sophisticated way to check on kernel integrity against rootkits that tend to hook linux main structures.

### Installation ###
---
* Building
    - `$ git clone https://github.com/SaadTalaat/Libkvmem.git`
    download source package
    - `$ make`
    build library


### Notes ###
---

Library is still under development



## Author ##
Saad Talaat
[Mail]: mailto://saadtalaat@gmail.com
[Twitter]: http://twitter.com
[Linkedin]: http://www.linkedin.com/in/saadtalaat
