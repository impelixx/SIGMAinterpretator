```
//есть
<Program> → <StatementList> 

//есть
<StatementList> → { <Statement> }

//есть
<Statement> → <FunctionDeclaration>
	          | <VariableDeclaration>
	          | <Assignment>
	          | <IfStatement>
	          | <WhileStatement>
	          | <PrintStatement>
	          | <ReturnStatement>
	          | <EmptyStatement>

//есть
<EmptyStatement> → ";" | NEWLINE

//есть
<FunctionDeclaration> → "def" + <Identifier> + "(" + <ParameterList> + ")" + ":" + NEWLINE + INDENT + <StatementList> + DEDENT

//нет
<ParameterList> → ℇ | ( <Identifier> + { "," + <Identifier> } )

//нет
<VariableDeclaration> → <Type> + <Identifier> + "=" + <Expression> + <StatementTerminator>
	              | <Type> + <Identifier> + "[" + <Number> + "]" + "=" + <Initianalizer list> + <StatementTerminator>

//нет
<Initianalizer list> → "{" +  [<Expression> + { "," + <Expression>] } + "}"

//нет
<Assignment> → (<Identifier> | (<Identifier> + "[" + <Number> "]")) + "=" + <Expression> + <StatementTerminator>

//нет
<IfStatement> → "if" + <CaseExpression> + <FieldView>

//нет
<WhileStatement> → "while" + <CaseExpression> + <FieldView>

//нет
<DoWhileStatement> → "do" + ":" + <FieldView> + "while" + <CaseExpression> + <StatementTerminator>

//нет
<ForStatement> -> "for" + <Type> + <Identifier> + "in range" + "(" + <Number> + "," + <Number> ")" + <FieldView>

//нет
<FieldView> -> ":" + NEWLINE + INDENT + <StatementList> + DEDENT

//нет
<PrintStatement> → "print" + "(" + <Expression> + ")" + <StatementTerminator>

//нет
<ReturnStatement> → "return" + <CaseExpression> + <StatementTerminator>

//нет
<CaseExpression> -> "(" + <Expression> + { (and | or) + <Expression> + ")"

//нет
<Expression> → <Term> + { ("+" | "-" ) + <Term> }

//нет
<Term> → <Factor> + { ("*" | "/" ) + <Factor> }

//нет
<Factor> → <Number> | <Identifier> | ( "(" + <Expression> + ")" )

//есть
<Type> → "bool" | "int" | "float" | "char"

//есть
<Identifier> → ( a-z | A-Z | _ ) + { ( a-z | A-Z | _ | 0-9 ) }

//есть
<Number> → ["+" | "-"] + (0-9) + {0-9} + [ "." ] + {0-9}

//есть
<StatementTerminator> → ";" | NEWLINE

//есть
INDENT → прибавилось "    "

//есть
DEDENT → уменьшилось "    "

//есть
KEYWORDS → "def" | "if" | "while" | "print" | "return"

//есть
OPERATORS → "=" | "+" | "-" | "*" | "/" | ":" | "," | "(" | ")" | "[" | "]" | ";"

//есть
BRACKETS → "(" | ")" | "[" | "]"
```