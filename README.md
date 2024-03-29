# JSONparser
A C library to build parse tree for JSON grammar.

## Setup instructions
**Install dependencies :**
 * [Cutlery](https://github.com/RohanVDvivedi/Cutlery)
 * [ConnMan](https://github.com/RohanVDvivedi/ConnMan)

**Download source code :**
 * `git clone https://github.com/RohanVDvivedi/JSONparser.git`

**Build from source :**
 * `cd JSONparser`
 * `make clean all`

**Install from the build :**
 * `sudo make install`
 * ***Once you have installed from source, you may discard the build by*** `make clean`

## Using The library
 * add `-ljsonparser -lconnman -lcutlery` linker flag, while compiling your application
 * do not forget to include appropriate public api headers as and when needed. this includes
   * `#include<json_parser.h>`
   * `#include<json_serializer.h>`

## Instructions for uninstalling library

**Uninstall :**
 * `cd JSONparser`
 * `sudo make uninstall`
