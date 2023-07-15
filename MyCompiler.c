#include <stdio.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STANDARD_BLOCK_SIZE 500

int DEVELOPER_OPTIONS = 0;
int WARNINGS = 1;
int GENERATE_CODE = 0;

/*						*
 *	Core Functions and Functionalities	*
 *						*/

FILE* source;

typedef struct Token {
    int code; // code (Atom name)
    union {
	char* text; // used for ID, CT_STRING (dynamically allocated)
	long int i; // used for CT_INT, CT_CHAR
	double r; // used for CT_REAL
    };
    int line; // the input file line
    struct Token* next; // link to the next token
    struct Token* prev; //link to the previous token

}Token;

Token* curr_token=NULL;
Token* root=NULL;

void err(const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error: ");
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}


void tkerr(const Token *tk,const char *fmt,...)
{
    va_list va;
    va_start(va,fmt);
    fprintf(stderr,"error in line %d: ",tk->line);
    vfprintf(stderr,fmt,va);
    fputc('\n',stderr);
    va_end(va);
    exit(-1);
}


Token *addTk(int code, int line)
{
    Token *tk;
    //SAFE ALLOC
    tk = (Token*)malloc(sizeof(Token));
    if(tk == NULL)
	err("not enough memory");
    tk->code=code;
    tk->line=line;
    tk->next=NULL;
    if(curr_token==NULL)
    {
	tk->prev=NULL;
	root = tk;
    }
    else
	{
	    tk->prev=curr_token;
	    curr_token->next=tk;
	}
    curr_token = tk;
    return tk;
}


char* SafeAlloc(int size)
{
    char *block;
    block = (char*)malloc(sizeof(char) * size);
    if(block == NULL)
	err("not enough memory");
    return block;
}


// function to convert simple char to ESC char
char char_to_ESC(char esc_char)
{
    switch(esc_char)
    {
	case 'n': return '\n';
	case 'a': return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'r': return '\r';
	case 't': return '\t';
	case '\'': return '\'';
	case '\"': return '\"';
	case '?': return '\?';
	case '\\': return '\\';
	case '0': return '\0';
	default: return '\0';
    }
}


//function to convert octal to decimal
long long OctalToDecimal(int octal_nr) {
    int decimal_nr = 0;
    int i = 0;
    while(octal_nr != 0) {
        decimal_nr += (octal_nr%10) * pow(8,i);
        ++i;
        octal_nr/=10;
    }
    i = 1;
    return decimal_nr;
}


/* function to read the next characters in the buffer and in case is not empty to just concatenate the new values */
char* ReadNext(char* block, char* new_start, int size)
{
    if(fgets(block, STANDARD_BLOCK_SIZE-1, source)==NULL)
	return (char*)0;

    return block;
}


//function to create a string given 2 positions in the buffer
char* createString(char *start, char *end)
{
    char* str;
    char* ch = start;
    str = (char*) malloc(end-start+1);
    int pos=0;
    while(ch != end)
    {
	    str[pos]=*ch;
	    pos++;
	    ch++;
    }
    str[pos]='\0';
    return str;
}

enum Atom {
    ID,		//identifiers
    BREAK, CHAR, DOUBLE, ELSE, FOR, IF, INT, RETURN, STRUCT, VOID, WHILE, END,		//keywords
    CT_INT, EXP, CT_REAL, ESC, CT_CHAR, CT_STRING,		//constants
    COMMA, SEMICOLON, LPAR, RPAR, LBRACKET, RBRACKET, LACC, RACC,		//delimiters
    ADD, SUB, MUL, DIV, DOT, AND, OR, NOT, ASSIGN, EQUAL, NOTEQ, LESS, LESSEQ, GREATER, GREATEREQ,		//operators
    SPACE, LINECOMMENT, COMMENT	//not important
}; // tokens codes

#define None COMMENT+1
#define COMPARE COMMENT+2
#define DECLARATION COMMENT+3

enum IntType { ZECIMAL, OCTAL, HEXA, REAL };


//printing atom function
void print_atom(enum Atom to_print)
{
    switch(to_print)
    {
	case ID: printf("ID");
		    break;
	case END: printf("END");
		    break;
	case INT: printf("INT");
			break;
	case BREAK: printf("BREAK");
			break;
	case CHAR: printf("CHAR");
			break;
	case FOR: printf("FOR");
			break;
	case COMMA: printf("COMMA");
			break;
	case LPAR: printf("LPAR");
			break;
	case RPAR: printf("RPAR");
			break;
	case CT_INT: printf("CT_INT");
			break;
	case LBRACKET: printf("LBRACKET");
			break;
	case RBRACKET: printf("RBRACKET");
			break;
	case LACC: printf("LACC");
			break;
	case RACC: printf("RACC");
			break;
	case IF: printf("IF");
			break;
	case DOUBLE: printf("DOUBLE");
			break;
	case ELSE: printf("ELSE");
			break;
	case RETURN: printf("RETURN");
			break;
	case STRUCT: printf("STRUCT");
			break;
	case VOID: printf("VOID");
			break;
	case WHILE: printf("WHILE");
			break;
	case CT_REAL: printf("CT_REAL");
			break;
	case EXP: printf("EXP");
			break;
	case ESC: printf("ESC");
			break;
	case CT_STRING: printf("CT_STRING");
			break;
	case CT_CHAR: printf("CT_CHAR");
			break;
	case ADD: printf("ADD");
			break;
	case SUB: printf("SUB");
			break;
	case DIV: printf("DIV");
			break;
	case MUL: printf("MUL");
			break;
	case DOT: printf("DOT");
			break;
	case AND: printf("AND");
			break;
	case OR: printf("OR");
			break;
	case NOT: printf("NOT");
			break;
	case ASSIGN: printf("ASSIGN");
			break;
	case EQUAL: printf("EQUAL");
			break;
	case NOTEQ: printf("NOTEQ");
			break;
	case LESS: printf("LESS");
			break;
	case LESSEQ: printf("LESSEQ");
			break;
	case GREATER: printf("GREATER");
			break;
	case GREATEREQ: printf("GREATEREQ");
			break;
	case SPACE: printf("SPACE");
			break;
	case LINECOMMENT: printf("LINECOMMENT");
			break;
	case COMMENT: printf("COMMENT");
			break;
	case SEMICOLON: printf("SEMICOLON");
			break;
	default: printf("%d",to_print);
    }
}


//function for printing the list
void print_Tokens()
{
    Token *tk=root;
    printf("\tList of tokens:\n\n");
    while(tk != NULL)
    {
    	printf("%d ", tk->line);
	print_atom((enum Atom)tk->code);
	if(tk->code == ID)
	    printf(": %s",tk->text);
	if(tk->code == CT_INT)
	    printf(": %ld",tk->i);
	if(tk->code == CT_REAL)
	    printf(": %lf",tk->r);
	if(tk->code == CT_CHAR)
	    printf(": '%c'",(char)tk->i);
	if(tk->code == CT_STRING)
	    printf(": %s",tk->text);
	if(tk->code != END)
	    printf(" -> ");
	else
	    printf("\n");
	tk=tk->next;
    }
}


//function for printing the list from a certain token
void print_from_Token(Token *tk)
{
    while(tk != NULL)
    {
	print_atom((enum Atom)tk->code);
	if(tk->code == ID)
	    printf(": %s",tk->text);
	if(tk->code == CT_INT)
	    printf(": %ld",tk->i);
	if(tk->code == CT_REAL)
	    printf(": %lf",tk->r);
	if(tk->code == CT_CHAR)
	    printf(": '%c'",(char)tk->i);
	if(tk->code == CT_STRING)
	    printf(": %s",tk->text);
	if(tk->code != END)
	    printf(" -> ");
	else
	    printf("\n");
	tk=tk->next;
    }
}


/*				*
 *	Lexical Analyzer	*
 *				*/


