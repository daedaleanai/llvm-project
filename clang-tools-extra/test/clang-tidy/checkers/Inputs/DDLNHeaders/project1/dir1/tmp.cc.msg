3 warnings generated.
/home/nf/TOOLS/llvm-project/clang-tools-extra/clang-tidy/../test/clang-tidy/checkers/Inputs/DDLNHeaders/project1/dir1/tmp.cc:1:10: warning: Use #include<> instead of include "" [daedalean-include-order]
#include "project1/dir1/cls1.hh"
         ^~~~~~~~~~~~~~~~~~~~~~~
         <project1/dir1/cls1.hh>
/home/nf/TOOLS/llvm-project/clang-tools-extra/clang-tidy/../test/clang-tidy/checkers/Inputs/DDLNHeaders/project1/dir1/tmp.cc:1:10: note: FIX-IT applied suggested code changes
/home/nf/TOOLS/llvm-project/clang-tools-extra/clang-tidy/../test/clang-tidy/checkers/Inputs/DDLNHeaders/project1/dir1/tmp.cc:4:10: warning: <project1/dir1/header2.hh> and <project1/dir2/header3.hh> should be in different groups [daedalean-include-order]
#include <project1/dir1/header2.hh>
         ^
/home/nf/TOOLS/llvm-project/clang-tools-extra/clang-tidy/../test/clang-tidy/checkers/Inputs/DDLNHeaders/project1/dir1/tmp.cc:5:10: warning: <project1/dir2/header3.hh> and <project2/header3.hh> should be in different groups [daedalean-include-order]
#include <project1/dir2/header3.hh>
         ^
clang-tidy applied 1 of 1 suggested fixes.
