#!/bin/bash
assert() {
  expected="$1" input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s call.o
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0  'int main() {return 0;}'
assert 42 'int main() {return 42;}'
assert 21 'int main() {return 5+20-4;}'
assert 41 'int main() {return  12 + 34 - 5;}'
assert 47 'int main() {return 5+6*7;}'
assert 15 'int main() {return 5*(9-6);}'
assert 4  'int main() {return (3+5)/2;}'
assert 10 'int main() {return -10+20;}'
assert 10 'int main() {return - -10;}'
assert 10 'int main() {return - - +10;}'

assert 0 'int main() {return 0==1;}'
assert 1 'int main() {return 42==42;}'
assert 1 'int main() {return 0!=1;}'
assert 0 'int main() {return 42!=42;}'

assert 1 'int main() {return 0<1;}'
assert 0 'int main() {return 1<1;}'
assert 0 'int main() {return 2<1;}'
assert 1 'int main() {return 0<=1;}'
assert 1 'int main() {return 1<=1;}'
assert 0 'int main() {return 2<=1;}'

assert 1  'int main() {return 1>0;}'
assert 0  'int main() {return 1>1;}'
assert 0  'int main() {return 1>2;}'
assert 1  'int main() {return 1>=0;}'
assert 1  'int main() {return 1>=1;}'
assert 0  'int main() {return 1>=2;}'
assert 1  'int main() {return 1; 2; 3;}'
assert 3  'int main() {int a; a=3; return a;}'
assert 4  'int main() {int a; a=3; return a+1;}'
assert 21 'int main() {int a; int b; int c; a=5;b=20;c=4;return a+b-c;}'
assert 41 'int main() {int a; int b; int c; a=12;b=34;c=5; return  a + b - c;}'
assert 3  'int main() {int foo; foo=3; return foo;}'
assert 4  'int main() {int bar; bar=3; return bar+1;}'
assert 21 'int main() {int foo; int bar; int baz; foo=5;bar=20;baz=4;return foo+bar-baz;}'
assert 41 'int main() {int foo; int bar; int baz; foo=12;bar=34;baz=5; return foo + bar - baz;}'
assert 12 'int main() {int foo; foo=12; if(foo == 12) return foo;}'
assert 12 'int main() {int foo; foo=12; if(foo == 13) return foo; return foo;}'
assert 34 'int main() {int foo; int bar; int baz; foo=12;bar=34;baz=5; if(foo == 12) return bar; else return baz;}'
assert 5  'int main() {int foo; int bar; int baz; foo=12;bar=34;baz=5; if(foo == 13) return bar; else return baz;}'
assert 3  'int main() {int a; a=1; a = a + 1; a = a + 1; return a;}'
assert 9  'int main() {int a; a=9;2+3;  return a;}'
assert 6  'int main() {int a; a=2; while(a < 3) a = a + 4; return a;}'
assert 10 'int main() {int a; int b; a=2; while(a < 3) a = a + 4; b = a + 4; return b;}'
assert 5  'int main() {int a; a=5; while(a < 3) return a; return a;}'
assert 16 'int main() {int a; a=1; while(a < 9) a = a + a; return a;}'
assert 3  'int main() {int a; int i; a=0;for (i = 0; i < 3; i = i + 1;) a = a + 1; return a;}'
assert 3  'int main() {int a; int i; a=0;i=0;for (; i < 3; i = i + 1;) a = a + 1; return a;}'
assert 3  'int main() {int i; for(i=0;;i=i+1;) if(i > 2) return i;}'
assert 3  'int main() {int i; for(i=0;i<3;;) i=i+1; return i;}'
assert 3  'int main() {int a; a=1; if(a < 3){a=a+1;a=a+1;}return a;}'
assert 1  'int main() {int a; a=1; if(a > 3){}return a;}'
assert 4  'int main() {int a; a=0; while(a < 3) {a=a+1; a=a+1;} return a;}'
assert 2  'int main() {int a; a=2; while(a > 3) {} return a;}'
assert 6  'int main() {int a; int i; a=0;for (i=0; i<3; i=i+1;){a=a+1;a=a+1;}return a;}'
assert 6  'int main() {int a; int i; a=0;i=0;for (;i<3;i=i+1;){a=a+1;a=a+1;}return a;}'
assert 4  'int main() {int i; for(i=0;i<3;;) {i=i+1;i=i+1;} return i;}'
assert 0  'int main() {foo(); return 0;}'
assert 0  'int main() {bar(5, 7+8); return 0;}'
assert 5  'int sum(int x) {return 5;} int main() {return sum(3);}'
assert 5  'int sum(int x) {return x;} int main() {return sum(5);}'
assert 12 'int sum(int x, int y) {return x+y;} int main() {return sum(5, 7);}'
assert 10 'int sum(int x, int y) {int a; int b; a=3; b=4;return x+y+a+b;} int main() {int x; int y; x=1; y=2;return sum(x, y);}'
assert 4  'int main() {int x; int y; x = 3; y = &x; return *y+1;}'
assert 3  'int main() {int x; int y; int z; x = 3; y = 5; z = &y + 8; return *z;}'
echo OK