int getNextToken()
{
    char *block = SafeAlloc(STANDARD_BLOCK_SIZE);
    fgets(block, STANDARD_BLOCK_SIZE - 1, source);

    int state=0,nCh;
    char ch;
    char *pCrtCh=block;
    char *pStartCh;
    Token *tk;
    int line=0;

    // infinite loop
    while(1)
{
	ch=*pCrtCh;

	// helpers for debug ;-)
	//printf("%c|%c|%c - block: %s \n",*(pCrtCh-1),ch,*(pCrtCh+1),block);
	//print_Tokens();
	//putchar('\n');

	switch(state){
	    case 0: // default case
		if(ch==0) // end of the file
		{
		    addTk(END,line);
		    return END;
		}
		else
		if(isalpha(ch)||ch=='_') //ID generator
		{
		    pStartCh=pCrtCh;
		    pCrtCh++;
		    state=1;
		}
		else
		if(ch=='=') // EQUAL or ASSIGN generator
		{
		    if(*(pCrtCh+1)=='=') // if next char is '='
		    {
			addTk(EQUAL,line);
			pCrtCh++;
		    }
		    else
			addTk(ASSIGN,line);
		    pCrtCh++;
		}
		else
		if(ch==' '||ch=='\r'||ch=='\t') //blank chars to consume
		{
		    pCrtCh++;
		}
		else
		if(ch=='\n') // handled separately in order to update the current line and read in buffer
		{
		    line++;
		    // we read the next line in a safe way ready to put an END token if needed
		    pCrtCh=ReadNext(block,pCrtCh+1,STANDARD_BLOCK_SIZE);
		    if(pCrtCh==(char *)0)
		    {
			    addTk(END,line);
			    return END;
		    }
		}
		else
		if(isdigit(ch)) // CT_INT (decimal, octal, hexadecimal) and CT_REAL (using 'e' or 'E', using '.')
		{
		    if(ch=='0' && (isdigit(*(pCrtCh+1)) || *(pCrtCh+1)=='x' || *(pCrtCh+1)=='.'))
		    {
			if(*(pCrtCh+1)=='x') //CT_INT hexa
			{
			    pCrtCh++; // ignore leading 0
			    pCrtCh++; // ignore char 'x'
			    pStartCh=pCrtCh;
			    pCrtCh++;
			    state=7;
			}
			else
			{
			    if(isdigit(*(pCrtCh+1))) //CT_INT octa
			    {
				pCrtCh++; // ignore leading 0
				pStartCh=pCrtCh;
				pCrtCh++;
				state=3;
			    }
			    else
				if(*(pCrtCh+1)=='.') //CT_REAL with '.'
				{
				    pStartCh=pCrtCh;
				    pCrtCh++;
				    state=5;
				}
			}
		    }
		    else //CT_INT decimal or CT_REAL with 'e' or 'E' or '.' or some combination of them
		    {
			pStartCh=pCrtCh;
			pCrtCh++;
			state=5;
		    }
		}
		else
		if(ch=='\"') // CT_STRING generator
		{
		    pCrtCh++;
		    pStartCh=pCrtCh;
		    state=9;
		}
		else
		if(ch=='\'' && (*(pCrtCh+2)=='\'' || (*(pCrtCh+1)=='\\' && *(pCrtCh+3)=='\''))) //CT_CHAR generator
		{
		    if(ch=='\''&& (*(pCrtCh+1)=='\\' && *(pCrtCh+3)=='\'')) //CT_CHAR with ESC
		    {
			tk=addTk(CT_CHAR,line);
			tk->i=char_to_ESC(*(pCrtCh+2));
			pCrtCh+=4;
		    }
		    else //CT_CHAR without ESC = normal char
		    {
		    	tk=addTk(CT_CHAR,line);
			tk->i=*(pCrtCh+1);
			pCrtCh+=3;
		    }
		}
		else
		if(ch=='/' && *(pCrtCh+1)=='/') //single line comments
		{
		    pCrtCh+=2;
		    state = 11;
		}
		else
		if(ch=='/' && *(pCrtCh+1)=='*') //multi-line comments
		{
		    pCrtCh+=2;
		    state = 12;
		}
		else
		if(ch==';')
		{
		    addTk(SEMICOLON,line);
		    pCrtCh++;
		}
		else
		if(ch=='(')
		{
		    addTk(LPAR,line);
		    pCrtCh++;
		}
		else
		if(ch==')')
		{
		    addTk(RPAR,line);
		    pCrtCh++;
		}
		else
		if(ch==',')
		{
		    addTk(COMMA,line);
		    pCrtCh++;
		}
		else
		if(ch=='[')
		{
		    addTk(LBRACKET,line);
		    pCrtCh++;
		}
		else
		if(ch==']')
		{
		    addTk(RBRACKET,line);
		    pCrtCh++;
		}
		else
		if(ch=='{')
		{
		    addTk(LACC,line);
		    pCrtCh++;
		}
		else
		if(ch=='}')
		{
		    addTk(RACC,line);
		    pCrtCh++;
		}
		else
		if(ch=='+')
		{
		    addTk(ADD,line);
		    pCrtCh++;
		}
		else
		if(ch=='-')
		{
		    addTk(SUB,line);
		    pCrtCh++;
		}
		else
		if(ch=='*')
		{
		    addTk(MUL,line);
		    pCrtCh++;
		}
		else
		if(ch=='/')
		{
		    addTk(DIV,line);
		    pCrtCh++;
		}
		else
		if(ch=='.')
		{
		    addTk(DOT,line);
		    pCrtCh++;
		}
		else
		if(ch=='!')
		{
		    pCrtCh++;
		    ch=*pCrtCh;
		    if(ch=='=')
		    {
			addTk(NOTEQ,line);
			pCrtCh++;
		    }
		    else
			addTk(NOT,line);
		}
		else
		if(ch=='<')
		{
		    pCrtCh++;
		    ch=*pCrtCh;
		    if(ch=='=')
		    {
			addTk(LESSEQ,line);
			pCrtCh++;
		    }
		    else
			addTk(LESS,line);
		}
		else
		if(ch=='>')
		{
		    pCrtCh++;
		    ch=*pCrtCh;
		    if(ch=='=')
		{
		    addTk(GREATEREQ,line);
		    pCrtCh++;
		    }
		    else
			addTk(GREATER,line);
		}
		else
		if(ch=='&')
		{
		    pCrtCh++;
		    ch=*pCrtCh;
		    if(ch=='&')
			addTk(AND,line);
		    pCrtCh++;
		}
		else
		if(ch=='|')
		{
		    pCrtCh++;
		    ch=*pCrtCh;
		    if(ch=='|')
			addTk(OR,line);
		    pCrtCh++;
		}
		else tkerr(addTk(END,line),"invalid character");
		break;

	    //consume char and ready to create ID string or keyword
	    case 1:
		if(isalnum(ch)||ch=='_')
		    pCrtCh++;
		else state=2;
		break;

	    //verify if keyword or is ID
	    case 2:
		nCh=pCrtCh-pStartCh; // the id length

		// keywords tests
		if(nCh==5&&!memcmp(pStartCh,"break",5))tk=addTk(BREAK, line);
		else if(nCh==4&&!memcmp(pStartCh,"char",4))tk=addTk(CHAR, line);
		    else if(nCh==6&&!memcmp(pStartCh,"double",6))tk=addTk(DOUBLE, line);
			else if(nCh==4&&!memcmp(pStartCh,"else",4))tk=addTk(ELSE, line);
			    else if(nCh==3&&!memcmp(pStartCh,"for",3))tk=addTk(FOR, line);
				else if(nCh==2&&!memcmp(pStartCh,"if",2))tk=addTk(IF, line);
				    else if(nCh==3&&!memcmp(pStartCh,"int",3))tk=addTk(INT, line);
					else if(nCh==6&&!memcmp(pStartCh,"return",6))tk=addTk(RETURN, line);
					    else if(nCh==6&&!memcmp(pStartCh,"struct",6))tk=addTk(STRUCT, line);
						else if(nCh==4&&!memcmp(pStartCh,"void",4))tk=addTk(VOID, line);
						    else if(nCh==5&&!memcmp(pStartCh,"while",5))tk=addTk(WHILE, line);
							else if(nCh==3&&!memcmp(pStartCh,"end",3))tk=addTk(END, line);

		else{ // if no keyword, then it is an ID
		    tk = addTk(ID,line);
		    tk->text=createString(pStartCh,pCrtCh); //using the start we recorded in state=0(pStartCh) and the end in state=1(pCrtCh)
		}
		state=0;
		break;

	//consume char and ready to create CT_INT octal
	case 3:
	    if(isdigit(ch))
		pCrtCh++;
	    else state=4;
	    break;

	//assemble CT_INT octal and add the token
	case 4:
	    tk = addTk(CT_INT,line);
	    tk->i = OctalToDecimal(atoi(createString(pStartCh,pCrtCh)));
	    state=0;
	    break;

	//consume char and ready to create CT_INT decimal or CT_REAL
	case 5:
	    if(isdigit(ch)||ch=='e'||ch=='E'||ch=='.')
	    {
		if((ch=='e'||ch=='E')&&(*(pCrtCh+1)=='+'||*(pCrtCh+1)=='-'))
		    pCrtCh++;
		pCrtCh++;
	    }
	    else state=6;
	    break;

	//assemble CT_INT decimal or CT_REAL and add the token
	case 6:
	    /*
		flag=0 -> CT_INT decimal
		flag=1 -> CT_REAL no EXP
		flag=2 -> CT_REAL with +EXP
		flag=3 -> CT_REAL witn -EXP
	    */
	    int flag=0,end_of_e_coef=0;
	    char* string;
	    string = createString(pStartCh,pCrtCh);
	    for(int i=0; string[i]!='\0';i++)
	    {
		if(string[i]=='.')
		{
		    flag=1;
		    if(!isdigit(string[i+1]))
			tkerr(addTk(END,line),"invalid character");
		}
		if(string[i]=='e'||string[i]=='E')
		{
		    end_of_e_coef=i-1;
		    flag=2;
		    if(string[i+1]=='+')
			flag=3;
		    if(string[i+1]=='-')
			flag=4;
		}
	    }

	    if(flag==0) // CT_INT decimal
	    {
		tk=addTk(CT_INT,line);
		tk->i=atoi(string);
	    }

	    if(flag==1) //CT_REAL no EXP
	    {
		tk=addTk(CT_REAL,line);
		tk->r=atof(string);
	    }

	    if(flag>=2) //CT real with EXP
	    {
		//e coefficient calculation
		char e_coef_str[100];
		strncpy(e_coef_str,string,end_of_e_coef+1);
		e_coef_str[end_of_e_coef+1]='\0';
		double e_coef = atof(e_coef_str);


		//exponent calculation
		int e_exp=0;
		if(flag>=3) //CT real with EXP with sign
		{
		    e_exp=atoi(string+end_of_e_coef+3);
		}
		else //CT real with EXP without sign
		{
		    e_exp=atoi(string+end_of_e_coef+2);
		}

		//add token to list
		tk=addTk(CT_REAL,line);
		if(flag==2 || flag==3)
		    tk->r =(double)(e_coef*pow(10,e_exp));
		if(flag==4)
		    tk->r =(double)(e_coef*pow(10,(-1)*e_exp));

	    }
	    state=0;
	    break;

	//consume char and ready to create CT_INT hex
	case 7:
	    if(isdigit(ch)||(ch>='A' && ch<='F')||(ch>='a'&&ch<='f'))
		pCrtCh++;
	    else state=8;
	    break;

	//assemble CT_INT hex and add the token
	case 8:
	    tk = addTk(CT_INT,line);
	    tk->i = strtol(createString(pStartCh,pCrtCh),NULL,16); //convert and add hex value to token
	    state = 0;
	    break;

	//consume char and ready to create CT_STRING
	case 9:
	    if(!(ch=='\"' && *(pCrtCh-1)!='\\'))
		pCrtCh++;
	    else
	    {
		pCrtCh++;
		state=10;
	    }
	    break;

	//assemble CT_STRING and add the token
	case 10:
	    tk = addTk(CT_STRING,line);
	    string = createString(pStartCh,pCrtCh-1);
	    for(int i=0; string[i]!=0; i++)
	    {
		if(string[i]=='\\') // identify and switch ESC char with their char values
		{
		    strcpy(string+i,string+i+1);
		    string[i]=char_to_ESC(string[i]);
		}
	    }
	    tk->text=string;
	    state=0;
	    break;

	//consume char until newline to ignore single-line comment
	case 11:
	    if(ch!='\n')
		pCrtCh++;
	    else
		state=0; //we do not eat \n and use it to read more data in the buffer
	break;

	//consume char until "*/" to ignore multi-line comment
	case 12:
	    if(!(ch=='/' && *(pCrtCh-1)=='*'))
	    {
		if(ch=='\n') //since is a multi-line the end will be in another line maybe
		    pCrtCh=ReadNext(block,pCrtCh+1,STANDARD_BLOCK_SIZE);
		else
		    pCrtCh++;
	    }
	    else
		{
		    pCrtCh+=1; // we do not need * or / so we skip them
		    state=0;
		}
	    break;


	}
    }
}



/*				*
 *	Syntactical Analyzer	*
 *				*/

Token* crtTk=NULL;

int consume(int code)
{
    if(crtTk->code==code)
    {
	crtTk=crtTk->next;
	return 1;
    }
    return 0;
}

//consume ordinary mathematical operators
int consume_operator()
{
    if(crtTk->code == ADD || crtTk->code == SUB || crtTk->code == MUL || crtTk->code == DIV)
    {
	crtTk=crtTk->next;
	return 1;
    }
    return 0;
}

int simple_expr();
int typecast();

//simple conditions ex: 'i==0' or 'x>=i' or '!i' or 'i' or '-i'
int simple_cond()
{
    Token *startTk=crtTk;

    if(simple_expr(COMPARE,None)){
	if(consume(EQUAL) || consume(NOTEQ) || consume(LESS) || consume(LESSEQ) || consume(GREATER) || consume(GREATEREQ)){
	    if(simple_expr(COMPARE,None)){
		return 1;
	    }else tkerr(crtTk,"Missing variable or constant in the condition after the equal");
	}
	return 1;
    }else tkerr(crtTk,"Missing variable in the condition");
    crtTk=startTk;
    return 0;
}

//multiple simple conditions
int cond()
{
    Token *startTk=crtTk;
    if(simple_cond()){
	while(consume(AND) || consume(OR)) //if there is another simple condition
	{
	    if(!simple_cond())
		return 0;
	}
	return 1;
    }else tkerr(crtTk,"Missing condition");
    crtTk=startTk;
    return 0;
}

// function to consume a struct type = STRUCT + ID
int struct_type()
{
    Token *startTk=crtTk;
    if(crtTk->code == STRUCT && crtTk->next->code == ID)
    {
	consume(STRUCT);
	consume(ID);
	return 1;
    }
    crtTk=startTk;
    return 0;
}

//apparition of a type EX: 'int' or 'double' or 'char' or 'struct s'
int type()
{
    Token *startTk=crtTk;
    if(consume(INT) || consume(DOUBLE) || consume(CHAR) || struct_type())
    {
	return 1;
    }
    crtTk=startTk;
    return 0;
}

//typecast helper function
int typecast()
{
    Token *startTk=crtTk;
    if(consume(LPAR))
    {
	if(type()){
	    if(consume(RPAR))
		return 1;
	    else
		tkerr(crtTk,"Error in typecast");
	}
    }
    crtTk=startTk;
    return 0;
}

//helper function to check if we have a vector
int checkif_vector()
{
    Token *startTk=crtTk;
    if(crtTk->code == ID)
    {
	if(crtTk->next->code == LBRACKET){
		    return 1;
	}
    }
    crtTk=startTk;
    return 0;
}


//helper function to consume a vector element
int consume_vector_el()
{
    Token *startTk=crtTk;
    if(consume(ID))
    {
	if(consume(LBRACKET)){
	    if(simple_expr(RBRACKET,None)){
		if(consume(RBRACKET)){
		    return 1;
		}else tkerr(crtTk,"Missing ] after vector index");
	    }else tkerr(crtTk,"Missing vector index");
	}
    }
    crtTk=startTk;
    return 0;
}

//verify the return type of the function
int return_type()
{
    Token *startTk=crtTk;
    if(consume(INT) || consume(DOUBLE) || consume(CHAR) || consume(VOID) || struct_type())
    {
	return 1;
    }else tkerr(crtTk,"Missing or invalid type");
    crtTk=startTk;
    return 0;
}

//simple declaration of a variable EX: 'int x' or 'int *x' or 'int x[]' or 'int x[20]'
int simple_decl(){
    Token *startTk=crtTk;
    if(type()){
	if(consume(MUL)){ //we have a pointer
	    if(consume(ID)){
		return 1;
	    }else tkerr(crtTk,"Invalid pointer name after *");
	}
	if(crtTk->code == ID){ //we have an ID which could be a vector or a simple variable
	    if(checkif_vector()){
		if(crtTk->next->code==LBRACKET){
		    if(crtTk->next->next->code==RBRACKET){ //'v[]'
			crtTk=crtTk->next->next->next;
			return 1;
		    }
		    else 
		    {
		    if(consume_vector_el()) //'v[simple_expr]'
		    	return 1;
		    else tkerr(crtTk,"Error in vector declaration");
		    }
		}
	    }
	    else
	    {
		consume(ID); //simple variable
		return 1;
	    }
	}else tkerr(crtTk,"Missing variable");
    }else tkerr(crtTk,"Missing variable type");
    crtTk=startTk;
    return 0;
}

//single argument for a function call
int arg_fcall()
{
    Token *startTk=crtTk;
    if(simple_expr(RPAR,COMMA)){
	return 1;
    }else tkerr(crtTk,"Error in argument");
    crtTk=startTk;
    return 0;
}

//list of arguments for a function call
int arg_list_fcall()
{
    Token *startTk=crtTk;
    if(crtTk->code == RPAR){
	return 1;
    }
    if(arg_fcall()){
	while(consume(COMMA)){
	    if(!arg_fcall())
		tkerr(crtTk,"Missing argument");
	}
	if(crtTk->code==RPAR)
	    return 1;
    }else tkerr(crtTk,"Missing argument");
    crtTk=startTk;
    return 0;
}

//used to identify function calls
int function_call()
{
    Token *startTk=crtTk;
    if(consume(ID)){
	if(consume(LPAR)){
	    if(arg_list_fcall()){
		if(consume(RPAR)){
		    return 1;
		}else tkerr(crtTk,"Missing ) after arguments list");
	    }else tkerr(crtTk,"Argument list error");
	}else tkerr(crtTk,"Missing ( after func");
    }else tkerr(crtTk,"Missing function name");
    crtTk=startTk;
    return 0;
}

//function to verify that the parenthesys respect the syntax and logic
int parenthesys_analyzer(int just_return)
{
    static int open_PAR=0;
    if(crtTk->prev->code == ID && crtTk->code == LPAR) // to not count function opening parenthesys
	return open_PAR;

    //count just the closing parenthesys which do not close also IF or WHILE
    while(crtTk->code==RPAR && !just_return && !(crtTk->next->code==LACC) && !(crtTk->next->code==ID))
    {
	consume(RPAR);
	open_PAR++;
    }

    //count the opening parenthesys
    while(consume(LPAR) && !just_return)
	open_PAR--;
    return open_PAR;
}


