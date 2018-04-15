I'm learning to create an OS from various online resources, like wiki.osdev.org

Pre-requisites:

A GNU/Linux or Cygwin environment.
A freestanding toolchain for compiling (https://wiki.osdev.org/Bare_Bones)
grub-mkrescue and xorriso programs
qemu for i386.

Just give executable permissions for the bash script compile.sh (Linux command "chmod +x compile.sh").
After creating the tool chain (see above) run compile.sh to build the iso.
The iso will be located in builds/ directory.
