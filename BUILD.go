package llvm

var CLang = in("bin/clang")
var ClangPp = in("bin/clang++")
var Ld = in("bin/ld.lld")
var Ar = in("bin/llvm-ar")
var As = in("bin/llvm-as")
var Objdump = in("bin/llvm-objdump")
var Objcopy = in("bin/llvm-objcopy")
var ClangTidy = in("bin/clang-tidy")
var ClangFormat = in("bin/clang-format")
var Cov = in("bin/llvm-cov")

var CLangLibs = ins(
	"lib/libclangFrontend.a",
	"lib/libclang.so",
	"lib/libclangDriver.a",
	"lib/libclangCodeGen.a",
	"lib/libclangSema.a",
	"lib/libclangAnalysis.a",
	"lib/libclangRewriteFrontend.a",
	"lib/libclangRewrite.a",
	"lib/libclangAST.a",
	"lib/libclangASTMatchers.a",
	"lib/libclangParse.a",
	"lib/libclangBasic.a",
	"lib/libclangARCMigrate.a",
	"lib/libclangEdit.a",
	"lib/libclangFrontend.a",
	"lib/libclangFrontendTool.a",
	"lib/libclangSerialization.a",
	"lib/libclangTooling.a",
	"lib/libclangStaticAnalyzerCheckers.a",
	"lib/libclangStaticAnalyzerCore.a",
	"lib/libclangStaticAnalyzerFrontend.a",
	"lib/libclangHandleCXX.a",
	"lib/libclangLex.a",
)