//expression for multiple simple mathematical operations EX: 'i+2' or 'x+3+7' or 'z/4+f' or '="this is a string";' or 'function(x,'y',"z",...)'
int simple_expr(int stop_code1, int stop_code2)
{
    Token *startTk=crtTk;

    //unary ops
    consume(NOT);
    consume(SUB);

    if(typecast())
    {
	consume(NOT);
	consume(SUB);
    }

    if(crtTk->code==CT_STRING) //string attribution
    {
	consume(CT_STRING);
	return 1;
    }

    parenthesys_analyzer(0);

    if(consume_vector_el() || consume(ID)||consume(CT_INT)||consume(CT_REAL)||consume(CT_CHAR)){ //non-string attribution
	if(consume_operator() || crtTk->code == SEMICOLON || crtTk->code == DOT || crtTk->code == LPAR){

	    if(consume(DOT)){ //struct variable
		if(!consume(ID))
		    tkerr(crtTk,"Missing field in struct");
		if(consume_operator())
		    simple_expr(stop_code1, stop_code2);
	    }

	    if(crtTk->prev->code == ID && crtTk->code == LPAR) //function call
	    {
		crtTk=crtTk->prev;
		if(!function_call())
		    tkerr(crtTk,"Error in function call");
		if(consume_operator())
		{
		    if(simple_expr(stop_code1, stop_code2))
			return 1;
		}
	    }

	    //count parenthesys but taking into account the closing parenthesys if is stopword
	   if(!(stop_code1 == RPAR && (crtTk->code == stop_code1 || crtTk->code == stop_code2)))
		parenthesys_analyzer(0);

	    if(stop_code1 == DECLARATION)
	    {
		if(crtTk->code == COMMA || crtTk->code == SEMICOLON || crtTk->code == ASSIGN)
		{
		    // not to have 'x+>'
		    crtTk=crtTk->prev;
		    if(consume_operator())
			tkerr(crtTk,"Invalid operator here");
		    crtTk=crtTk->next;
		    return 1;
		}
	    }

	    if(stop_code1 == COMPARE)
	    {
		if(crtTk->code == EQUAL || crtTk->code == NOTEQ || crtTk->code == LESS || crtTk->code == LESSEQ || crtTk->code == GREATER || crtTk->code == GREATEREQ || crtTk->code == AND || crtTk->code == OR || crtTk->code == SEMICOLON || crtTk->code == RPAR)
		{
		    // not to have 'x+>'
		    crtTk=crtTk->prev;
		    if(consume_operator())
			tkerr(crtTk,"Invalid operator here");
		    crtTk=crtTk->next;
		    return 1;
		}
	    }

	    if(crtTk->code == stop_code1 || crtTk->code == stop_code2) //end of simple expression
	    {
		// not to have 'x+;'
		crtTk=crtTk->prev;
		if(consume_operator())
		    tkerr(crtTk,"Invalid operator here");
		crtTk=crtTk->next;
		return 1;
	    }

	    if(simple_expr(stop_code1,stop_code2)) //another simple expression
	    {
		return 1;
	    }else tkerr(crtTk,"Missing expression");

	}else{

	//same as up here

	   if(!(stop_code1 == RPAR && (crtTk->code == stop_code1 || crtTk->code == stop_code2)))
		parenthesys_analyzer(0);

	    if(stop_code1 == DECLARATION)
	    {
	    
		if(crtTk->code == COMMA || crtTk->code == SEMICOLON || crtTk->code == ASSIGN)
		{
		    return 1;
		}
	    }

	    if(stop_code1 == COMPARE)
	    {
		if(crtTk->code == EQUAL || crtTk->code == NOTEQ || crtTk->code == LESS || crtTk->code == LESSEQ || crtTk->code == GREATER || crtTk->code == GREATEREQ || crtTk->code == AND || crtTk->code == OR || crtTk->code == SEMICOLON || crtTk->code == RPAR) //end of simple expression
		{
		    return 1;
		}
	    }

	    if(crtTk->code == stop_code1 || crtTk->code == stop_code2)
	    {
		return 1;
	    }else tkerr(crtTk,"Invalid operator");

	}
    }else tkerr(crtTk,"Missing variable/value");

    crtTk=startTk;
    return 0;
}

//function to consume a line of variables declaration
int var_decl_line()
{
    Token *startTk=crtTk;
    if(simple_decl()) // 'int x'
    {
	while(consume(COMMA) || consume(ASSIGN))
	{
	    if(crtTk->prev->code == COMMA) // 'int x, '
	    {
		if(consume(MUL)){ // 'int x,*p'
		    if(!consume(ID))
			tkerr(crtTk,"Invalid pointer name after *");
		}

		else if(crtTk->code == ID) // 'int x, p'
		{
		    if(checkif_vector()) // 'int x,p['
		    {
			if(crtTk->next->code==LBRACKET)
			{
			    if(crtTk->next->next->code==RBRACKET) // 'int x,p[]'
			    {
				crtTk=crtTk->next->next->next;
			    }
			    else
			    {
				if(!consume_vector_el()) //'int x,p[simple_expr]'
				    tkerr(crtTk,"Error in vector declaration");
			    }
			}
		    }
		    else
		    {
			consume(ID); //'int x,p'
		    }
		}else tkerr(crtTk,"Missing variable");
	    }
	    else if(crtTk->prev->code == ASSIGN) //'int x ='
	    {
	    if(!simple_expr(DECLARATION,None))
		 tkerr(crtTk,"Missing expression");
	    }
	}
	if(crtTk->code == SEMICOLON)
	    return 1;
    } else tkerr(crtTk,"Error in declaration of types");
    crtTk=startTk;
    return 0;
}

//function to consume a line of variables declaration
int struct_decl_line()
{
    Token *startTk=crtTk;
    if(consume(MUL)){ // 'x,*p'
	if(!consume(ID))
	    tkerr(crtTk,"Invalid pointer name after *");
		    }
	else if(crtTk->code == ID) // 'int x, p'
	    {
		if(checkif_vector()) // 'int x,p['
		{
		    if(crtTk->next->code==LBRACKET)
		    {
			if(crtTk->next->next->code==RBRACKET) // 'x,p[]'
			{
			    crtTk=crtTk->next->next->next;
			}
			else
			{
			    if(!consume_vector_el()) //'x,p[simple_expr]'
				tkerr(crtTk,"Error in vector declaration");
			}
		    }
		}
		else
		{
		    consume(ID); //'x,p'
		}
		}else tkerr(crtTk,"Missing variable");
	while(consume(COMMA))
	{
		if(consume(MUL)){ // 'x,*p'
		    if(!consume(ID))
			tkerr(crtTk,"Invalid pointer name after *");
		}

		else if(crtTk->code == ID) // 'x, p'
		{
		    if(checkif_vector()) // 'x,p['
		    {
			if(crtTk->next->code==LBRACKET)
			{
			    if(crtTk->next->next->code==RBRACKET) // 'x,p[]'
			    {
				crtTk=crtTk->next->next->next;
			    }
			    else
			    {
				if(!consume_vector_el()) //'x,p[simple_expr]'
				    tkerr(crtTk,"Error in vector declaration");
			    }
			}
		    }
		    else
		    {
			consume(ID); //'x,p'
		    }
		}else tkerr(crtTk,"Missing variable");
	}
	if(crtTk->code == SEMICOLON)
	    return 1;
    crtTk=startTk;
    return 0;
}


//function to consume an assign 'x=3' or 'x=x+1' or 'x=f(4)'
int simple_assign()
{
    Token *startTk=crtTk;
    int ok = 0;
    if(consume_vector_el() || consume(ID)){
	while(consume(ASSIGN)){
	    simple_expr(SEMICOLON, ASSIGN);
	    ok = 1;
	    }
	if(ok)
	    return 1;
	else
	    tkerr(crtTk,"Missing = or invalid expression after =");
    } else tkerr(crtTk,"Missing variable");
    crtTk=startTk;
    return 0;
}

//function to consume an assign 'x=3)' or 'x=x+1)' or 'x=f(4))'
int simple_assign_for()
{
    Token *startTk=crtTk;
    if(consume(ID)){
	if(consume(ASSIGN)){
	    if(simple_expr(RPAR, None)){
		return 1;
	    } else tkerr(crtTk,"Missing expression after =");
	} else tkerr(crtTk,"Missing = in assign statement");
    } else tkerr(crtTk,"Missing variable");
    crtTk=startTk;
    return 0;
}

int single_instr();

// function to consume return statement 'return x' or 'return f(x)' or 'return'
int return_statement()
{
    Token *startTk=crtTk;
    if(consume(RETURN)){
	if(crtTk->code == SEMICOLON) // return;
	    return 1;
	if(single_instr()){ // return simple_expr;
	    return 1;
	} else tkerr(crtTk,"Invalid expression in return statement");
    } else tkerr(crtTk, "Missing RETURN");
    crtTk=startTk;
    return 0;
}

// verify if we have in the next sequence of tokens a comparison
int is_comp()
{
    Token *tk=crtTk;
    while(tk != NULL && tk->code != SEMICOLON)
    {
	if(tk->code == EQUAL || tk->code == NOTEQ || tk->code == LESS || tk->code == LESSEQ || tk->code == GREATER || tk->code == GREATEREQ)
	    return 1;
	tk=tk->next;
    }
    return 0;
}

//function used to see the next tokens and predict the following rule to apply
int chooser()
{
    Token *startTk=crtTk;

    /* Legend:

	0 - error/invalid
	1 - declaration of variables -> var_decl_line()
	2 - function calls -> function_call()
	3 - simple assign -> simple_assign()
	4 - return statement -> return_statement()
	5 - while statement -> while_statement()
	6 - for statement -> for_statement()
	7 - break keyword -> skip it + semicolon
	8 - if statement -> if_statement()
	9 - comparation statement -> cond()
	10 - simple expression -> simple_expr()
	11 - structure declaration -> struct_decl()
	12 - function definition -> 
    */

    //we remove any possible typecast
    typecast();
    parenthesys_analyzer(0);

    if(crtTk->code == INT || crtTk->code == DOUBLE || crtTk->code == CHAR || (crtTk->code == STRUCT && crtTk->next->code == ID)) // 'int' 'double' 'struct struct_name'
    {
	if(crtTk->code == STRUCT && crtTk->next->code == ID){ // 'struct struct_name'
		if(crtTk->next->next->code == LACC)
		{
			return 11;
		}
		else if(crtTk->next->next->code == ID)
		{
			if(crtTk->next->next->next->code == LPAR)
			{
				return 2;
			}
			else return 1;
		}	
			
	}
	else if(crtTk->next->code == ID) // 'int x' or 'double f'
	{
	    if(crtTk->next->next->code == LPAR) // 'int f(' 'double f('
	    {
	    	return 12;
	    }
	    else if(crtTk->next->next->code == SEMICOLON || crtTk->next->next->code == ASSIGN || crtTk->next->next->code == COMMA || crtTk->next->next->code == LBRACKET) // 'int x=' or 'int x;' or 'int x,' or 'int x['
	    {
		return 1;
	    }
	}
    }
    else if(crtTk->code == RETURN){
	return 4;
    }
    else if(crtTk->code == WHILE){
	return 5;
    }
    else if(crtTk->code == FOR){
	return 6;
    }
    else if(crtTk->code == BREAK){
	return 7;
    }
    else if(crtTk->code == IF){
	return 8;
    }
    else if(crtTk->code == ID) // 'f' or 'x'
    {
	if(crtTk->next->code == LPAR) // 'f(' 
	    return 2;
	if(crtTk->next->code == ASSIGN) // 'x='
	    return 3;
	if(checkif_vector()) // 'v['
	    return 3;
    }

    if(is_comp())
	return 9;

    if(crtTk->code == ID)
	return 10;

    crtTk=startTk;
    return 0;
}

//simple declaration or declaration with assignment
int decl()
{
    Token *startTk=crtTk;
    if(var_decl_line()){
	while(consume(SEMICOLON)){
	    if(crtTk->code==RACC){
		crtTk=crtTk->prev; //we go back to the last SEMICOLON to help us in the other functions
		return 1;
		}
	    if(!var_decl_line())
		tkerr(crtTk,"Error in declaration of variables");
	}
    }else tkerr(crtTk,"Missing element in declaration");
    crtTk=startTk;
    return 0;
}

int while_statement();
int for_statement();
int if_statement();
int struct_decl();
int function_prototype();

//simple instruction, one line
int single_instr()
{
    switch(chooser())
    {
	case 1: if(!var_decl_line()) tkerr(crtTk,"Error in declaration"); break;
	case 2: if(!function_call()) tkerr(crtTk,"Error in function call"); break;
	case 3: if(!simple_assign()) tkerr(crtTk,"Error in assign statement"); break;
	case 4: if(!return_statement()) tkerr(crtTk,"Error in return statement"); break;
	case 5: if(!while_statement()) tkerr(crtTk,"Error in while structure"); break;
	case 6: if(!for_statement()) tkerr(crtTk,"Error in the for structure"); break;
	// in case we find BREAK we just verify the SEMICOLON and move on
	case 7: consume(BREAK); if(!consume(SEMICOLON)) tkerr(crtTk,"Missing semicolon after BREAK"); return single_instr();
	case 8: if(!if_statement()) tkerr(crtTk,"Error in the if statement"); break;
	case 9: if(!cond()) tkerr(crtTk,"Error in the conditional statement"); break;
	case 10: if(!simple_expr(SEMICOLON,None)) tkerr(crtTk,"Error in the expression"); break;
	case 11: if(!struct_decl()) tkerr(crtTk,"Error in structure declaration"); break;
	case 12: if(!function_prototype()) tkerr(crtTk,"Error in function prototype"); break;
	default: tkerr(crtTk,"Invalid instruction"); break;
    }

    if(parenthesys_analyzer(1)!=0) //we see if the parenthesys are used correctly
	tkerr(crtTk,"Parenthesys closed or open incorrectly");

    return 1;
}

