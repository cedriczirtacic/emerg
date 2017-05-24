# emerg
## Purpose
Playing around with loadable kernel modules. Got the idea from @jarun keysniffer and @hephaest0s usbkill to make a tool that will be always runing in the background (from kernel-space) and when you type a "secret" phrase it will execute whatever was specified. 
_NOTE: this works with standard terminals (tty) not with pseudo (pty)._

## Usage
Is simple, actually, but you will need to have superuser access to the GNU/Linux machine.

Clone the code:
```bash
    $ git clone https://github.com/cedriczirtacic/emerg.git
    $ cd emerg/
```
Compile the module:
```bash
    $ make
```
Then install the kernel module:
```bash
    $ sudo insmod emerg.ko phrase="test" exec="/root/binary"
```
    
## Module parameters
The module has two parameters: one for the phrase (be careful with this one) and the second will be the path to an executable.  
  1. **phrase**: typing this phrase will be the trigger so *chose wisely*
  2. **exec**: file to be executed (it can be a binary or script, just `chmod +x` the thing...)

## Remove the module
Use the Makefile:
```bash
    $ make remove
```
