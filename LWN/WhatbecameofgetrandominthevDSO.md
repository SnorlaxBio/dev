What became of `getrandom()` in the virtual Dynamic Shared Object <sup>vDSO</sup>
=================================================================================

> By Jonathan Corbet
>
> July 25, 2024

In the previous episod of `vgetrandom()` story. Jason Donenfeld had put together a version of the `getrandom()` system call that ran in user space, significantly improving performance for applications that need a lot of random data while retaining all of the guarantees provided by the system call. At that time, it seeded that a consensus had built around the implementation and that it was headed toward the mainline in that form. A few milliseconds after that article was posted, though, a Linux Torvalds shaped obstracle appeared in its path. That ostacle has been overcome and this work has now been merged for the 6.11 kernel, but its form has changed somewhat.
