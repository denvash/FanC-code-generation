declare i32 @printf (i8*, ...)

; Print
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define void @print (i8*) {
  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)
  ret void
}