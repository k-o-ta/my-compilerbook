https://www.sigbus.info/compilerbook

 `docker build -t compilerbook https://www.sigbus.info/compilerbook/Dockerfile`
`docker run --rm -it -v $HOME/development/my-compilerbook:/my-compilerbook -w /my-compilerbook compilerbook
`
`gdb -tui --args ./9cc "a=1; if(a < 3){return a;}return a;"`


### binaryをdebug
`gdb tmp`
* 基本の始め方
```asm
set disassembly-flavor intel
layout asm
layout regs
break main
run
```

* スタックtopの値
```asm
x $rsp
```

* 任意のメモリの値
```asm
x $0x7ffd5e61dbc8
```
