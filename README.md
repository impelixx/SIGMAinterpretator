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
	          | <DoWhileStatement>
	          | <ForStatement>
	          | <PrintStatement>
	          | <ReturnStatement>
	          | <EmptyStatement>

//есть
<EmptyStatement> → ";" | NEWLINE

//есть
<FunctionDeclaration> → "def" + <Identifier> + "(" + <ParameterList> + ")" + ":" + NEWLINE + INDENT + <StatementList> + DEDENT

//есть
<ParameterList> → ℇ | ( <Identifier> + { "," + <Identifier> } )

//есть
<VariableDeclaration> → <Type> + <Identifier> + "=" + <Expression> + <StatementTerminator>
	              | <Type> + <Identifier> + "[" + <Number> + "]" + "=" + <Initianalizer list> + <StatementTerminator> 

//есть
<InitianalizerList> → "{" +  [<Expression> + { "," + <Expression>] } + "}"

//есть
<Assignment> → (<Identifier> | (<Identifier> + "[" + <Number> "]")) + "=" + <Expression> + <StatementTerminator>

//есть
<IfStatement> → "if" + <CaseExpression> + <FieldView>

//есть
<WhileStatement> → "while" + <CaseExpression> + <FieldView>

//есть
<DoWhileStatement> → "do" + <FieldView> + "while" + <CaseExpression> + <StatementTerminator>

//есть
<ForStatement> -> "for" + <Type> + <Identifier> + "in" + "(" + <Number> + "," + <Number> ")" + <FieldView>

//есть
<FieldView> -> ":" + NEWLINE + INDENT + <StatementList> + DEDENT

//есть
<PrintStatement> → "print" + "(" + <Expression> + ")" + <StatementTerminator>

//есть
<ReturnStatement> → "return" + <CaseExpression> + <StatementTerminator>

//есть
<CaseExpression> -> "(" + <Expression> + { (and | or) + <Expression> + ")"

//есть
<Expression> → <Term> + { ("+" | "-" ) + <Term> }

//есть
<Term> → <Factor> + { ("*" | "/" ) + <Factor> }

//есть
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