declare i32 @printf (i8*, ...)

; Print
@.int_specifier = constant [4 x i8] c"%d\0A\00"
define void @printi (i32) {
  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)
  ret void
}

; Example
define i32 @main() {
  call void @printi(i32 5)
  ret i32 0
}