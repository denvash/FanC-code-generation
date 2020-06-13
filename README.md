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
void printOk(bool isOk) {
    if (isOk) print("ok");
}

void main() {
    int x = 4;
    byte y = 100 b;
    printOk(x>y);
    int z;
}
```

Outputs:

```text
---end scope---
---end scope---
isOk BOOL -1
---end scope---
x INT 0
y BYTE 1
z INT 2
---end scope---
print (STRING)->VOID 0
printi (INT)->VOID 0
printOk (BOOL)->VOID 0
main ()->VOID 0
```
