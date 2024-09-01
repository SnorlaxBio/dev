SpookyHash: a 128 bit noncrypographic hash
==========================================

SpookyHash is a public domain noncryptogrphic hash function producing well-distributed 128 bit hash values for byte arrays of any length. It can produce 64 bit and 32 bit hash values too, at the same speed, just use the bottom n bits. The C++ reference implementation is specific to 64 bit x86 platforms, in particular it assumes the processor is little endian. Long keys hash in 3 bytes per cycle, short keys take about 1 byte per cycle, and there is a 30 cycle startup cost. Keys can be supplied in fragments. The function allows a 128 bit seed. It's named SpookyHash because it was released on Halloween.

C++ code for SpookyHash V1 (groundhog's day 2012):

- [spooky.h](https://www.burtleburtle.net/bob/c/spooky.h)
- [spooky.cpp](https://www.burtleburtle.net/bob/c/spooky.cpp)
- [testspooky.cpp](https://www.burtleburtle.net/bob/c/testspooky.cpp)

C++ code for SpookyHash V2 (labor day 2012):

- [SpookyV2.h](https://www.burtleburtle.net/bob/c/SpookyV2.h)
- [SpookyV2.cpp](https://www.burtleburtle.net/bob/c/SpookyV2.cpp)
- [TestSpookyV2.cpp](https://www.burtleburtle.net/bob/c/TestSpookyV2.cpp)

Both V1 and V2 pass all the tests. V2 corrects two oversights in V1:

1. In the short hash, there was a d = length thath should have been d += length, which means some entropy got dropped on the floor. It passed the test anyhow, but fixing this probably means more distinct info from the message makes it into the result.
2. The long hash always ended in mix() + end(), but only end() was needed. Removing the extra call to mix() makes all long hashes faster by a small constant amount.

These changes are not backwards compatible: messages of all lengths will get different hash values from V1 and V2. The interface is unchanged, though. Both the short and long V2 hash were tested by froggy out to 2<sup>73</sup> key pairs.

Why use SpookyHash?

- It's fast. For short keys it's 1 byte per cycle, with a 30 cycle startup cost. For long keys, well, it would be 3 bytes per cycle, and that only occupies one core. Except you'll hit the limit of how fast uncached memory can be read, which on my home machines is less than 2 bytes per cycle.
- It's good. It achieves avalance for 1 bit and 2 bit inputs. It works for any type of key that can be made to look like an array of bytes, or list of arrays of bytes. It takes a seed, so it can produce many different independent hashes for the same key.
- It can produce up to 128-bit results. Large systems should consider using 128-bit checksums nowadays. A library with 4 billion documents is expected to have about 1 colliding 64-bit checksum no matter how good the hash is. Libraries using 128-bit checksums should expect 1 collision once they hit 16 quintillion documents. (Due to limited hardware, I can only verify SpookyHash is as good as a good 73-bit checksum. It might be better, I can't tell.)
- When not to use it: if you have an opponent. This is not cryptographic. Given a message, a resourceful opponent could write a tool that would produce a modified message with the same hash as the original message. Once such a tool is written, a not-so-resouceful opponent could borrow the tool and do the same thing.
- Another case not to use it: CRC have a nice property that you can split a message up inot pieces arbitrarily, calculate the CRC all the pieces, then aftewards combine the CRC for the pieces to find the CRC for the concatenation of all those pieces. SpookyHash can't. If you could deterministically choose what the pieces were, though, you could compute the hashes for pieces with SpookyHash (or [CityHash](https://github.com/google/cityhash) or any other hash.), then treat those hash values as the raw data, and do CRC on top of that.
- When not to use it: if you have an opponent. This is not cryptographic. Given a message, a resourceful opponent could write a tool that would produce a modified message with the same hash as the original message. Once such a tool is written, a not-so-resourceful opponent could borrow the tool and do the same thing.
- Big-endian machines aren't supported by the current implementation. The hash would run, and it would produce equally good results, but they'd be different results from little-endain platforms. Machines that can't handle unaligned reads also won't work by default, but there's a macro in the implementation to tweak that will let it deal with unaligned reads. x86 compatible machines are little endian and support unaligned reads.

For longh keys, the inner loop of SpookyHash is Spooky::Mix(). It consumes an 8 byte input, then does and xor, another xor, a rotation, and an addition. The internal state won't entirely fit in registers after 3 or 4 7 byte variables. But parallelism keeps increasing, and so does avalanche per pass. There was a sweet spot around 12 variables.

My previous hashes <sub>([lookup2.c](https://www.burtleburtle.net/bob/c/lookup2.c), [lookup3.c](https://www.burtleburtle.net/bob/c/lookup3.c))</sub> would have added 12 8 byte input to the 12 variables, then done a few passes through all the variables to mix them well, then added the next batch of inputs. That is tested by [avalanche.html](https://www.burtleburtle.net/bob/hash/avalanche.html). Unlike my previous hashes, Spooky::Mix() is trickle-feed. It adds an input to one variable, does a little mixing, then adds data to the next variable. There is no bulk mixing state in between groups of 12 variables; the processing treats all variables symmetically. Trickle-feeding should allow chips to keep a static workload, doing a steady propotion of memory fetches, additions, XOR, and rotates simultaneously.

Testing a trickle-feed mix was a new problem. As always, it has to work equally well when run forward or backwards. As always every input bit has to produce 128 bits of apparent entropy before all those bits could be canceled by other input bits. (128 bits, because the hash produces a 128 bit result.) My handwave is that after 128 byte inputs, there has been as much input as there is internal state, so anything could be overwritten. So if a delta appears to produce 128 bits of entropy by 128 byte inputs after delta starts, that's good enough. I wrote [screen.cpp](https://www.burtleburtle.net/bob/c/screen.cpp) to check for that.

I tried SSE2 instructions, and 64 bit multiplications, but it turned out that plain 64 bit rotate, addition, and XOR got results faster than those. I thought 4 or 6 variable schemes were going to win, but 12 variables won in the end. My boss suggests I look into GPU. I haven't tried that yet. But given that the memory bandwidth is maxed out, I doubt they would help.

While Spooky::Mix() handles the middle of long keys well, it would need 4 repetitions for the final mix; and it has a huge startup cost. That would make short keys expensive. So I found the ShortHash to produce a 128 bit hash of short keys with little startup cost, and Spooky::End() to reduce the final mixing cost (this shows up most for keys just over 192 bytes long). Those components aren't trickle-feed, they work the old way. ShortHash is used automatically for short keys.

My [forg test](https://www.burtleburtle.net/bob/c/froggy.cpp) is the most stringent test I have for 64 bit and 128 bit noncryptographic hashes. It hashes n byt keys that are all zero except for m bits set, for all possible choices of m bits, and looks for collisions. Once it maxes out memory it keeps as many hashes as it can and continues chruning through keys. Ideally you'd test a 128 bit hash by checking 2<sup>128</sup> key-pairs, which should produce about 1 collision if the function is dadequately random. That's way beyound my means. 2<sup>72</sup> key paris is about the limit of my testing abilities.

