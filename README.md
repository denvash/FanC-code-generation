# Slim - FanC Semantic Analyzer

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

## Example

Input FanC program to be analyzed:

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

Outputs:

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
