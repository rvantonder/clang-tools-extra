#include <stdio.h>

int main() {

for (int i = 0; i < 10; i++) {
  printf("%d\n", i);
}

}

/* output:
ForStmt 0x5243f18
|-DeclStmt 0x5243c18
| `-VarDecl 0x5243b98  used i 'int' cinit
|   `-IntegerLiteral 0x5243bf8 'int' 0
|-<<<NULL>>>
|-BinaryOperator 0x5243c90 '_Bool' '<'
| |-ImplicitCastExpr 0x5243c78 'int' <LValueToRValue>
| | `-DeclRefExpr 0x5243c30 'int' lvalue Var 0x5243b98 'i' 'int'
| `-IntegerLiteral 0x5243c58 'int' 10
|-UnaryOperator 0x5243ce0 'int' postfix '++'
| `-DeclRefExpr 0x5243cb8 'int' lvalue Var 0x5243b98 'i' 'int'
`-CompoundStmt 0x5243ef8
  `-CallExpr 0x5243e90 'int'
    |-ImplicitCastExpr 0x5243e78 'int (*)(const char *__restrict, ...)' <FunctionToPointerDecay>
    | `-DeclRefExpr 0x5243df0 'int (const char *__restrict, ...)' lvalue Function 0x5230cc8 'printf' 'int (const char *__restrict, ...)'
    |-ImplicitCastExpr 0x5243ec8 'const char *' <ArrayToPointerDecay>
    | `-StringLiteral 0x5243d98 'const char [4]' lvalue "%d\n"
    `-ImplicitCastExpr 0x5243ee0 'int' <LValueToRValue>
      `-DeclRefExpr 0x5243dc8 'int' lvalue Var 0x5243b98 'i' 'int'
*/
