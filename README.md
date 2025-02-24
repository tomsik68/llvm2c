# llvm2c

Translation of LLVM bitcode to C

## Dependencies

The only requirements are cmake in version at least 2.8 and LLVM 6 or 7.
The project should be compilable also on older LLVM's, but it is not tested on these versions (patches are welcome :)

## Building

```
git clone https://github.com/staticafi/llvm2c
cd llvm2c
mkdir build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=installation/path/
make
```
    
## Testing

Copy the built `llvm2c` binary into test directory and run `./run` script

## Unsupported features

- vector instructions
- atomic operations
- some special intrinsics
- the code generation is currently fitted to x86_64 bitcode

## Authors

* **Petr Vitovský** - [petrv7](https://github.com/petrv7)
* **Tomáš Jašek** - [tomsik68](https://github.com/tomsik68)
* **Marek Chalupa** - [mchalupa](https://github.com/mchalupa)
