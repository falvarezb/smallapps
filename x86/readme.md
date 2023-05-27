Assembler reference: [NASM](https://www.nasm.us/xdoc/2.15.05/html/nasmdoc0.html)


## Utilities

Delete Mach-O executable files

```sh
find . -type f -perm +111 -exec sh -c 'file "{}" | grep -q Mach-O && rm "{}"' \;
```