//multiple simple instructions, multiple lines
int instr()
{
    if(single_instr()){
	while(consume(SEMICOLON) || crtTk->prev->code == RACC){
	    if(crtTk->code==RACC)
		    return 1;
	    if(!single_instr())
		tkerr(crtTk,"Missing instruction");
	}
	if(crtTk->code == RACC)
	    return 1;
    }else tkerr(crtTk,"Invalid argument");

    return 0;
}

//body of functions or while, for, etc.
int body(){
    Token *startTk=crtTk;
    if(consume(LACC)){
	if(instr()){
	    if(consume(RACC)){
		return 1;
	    }else tkerr(crtTk,"missing } after body");
	}else tkerr(crtTk,"invalid instruction in body");
    }else tkerr(crtTk,"missing { before body");
    crtTk=startTk;
    return 0;
}

int while_statement()
{
    Token *startTk=crtTk;
    if(consume(WHILE)){
	if(consume(LPAR)){
	    if(cond()){
		if(consume(RPAR)){ // 'while(cond)'

		    if(crtTk->code == LACC){ //while(cond) { }
			if(body()){
				return 1;
			} else tkerr(crtTk,"Error in body");
		    }
		    else if(single_instr()){ //while(cond) single_instruction
			return 1;
		    }
		    else tkerr(crtTk,"Invalid body/instruction for while");

		 }else tkerr(crtTk,"Missing ( after WHILE");
	    }else tkerr(crtTk,"Error in condition");
	}else tkerr(crtTk,"Missing ( after WHILE");
    }else tkerr(crtTk,"Missing WHILE statement");
    crtTk=startTk;
    return 0;
}

int if_statement()
{
    Token *startTk=crtTk;
    if(consume(IF)){
	if(consume(LPAR)){
	    if(cond()){
		if(consume(RPAR)){ // if (cond)

		    if(crtTk->code == LACC){
			if(body()){ //if (cond) { }
			    if(consume(ELSE)){

				//if (cond) { } else
				if(crtTk->code == LACC){
				    if(body()){ //if (cond) { } else { }
					return 1;
				    } else tkerr(crtTk,"Error in body in else branch of IF");

				}else if(single_instr()){ //if (cond) { } else single_instruction
				    return 1;

				}else tkerr(crtTk,"Error in body in else branch of IF");

			    } else return 1; //if (cond){ }
			}else tkerr(crtTk,"Error in body in if-true branch of IF");
		    }

		    else if(single_instr())
		    {
			if(crtTk->next->code == ELSE){
			    if(!consume(SEMICOLON))
				tkerr(crtTk,"Missing semicolon");
			    consume(ELSE);

			//if (cond) single_instruction else
			if(crtTk->code == LACC){
			    if(body()){ //if (cond) single_instruction else { }
				return 1;
			    } else tkerr(crtTk,"Error in body in else branch of IF");

			}else if(single_instr()){ //if (cond) single_instruction else single_instruction
			    return 1;

			}else tkerr(crtTk,"Error in body in else branch of IF");

			}else return 1; //if (cond) single_instruction
		    }
		    else tkerr(crtTk,"Error in if-true branch of IF");

		} else tkerr(crtTk,"Missing ) after condition of IF");
	    }else tkerr(crtTk,"Error in condition after (");
	}else tkerr(crtTk,"Missing ( after IF");
    }else tkerr(crtTk,"Missing IF statement");
    crtTk=startTk;
    return 0;
}

int for_statement()
{
    Token *startTk=crtTk;
    if(consume(FOR)){
	if(consume(LPAR)){
	    if(chooser()==3){ //simple assign

		if(simple_assign()){
		    if(consume(SEMICOLON)){
			if(cond()){
			    if(consume(SEMICOLON)){
				if(simple_assign_for()){
				    if(consume(RPAR)){ //for(init,cond,modif)
					
					if(crtTk->code == LACC){ //for(init,cond,modif) { }
					    if(body()){
						return 1;
					    } else tkerr(crtTk,"Error in body");
					}
					else if(single_instr()){ //for(init,cond,modif) single_instruction
					    return 1;
					}
					else tkerr(crtTk,"Invalid body/instruction in for");
					
				    }else tkerr(crtTk,"Missing ) after variable modification in for");
				}else tkerr(crtTk,"Error in modification of the variable in for");
			    }else tkerr(crtTk,"Missing SEMICOLON after condition in for");
			}else tkerr(crtTk,"Error in condition in for");
		    } else tkerr(crtTk,"Missing SEMICOLON after assignment in for");
		 }else tkerr(crtTk,"Error in assign statement in for");

	    }else if(chooser() ==1){ //variable declaration

		if(var_decl_line()){
		    if(consume(SEMICOLON)){
			if(cond()){
			    if(consume(SEMICOLON)){
				if(simple_assign_for()){
				    if(consume(RPAR)){
					
					if(crtTk->code == LACC){ //for(init,cond,modif) { }
					    if(body()){
						return 1;
					    } else tkerr(crtTk,"Error in body");
					}
					else if(single_instr()){ //for(init,cond,modif) single_instruction
					    return 1;
					}
					else tkerr(crtTk,"Invalid body/instruction in for");
					
				    }else tkerr(crtTk,"Missing ) after variable modification in for");
				}else tkerr(crtTk,"Error in modification of the variable in for");
			    }else tkerr(crtTk,"Missing SEMICOLON after condition in for");
			}else tkerr(crtTk,"Error in condition in for");
		    } else tkerr(crtTk,"Missing SEMICOLON after assignment in for");
		 }else tkerr(crtTk,"Error in assign statement in for");

	    }else tkerr(crtTk,"Invalid initialization of variable in for");

	}else tkerr(crtTk,"Missing ( after FOR");
    }else tkerr(crtTk,"Missing FOR statement");
    crtTk=startTk;
    return 0;
}

//structure declaration
int struct_decl()
{
    Token *startTk=crtTk;
    if(consume(STRUCT)){
	if(consume(ID)){
	    if(consume(LACC)){
		if(decl() && consume(SEMICOLON)){
		    if(consume(RACC)){
			    return 1;
		     }else tkerr(crtTk,"Missing } after variable declarations");
		 }else tkerr(crtTk,"Error in variable declaration in structure");
	    }else tkerr(crtTk,"Missing { after variable name");
	}else tkerr(crtTk,"Missing struct name");
    }else tkerr(crtTk,"Missing STRUCT statement");
    crtTk=startTk;
    return 0;
}

//helper function to help us build the function prototype, this function is simple_expr
//expression for multiple simple mathematical operations EX: 'i+2' or 'x+3+7' or 'z/4+f' or "this is a string" or "f(x)+3"
int simple_expr_fprototype()
{
    Token *startTk=crtTk;
    if(type()){
	consume(MUL);
	if(consume(ID))
	{
	    if(consume(LPAR))
	    {
		if(!consume(RPAR))
		    tkerr(crtTk,"Missing ) in argument list");
	    }
	    if(consume(LBRACKET))
	    {
		consume(CT_INT);
		if(!consume(RBRACKET))
		    tkerr(crtTk,"Missing ] in argument list");
	    }
	}else tkerr(crtTk,"Missing variable name");
    }else tkerr(crtTk,"Missing variable type");

    if(crtTk->code == COMMA || crtTk->code == RPAR) //end of argument
    {
	return 1;
    } else tkerr(crtTk,"Invalid character atfer argument");
    crtTk=startTk;
    return 0;
}

//single argument for a function prototype
int arg_prototype()
{
    Token *startTk=crtTk;
    if(simple_expr_fprototype()){
	return 1;
    }else tkerr(crtTk,"Error in argument");
    crtTk=startTk;
    return 0;
}

//list of arguments for a function prototype
int arg_list_prototype()
{
    Token *startTk=crtTk;
    if(crtTk->code == RPAR){
	return 1;
    }
    if(arg_prototype()){
	while(consume(COMMA)){
	    if(!arg_prototype())
		tkerr(crtTk,"Missing argument");
	}
	if(crtTk->code==RPAR)
	    return 1;
    }else tkerr(crtTk,"Missing argument");
    crtTk=startTk;
    return 0;
}

//used to identify functions prototypes
int function_prototype()
{
    Token *startTk=crtTk;
    if(consume(VOID) || type()){
	if(consume(ID)){
	    if(consume(LPAR)){
		if(arg_list_prototype()){
		    if(consume(RPAR)){
			if(body()){
			    return 1;
			}else tkerr(crtTk,"Error in function body");
		    }else tkerr(crtTk,"Missing ) after arguments list");
		}else tkerr(crtTk,"Argument list error");
	    }else tkerr(crtTk,"Missing ( after func");
	}else tkerr(crtTk,"Missing function name");
    }else tkerr(crtTk,"Missing return type");
    crtTk=startTk;
    return 0;
}

//remove abuiguity & left recursivity
void ambiguos_and_LeftRecursivity(){
    crtTk=root;

    Token* crtFunc = NULL;

    while(crtTk!=NULL)
    {
	if(type() && crtTk->code == ID && crtTk->next->code == LPAR)
	{
	    crtFunc = crtTk;
	}

	//we find a recursive call
	if(crtFunc != NULL && crtTk->code == ID && strcmp(crtTk->text,crtFunc->text)==0 && crtTk!=crtFunc)
	{
	    while(!consume(RPAR))
		crtTk=crtTk->next;
	    if(consume(AND) || consume(OR))
	    {
		tkerr(crtTk,"Left recursivity found");
	    }
	}

	//ambiguities of kind constant = something are removed in the expression function

	crtTk=crtTk->next;
    }
}

//main structure of the syntactical analyzer
int syntactical_analyzer(){
    crtTk=root;

    // we identify variable declaration, structure + declaration, function declaration
    while(1)
    {
	if(struct_type())
	{
	    if(crtTk->code == ID)
	    {
		if(crtTk->next->code == ASSIGN || crtTk->next->code == SEMICOLON || crtTk->next->code == COMMA || crtTk->next->code == LBRACKET) //struct variable declaration declaration
		{
		crtTk=crtTk->prev;
		crtTk=crtTk->prev;
		if(!var_decl_line())
		    return 0;
		consume(SEMICOLON);
		}
		else if(crtTk->next->code == LPAR) //struct function
		{
		    crtTk=crtTk->prev;
		    crtTk=crtTk->prev;
		    if(!function_prototype())
			return 0;
		}
		else return 0;
	    }
	    else if(crtTk->code == LACC) //struct declaration
	    {
		crtTk=crtTk->prev;
		crtTk=crtTk->prev;
		if(!struct_decl())
		    return 0;
		if(!consume(SEMICOLON))
		{
			if(!struct_decl_line()) // simple declaration
			return 0;
		}
		consume(SEMICOLON);
	    }
	    
	    else return 0;
	}
	else if(type() || consume(VOID))
	{
	    if(crtTk->code == ID)
	    {
		if(crtTk->next->code == LPAR) //simple function
		{
		    crtTk=crtTk->prev;
		    if(!function_prototype())
			return 0;
		}
		else
		{
		    crtTk=crtTk->prev;
		    if(!var_decl_line()) // simple declaration
			return 0;
		    consume(SEMICOLON);
		}
	    }
	    else
		return 0;
	}
	else if(consume(END))
	{
	    ambiguos_and_LeftRecursivity();
	    return 1;
	}
	else
	{
	    tkerr(crtTk,"Unexpected token");
	    return 0; //Syntactical error
	}
    }

    ambiguos_and_LeftRecursivity();
    return 1;
}

/*						*
 *	Domain Analysis & Table of Symbols	*
 *						*/

typedef struct Symbol{
    Token *tk; // a reference to the token from which we extracted the symbol name
    char *name; // a reference to the name stored in a token
    int cls; // class = what exactly is
    int type; // type of the symbol
    char *struct_name; // struct name only for struct variable
    int depth; // 0-global, 1-in function, 2... - nested blocks in function
    int line; // line of the symbol
    int size; // for vectors or structures (the number of tokens after the LBRACKET which compose the size)
    union{
    struct Symbol* args[15]; // used only of functions
    struct Symbol* fields[15]; // used only for structs
    };
    int nr_argsORmembers; // nr of arguments or members only for functions and structs
    struct Symbol* next; // next symbol in the table
    struct Symbol* prev; // previous symbol in the table
}Symbol;

Symbol* Symbol_root = NULL;
Symbol* crtSymbol = NULL;

//Safely allocate a symbol
Symbol* SafeAllocSymbol()
{
    Symbol *block;
    block = (Symbol*)malloc(sizeof(Symbol));
    if(block == NULL)
	err("not enough memory");
    return block;
}

enum Class { FUNCTION, VARIABLE, VECTOR, FUNCTION_ARGUMENT, STRUCT_FIELD, STRUCT_FIELD_VECTOR, FUNCTION_ARGUMENT_VECTOR };
enum Type { _INT, _DOUBLE, _CHAR, _STRUCT, _VOID };

//returns the first symbol from the current and relevant depth
Symbol *start_last_depth(int my_depth)
{
    Symbol *sy=Symbol_root;

    //case empty list
    if(sy==NULL)
	return NULL;

    //case list with 2 or more elements

    //we find the last element
    while(sy->next != NULL)
	sy=sy->next;

    //we find the first element of the current depth
    while(sy!=NULL && sy->depth == my_depth && sy->line >= 0)
    {
	sy=sy->prev;
    }

    //if the last element is predefined function
    if(sy->line == -1)
	return NULL;

    return sy;
}

