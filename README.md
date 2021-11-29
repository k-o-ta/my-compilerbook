https://www.sigbus.info/compilerbook

 `docker build -t compilerbook https://www.sigbus.info/compilerbook/Dockerfile`
`docker run --rm -it -v $HOME/development/my-compilerbook:/my-compilerbook -w /my-compilerbook compilerbook
`
`gdb -tui --args ./9cc "a=1; if(a < 3){return a;}return a;"`


### binaryをdebug
gdb tmp
set disassembly-flavor intel
layout asm
layout regs
break foo
run
