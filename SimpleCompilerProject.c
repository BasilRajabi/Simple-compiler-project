#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define BSIZE 128
#define NONE -1
#define EOS '\0'
#define NUM 256
#define DIV 257
#define MOD 258
#define ID 259
#define DONE 260
#define program 261
#define input 262
#define output 263
#define CONST 264
#define var 265
#define integer 266
#define real 267
#define CHAR 268
#define boolean 269
#define begin 270
#define end 271
#define IF 272
#define THEN 273
#define WHILE 274
#define DO 275
#define WRITELN 276
#define ELSE 277
#define OR 278
#define AND 279
int tokenval;
struct entry{/* from of symbol table entry*/
    char *lexptr;
    int token;
    };
    struct entry symtable[];
        char lexbuf[BSIZE];
        int lineno = 1;
       int tokenval = NONE;

        int lexan() //////////////		lexan
        {       
        int t;
            while(1) {
                t=getchar();

                if (t==' ' || t=='\t')
                    ;
                else if (t=='\n')
                    lineno = lineno+1;
                else if (isdigit(t)){
                    ungetc(t, stdin);

                    scanf("%d", &tokenval);
                    return NUM;
                }
                else if (isalpha(t)){
                    int p,b=0;
                    while(isalnum(t)){
                        lexbuf[b]=t;
                        t=getchar();
                        b=b+1;
                        if (b>= BSIZE)
                            error("compiler error");
                    }
                    lexbuf[b] = EOS;
                    if(t!=EOF)
                        ungetc(t, stdin);
                    p=lookup(lexbuf);
                    if(p==0)
                        p=insert(lexbuf,ID);
                    tokenval=p;
                    return symtable[p].token;
                }
                else if (t==EOF)
                    return DONE;
                else {
                    tokenval=NONE;
                    return t;
                }

            }
        }
        int lookahead;
        parse() /////////////////////////////		parse
        {
            lookahead = lexan();
            while (lookahead != DONE){
            	Program();
            }
        }
        expr() ////////////////////////////////////		expr
        {
            int t;
            term();
            while(1)
            switch (lookahead){
        case '+' : case '-':
            t=lookahead;
            match(lookahead); term(); emit(t,NONE);
            continue;
            default:
            return;
            }
        }
        term() /////////////////////////////		term
        {
           int t;
            factor();
            while(1)
            switch (lookahead){
        case '*': case '/': case DIV: case MOD: case AND: case OR: case '<': case '>': case '=': // case "<=": case ">=": case "<>":
            t=lookahead;
            match(lookahead); factor(); emit(t,NONE);
            continue;
        default:
            return;
                        }
        }
        factor() ////////////////////////////////		factor
        {
            switch(lookahead){
            case '(':
                match('(');
				 expr(); 
				  match(')'); 
				  break;
			case 34: // " in ascii
				match('"');
				break;
            case NUM:
                emit(NUM, tokenval);  match(NUM); break;
            case ID:
                emit(ID, tokenval);  match(ID); break;
            default:
                error("syntax error at factor");

            }
        }
         
        Program() ////////////////////////////// 		Program
        {
            Header();
            
            while(lookahead == CONST || lookahead == var)
           		{
				   Declarations();
           			
				}
           printf("\nint main()");
		    if(lookahead == begin)
            	Block();
            	else error("syntax error");
            match('.'); 
        }
        Header() 	////////////////////////////// 		Header
        {
            match(program);
          //  printf("program ");
          //  emit(ID, tokenval);
            match(ID);
            match(40); // = (
         //   printf("(");
            match(input);
		//	printf("input");
            match(44); // = ,
         //   printf(",");
            match(output);
         //   printf("output");
            match(41); // = )
         //   printf(") ");
		//	emit(';',tokenval);
            match(';');

        }
        Declarations() 	/////////////////////////// 		Declaration
        {
        	if(lookahead == CONST)
        	{
//        	emit(CONST, tokenval);
        	match(CONST);
//        		printf("CONST ");
        		ConstantDefinitions(); 
        		return;
			}
			else if(lookahead == var)
			{
//				emit(var, tokenval);
				match(var);
//				printf("var \n");
				VariableDeclarations();
				return;
				
			}
			else error("syntax error at declaration");       	          	
		}
		ConstantDefinitions () //////////////////////////		ConstantDefinitions
		{
			ConstantDefinition(); 
			
		}
		
		ConstantDefinition() ///////////////////////		ConstantDefinition
		{
			printf("const ");
			factor();
            match(61); // = in ascii
            printf(" = ");
            factor();
            emit(';', tokenval);
            match(';');
            printf("\n");
		} 
		
		RestConstantDefinitions() ///////////////////		RestConstantDefinitions
		{
			ConstantDefinition();
		}
		
		VariableDeclarations() /////////////////////		VariableDeclarations 
		{
			VariableDeclaration();
			RestVariableDeclarations();
		}
		
		VariableDeclaration() ///////////////////////////	VariableDeclaration
		{
			IdentifierList();
			match(':'); 
			if (lookahead == integer)
			{
				match(integer);
				printf(" int ");
			}
			else if (lookahead == boolean)
			{
				match(boolean);
				printf(" bool ");
			}
			else if (lookahead == real)
			{
				match(real);
				printf(" float ");
			}
			else if (lookahead == CHAR)
			{
				match(CHAR);
				printf(" char ");
			}
			else error("undefined data type");			
			emit(';', tokenval);
			match(';');		
		}
		
		RestVariableDeclarations() /////////////////// RestVariableDeclaration
		{
			VariableDeclaration();
			if(lookahead == 44)
			RestVariableDeclarations();
		}
		
		IdentifierList() /////////////////////	IdentifierList
		{
			factor(); // for id
			if(lookahead == 44)
			RestIdentifierList();	
		}
		
		RestIdentifierList() ////////////////////	RestIdentifierList
		{
			match(44); // , in ascii
			printf(" , ");
			factor();
			if(lookahead == 44)
			RestIdentifierList();
			
		}
		
		Block()		 ///////////////// 		Block
		{
			match(begin);
			printf("\n{\n");	
			Statements();			
			match(end);
			printf("\n}");		
		}
		
		Statements() //////////////////		Statements
		{
			int t;
	Statement();
	while(1)
	{
		switch(lookahead)
		{
			case ';':
				t = lookahead;
                match(lookahead);
                Statements();
                emit(t, NONE);
				continue;
            default:
                return;
		}
	}
		}
		Statement() /////////////////////		Statement
		{
			int t;
			switch(lookahead)
			{
				case ID:
					t=lookahead;
					factor();
					match(58); // : in ascii
					match(61); // = in ascii
					printf(" = ");
					Expression();
					printf(";\n");
					break;
				
				case begin:
					t=lookahead;
					Block();
					break;
					
				case IF:
					match(IF);
					printf("if (");
					Expression();
					printf(")\n");
					match(THEN);
					
					Statement();
					if (lookahead == ELSE )
						{	
							printf("else \n");
							ElseClause();
						}
					break;						
				case WHILE:
					match(WHILE);
					printf("while(");
					Expression();
					printf(")\n");
					match(DO);
					printf("do\n  {\n   ");
					Statement();
					printf("  }\n");
					break;					
				case WRITELN:
					match(WRITELN);
					match(40); // ( in ascii
					printf("cout << ");
					Statement();
					match(41); // ) in ascii
					printf(';');
					break;
				case '"':
					printf("\"");
					match(34);
					if (lookahead != 34)
					Statement();					
					break;					
			}			
		}
		RestStatements() 	///////////////		RestStatements
		{
			Statement();
			if(isalpha(lookahead) || lookahead == IF || lookahead == WHILE || lookahead == WRITELN)
			RestStatements();
		}
		
		ElseClause() /////////////////////////		ElseClause
		{
			if (lookahead == ELSE)
			{
				match(ELSE);
				Statement();
			}
			else return;		
		}
		
		ExpressionList() 	/////////////////// ExpressionList
		{
			Expression();
			RestExpressionList();
		}
		
		Expression() ///////////////////////		Expression
		{
			SimpleExpression();
			
			if (lookahead == 61 || lookahead == '>' || lookahead == 60 ) //|| lookahead == "<=" || lookahead == ">=" || lookahead == "<>")
			{
				//printf("relop");
				SimpleExpression();
			}
				
		}
		RestExpressionList() //////////////	RestExpressionList
		{
			match(44);
			Expression();
			RestExpressionList();
		}
		SimpleExpression() ///////////////////	SimpleExpression
		{
			int t = lookahead;
			if(lookahead == ID)
			{
				term();
				RestSimpleExpression();
			}
			else if(lookahead = NUM)
							{
								factor();
							}
			else {
				if(lookahead == 43) // + in ascii
				{
					match('+');
					term();
					RestSimpleExpression();
					
				}
				else if(lookahead == '-')
						{
							match('-');
							printf(" - ");
							term();
							RestSimpleExpression();
												
						}
				else if (lookahead == OR)
						{
							match(OR);
							printf("||");
							term();
							RestSimpleExpression();
					}
			}	
		}
		
		RestSimpleExpression() 	/////////////////////	RestSimpleExpression
		{
			if(lookahead == '+' || lookahead == '-' || lookahead == OR)
			{
			if(lookahead == '+')
				{
					match('+');
					printf(" + ");
					term();
					RestSimpleExpression();		
				}
				else if(lookahead == '-')
						{
							match('-');
							printf(" - ");
							term();
							RestSimpleExpression();												
						}
				else if (lookahead == OR)
						{
							match(OR);
							printf("||");
							term();
							RestSimpleExpression();							
						}		
			}			
		}
		
		RestTerm() /////////////////////	RestTerm
		{
			if(lookahead == '*' || lookahead == '/' || lookahead == MOD || lookahead == AND || lookahead == DIV)
			{
				if(lookahead == '*')
				{
					match('*');
					printf(" * ");
					factor();
					RestTerm();
				}
				else if(lookahead == '/')  
				{
					match('/');
					printf(" / ");
					factor();
					RestTerm();
				}
				else if (lookahead == DIV)
				{
					match(DIV);
					printf(" / ");
					factor();
					RestTerm();
				}
				else if(lookahead == MOD)
				{
					match(MOD);
					printf(" % ");
					factor();
					RestTerm();
				}
				else if(lookahead == AND)
				{
					match(AND);
					printf(" && ");
					factor();
					RestTerm();
				}
			}
			else return;
		}
		
        match(t) ////////////////////		match
        int t;
        {
            if (lookahead == t)
                lookahead = lexan();
            else
			{error("syntax error at match");
			error(t);
			 } 
        }

        emit(t,tval) 	/////////////////////// 	emit
        int t,tval;
        {
            switch(t){
            case '+' : case '-': case '*' : case '/':
                printf("%c ",t); break;
            case DIV:
                printf("DIV "); break;
            case MOD:
                printf("MOD "); break;
            case NUM:
                printf("%d ",tval); break;
            case ID:
                printf("%s ", symtable[tval].lexptr); break;
            case program:
            	printf("program "); break;
            case input: 
           		printf("input");break;
           	case output:
           		printf("output"); break;
           	case CONST:
           		printf("CONST ");break;
           	case var:
           		printf("var "); break;
           	case ';':
           		printf(";"); break;
           	case ':':
           		printf(":"); break;
           	case WRITELN:
           		printf("cout << ");
            
            default:
                printf("token %d, tokenval %d\n", t,tval);
            }

        }
        #define STRMAX 999
        #define SYMMAX 100
    char lexemes[STRMAX];
    int lastchar = -1;
    struct entry symtable[SYMMAX];
    int lastentry = 0;
    int lookup(s) //////////////////////// 		lookup
    char s[];
    {
        int p;
        for (p=lastentry; p>0; p=p-1)
            if(strcmp(symtable[p].lexptr,s)==0)
              return p;
        return 0;
    }
    int insert(s, tok) 	////////////////////////////		insert
    char s[];
    int tok;
    {
        int len;     
        len=strlen(s);        
        if(lastentry+1>=SYMMAX)
            error("symbol table full");
        if (lastchar+len+1>=STRMAX)
            error("lexemes array full");
        lastentry=lastentry+1;
        symtable[lastentry].token = tok;
        symtable[lastentry].lexptr=&lexemes[lastchar+1];
        lastchar=lastchar+len+1;
        strcpy(symtable[lastentry].lexptr, s);
        return lastentry;
    }
    struct entry keywords[]={
        "div", DIV,
        "mod", MOD,
        "program", program,
        "input", input,
        "output", output,
        "CONST", CONST,
        "VAR", var,
        "integer", integer,
        "real", real,
        "CHAR", CHAR,
        "boolean", boolean,
        "BEGIN", begin,
        "END", end,
        "IF", IF,
        "THEN", THEN,
        "WHILE", WHILE,
        "DO", DO,
        "WRITELN", WRITELN,
        "ELSE", ELSE,
        "OR", OR,
        "AND", AND,
        0,0
        };

    init() /////////////////////		init
    {
        struct entry *p;
        for (p=keywords; p->token; p++)
            insert(p->lexptr, p->token);
    }
    error(m) /////////////////////////////		error
        char *m;
    {
        fprintf(stderr, "line %d: %s\n", lineno, m);
        exit(1);
    }
int main(int argc, char* argv[])
{
	if (argc > 3)
	{
		printf("Too many arguments");
		exit(1);
	}
	else if (argc < 3)
		{
			printf("There is missing arguments");
			exit(1);
		}
		
		freopen(argv[1], "r", stdin);
		freopen(argv[2], "w", stdout);		
			
	printf("#include <iostream>\n");
	printf("using namespace std;\n");

    init();
    parse();
    exit(0);
}