//adding a symbol to the table
int addSymbol(Token** its_token, char *its_name, int its_cls, int its_type, int its_depth, int its_line)
{
    //checking for already declared variables
    Symbol *sy_trav=start_last_depth(its_depth);
    while(sy_trav != NULL)
    {
	if(strcmp(its_name,sy_trav->name)==0)
	{
	    printf("Error, %s already present in this scope: line %d\n",its_name,(*(its_token))->line);
	    return 0;
	}
	sy_trav=sy_trav->next;
    }

    //allocating and storing
    Symbol *sy = SafeAllocSymbol();
    sy->tk = crtTk;
    sy->name = its_name;
    sy->cls = its_cls;
    sy->type = its_type;
    sy->depth = its_depth;
    sy->line = its_line;

    //adding to the table
    if(crtSymbol==NULL)
    {
	sy->prev=NULL;
	Symbol_root = sy;
    }
    else
	{
	    sy->prev=crtSymbol;
	    crtSymbol->next=sy;
	}
    crtSymbol = sy;

    return 1;
}

//printing a class
char *print_class(int cls)
{
    switch(cls)
    {
	case FUNCTION: return "FUNCTION";
	case VARIABLE: return "VARIABLE";
	case VECTOR: return "VECTOR";
	case FUNCTION_ARGUMENT: return "FUNCTION ARGUMENT";
	case STRUCT_FIELD: return "STRUCTURE FIELD";
	case STRUCT_FIELD_VECTOR: return "STRUCTURE FIELD VECTOR";
	case FUNCTION_ARGUMENT_VECTOR: return "FUNCTION ARGUMENT VECTOR";
	default: return "NOT FOUND";
    }
}

//printing a type
char *print_type(int type)
{
    switch(type)
    {
	case _INT: return "INTEGER";
	case _DOUBLE: return "DOUBLE";
	case _CHAR: return "CHAR";
	case _VOID: return "VOID";
	case _STRUCT: return "STRUCT";
	default: return "NOT FOUND";
    }
}

//printing the actual size
void actual_size(Symbol* sy)
{
    int curr_size = 0;
    char string[20] = "";
    Token* curr_tk = sy->tk;
    curr_tk=curr_tk->next->next;
    while(curr_size != sy->size)
    {
	if(curr_tk->code == ID)
	    strcat(string,curr_tk->text);
	if(curr_tk->code == CT_INT)
	    sprintf(string+strlen(string),"%ld",curr_tk->i);
	if(curr_tk->code == CT_REAL)
	    sprintf(string+strlen(string),"%lf",curr_tk->r);
	if(curr_tk->code == SUB)
	    strcat(string,"-");
	if(curr_tk->code == ADD)
	    strcat(string,"+");
	if(curr_tk->code == MUL)
	    strcat(string,"*");
	if(curr_tk->code == DIV)
	    strcat(string,"/");
	if(curr_tk->code == LPAR)
	    strcat(string,"(");
	if(curr_tk->code == RPAR)
	    strcat(string,")");
	if(curr_tk->code == DOT)
	    strcat(string,".");
	curr_tk=curr_tk->next;
	curr_size++;
    }
    printf("%s",string);
}

//function for printing the table
void print_Symbols()
{
    Symbol *sy=Symbol_root;
    printf("\n\tSymbol Table:\n\n");
    while(sy != NULL)
    {
	if(sy!=Symbol_root && (sy->prev->line==-1 && sy->line >= 0))
	    printf("--------------------------------------------------------------------------------\n");
	printf("Name:%s - Class:%s - ", sy->name, print_class(sy->cls));
	if(sy->cls == STRUCT_FIELD || sy->cls == STRUCT_FIELD_VECTOR || sy->type == _STRUCT)
	    printf("Struct name: %s - ",sy->struct_name);
	printf("Type:%s - Depth:%d - Line:%d", print_type(sy->type), sy->depth, sy->line);
	if(sy->cls == FUNCTION)
	{
	    printf(" - Nr Args:%d",sy->nr_argsORmembers);
	    if(sy->nr_argsORmembers!=0)
		printf("\n\t");
	    for(int i=0; i<(sy->nr_argsORmembers); i++)
		printf("arg[%d] = %s; ",i,sy->args[i]->name);
	    printf("\n");
	}
	else if(sy->cls == VECTOR || sy->cls == STRUCT_FIELD_VECTOR)
	{
	    printf(" - Size:");
	    actual_size(sy);
	    printf("\n");
	}
	else
	    printf("\n");
	sy=sy->next;
    }
}

#define NEXT_TK crtTk=crtTk->next;
#define IF_NOT_CORRECT_EXIT if(correctness == 0) return 0;

//verify if the crtTk is a certain code
int crtTk_is(int this_code)
{
    if(crtTk->code == this_code)
	return 1;
    else
	return 0;
}

//verify if the given token is a certain code
int Tk_is(Token* tk, int this_code)
{
    if(tk->code == this_code)
	return 1;
    else
	return 0;
}

//apparition of a type EX: 'int' or 'double' or 'char' or 'struct s' or 'void' but without consume
int if_is_type(Token *tk)
{
    if(Tk_is(tk, INT) || Tk_is(tk, DOUBLE) || Tk_is(tk, CHAR) || Tk_is(tk, VOID) || ( tk->prev != NULL && Tk_is(tk->prev, STRUCT)))
    {
	return 1;
    }
    return 0;
}

//returns the class of a simple token
int symbol_class(Token *tk)
{
    if(tk->next->code == LPAR)
	return FUNCTION;
    if(tk->next->code == LBRACKET)
	return VECTOR;
    return VARIABLE;
}

//returns the type of a simple token
int symbol_type(Token *tk)
{
    if(tk->prev->code == ID)
    {
	return _STRUCT;
    }
    else
	if(tk->prev->code == INT)
	    return _INT;
	else
	if(tk->prev->code == CHAR)
	    return _CHAR;
	else
	if(tk->prev->code == DOUBLE)
	    return _DOUBLE;
	else
	if(tk->prev->code == VOID)
	    return _VOID;
    return _VOID;
}

//helper function to store size and process vectors of any type
void vector_TS()
{
    if(crtSymbol->cls == STRUCT_FIELD && crtTk->code == LBRACKET)
	crtSymbol->cls = STRUCT_FIELD_VECTOR;
    if(crtSymbol->cls == FUNCTION_ARGUMENT && crtTk->code == LBRACKET)
	crtSymbol->cls = FUNCTION_ARGUMENT_VECTOR;
    if(crtSymbol->cls == VECTOR || crtSymbol->cls == STRUCT_FIELD_VECTOR || crtSymbol->cls == FUNCTION_ARGUMENT_VECTOR)
    {
	consume(LBRACKET);
	int curr_size = 0;
	/* in case the size is a more complex value even if the TS will record an incorrect one 
		the program will continue to operate normally		ex: v[3*5+a] */
	while(!crtTk_is(RBRACKET))
	{
	    curr_size++;
	    NEXT_TK;
	}
	crtSymbol->size = curr_size;
	consume(RBRACKET);
    }
}

// function to verify if a certain variable is in the symbol table
int if_symbol_in_table(char *variable_name)
{
    Symbol *sy = Symbol_root;
    while(sy != NULL)
    {
	if(strcmp(sy->name,variable_name)==0)
	    return 1;
	sy=sy->next;
    }
    return 0;
}

// adding predifined functions - Types analysis function
void add_predifined_func()
{
    addSymbol(NULL,"put_s",FUNCTION,_VOID,0,-1);
    addSymbol(NULL,"c",FUNCTION_ARGUMENT_VECTOR,CHAR,0,-1);

    (crtSymbol->prev)->nr_argsORmembers=1;
    (crtSymbol->prev)->args[0]=crtSymbol;


    addSymbol(NULL,"get_s",FUNCTION,_VOID,0,-1);
    addSymbol(NULL,"c",FUNCTION_ARGUMENT_VECTOR,CHAR,0,-1);

    (crtSymbol->prev)->nr_argsORmembers=1;
    (crtSymbol->prev)->args[0]=crtSymbol;


    addSymbol(NULL,"put_i",FUNCTION,_VOID,0,-1);
    addSymbol(NULL,"i",FUNCTION_ARGUMENT,_INT,0,-1);

    (crtSymbol->prev)->nr_argsORmembers=1;
    (crtSymbol->prev)->args[0]=crtSymbol;


    addSymbol(NULL,"get_i",FUNCTION,_INT,0,-1);


    addSymbol(NULL,"put_d",FUNCTION,_VOID,0,-1);
    addSymbol(NULL,"d",FUNCTION_ARGUMENT,_DOUBLE,0,-1);

    (crtSymbol->prev)->nr_argsORmembers=1;
    (crtSymbol->prev)->args[0]=crtSymbol;


    addSymbol(NULL,"get_d",FUNCTION,_DOUBLE,0,-1);

    addSymbol(NULL,"put_c",FUNCTION,_VOID,0,-1);
    addSymbol(NULL,"c",FUNCTION_ARGUMENT,_CHAR,0,-1);

    (crtSymbol->prev)->nr_argsORmembers=1;
    (crtSymbol->prev)->args[0]=crtSymbol;


    addSymbol(NULL,"get_c",FUNCTION,_CHAR,0,-1);

    addSymbol(NULL,"seconds",FUNCTION,_DOUBLE,0,-1);
}

//main structure of the domain analysis and table of symbols analyzer
int domain_and_symbols()
{
    add_predifined_func();

    crtTk=root;
    int correctness = 1;
    int depth = 0;

    while(crtTk != NULL)
    {
	if(crtTk_is(LACC))
	    depth++;
	if(crtTk_is(RACC))
	    depth--;

	// symbol found
	if(type() || consume(VOID))
	{
	    // structure
	    if(consume(LACC))
	    {
		char *structure_name = crtTk->prev->prev->text;

		//structure declaration
		while(!crtTk_is(RACC))
		{
		    //now we store only structure fields
		    type();
		    int curr_type = symbol_type(crtTk);
		    correctness = addSymbol(&crtTk,crtTk->text,STRUCT_FIELD,curr_type,depth,crtTk->line);
		    NEXT_TK
		    IF_NOT_CORRECT_EXIT
		    crtSymbol->struct_name = structure_name;
		    vector_TS();

		    while(crtTk_is(COMMA))
		    {
			NEXT_TK
			correctness = addSymbol(&crtTk,crtTk->text,STRUCT_FIELD,curr_type,depth,crtTk->line);
			NEXT_TK
			IF_NOT_CORRECT_EXIT
			crtSymbol->struct_name = structure_name;

			vector_TS();
		    }
		    consume(SEMICOLON);
		}
		consume(RACC);
		//we may have variable instatiation right after declaration
		if(!crtTk_is(SEMICOLON))
		{
		    correctness = addSymbol(&crtTk,crtTk->text,VARIABLE,_STRUCT,depth,crtTk->line);
		    crtSymbol->struct_name = structure_name;
		    NEXT_TK
		    IF_NOT_CORRECT_EXIT
		    while(crtTk_is(COMMA))
		    {
			NEXT_TK
			correctness = addSymbol(&crtTk,crtTk->text,VARIABLE,_STRUCT,depth,crtTk->line);
			crtSymbol->struct_name = structure_name;
			NEXT_TK
			IF_NOT_CORRECT_EXIT
			vector_TS();
		    }
		    vector_TS();
		}
		consume(SEMICOLON);
	    }
	    else
	    // function or simple variable
	    {
		int curr_type = symbol_type(crtTk);
		int curr_class = symbol_class(crtTk);
		char *structure_Name;
		correctness = addSymbol(&crtTk,crtTk->text,curr_class,curr_type,depth,crtTk->line);
		if(crtSymbol->type == _STRUCT)
		{
		    structure_Name = crtTk->prev->text;
		    crtSymbol->struct_name = structure_Name;
		}
		IF_NOT_CORRECT_EXIT
		NEXT_TK

		// function
		if(curr_class == FUNCTION)
		{
		    consume(LPAR);
		    Symbol *function=crtSymbol;
		    int nr_arg=0;

		    // function arguments
		    while(type() || consume(VOID))
		    {
			correctness = addSymbol(&crtTk,crtTk->text,FUNCTION_ARGUMENT,symbol_type(crtTk),depth,crtTk->line);
			NEXT_TK
			IF_NOT_CORRECT_EXIT
			vector_TS();
			consume(COMMA);
			nr_arg++;
		    }

		    // modify function to include the argument list
		    function->nr_argsORmembers=nr_arg;
		    Symbol *arg=function->next;
		    for(int i=0;i<nr_arg;i++)
		    {
			function->args[i]=arg;
			arg=arg->next;
		    }
		}
		else
		    vector_TS();

		// simple variable
		while(crtTk_is(COMMA) && curr_class!=FUNCTION && curr_type!=STRUCT)
		{
		    NEXT_TK
		    correctness = addSymbol(&crtTk,crtTk->text,symbol_class(crtTk),curr_type,depth,crtTk->line);
		    if(crtSymbol->type == _STRUCT)
			crtSymbol->struct_name = structure_Name;
		    NEXT_TK

		    // vector
		    vector_TS();

		    IF_NOT_CORRECT_EXIT
		}
	    }

	}
	else
	    NEXT_TK
    }

    if(Symbol_root!=NULL)
    {
	if(DEVELOPER_OPTIONS)
	{
	    print_Symbols();
	}
    }
    else
	{
	    // no symbols this means error
	    printf("Error, no variable of function present in the file\n");
	    return 0;
	}

    //search for unused
    crtTk=root;
    while(crtTk != NULL)
    {
	if(crtTk_is(ID) && crtTk->prev->code != STRUCT && !if_symbol_in_table(crtTk->text))
	{
	    printf("\n%s is not declared in the current scope\n",crtTk->text);
	    return 0;
	}
	NEXT_TK;
    }

    return correctness;
}

/*						*
 *		 Types Analysis			*
 *						*/

 //printing atom function
