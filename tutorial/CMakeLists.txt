set(LLVM_LINK_COMPONENTS support)

add_clang_executable(tutorial Tutorial.cpp)

target_link_libraries(tutorial
  clangTooling
  clangBasic
  clangASTMatchers
  )
