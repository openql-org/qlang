; ModuleID = 'test_q_test.c'
source_filename = "test_q.c"
target datalayout = "e-m:e-p:64:64-i64:64-i128:128-n64-S128"
target triple = "riscv64-unknown-linux-gnu"

; Function Attrs: nounwind
define dso_local signext i32 @calc() local_unnamed_addr #0 {
  tail call void asm sideeffect "qooh.k  qa0,qt1,qzero,1", ""() #1, !srcloc !2
  %1 = tail call i32 asm sideeffect "qmeas.k  $0,qt1,qzero,1", "=r"() #1, !srcloc !3
  ret i32 %1
}

attributes #0 = { nounwind "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "frame-pointer"="none" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-features"="+relax" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 10.0.0 (https://github.com/llvm/llvm-project.git 4e71702cd4ab2c77aaa753770e99526085e5e6eb)"}
!2 = !{i32 111}
!3 = !{i32 185}