char *print_code(int code)
{
    switch(code)
    {
	case ID: return "ID";
	case END: return "END";
	case INT: return "INT";
	case BREAK: return "BREAK";
	case CHAR: return "CHAR";
	case FOR: return "FOR";
	case COMMA: return "COMMA";
	case LPAR: return "LPAR";
	case RPAR: return "RPAR";
	case CT_INT: return "CT_INT";
	case LBRACKET: return "LBRACKET";
	case RBRACKET: return "RBRACKET";
	case LACC: return "LACC";
	case RACC: return "RACC";
	case IF: return "IF";
	case DOUBLE: return "DOUBLE";
	case ELSE: return "ELSE";
	case RETURN: return "RETURN";
	case STRUCT: return "STRUCT";
	case VOID: return "VOID";
	case WHILE: return "WHILE";
	case CT_REAL: return "CT_REAL";
	case EXP: return "EXP";
	case ESC: return "ESC";
	case CT_STRING: return "CT_STRING";
	case CT_CHAR: return "CT_CHAR";
	case ADD: return "ADD";
	case SUB: return "SUB";
	case DIV: return "DIV";
	case MUL: return "MUL";
	case DOT: return "DOT";
	case AND: return "AND";
	case OR: return "OR";
	case NOT: return "NOT";
	case ASSIGN: return "ASSIGN";
	case EQUAL: return "EQUAL";
	case NOTEQ: return "NOTEQ";
	case LESS: return "LESS";
	case LESSEQ: return "LESSEQ";
	case GREATER: return "GREATER";
	case GREATEREQ: return "GREATEREQ";
	case SPACE: return "SPACE";
	case LINECOMMENT: return "LINECOMMENT";
	case COMMENT: return "COMMENT";
	case SEMICOLON: return "SEMICOLON";
	default: return "NOT FOUND";
    }
}


//we find the symbol based on a certain token
Symbol* find_symbol(Token* tk)
{
    Symbol* sy=Symbol_root;
    Symbol* possible_match=NULL;

    //if we have a vector & we search left_operand
    if(tk->code == RBRACKET)
    {
	while(tk->code != LBRACKET)
	    tk=tk->prev;
	tk=tk->prev;
    }

    //we find the correct symbol
    while(sy!=NULL)
    {
	int line_prev_found=-1;

	//we search for the same name on the closest line to our token but not after it
	if(strcmp(sy->name, tk->text)==0 && line_prev_found<=sy->line && sy->line<=tk->line)
	{
	    possible_match=sy;
	    line_prev_found = sy->line;
	}
	sy=sy->next;
    }

    if(possible_match==NULL)
    {
	tkerr(tk,"Invalid token found");
	return Symbol_root;
    }
    else
	return possible_match;
}

//we verify if 2 types are compatible
int compatible_types(int type_l, int type_r, int line)
{
    if(type_l == _INT)
    {
	if(type_r == _INT)
	    return 1;
	else
	if(type_r == _CHAR || type_r == _DOUBLE)
	{
	    if(WARNINGS)
	    	printf("\nWarning at line %d: Implicit conversion from '%s' to '%s'\n",line, print_type(type_r), print_type(type_l));
	    return 1;
	}
	printf("\n\nImpossible to implicitly convert types at line %d\n",line);
	return 0;
    }

    if(type_l == _CHAR)
    {
	if(type_r == _CHAR)
	    return 1;
	else
	if(type_r == _INT || type_r == _DOUBLE)
	{
	    if(WARNINGS)
	    	printf("\nWarning at line %d: Implicit conversion from '%s' to '%s'\n",line, print_type(type_r), print_type(type_l));
	    return 1;
	}
	printf("\n\nImpossible to implicitly convert types at line %d\n",line);
	return 0;
    }

    if(type_l == _DOUBLE)
    {
	if(type_r == _DOUBLE)
	    return 1;
	else
	if(type_r == _CHAR || type_r == _INT)
	{
	    if(WARNINGS)
	    	printf("\nWarning at line %d: Implicit conversion from '%s' to '%s'\n",line, print_type(type_r), print_type(type_l));
	    return 1;
	}
	printf("\n\nImpossible to implicitly convert types at line %d\n",line);
	return 0;
    }

    printf("\n\nUnexpected type error at line %d\n",line);
    return 0;
}

//we verify if 2 classes are compatible
int compatible_classes(int class_l, int class_r, int line)
{
    if(class_l == VARIABLE || class_l == FUNCTION_ARGUMENT || class_l == STRUCT_FIELD)
    {
	if(class_r == VARIABLE || class_r == FUNCTION_ARGUMENT || class_r == STRUCT_FIELD)
	    return 1;
    }

    if(class_l == VECTOR || class_l == FUNCTION_ARGUMENT_VECTOR || class_l == STRUCT_FIELD_VECTOR)
    {
	if(class_r == VECTOR || class_r == FUNCTION_ARGUMENT_VECTOR || class_r == STRUCT_FIELD_VECTOR)
	    return 1;
    }

    if(class_r == FUNCTION)
	return 1;

    printf("Error at line %d: Incompatible left argument '%s' with right argument '%s'",line,print_class(class_l),print_class(class_r));
    return 0;
}

// verify if a vector is a vector element or a vector base pointer
int vector_element(Token* tk, int cls)
{
    if(cls == VECTOR || cls == FUNCTION_ARGUMENT_VECTOR || cls == STRUCT_FIELD_VECTOR)
    {
	tk=tk->next;
	if(Tk_is(tk,LBRACKET))
	{
	    while(!Tk_is(tk,RBRACKET))
		tk=tk->next;
	    tk=tk->next;
	    return VARIABLE;
	}
    }
    return cls;
}

//finds out the class of a token and returns it
int find_class(Token *tk)
{
    if(Tk_is(tk,ID))
    {
	// to find out if element or vector pointer from its tokens near him and his class
	return vector_element(tk,(find_symbol(tk))->cls);
    }
    if(Tk_is(tk,CT_INT))
	return VARIABLE;
    if(Tk_is(tk,CT_REAL))
	return VARIABLE;
    if(Tk_is(tk,CT_CHAR))
	return VARIABLE;
    if(Tk_is(tk,CT_STRING))
	return VECTOR;

    return 100;
}

//finds out the type of a token and returns it
int find_type(Token *tk)
{
    if(Tk_is(tk,ID))
	return (find_symbol(tk))->type;
    if(Tk_is(tk,CT_INT))
	return _INT;
    if(Tk_is(tk,CT_REAL))
	return _DOUBLE;
    if(Tk_is(tk,CT_CHAR))
	return _CHAR;
    if(Tk_is(tk,CT_STRING))
	return _CHAR;

    return 100;
}

// this helps us determine the class of a certain expression and its compatibility
int expr_cls(Token *tk, int stop_code1, int stop_code2, int class_l)
{
    int arg;

    //first term
    if(tk->code != stop_code1 && tk->code != stop_code2)
    {
	arg = find_class(tk);
	tk=tk->next;

	//verify comaptibility
	if(!compatible_classes(class_l, arg, tk->prev->line))
	    return -1;

	// found a function as a term
	if(tk->code == LPAR && (find_symbol(tk->prev))->cls == FUNCTION)
	{
	    while(tk->code!=RPAR)
		tk=tk->next;
	    tk=tk->next;
	}

	//found a vector as a term
	if(tk->code == LBRACKET)
	{
	    while(tk->code != RBRACKET)
		tk=tk->next;
	    tk=tk->next;
	}
    }

    //rest of the terms and operands
    while(!(tk->code == stop_code1 || tk->code == stop_code2))
    {
	if(tk->code == MUL || tk->code == ADD || tk->code == SUB || tk->code == DIV)
	    tk=tk->next;
	arg = find_class(tk);

	if(!compatible_classes(class_l, arg, tk->line))
	    return -1;

	tk=tk->next;

	// found a function as an argument
	if(tk->code == LPAR && (find_symbol(tk->prev))->cls == FUNCTION)
	{
	    while(tk->code!=RPAR)
		tk=tk->next;
	    tk=tk->next;
	}

	//found a vector as an argument
	if(tk->code == LBRACKET)
	{
	    while(tk->code != RBRACKET)
		tk=tk->next;
	    tk=tk->next;
	}

    }

    return class_l;
}

//expression type helps us analyze the type of an expression based on another type and see if conversions or errors needs to occur
int expr_type(Token *tk, int stop_code1, int stop_code2, int type_l, Token **func_end, Token **vector_end)
{
    int arg,conversion_needed=0;

    // argument
    if(tk->code != stop_code1 && tk->code != stop_code2)
    {
	arg = find_type(tk);
	tk=tk->next;

	//verify types
	if(!compatible_types(type_l, arg, tk->prev->line))
	    return -1;

	//verify if conversion is needed
	if(arg != type_l)
	    conversion_needed = 1;

	// found a function as an argument
	if(tk->code == LPAR && (find_symbol(tk->prev))->cls == FUNCTION)
	{
	    while(tk->code!=RPAR)
		tk=tk->next;
	    tk=tk->next;
	    *func_end=tk;
	}

	//found a vector as an argument
	if(tk->code == LBRACKET)
	{
	    while(tk->code != RBRACKET)
		tk=tk->next;
	    tk=tk->next;
	    *vector_end=tk;
	}
    }

    //rest of operands and tokens
    while(!(tk->code == stop_code1 || tk->code == stop_code2))
    {
	if(tk->code == MUL || tk->code == ADD || tk->code == SUB || tk->code == DIV)
	    tk=tk->next;
	arg = find_type(tk);

	//verify types
	if(!compatible_types(type_l, arg, tk->line))
	    return -1;

	//verify if conversion is needed
	if(arg != type_l)
	    conversion_needed = 1;

	tk=tk->next;

	// found a function as an argument
	if(tk->code == LPAR && (find_symbol(tk->prev))->cls == FUNCTION)
	{
	    while(tk->code!=RPAR)
		tk=tk->next;
	    tk=tk->next;
	    *func_end=tk;
	}
	
	//found a vector as an argument
	if(tk->code == LBRACKET)
	{
	    while(tk->code != RBRACKET)
		tk=tk->next;
	    tk=tk->next;
	    *vector_end=tk;
	}
    }

    //if conversion is needed convert everything to int
    if(!conversion_needed)
	return type_l;
    else
	return _INT;
}

//main body of type analysis algorithm
int type_analysis()
{
    crtTk = root;
    Symbol *left_operand=NULL;
    int correctness = 1;

    //we verify the type of the functions in the source code
    while(crtTk!=NULL)
    {
	//we found a function
	// token is an ID && is not a struct name ID && we have no types before the ID (so, is no function decl) && is a function => function call
	if(crtTk_is(ID) && (crtTk->prev!=NULL && crtTk->prev->code != STRUCT) && !(if_is_type(crtTk->prev)) && (find_symbol(crtTk)->cls) == FUNCTION)
	{
	    Symbol* function = find_symbol(crtTk);

	    //useful info about the function
	    if(DEVELOPER_OPTIONS)
	    {
	    	printf("Function: %s \n\tdeclared arguments:\n",function->name);
	    	for(int i=0; i<function->nr_argsORmembers; i++)
	    	{
			printf("\t\targ[%d] = %s ~ %s of type %s\n",i,function->args[i]->name, print_class(function->args[i]->cls), print_type(function->args[i]->type));
	    	}
	    	printf("\n");
	    	if(function->nr_argsORmembers!=0)
			printf("\targument called with: \n");
	    }

	    NEXT_TK //skip the name
	    NEXT_TK //skip the LPAR

	    int index;
	    int LPARopen;
	    Token* func_end;
	    Token* vector_end;

	    vector_end=NULL;
	    func_end = NULL;
	    index = 0;
	    LPARopen = 0;
	    while(!crtTk_is(RPAR))
	    {
		//we have an argument
		/*
		    We find its type & class, store them vectors to be compared later
		*/
		if((Tk_is(crtTk->prev,COMMA) || (Tk_is(crtTk->prev,LPAR) && !LPARopen)) && !crtTk_is(RPAR))
		{
		    LPARopen = 1;

		    int arg_class = expr_cls(crtTk,COMMA,RPAR,function->args[index]->cls);
		    int arg_type = expr_type(crtTk,COMMA,RPAR,function->args[index]->type,&func_end,&vector_end);

		    //verify types and classes
		    if(arg_type == -1 || arg_class == -1)
			return 0;
		    if(DEVELOPER_OPTIONS)
		    	printf("\t\t%s - %s of type %s\n",print_code(crtTk->code), print_class(arg_class), print_type(arg_type));
		    index++;

		    //if we found a function move to the next argument
		    if(func_end != NULL)
		    {
			crtTk = func_end;
			func_end = NULL;
		    }

		    //if we found a vector move to the next argument
		    if(vector_end != NULL)
		    {
			crtTk = vector_end;
			vector_end = NULL;
		    }
		}
		if(!crtTk_is(RPAR))
		    NEXT_TK
	    }
	}

	NEXT_TK
    }

    crtTk = root;

    //we verify the type of the assign in the source code
    while(crtTk!=NULL)
    {
	//we found an ASSIGN token
	if(crtTk_is(ASSIGN))
	{
	    //we store and search the left operand
	    left_operand = find_symbol(crtTk->prev);
	    if(DEVELOPER_OPTIONS)
	    {
		if(left_operand->type != _STRUCT)
		    printf("line: %d ~ left operand: %s -> %s --- ",crtTk->line,left_operand->name,print_type(left_operand->type));
		else
		    printf("line: %d ~ left operand: %s -> %s-%s --- ",crtTk->line,left_operand->name,print_type(left_operand->type),left_operand->struct_name);
	    }

	    NEXT_TK

	    if(DEVELOPER_OPTIONS)
		printf("right operand types: ");

	    //we store and search the right operands

		Token* func_end=NULL;
		Token* vector_end=NULL;
		int right_operand_type = expr_type(crtTk,SEMICOLON,RPAR,left_operand->type,&func_end,&vector_end);
		int right_operand_class = expr_cls(crtTk,SEMICOLON,RPAR,find_class(left_operand->tk));

		if(DEVELOPER_OPTIONS)
			printf("%s of type %s     ", print_class(right_operand_class), print_type(right_operand_type));

		//verify types and classes
		if(right_operand_type == -1 || right_operand_class == -1)
		    return 0;

		//some operands may be structures of different kind
		if(left_operand->type == right_operand_type && left_operand->type == _STRUCT)
		{
		    if(strcmp(left_operand->struct_name, (find_symbol(crtTk))->struct_name)!=0)
		    {
			printf("\nError at line %d: type of left operand 'STRUCT %s' is different from type of right operand 'STRUCT %s'\n",crtTk->line,left_operand->struct_name, ((find_symbol(crtTk))->struct_name));
			correctness = 0;
		    }
		}
		IF_NOT_CORRECT_EXIT
		NEXT_TK

		//if we found a function move to the next argument
		if(func_end != NULL)
		{
		    crtTk = func_end;
		    func_end = NULL;
		}

		//if we found a vector move to the next argument
		if(vector_end != NULL)
		{
		    crtTk = vector_end;
		    vector_end = NULL;
		}

	    if(DEVELOPER_OPTIONS)
		printf("\n");
	}
	NEXT_TK
    }

    return 1;
}


