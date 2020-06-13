# FanC LLVM Code Generator

## Usage

```bash
# Library Dir
cd Lib

# Build parser
make all

# Input FanC program
parser < input.in > output.out
```

## Testing

Use `Test/in` folder for input files and `Test/res` for expecting results.

```bash
# Run Tests
./Tests/runTests.sh
```

## LLVM Example

```llvm
declare i32 @printf (i8*, ...)

; Print
@.str_specifier = internal constant [4 x i8] c"%s\0A\00"
define void @print (i8*) {
  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)
  ret void
}

; Example
@msg = internal constant [13 x i8] c"Hello World!\00"

define i32 @main() {
  call void @print(i8* getelementptr ([13 x i8], [13 x i8]* @msg, i32 0, i32 0))
  ret i32 0
}
```

## Example

```java
void printByValue(bool val) {
  if (val)
    print("val is true");
  else {
    print("val is false");
  }
}

void main() {
  printByValue(true);
  printByValue(false);

  if(true)
    {
        print("true");
    }

    if(false or false and  true)
    {
        print("true");
    }
}
```

Outputs an LLVM Program that can be executed as:

```console
val is true
val is false
true
```

Input:

```java
int fib(byte n) {
    if (n == 0 or n == 1) return 1;
    return fib(n-1b) + fib(n-2b);
}

void main() {
    byte i = 0b;
    while (i < 10) {
        printi(fib(i));
        if (i+1 < 10)
            print("*");
        i = i + 1b;
    }
    print("*");
}
```

Outputs:

```
1
*
1
*
2
*
3
*
5
*
8
*
13
*
21
*
34
*
55
*
```
