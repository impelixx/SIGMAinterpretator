```
<Program> → <StatementList>

<StatementList> → { <Statement> }

<Statement> → <FunctionDeclaration>
	          | <VariableDeclaration>
	          | <Assignment>
	          | <IfStatement>
	          | <WhileStatement>
	          | <PrintStatement>
	          | <ReturnStatement>
	          | <EmptyStatement>

<EmptyStatement> → ";" | NEWLINE

<FunctionDeclaration> → "def" + <Identifier> + "(" + <ParameterList> + ")" + ":" + NEWLINE + INDENT + <StatementList> + DEDENT

<ParameterList> → ℇ | <Identifier> + { "," + <Identifier> }

<VariableDeclaration> → <Type> + <Identifier> + "=" + <Expression> + <StatementTerminator>
	              | <Type> + <Identifier> + "[" + <Number> + "]" + "=" + <Initianalizer list> + <StatementTerminator>

<Initianalizer list> → "{" +  [<Expression> + { "," + <Expression>] } + "}"

<Assignment> → (<Identifier> | (<Identifier> + "[" + <Number> "]")) + "=" + <Expression> + <StatementTerminator>

<IfStatement> → "if" + <Expression> + ":" + NEWLINE + INDENT + <StatementList> + DEDENT

<WhileStatement> → "while" + <Expression> + ":" + NEWLINE + INDENT + <StatementList> + DEDENT

<PrintStatement> → "print" + "(" + <Expression> + ")" + <StatementTerminator>

<ReturnStatement> → "return" + <Expression> + <StatementTerminator>

<Expression> → <Term> + { ("+" | "-" ) + <Term> }

<Term> → <Factor> + { ("*" | "/" ) + <Factor> }

<Factor> → <Number> | <Identifier> | "(" + <Expression> + ")"

<Type> → "INT" | "FLOAT" | "STRING" | "BOOL"

<Identifier> → ( a-z | A-Z | _ ) + { ( a-z | A-Z | _ | 0-9 ) }

<Number> → ["+" | "-"] + (0-9) + {0-9} + [ "." ] + {0-9}

<StatementTerminator> → ";" | NEWLINE

INDENT → прибавилось "    "

DEDENT → уменьшилось "    "

KEYWORDS → "def" | "if" | "while" | "print" | "return"

OPERATORS → "=" | "+" | "-" | "*" | "/" | ":" | "," | "(" | ")" | "[" | "]" | ";"

BRACKETS → "(" | ")" | "[" | "]"
```