/*						*
 *		 Code Generation		*
 *						*/

typedef struct Stack{
    char* name;
    int memory_loc;
    int type;
    union
    {
	int value_i;
	double value_f;
    };
    struct Stack *next;
}Stack;

Stack* st_root=NULL;
Stack* crt_st=NULL;

//function to print the whole stack
void print_stack()
{
    Stack* st=st_root;
    while(st!=NULL)
    {
	printf("Name: %s = ", st->name);
	if(st->type == _INT)
	    printf("%d", st->value_i);
	if(st->type == _DOUBLE)
	    printf("%lf", st->value_f);
	if(st->type == _CHAR)
	    printf("'%c'",(char)(st->value_i));
	printf(" at %d\n", st->memory_loc);
	st=st->next;
    }
}

//memory counter
int mem=0;

//type of registers (data_type + empty/non-empty)
enum type_reg { I_NO_VAL, I_VAL, D_NO_VAL, D_VAL, C_NO_VAL, C_VAL};

//push an element on the stack
void push_stack(Symbol *sy, int enable, int value_int, double value_float)
{
    Stack *st;
    //SAFE ALLOC
    st = (Stack*)malloc(sizeof(Stack));
    if(st == NULL)
	err("not enough memory");

    st->name = sy->name;
    st->memory_loc = mem;
    if(sy->cls == VECTOR || sy->cls == FUNCTION_ARGUMENT_VECTOR || sy->cls == STRUCT_FIELD_VECTOR)
    {
	if(sy->line>=0)
	    mem=mem+(sy->tk->next->next->i);
	else
	    mem+=50;
    }
    else
	mem++;

    //integer
    if(enable == I_NO_VAL)
	st->value_i = 0;
    if(enable == I_VAL)
	st->value_i = value_int;
    //double
    if(enable == D_NO_VAL)
	st->value_f = 0;
    if(enable == D_VAL)
	st->value_f = value_float;
    //char
    if(enable == C_NO_VAL)
	st->value_i = 0;
    if(enable == C_VAL)
	st->value_i = value_int;

    st->next=NULL;

    //determining the basic type
    if(enable == I_NO_VAL || enable == I_VAL)
	st->type = _INT;
    if(enable == D_NO_VAL || enable == D_VAL)
	st->type = _DOUBLE;
    if(enable == C_NO_VAL || enable == C_VAL)
	st->type = _CHAR;

    if(crt_st==NULL)
    {
	st_root = st;
    }
    else
    {
	crt_st->next = st;
    }
    crt_st = st;
}

enum OpCode {
    O_STORE_I, // store integer
    O_STORE_C, // store char
    O_STORE_D, // store double
    O_ADD_I, // add integer
    O_ADD_C, // add char
    O_ADD_D, // add double
    O_SUB_I, // subtract integer
    O_SUB_C, // subtract char
    O_SUB_D, // subtract double
    O_LOAD_I, // loads integer
    O_LOAD_C, // load char
    O_LOAD_D, // load double
    O_MUL_I, // multiply integer
    O_MUL_C, // multiply char
    O_MUL_D, // multiply double
    O_DIV_I, // divide integer
    O_DIV_C, // divide char
    O_DIV_D, // divide double
    O_MODIFY_I, // modify an already stored integer
    O_MODIFY_C, // modify an already stored char
    O_MODIFY_D, // modify an already stored double

    O_LOAD_F, // load a function (arguments)


    O_HALT, // halt operation
};

int neutral_el = 0;

//choose the next operator & neutral_element
int next_op(Token* tk, int type)
{
    if(tk->code == ADD)
    {
	neutral_el = 0;
	if(type == _INT)
	    return O_ADD_I;
	if(type == _CHAR)
	    return O_ADD_C;
	if(type == _DOUBLE)
	    return O_ADD_D;
    }
    if(tk->code == SUB)
    {
	neutral_el = 0;
	if(type == _INT)
	    return O_SUB_I;
	if(type == _CHAR)
	    return O_SUB_C;
	if(type == _DOUBLE)
	    return O_SUB_D;
    }
    if(tk->code == MUL)
    {
	neutral_el = 1;
	if(type == _INT)
	    return O_MUL_I;
	if(type == _CHAR)
	    return O_MUL_C;
	if(type == _DOUBLE)
	    return O_MUL_D;
    }
    if(tk->code == DIV)
    {
	neutral_el = 1;
	if(type == _INT)
	    return O_DIV_I;
	if(type == _CHAR)
	    return O_DIV_C;
	if(type == _DOUBLE)
	    return O_DIV_D;
    }
    return O_HALT;
}

//load a register
double load_reg(char* name_to_find)
{
    Stack* st=st_root;
    while(st!=NULL)
    {
	if(strcmp(st->name,name_to_find)==0)
	{
	    if(st->type == _INT || st->type == _CHAR)
		return st->value_i;
	    if(st->type == _DOUBLE)
		return st->value_f;
	}
	st=st->next;
    }
    return 0;
}

double get_value_token(Token* tk, int type, int show_msg);

int op_code_find(Token* tk);

double op_code_execute(int op_code, Token *tk, int aux, int value_i, float value_f);

//find a value in the stack
Stack* find_reg(char* its_name)
{
    Stack* st=st_root;
    while(st!=NULL)
    {
	if(strcmp(st->name,its_name)==0)
	    return st;
	st=st->next;
    }
    return st_root;
}

//return a value from a function
void return_func(Token* func)
{
    Token* tk = func;
    Symbol* f = find_symbol(func);
    int value_int = 0;
    double value_float = 0;
    if(f->type != _VOID)
    {
	while(tk!=NULL)
	{
	    if(tk->code == RETURN)
	    {
		tk=tk->next;
		if(f->type == _INT)
		{
		    if(tk->next->code != SEMICOLON)
			value_int = (int) op_code_execute(next_op(tk->next, _INT),tk, 0, 0, 0);
		    else
			value_int = (int) get_value_token(tk, _INT, 0);
		    op_code_execute(O_MODIFY_I, func, 0, value_int, 0);
		    break;
		}
		else if(f->type == _CHAR)
		{
		    if(tk->next->code != SEMICOLON)
			value_int = (char) op_code_execute(next_op(tk->next, _CHAR),tk, 0, 0, 0);
		    else
			value_int = (char) get_value_token(tk, _CHAR, 0);
		    op_code_execute(O_MODIFY_C, func, 0, value_int, 0);
		    break;
		}
		else if(f->type == _DOUBLE)
		{
		    if(tk->next->code != SEMICOLON)
			value_float = (double) op_code_execute(next_op(tk->next, _DOUBLE),tk, 0, 0, 0);
		    else
			value_float = (double) get_value_token(tk, _DOUBLE, 0);
		    op_code_execute(O_MODIFY_D, func, 0, 0, value_float);
		    break;
		}
	    }
	    tk = tk->next;
	}
    }
}

//push argument of a function in the stack
void push_args(Token* func, Token* f_call)
{
    Token* tk = func;
    while(tk!=NULL)
    {
	if(tk->code == ID && (tk->next->code == COMMA || tk->next->code == RPAR) && (tk->prev->code == INT || tk->prev->code== CHAR || tk->prev->code == DOUBLE))
	{
	    if(tk->prev->code == INT)
	    {
		op_code_execute(O_STORE_I, tk, I_NO_VAL, 0, 0);
	    }
	}
	tk=tk->next;
    }
    tk=f_call;
    /*while(tk!=NULL)
    {
	if((tk->prev->code == LPAR || tk->prev->code == COMMA) && (tk->next->code == COMMA || tk->next->code == RPAR))
	{
	    op_code_execute(O_MODIFY_I, func, mem_loc, value_int, 0);
	}
    }*/
}

