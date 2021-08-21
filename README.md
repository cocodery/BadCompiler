# BadCompiler
a bad compiler which have front-end can only be used to cope with curriculum design.

It can generate token-list, quaternion of a piece or whole program in SysY, a C-like programming-language.

## Compiling

```bash
mkdir build && cd build
cmake ..
make -j 4
./compiler -S -o test.s test.sy
```


and then it will product two new file called 123.tk, 123.qt, which include token-list and quaternion.
