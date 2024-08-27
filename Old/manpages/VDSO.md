Virtual Dynamic Shared Object<sup>vDSO</sup>
============================================

## NAME

vdso - overview of the virtual ELF dynamic shared object

## SYNOPSIS

```c
#include <sys/auxv.h>

void * vdso = (uintptr_t) getauxval(AT_SYSINFO_EHDR);
```

## DESCRIPTION

The Virtual Dynamic Shared Object<sup>vDSO</sup> is a small shared library that the kernel automatically maps into the address space of all user space application. Applications usually do not need to concern themselves with these details as the Virtual Dynamic Shared Object<sup>vDSO</sup> is most commonly called by the C library. This way you can code in the normal way using standard functions and the C library will take care of using any functionality that is available via the Virtual Dynamic Shared Object<sup>vDSO</sup>.

Why does the Virtual Dynamic Shared Object<sup>vDSO</sup> exist at all? There are some system calls the kernel provides that user space ends up using frequently to the point that such calls can dominate overall performance. This is due both to the frequency of the call as well as the context-switch overhead that results from existing user space and entering the kernel.

The rest of this documentation is geared toward the curious and/or C library writers rather than general developers. If you're trying to call the Virtual Dynamic Shared Object<sup>vDSO</sup> in your own application rather than using the C library, you're most likely doing it wrong.

### Example background

Making system calls can be slow. In x86 32 bit systems, you can trigger a software interrupt (`int $0x80`) to tell the kernel you wish to make a system call. However, this instruction is expensive: it goes through the full interrupt handling paths in the processor's microcode as well as in the kernel. Newer processors have faster (but backward incompatible) instructions to initiate system calls. Rather than require the C library to figure out if this functionality is available at run time, the C library can use functions provided by the kernel in the Virtual Dynamic Shared Object<sup>vDSO</sup>.

Note that the terminology can be confusing. On x86 systems, the Virtual Dynamic Shared Object<sup>vDSO</sup> function used to determine the preferred method of making a system call is named `__kernel_vsyscall`, but on x86-64, the term "vsyscall" also refers to an obsolte way to ask the kernel what time it is or what CPU the caller is on.

One frequently used system call is `gettimeofday`. This system call is called both directly by user space application as well as indirectly by the C library. Think timestamps or timing loops or polling all of these frequently need to know what time it is right now. This information is also not secret any application in any privilege mode (root or any unprivileged user) will get the same answer. Thus the kernel arranges for the information required to answer this question to be placed in memory the process can access. Now a call to `gettimeofday` changes from a system call to a normal function call and a few memory accesses.

### Findning the Virtual Dynamic Shared Object<sup>vDSO</sup>

The base address of the Virtual Dynamic Shared Object<sup>vDSO</sup> (if one exists) is passed by the kernel to each program in the initial auxiliary vector (see `getauxval(3)`), via the `AT_SYSINFO_EHDR` tag.

You must not assume the Virtual Dynamic Shared Object<sup>vDSO</sup> is mapped at any particular location in the user's memory map. The base address will usually be randomized at run time every time a new process image is created (at `execve(2)` time). This is done for security reasons, to prevent "return-to-libc" attacks.

For some architectures, there is also an `AT_SYSINFO` tag. This is used only for locating the vsyscall entry point and is frequently omitted or set to 0 (meaning it's not available). This tag is a throwback to the initial Virtual Dynamic Shared Object<sup>vDSO</sup> work (see History below) and its use should be avoided.

### File Format

Since the Virtual Dynamic Shared Object<sup>vDSO</sup> is a fully formed ELF image, you can do symbol lookups on it. This allows new symbols to be added with newer kernel releases, and allows the C library to detect available functionality at run time when running under different kernel versions. Oftentimes the C library will do detection with the first call and then cache the result for subsequent calls.

All symbos are also versioned (using the GNU version format). This allows the kernel to update the function signature without breaking backward compatiblity. This means changing the arguments that the function accepts as well as the return value. Thus, when looking up a symbol in the Virtual Dynamic Shared Object<sup>vDSO</sup>, you must always include the version to match the ABI you except.

Typically the Virtual Dynamic Shared Object<sup>vDSO</sup> follows the naming convention of prefixing all symbols with `__vdso__` or `__kernel__` so as to distinguish them from other standard symbols. For example, the "gettimeofday" function is named `__vdso_gettimeofday`.

You use the standard C calling conventions when calling any of these functions. No need to worry about weired register or stack behavior.

## NOTES

### Source

When you compile the kernel, it will automatically compile and link the Virtual Dynamic Shared Object<sup>vDSO</sup> code for you. You will frequently find it under the architecture specific directory:

```sh
$ find arch/$ARCH/ -name '*vdso*.so*' -o -name '*gate*.so*'
```