// finite state machine for OP codes
double op_code_execute(int op_code, Token* tk, int aux, int value_i, float value_f)
{
    Stack* reg=NULL;
    switch(op_code){

    case O_STORE_I:	if(aux==0) //no value to store given
			{
			    printf("O_STORE_I: %s with %d\n",tk->text, 0);
			    push_stack(find_symbol(tk), I_NO_VAL, 0, 0 );
			}
			else
			{
			    printf("O_STORE_I: %s with %d\n",tk->text, value_i);
			    push_stack(find_symbol(tk), I_VAL, value_i, 0 );
			}
			return 1;

    case O_STORE_C: if(aux==0) //no value to store given
			{
			    printf("O_STORE_C: %s with ''\n",tk->text);
			    push_stack(find_symbol(tk), C_NO_VAL, 0, 0 );
			}
			else
			{
			    printf("O_STORE_C: %s with '%c'\n",tk->text, (char)(value_i));
			    push_stack(find_symbol(tk), C_VAL, value_i, 0 );
			}
			return 1;

    case O_STORE_D: if(aux==0) //no value to store given
			{
			    printf("O_STORE_D: %s with %lf\n",tk->text, 0.0);
			    push_stack(find_symbol(tk), D_NO_VAL, 0, 0 );
			}
			else
			{
			    printf("O_STORE_D: %s with %lf\n",tk->text, value_f);
			    push_stack(find_symbol(tk), D_VAL, 0, value_f );
			}
			return 1;

    case O_ADD_I:	printf("O_ADD_I: %d\n", (int)(get_value_token(tk,_INT,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (int)(get_value_token(tk,_INT,1))+op_code_execute(next_op(tk->next, _INT), tk->next->next, 0, 0, 0);
			}
			return 0;

    case O_ADD_C:	printf("O_ADD_C: '%c'\n",(char)(get_value_token(tk,_CHAR,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (char)(get_value_token(tk,_CHAR,1))+op_code_execute(next_op(tk->next, _CHAR), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_ADD_D: 	printf("O_ADD_D: %lf\n", (double)(get_value_token(tk,_DOUBLE,0)));
    			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (double)(get_value_token(tk,_DOUBLE,1))+op_code_execute(next_op(tk->next, _DOUBLE), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_SUB_I:	printf("O_SUB_I: %d\n", (int)(get_value_token(tk,_INT,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (int)(get_value_token(tk,_INT,1))-op_code_execute(next_op(tk->next, _INT), tk->next->next, 0, 0, 0);
			}
			return 0;

    case O_SUB_C:	printf("O_SUB_C: '%c'\n",(char)(get_value_token(tk,_CHAR,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (char)(get_value_token(tk,_CHAR,1))-op_code_execute(next_op(tk->next, _CHAR), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_SUB_D:	printf("O_SUB_D: %lf\n", (double)(get_value_token(tk,_INT,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (int)(get_value_token(tk,_INT,1))-op_code_execute(next_op(tk->next, _DOUBLE), tk->next->next, 0, 0, 0);
			}
			return 0;

    case O_MUL_I:	printf("O_MUL_I: %d\n",(int)(get_value_token(tk,_CHAR,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (int)(get_value_token(tk,_INT,1))*op_code_execute(next_op(tk->next, _INT), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_MUL_C: 	printf("O_MUL_C: '%c'\n", (char)(get_value_token(tk,_CHAR,0)));
    			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (char)(get_value_token(tk,_DOUBLE,1))*op_code_execute(next_op(tk->next, _CHAR), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_MUL_D: 	printf("O_MUL_D: %lf\n", (double)(get_value_token(tk,_DOUBLE,0)));
    			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (double)(get_value_token(tk,_DOUBLE,1))*op_code_execute(next_op(tk->next, _DOUBLE), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_DIV_I: 	printf("O_DIV_I: %d\n", (int)(get_value_token(tk,_INT,0)));
    			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (int)(get_value_token(tk,_INT,1))/op_code_execute(next_op(tk->next, _INT), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_DIV_C: 	printf("O_DIV_C: '%c'\n",(char)(get_value_token(tk,_CHAR,0)));
			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (char)(get_value_token(tk,_CHAR,1))/op_code_execute(next_op(tk->next, _CHAR), tk->next->next, 0, 0, 0);
			}
			return 0;
			
    case O_DIV_D: 	printf("O_DIV_D: %lf\n", (double)(get_value_token(tk,_DOUBLE,0)));
    			if(tk->code!=SEMICOLON && tk->code!=COMMA)
			{
			    return (double)(get_value_token(tk,_DOUBLE,1))/op_code_execute(next_op(tk->next, _DOUBLE), tk->next->next, 0, 0, 0);
			}
			return 0;

    case O_LOAD_I:	if(aux) // show load instruction message
			    printf("O_LOAD_I: %s\n",tk->text);
			return (int)(load_reg(tk->text));

    case O_LOAD_C:	if(aux) // show load instruction message
			    printf("O_LOAD_C: %s\n",tk->text);
			return (char)(load_reg(tk->text));

    case O_LOAD_D:	if(aux) // show load instruction message
			    printf("O_LOAD_D: %s\n",tk->text);
			return (double)(load_reg(tk->text));

    case O_MODIFY_I:	printf("O_MODIFY_I: %s = %d\n",tk->text,value_i);
			reg = find_reg(tk->text);
			reg->value_i = value_i;
			return 1;

    case O_MODIFY_C:	printf("O_MODIFY_C: %s = '%c'\n",tk->text,(char)(value_i));
			reg = find_reg(tk->text);
			reg->value_i = (char)(value_i);
			return 1;

    case O_MODIFY_D:	printf("O_MODIFY_D: %s = %lf\n",tk->text, (double)(value_f));
			reg = find_reg(tk->text);
			reg->value_f = (double)(value_f);
			return 1;

    case O_LOAD_F:	printf("\n----START FUNC-----\n");
			printf("O_LOAD_F: %s\n",tk->text);
			tk=tk->next;
			Symbol* f = find_symbol(tk->prev);
			Token* func = f->tk;
			//create a stack variable with the return value in which to store it
			if(f->type == _INT)
			    op_code_execute(O_STORE_I, func, I_NO_VAL, 0, 0);
			else
			    if(f->type == _DOUBLE)
				op_code_execute(O_STORE_D, func, D_NO_VAL, 0, 0);
			    else
				if(f->type == _CHAR)
				    op_code_execute(O_STORE_C, func, C_NO_VAL, 0, 0);
			//push_args(func,tk); // push arguments of the function
			crtTk=func->next;
			//skip the already done part
			while(!Tk_is(crtTk,RACC))
			{
			    op_code_find(crtTk);
			    NEXT_TK
			}
			NEXT_TK
			crtTk=tk;
			return_func(func); //get the return value
			printf("----END FUNC-----\n");
			return 1;

    case O_HALT: printf("O_HALT\n"); return neutral_el;
    default: printf("OP CODE NOT FOUND!\n"); return -1;
    }
}

//get the value from a certain variable
double get_value_token(Token* tk, int type, int show_msg)
{
    if(tk->code == CT_INT || tk->code == CT_CHAR)
	return tk->i;
    if(tk->code == CT_REAL)
	return tk->r;
    if(tk->code == ID)
    {
	if(type == _INT)
	    return op_code_execute(O_LOAD_I, tk, show_msg, 0, 0);
	if(type == _CHAR)
	    return op_code_execute(O_LOAD_C, tk, show_msg, 0, 0);
	if(type == _DOUBLE)
	    return op_code_execute(O_LOAD_D, tk, show_msg, 0, 0);
    }
    return -1;
}

//choose the op code based on certain criteria
int op_code_find(Token* tk)
{
    int correct=1;

    //predifined functions

    if(tk->code == ID && strcmp(tk->text,"put_i")==0)
    {
	int value_int = (int) get_value_token(tk->next->next, _INT, 1);
	printf("%d\n",value_int);
	NEXT_TK
	return 1;
    }

    if(tk->code == ID && strcmp(tk->text,"put_d")==0)
    {
	double value_float = (double) get_value_token(tk->next->next, _DOUBLE, 1);
	printf("%lf\n",value_float);
	NEXT_TK
	return 1;
    }

    if(tk->code == ID && strcmp(tk->text,"put_c")==0)
    {
	int value_int = (char) get_value_token(tk->next->next, _CHAR, 1);
	printf("%c\n",(char) value_int);
	NEXT_TK
	return 1;
    }

    //skip function declarations except main
    if(tk->code == ID && strcmp(tk->text,"main")!=0  && find_symbol(tk)->cls == FUNCTION && (tk->prev->code == INT || tk->prev->code== VOID || tk->prev->code== CHAR || tk->prev->code == DOUBLE))
    {
	while(!consume(RACC))
	    NEXT_TK
	return 1;
    }

    // Load a function
    if(tk->code == ID && find_symbol(tk)->cls == FUNCTION && (tk->prev->code!=INT && tk->prev->code!= VOID && tk->prev->code!= CHAR && tk->prev->code!= DOUBLE))
    {
	while(!consume(RACC))
	    NEXT_TK
	return op_code_execute(O_LOAD_F,tk,0, 0, 0);
    }

    //store variables - add them to the stack
    if(tk->code == ID && find_symbol(tk)->cls == VARIABLE && (tk->prev->code==INT || tk->prev->code==CHAR || tk->prev->code==DOUBLE))
    {
	while(tk->code!=SEMICOLON && tk->code != ASSIGN)
	{
	    if(tk->code == ID) // int/char/double x
	    {
		if(tk->next->code == ASSIGN) // int/char/double x =
		{
		    if(find_symbol(tk)->type == _INT)
		    {
			int value_int = 0;
			if(tk->next->next->next->code != SEMICOLON && tk->next->next->next->code != COMMA)
			    value_int = (int) op_code_execute(next_op(tk->next->next->next, _INT),tk->next->next, 0, 0, 0); //int x=a+/-*5
			else
			    value_int = (int) get_value_token(tk->next->next, _INT, 1);  //int x=a;
			correct = op_code_execute(O_STORE_I, tk, 1, value_int, 0); //store the value
		    }
			    //same for the rest down here
		    if(find_symbol(tk)->type == _DOUBLE)
		    {
			double value_float = 0;
			if(tk->next->next->next->code != SEMICOLON && tk->next->next->next->code != COMMA)
			    value_float = (double) op_code_execute(next_op(tk->next->next->next, _DOUBLE),tk->next->next, 0, 0, 0);
			else
			    value_float = (double) get_value_token(tk->next->next, _DOUBLE, 1);
			correct = op_code_execute(O_STORE_D, tk, 1, 0, value_float);
		    }
		    if(find_symbol(tk)->type == _CHAR)
		    {
			char value_char = 0;
			if(tk->next->next->next->code != SEMICOLON && tk->next->next->next->code != COMMA)
			    value_char = (char) op_code_execute(next_op(tk->next->next->next, _CHAR),tk->next->next, 0, 0, 0);
			else
			    value_char = (char) get_value_token(tk->next->next, _CHAR, 1);
			correct = op_code_execute(O_STORE_C, tk, 1, value_char, 0);
		    }
		}
		else //uninitialized variables
		{
		    if(find_symbol(tk)->type == _INT)
			correct = op_code_execute(O_STORE_I, tk, 0, 0, 0);
		    if(find_symbol(tk)->type == _DOUBLE)
			correct = op_code_execute(O_STORE_D, tk, 1, 0, 0);
		    if(find_symbol(tk)->type == _CHAR)
			correct = op_code_execute(O_STORE_C, tk, 0, 0, 0);
		}
	    }
	    if(!correct)
		return 0;
	    NEXT_TK
	    tk=tk->next;
	}
    }

    //modify variables - already in stack but we need to change their value
    if(tk->code == ID && (find_symbol(tk)->cls == VARIABLE) && !(tk->prev->code==INT || tk->prev->code==CHAR || tk->prev->code==DOUBLE)) // x 
    {
	if(tk->next->code == ASSIGN) // x = 
	{
	    // we go based on the type
	    if(find_symbol(tk)->type == _INT)
	    {
		int value_int = 0;
		if(tk->next->next->next->code != SEMICOLON) // x = f
		{
		    if(tk->next->next->code == ID && tk->next->next->next->code == LPAR) // x = f(
		    {
			//we assign it a function => update the crtTk
			while(!consume(RACC))
				NEXT_TK
			//load and execute the function
			op_code_execute(O_LOAD_F,tk->next->next,0, 0, 0);
			//store the return value
			value_int = (int) op_code_execute(O_LOAD_I,tk->next->next, 1, 0, 0);
		    }
		    else
			value_int = (int) op_code_execute(next_op(tk->next->next->next, _INT),tk->next->next, 0, 0, 0); // x = 5+-*/g
		}
		else
		    value_int = (int) get_value_token(tk->next->next, _INT, 1); // x = a;
		//here we update the entry in the stack
		correct = op_code_execute(O_MODIFY_I, tk, 0, value_int, 0);
	    }
		    //analog like we did up here for DOUBLE & CHAR
	    if(find_symbol(tk)->type == _DOUBLE)
	    {
		double value_float = 0;
		if(tk->next->next->next->code != SEMICOLON && tk->next->next->next->code != COMMA)
		{
		    if(tk->next->next->code == ID && tk->next->next->next->code == LPAR)
		    {
			while(!consume(RACC))
				NEXT_TK
			op_code_execute(O_LOAD_F,tk->next->next,0, 0, 0);
		    	value_float = (double) op_code_execute(O_LOAD_D,tk->next->next, 1, 0, 0);
		    }
		    else
			value_float = (double) op_code_execute(next_op(tk->next->next->next, _DOUBLE),tk->next->next, 0, 0, 0);
		}
		else
		    value_float = (double) get_value_token(tk->next->next, _DOUBLE, 1);
		correct = op_code_execute(O_MODIFY_D, tk, 0, 0, value_float);
	    }
	    if(find_symbol(tk)->type == _CHAR)
	    {
		char value_char = 0;
		if(tk->next->next->next->code != SEMICOLON && tk->next->next->next->code != COMMA)
		{
		    if(tk->next->next->code == ID && tk->next->next->next->code == LPAR)
		    {
			while(!consume(RACC))
				NEXT_TK
			op_code_execute(O_LOAD_F,tk->next->next,0, 0, 0);
			value_char = (char) op_code_execute(O_LOAD_C,tk->next->next, 1, 0, 0);
		    }
		    else
		    	value_char = (char) op_code_execute(next_op(tk->next->next->next, _CHAR),tk->next->next, 0, 0, 0);
		}
		else
		    value_char = (char) get_value_token(tk->next->next, _CHAR, 1);
		correct = op_code_execute(O_MODIFY_C, tk, 0, value_char, 0);
	    }
	if(!correct)
		return 0;
	}
    }

    return correct;
}

// the main function to generate code
int Generate_code()
{
    crtTk=root;
    int correct=1;
    while(crtTk != NULL)
    {
	correct = op_code_find(crtTk);
	NEXT_TK
    }
    printf("\n\n");
    print_stack();
    return correct;
}

int main(int argc, char* argv[]) {

    int help=0;

    if(argc>=3)
    {
	if(strcmp(argv[2],"-DEBUG")==0)
	    DEVELOPER_OPTIONS = 1;
	else
	if(strcmp(argv[2],"-NoWarnings")==0)
	    WARNINGS = 0;
	else
	if(strcmp(argv[2],"-Code")==0)
	    GENERATE_CODE = 1;
	else
	    help=1;
    }

    if(argc>=4)
    {
	if(strcmp(argv[3],"-DEBUG")==0)
	    DEVELOPER_OPTIONS = 1;
	else
	if(strcmp(argv[3],"-NoWarnings")==0)
	    WARNINGS = 0;
	else
	if(strcmp(argv[3],"-Code")==0)
	    GENERATE_CODE = 1;
	else
	    help=1;
    }

    if(argc>=5)
    {
	if(strcmp(argv[4],"-DEBUG")==0)
	    DEVELOPER_OPTIONS = 1;
	else
	if(strcmp(argv[4],"-NoWarnings")==0)
	    WARNINGS = 0;
	else
	if(strcmp(argv[4],"-Code")==0)
	    GENERATE_CODE = 1;
	else
	    help=1;
    }

    char *file;
    if(argc < 2 || argc > 4)
    {
	printf("Wrong format!\nCorrect format: ./exe file_to_compile -options\n");
	printf("Options: \n\t'-DEBUG' = used to show more informations about the compiling process\n");
	printf("\t'-NoWarnings' = used to hide the warnings\n");
	return -1;
    }

    if(help)
    {
	printf("I saw you used an option wrong, this could maybe help you? :D\n");
	printf("Options: \n\t'-DEBUG' = used to show more informations about the compiling process\n");
	printf("\t'-NoWarnings' = used to hide the warnings\n");

    }

    file = argv[1];
    source = fopen(file, "r");
    if (source == NULL)
	perror("ERROR opening the file\n");

    //LEXICAL ANALYZER
    getNextToken();
    if(DEVELOPER_OPTIONS)
    {
	printf("\n");
	print_Tokens();
    }

    //SYNTACTICAL ANALYZER
    if(syntactical_analyzer()==1)
    {
	printf("\n\n\nSyntax is correct\n\n\n");
    }
    else
    {
	printf("\n\n\nSyntactical Error\n\n\n");
	return -1;
    }

    //Domain Analysis & Table of Symbols
    if(domain_and_symbols()==1)
    {
	printf("\nDomain Analysis & Table of Symbols is correct\n\n\n");
    }
    else
    {
	printf("\nDomain Analysis & Table of Symbols Error\n\n\n");
	return -1;
    }

    //Type Analysis
    if(type_analysis()==1)
    {
	printf("\nType Analysis is correct\n\n\n");
    }
    else
    {
	printf("\nType Analysis Error\n\n\n");
	return -1;
    }

    //Code Generation
    if(GENERATE_CODE)
    {
	if(Generate_code()==1)
	{
	    printf("\nCode Generation is correct & completed\n\n\n");
	}
	else
	{
	    printf("\nCode Generation Error\n\n\n");
	    return -1;
	}
    }
    else
    {
	printf("Code not generated due to option not selected\n");
	printf("If you want code generation on the selected file please use the option '-Code'\n");
    }


    fclose(source);
    return 0;
}
