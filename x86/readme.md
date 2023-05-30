Assembler reference: [NASM](https://www.nasm.us/xdoc/2.15.05/html/nasmdoc0.html)
[NASM tutorial](https://cs.lmu.edu/~ray/notes/nasmtutorial/)
[Intel processor manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html)
[Maco-O executables](https://www.objc.io/issues/6-build-tools/mach-o-executables/)
Use of %rsp and %rbp in [stack frame layout](https://eli.thegreenplace.net/2011/09/06/stack-frame-layout-on-x86-64/)
[ABI](https://github.com/hjl-tools/x86-psABI/wiki/X86-psABI)
[Dive into systems](https://diveintosystems.org/book/)


## Utilities

Delete Mach-O executable files

```sh
find . -type f -perm +111 -exec sh -c 'file "{}" | grep -q Mach-O && rm "{}"' \;
```