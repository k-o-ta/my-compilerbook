#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 'return 0;'
assert 42 'return 42;'
assert 21 'return 5+20-4;'
assert 41 'return  12 + 34 - 5;'
assert 47 'return 5+6*7;'
assert 15 'return 5*(9-6);'
assert 4  'return (3+5)/2;'
assert 10 'return -10+20;'
assert 10 'return - -10;'
assert 10 'return - - +10;'

assert 0 'return 0==1;'
assert 1 'return 42==42;'
assert 1 'return 0!=1;'
assert 0 'return 42!=42;'

assert 1 'return 0<1;'
assert 0 'return 1<1;'
assert 0 'return 2<1;'
assert 1 'return 0<=1;'
assert 1 'return 1<=1;'
assert 0 'return 2<=1;'

assert 1 'return 1>0;'
assert 0 'return 1>1;'
assert 0 'return 1>2;'
assert 1 'return 1>=0;'
assert 1 'return 1>=1;'
assert 0 'return 1>=2;'
assert 1 'return 1; 2; 3;'
assert 3 'a=3; return a;'
assert 4 'a=3; return a+1;'
assert 21 'a=5;b=20;c=4;return a+b-c;'
assert 41 'a=12;b=34;c=5; return  a + b - c;'
assert 3 'foo=3; return foo;'
assert 4 'bar=3; return bar+1;'
assert 21 'foo=5;bar=20;baz=4;return foo+bar-baz;'
assert 41 'foo=12;bar=34;baz=5; return foo + bar - baz;'

echo OK
