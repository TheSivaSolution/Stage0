//Stage2 

#include <iostream>
#include <ctime>
#include <iomanip>
#include <cctype>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <stack>
#include <deque>

using namespace std;

const int MAX_SYMBOL_TABLE_SIZE = 256;
enum storeType {INTEGER, BOOLEAN, PROG_NAME, UNKNOWN};
enum allocation {YES, NO};
enum modes {VARIABLE, CONSTANT};

int numBegins = 0;
int error = 0;
int lineNo = 1;
bool finish = false;
bool underScore = false;
int tableCount = 0;
bool stupidfuckingclass = true;
int currentTempNo, maxTempNo;
string ARegister, QRegister;

void CreateListingHeader();
void Parser();
void CreateListingTrailer();
void PrintSymbolTable();
void Prog();
void ProgStmt();
void Consts();
void Vars();
void BeginEndStmt();
void ConstStmts();
void VarStmts();
string Ids();
void Insert(string externalName, storeType inType, modes inMode, string inValue, allocation inAlloc, int inUnits);
string GenInternalName(storeType x);
storeType WhichType(string name);
string WhichValue(string name);
string NextToken();
char NextChar();
bool isNonKeyID(string x);
bool isInteger(string x);
void Error();
void ExecStmts();
bool isLit(string name);
void ExecStmt();
void ReadStmt();
void ReadList();
void WriteList();
void WriteStmt();
void Express();
void Expresses();
void Term();
void Terms();
void Factor();
void Factors();
void Part();
bool isRelOp();
bool isAddLevelOp();
bool isMultLevelOp();
bool doesExist(string x);
string PopOperand();
string PopOperator();
void PushOperand(string x);
void PushOperator(string x);
void Code(string);
int getIndex(string x);
void printCode(string location, string operation, string address, string signNum, string comment);
void printCondensedSymbolTable();
void EmitEndCode();
void EmitReadCode(string op1);
void EmitWriteCode(string op1);
void EmitAdditionCode(string op1, string op2);
void EmitSubtractionCode(string op1, string op2);
void EmitNegCode(string op1);
void EmitPosCode(string op1);
void EmitNotCode(string op1);
void EmitMultiplicationCode(string op1, string op2);
void EmitDivisionCode(string op1, string op2);
void EmitModCode(string op1, string op2);
void EmitAndCode(string op1, string op2);
void EmitOrCode(string op1, string op2);
void EmitEqualsCode(string op1, string op2);
void EmitNotEqualsCode(string op1, string op2);
void EmitLessEqCode(string op1, string op2);
void EmitGreaterEqCode(string op1, string op2);
void EmitLessThanCode(string op1, string op2);
void EmitGreaterThanCode(string op1, string op2);
void EmitAssignCode(string op1, string op2);
void AssignRegister(string x, string reg);
string GetTemp();
void FreeTemp();
void NextLine();
string getInternalName(string x);
string getExternalName(string x);
void AssignStmt();
void ReadStmt();
void WriteStmt();
void IfStmt();
void WhileStmt();
void RepeatStmt();
void NullStmt();
void ElsePt();
void EmitThenCode(string op1);
void EmitElseCode(string op1);
void EmitPostIfCode(string op1);
void EmitWhileCode();
void EmitDoCode(string op1);
void EmitPostWhileCode(string op1, string op2);
void EmitRepeatCode();
void EmitUntilCode(string op1, string op2);
int countif=0;
int countelse=0;

struct entry {
	string internalName;
	string externalName;
	storeType dataType;
	modes mode;
	string Value;
	allocation alloc;
	int units;
};
string token;
char charac;
const char END_OF_File = '$'; //arbitrary choice
std::vector<entry> symbolTable;
ifstream sourceFile;
ofstream listingFile, objectFile;
stack<string> operandStack;
stack<string> operatorStack;
string currentLineLabel, label;


int main(int argc, char **argv) {
	//this program is the stage1 compiler for Pascallite. It will accept
	//input from argv[1], generating a listing to argv[2], and object code to
	//argv[3]

	//cout << "int main(" << argc << ", " << argv << ")" << endl;

	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
	CreateListingHeader();
	Parser();
	CreateListingTrailer();
	//PrintSymbolTable();
	//Code("end");
	listingFile.close();
	objectFile.close();

	return 0;
}

void Error()
{
	//cout << "void Error()" << endl;
	CreateListingTrailer();
	//PrintSymbolTable();
	listingFile.close();
	objectFile.close();
	exit(0);
}

void CreateListingHeader() {
	//cout << "void CreateListingHeader()" << endl;
	time_t now = time(0);
	char* dt = ctime(&now);

	listingFile << setw(0) << left << "NAME";
	listingFile << setw(0) << right << dt << endl;

}

void Parser() {
	//cout << "void Parser()" << endl;
	listingFile << setw(22) << left << "LINE NO." << right << "SOURCE STATEMENT" << endl << endl;
	listingFile << setw(5)<<right <<lineNo << '|';
	lineNo++;
	currentTempNo = -1;
	maxTempNo = -1;
	NextChar();
		//charac must be initialized to the first character of the source filebuf
	if(NextToken() != "program")
		listingFile<<"Error: Line " << lineNo<<": keyword \"program\" expected";
			//(1) the variable, token, is assigned the value of the next token
			//(2) the next token is read from the source file in order to make
				//the assignment. The value returnedbyNextToken() is also
				//the next token.
	Prog();
		//parser implements the grammar rules, calling the first rules
}

void CreateListingTrailer() {
	//cout << "void CreateListingTrailer()" << endl;
	//print "COMPILATION TERMINATED", "# ERRORS ENCOUNTERED"
	listingFile << setw(28) << left << "COMPILATION TERMINATED";
	listingFile << setw(0) << right << error << " ERRORS ENCOUNTERED\n";
}

void PrintSymbolTable() {
	//cout << "void PrintSymbolTable()" << endl;
	//print symbol table to object file

		time_t now = time(0);
	char* dt = ctime(&now);

	objectFile << setw(0) << left << "Name";
	objectFile << setw(0) << right << dt << endl;

	objectFile << "Symbol Table" << endl<<endl;
	for(int j=0; j<symbolTable.size(); j++)
	{
		objectFile<<left<<setw(17)<<symbolTable[j].externalName;
		objectFile<<setw(6)<<left<<symbolTable[j].internalName;
		if(symbolTable[j].dataType==0)
			objectFile<<right<<setw(9)<<"INTEGER";
		else if(symbolTable[j].dataType==1)
			objectFile<<right<<setw(9)<<"BOOLEAN";
		else
			objectFile<<right<<setw(9)<<"PROG_NAME";
		if(symbolTable[j].mode==0)
			objectFile<<setw(10)<<right<<"VARIABLE";
		else
			objectFile<<setw(10)<<right<<"CONSTANT";
		if(symbolTable[j].dataType==2)
			objectFile<<setw(17)<<right<<symbolTable[j].externalName;
		else
			objectFile<<setw(17)<<right<<symbolTable[j].Value;
		if(symbolTable[j].alloc==0)
			objectFile<<right<<setw(5)<<"YES";
		else
			objectFile<<right<<setw(5)<<"NO";
		objectFile<<setw(3)<<right<<symbolTable[j].units<<endl;
	}

	objectFile.close();
}

void printCondensedSymbolTable()
{
	//cout << "void printCondensedSymbolTable()" << endl;
	string temp, temp2;

	for(int j=0; j<symbolTable.size(); j++)
	{
		if(symbolTable[j].alloc==0)
		{
			if(symbolTable[j].mode==0){
				temp="BSS";
				temp2="1";
			}
			else{
				temp="DEC";
				temp2=symbolTable[j].Value;
			}
			printCode(symbolTable[j].internalName, temp, temp2, "", symbolTable[j].externalName);
			temp="";
			temp2="";
		}
	}
}

void Prog() { //token should be "program"
//cout << "void Prog()" << endl;
	if (token != "program")
	{
		error = 1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"program\" expected\n\n";
		Error();
	}
	ProgStmt();
	if (token == "const")
		Consts();
	if (token == "var")
		Vars();
	if (token != "begin")
	{
		error = 1;

		listingFile << "\nError: Line " << lineNo-1<<": keyword \"begin\" expected\n\n";
		Error();
	}
	BeginEndStmt();
	if (token != "$")
	{
		error = 1;
		listingFile << "\nError: Line " << lineNo-1<<": no text may follow \"end\"\n\n";
		Error();
	}
}

void ProgStmt() { //token should be "program"
	//cout << "void ProgStmt()" << endl;
	string x;
	if (token != "program")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"program\" expected\n\n";
		Error();
	}
	x = NextToken();
	if (!isNonKeyID(token))
	{
		error =1;
		listingFile << "\nError: Line " << lineNo-1<<": program name expected\n\n";
		Error();
	}

	if(NextToken() != ";")
	{
		//cout << token << "RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR" << endl;
		error=1;
		listingFile <<"\nError: Line " << lineNo-1<<": \";\" expected\n\n";
		Error();
	}


	NextToken();

	Insert(x, PROG_NAME, CONSTANT, x, NO, 0);
	Code("program");
}

//Production 3
void Consts() {
	//cout << "void Consts()" << endl;
	if (token != "const")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"const\" expected\n\n";
		Error();
	}
	if (!isNonKeyID(NextToken()))
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier must follow \"const\"\n\n";
		Error();
	}

	ConstStmts();
}

//Production 4
void Vars() {
	//cout << "void Vars()" << endl;
	if (token != "var")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"var\" expected\n\n";
		Error();
	}
	if (!isNonKeyID(NextToken()))
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier must follow \"var\"\n\n";
		Error();
	}
	VarStmts();
}

//Production 5
void BeginEndStmt() {
	//cout << "void BeginEndStmt()" << endl;
	if (token != "begin")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"begin\" expected\n\n";
		Error();
	}
	numBegins++;
	//cout << "    NNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNNN   " << numBegins << endl;
	ExecStmts();
	
	//cout << token << endl;
	if (token != "end")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"end\" expected\n\n";
		Error();
	}
	numBegins--;
	//cout << "    HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH   " << numBegins << endl;
	if (NextToken() != "." && token != ";")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \".\" or \";\" expected\n\n";
		Error();
	}
	//numBegins--;
	//cout << "NUM BEGINS IS " << numBegins << endl;
	if (token == "." && numBegins > 0) {
	//cout << token << "    MMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM   " << numBegins << endl;
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \";\" expected\n\n";
		Error();
	}
	else if(token=="." && numBegins == 0)
	{
		finish=true;
		Code("end");
		NextToken();
	}
}

//Production 6
void ConstStmts() {
	//cout << "void ConstStmts()" << endl;
	string x, y;


	if (!isNonKeyID(token))
	{
		error=1;

		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier expected\n\n";
		Error();
	}

	x = token;
	if (NextToken() != "=")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \"=\" expected\n\n";
		Error();
	}
	y = NextToken();
	if (!isNonKeyID(y) && y != "+" && y!= "-" && y != "not" && y != "true" && y != "false" && !isInteger(y))
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": token to right of \"=\" illegal\n\n";
		Error();
	}

	if (y == "+" || y == "-") {
		if (!isInteger(NextToken()))
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": integer expected after sign\n\n";
			Error();
		}
		y += token;
	}

	if (y == "not") {
		if (WhichType(NextToken()) != BOOLEAN)
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": boolean expected after not\n\n";
			Error();
		}

		if (token == "true")
			y = "false";
		else
			y = "true";
	}
	if (NextToken() != ";")
	{
	//cout << token << " LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL" << endl;
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \";\" expected\n\n";
		Error();
	}

	Insert(x, WhichType(y), CONSTANT, WhichValue(y), YES, 1);

	if (!isNonKeyID(NextToken()) && token != "begin" && token != "var")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier, \"begin\", or \"var\" expected\n\n";
		Error();
	}

	if (isNonKeyID(token))
		ConstStmts();
}

//Production 7
void VarStmts() {
	//cout << "void VarStmts()" << endl;
	string x, y;
	storeType temp;

	if (!isNonKeyID(token))
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier expected\n\n";
		Error();
	}

	x = Ids();

	if (token != ":")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \":\" expected\n\n";
		Error();
	}

	if (NextToken() != "integer" && token != "boolean" )
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": illegal type follows \":\"\n\n";
		Error();
	}

	y = token;
	if(y=="integer")
		temp=INTEGER;
	else
		temp=BOOLEAN;

	if (NextToken() != ";")
	{
	//cout << token << " PPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP" << endl;
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \";\" expected\n\n";
		Error();
	}


	Insert(x, temp, VARIABLE, "", YES, 1);

	if (!isNonKeyID(NextToken()) && token!="begin")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier or \"begin\" expected\n\n";
		Error();
	}

	if (isNonKeyID(token))
		VarStmts();
}

//Production 8
string Ids() {
	//cout << "string Ids()" << endl;
	string temp, tempString;

	if (!isNonKeyID(token))
	{
		error=1;

		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier expected\n\n";
		Error();
	}

	tempString = token;
	temp = token;
	if (NextToken() == ",") {
		if (!isNonKeyID(NextToken()))
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier expected\n\n";
			Error();
		}

		tempString = temp + "," + Ids();
	}
	return tempString;
}

void ExecStmts()
{
	//cout << "void ExecStmts()" << endl;
	/*
	while(token != "end" && token != "until")
	{
		ExecStmt();
		if (stupidfuckingclass)
			NextToken;
		stupidfuckingclass = true;
	}
	*/
	if(NextToken()=="end" || token=="until")
		return;
	else
	{
		ExecStmt();
		ExecStmts();
	}

}

void ExecStmt()
{
	//cout << "void ExecStmt()" << token << endl; 
	if(isNonKeyID(token))
		AssignStmt();

	else if(token=="read")
		ReadStmt();

	else if(token=="write")
		WriteStmt();

	else if(token=="if")
	{
		countif++;
		IfStmt();
	}
	else if(token=="while")
		WhileStmt();

	else if(token=="repeat")
		RepeatStmt();

	else if(token==";")
		NullStmt();

	else if(token=="begin")
		BeginEndStmt();
	else if (token =="$") {
		Code("end");
	}
	//else if (token != "end")
	//{
		//error=1;
		//listingFile << "\nError: Line " << lineNo-1<<": some bullshit\n\n";
		//Error();
	//}
}

void AssignStmt()
{
	//cout << "void AssignStmt()" << endl;
	if(!isNonKeyID(token))
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier expected\n\n";
		Error();
	}

	PushOperand(token);
	if(NextToken()!=":=")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \":=\" expected\n\n";
		Error();
	}
	PushOperator(token);
	Express();
	Code(PopOperator());
}

void ReadStmt()
{
	//cout << "void ReadStmt()" << endl;
	ReadList();
}

void WriteStmt()
{
	//cout << "void WriteStmt()" << endl;
	WriteList();
}

void ReadList()
{
	//cout << "void ReadList()" << endl;
	string x="";

	if (NextToken()!="(")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \"(\" expected\n\n";
		Error();
	}

	NextToken();
	x = Ids();
	//cout << "x = Ids();  " << x << endl;
	string temp = "";
	for (int i = 0; i <= x.length(); i++) {
		if (x[i] == ',' || i == x.length())
		{
			//cout << "AYY BB TEMP IS " << temp << endl;
			PushOperand(temp);
			Code("read");
			temp = "";
		}
		else
			temp += x[i];
	}

	if (token!=")")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \")\" expected\n\n";
		Error();
	}
}

void WriteList()
{
	//cout << "void WriteList()" << endl;
	string x="";

	if (NextToken()!="(")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \"(\" expected\n\n";
		Error();
	}

	NextToken();
	x = Ids();
	string temp = "";
	for (int i = 0; i <= x.length(); i++) {
		if (x[i] == ',' || i == x.length())
		{
			PushOperand(temp);
			Code("write");
			temp = "";
		}
		else
			temp += x[i];
	}

	if (token!=")")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \")\" expected\n\n";
		Error();
	}
}

void Express()
{
	//cout << "void Express()" << endl;
	Term();
	Expresses();
}

void Term()
{
	//cout << "void Term()" << endl;
	Factor();
	Terms();
}

void Factor()
{
	//cout << "void Factor()" << endl;
	Part();
	Factors();
}

void Terms()
{
	//cout << "void Terms()" << endl;
	if(isAddLevelOp())
	{
		PushOperator(token);
		Factor();
		Code(PopOperator());
		Terms();

		if (token != "=" && token != "<>" && token != "<=" && token != ">="&& token != "<" && token != ">" && token != ")" && token!=";" && token != "do"){
			error=1;
			listingFile << "\nError: Line " << lineNo-2<<": relational operator or \")\" or \";\" expected\n\n";
			Error();
		}
	}
}

void Expresses()
{
	//cout << "void Expresses()" << endl;
	if (isRelOp())
	{
		PushOperator(token);
		Term();
		Code(PopOperator());
		Expresses();

		if (token != ")" && token!=";" && token!="then" && token!="do" )
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-2<<": \")\" or \";\" expected\n\n";
			Error();
		}
	}

}

void Factors()
{
	//cout << "void Factors()" << endl;
	if(isMultLevelOp())
	{
		PushOperator(token);
		Part();
		Code(PopOperator());
		Factors();


		if (token!="+" && token!="-" && token!="or" && token != "=" && token != "<>" && token != "<=" && token != ">="&& token != "<" && token != ">" && token != ")" && token!=";" ){
			error=1;
			listingFile << "\nError: Line " << lineNo-2<<": relational or arithmetic operator or \")\" or \";\" expected\n\n";
			Error();
		}

	}

}

void Part()
{
	//cout << "void Part()" << endl;
	if (NextToken()=="not")
	{
		PushOperator(token);
		if(NextToken()=="(")
		{
			Express();
			if (token!=")")
			{
				error=1;
				listingFile << "\nError: Line " << lineNo-1<<": \")\" expected\n\n";
				Error();
			}
			Code(PopOperator());
		}

		else if(isNonKeyID(token) || WhichType(token) == BOOLEAN) {
			PushOperand(token);
			NextToken();
			Code(PopOperator());
		}

		else
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<" illegal character or variable after \"not\": \n\n";
			Error();
		}

		if(WhichType(token) == BOOLEAN)
		{
			if(WhichValue(token) == "1")
				PushOperand("false");
			else
				PushOperand("true");

			NextToken();
		}




	}

	else if(token=="+")
	{
		if(NextToken()=="(")
		{
			Express();
			if (token!=")")
			{
				error=1;
				listingFile << "\nError: Line " << lineNo-1<<": \")\" expected\n\n";
				Error();
			}

			Code("pos");
		}

		else if(isInteger(token)) {
			string t = "+" + token;
			PushOperand(t);
			NextToken();
		}

		else if (isNonKeyID(token)) {
			PushOperand(token);
			Code("pos");
			NextToken();
		}

		else
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": illegal use of operator: \n\n";
			Error();
		}

	}

	else if(token=="-")
	{
		if(NextToken()=="(")
		{
			Express();
			if (token!=")")
			{
				error=1;
				listingFile << "\nError: Line " << lineNo-1<<": \")\" expected\n\n";
				Error();
			}

			Code("neg");

		}

		else if(isInteger(token))
		{
			string t="-" + token;
			PushOperand(t);
			NextToken();
		}

		else if(isNonKeyID(token))
		{
			PushOperand(token);
			Code("neg");
			NextToken();
		}

		else
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": illegal use of operator\n\n";//
			Error();
		}


	}
	else if(isInteger(token)) {
		PushOperand(token);
		NextToken();
	}

	else if(isNonKeyID(token) || isLit(token)) {
		PushOperand(token);
		NextToken();
	}

	else if(token=="(")
	{
		Express();
		if (token!=")")
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": \")\" expected\n\n";
			Error();
		}
		NextToken();
	}

	else
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": illegal character or symbol used\n\n";
		Error();
	}

}

bool isRelOp()
{
	//cout << "bool isRelOp()" << endl;
	if (token != "=" && token != "<>" && token != "<=" && token != ">="&& token != "<" && token != ">")
		return false;

	else
		return true;
}

bool isAddLevelOp()
{
	//cout << "bool isAddLevelOp()" << endl;
	if (token!="+" && token!="-" && token!="or" )
		return false;

	else
		return true;
}

bool isMultLevelOp()
{
	//cout << "bool isMultLevelOp()" << endl;
	if (token!="*" && token!="div" && token!="mod" && token!="and")
		return false;

	else
		return true;
}

void IfStmt()
{
	//cout << "void IfStmt()" << endl;
	if (token != "if")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"if\" expected\n\n";
		Error();
	}

	Express();
	if (WhichType(operandStack.top())!= BOOLEAN && operandStack.top()!="true" && operandStack.top()!="false") {
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": predicate of if must be of type boolean\n\n";
		Error();
	}

	if (token!="then")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"then\" expected\n\n";
		Error();
	}
	Code("then");
	NextToken();
	if (token == "else"){
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier, \"read\", \"write\", \"if\", \"while\", \"repeat\", \";\", or \"begin\" expected\n\n";
		Error();
	}
	ExecStmt();
	ElsePt();

}

void ElsePt()
{
	//cout << "void ElsePt()" << endl;
	string temp=currentLineLabel;

	if(NextToken()=="else")
	{
		countelse++;
		Code("else");
		NextToken();

		ExecStmt();
		Code("postIf");
	}
	
	else if(token!="end" && !isNonKeyID(token) && token!=";" && token!="until" && token!="begin" && token!="while" && token!="if" && token!="repeat" && token!="read"&& token!="write")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": non-keyword identifier, \"read\", \"write\", \"if\", \"while\", \"repeat\", \";\", or \"begin\" expected\n\n";
		Error();
	}

	else {
		//cout << token << endl;
		Code("postIf");
		ExecStmt();
	}
	

}

void WhileStmt()
{
//cout << "void WhileStmt()" << endl;
	Code("while");
	Express();
	if (WhichType(operandStack.top())!= BOOLEAN && operandStack.top()!="true" && operandStack.top()!="false") {
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": predicate of while must be of type boolean\n\n";
		Error();
	}

	if(token!="do")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"do\" expected  \n\n";
		Error();
	}
	Code("do");

	NextToken();
	ExecStmt();
	Code("postWhile");

	if(token!=";")
	{
	//cout << token << " JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJ" << endl;
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \";\" expected  \n\n";
		Error();
	}
}

void RepeatStmt()
{
	//cout << "void RepeatStmt()" << endl;
	EmitRepeatCode();
	ExecStmts();
	if(token!="until")
	{
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": keyword \"until\" expected  \n\n";
		Error();
	}
	Express();
	if (WhichType(operandStack.top())!= BOOLEAN && operandStack.top()!="true" && operandStack.top()!="false") {
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": predicate of until must be of type boolean\n\n";
		Error();
	}
	Code("until");
	if(token!=";")
	{
	//cout << token << " YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY" << endl;
		error=1;
		listingFile << "\nError: Line " << lineNo-1<<": \";\" expected  \n\n";
		Error();
	}




}

void NullStmt()
{
	//cout << "void NullStmt()" << endl;
	return;
}

void Insert(string externalName, storeType inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	//cout << "void Insert(" << externalName << ", " << inType << ", " << inMode  << ", " << inValue  << ", " << inAlloc  << ", " << inUnits << ")" << endl;
  string tempName="";


  for(int i=0; i<=externalName.length(); i++)
  {
	if(externalName[i]!=',' && i<externalName.length())
	{
		if(tempName.length()<15)
			tempName+=externalName[i];
	}
	else
	{
		if (tableCount >= MAX_SYMBOL_TABLE_SIZE){
			error = 1;
			listingFile << "\nError: Line " << lineNo-1 << ": symbol table overflow\n\n";
			Error();
		}
		tableCount++;
		for(int k=0; k<symbolTable.size(); k++)
		{
			if (symbolTable[k].externalName == tempName)
			{
				error=1;
				listingFile<<"\nError: Line " << lineNo-1<<": multiple name definition\n\n";
				Error();
			}
		}

		if(tempName=="program" || tempName=="begin" || tempName=="end" || tempName=="var" || tempName=="const" || tempName=="integer" || tempName=="boolean" || tempName=="not" || tempName == "mod"|| tempName == "div" || tempName == "and"|| tempName == "or" || tempName == "read"|| tempName == "write")
		{
			error=1;
			listingFile<<"\nError: Line " << lineNo-1<<": illegal use of keyword\n\n";
			Error();
		}
		else
		{
			int n=symbolTable.size();
			symbolTable.push_back(entry());
			if(isupper(tempName[0]))
			{
				//symbolTable[n]=(tempName, inType, inMode, inValue, inAlloc, inUnits);
				symbolTable[n].internalName = tempName;
				symbolTable[n].externalName = tempName;
				symbolTable[n].dataType = inType;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			else if(tempName=="true")
			{
				symbolTable[n].internalName = "TRUE";
				symbolTable[n].externalName = "TRUE";
				symbolTable[n].dataType = inType;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			else if(tempName=="false")
			{
				symbolTable[n].internalName = "FALS";
				symbolTable[n].externalName = "FALSE";
				symbolTable[n].dataType = inType;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			else if(tempName=="tRuE")
			{
				symbolTable[n].internalName = GenInternalName(inType);
				symbolTable[n].externalName = "true";
				symbolTable[n].dataType = BOOLEAN;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			else if(tempName=="fAlSe")
			{
				symbolTable[n].internalName = GenInternalName(inType);
				symbolTable[n].externalName = "false";
				symbolTable[n].dataType = BOOLEAN;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			else if(tempName=="ZERO")
			{
				symbolTable[n].internalName = "ZERO";
				symbolTable[n].externalName = "ZERO";
				symbolTable[n].dataType = inType;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			else
			{
				//symbolTable[n]=(GenInternalName(inType), inType, inMode, inValue, inAlloc, inUnits);
				symbolTable[n].internalName = GenInternalName(inType);
				symbolTable[n].externalName = tempName;
				symbolTable[n].dataType = inType;
				symbolTable[n].mode = inMode;
				symbolTable[n].Value = inValue;
				symbolTable[n].alloc = inAlloc;
				symbolTable[n].units = inUnits;
			}

			tempName="";
		}
	}
  }
 }

storeType WhichType(string name)
{
	//cout << "storeType WhichType(" << name << ")" << endl;
  if(isgraph(name[0])) {
	if (symbolTable[getIndex(name)].dataType == BOOLEAN || name == "true" || name == "false") {
		return BOOLEAN;
		}
	else {
		return INTEGER;
		}
	}
  else
	{
	  for(int k=0; k<symbolTable.size(); k++)
	  {
		if (symbolTable[k].externalName == name)
		  return symbolTable[k].dataType;
	  }
		error=1;
		listingFile<<"\nError: Line " << lineNo-1<<": reference to undefined variable\n\n";
		Error();
	}
	return BOOLEAN;
}

string WhichValue(string name)
{
	//cout << "string WhichValue(" << name << ")" << endl;
	if(isdigit(name[0]) || name[0]=='-' || name[0] == '+')
		return name;
	else if(isalpha(name[0]))
	{
		if(name=="true")
			return "1";
		else if(name=="false")
			return "0";
		else
		{
			for(int k=0; k<symbolTable.size(); k++)
			{
				if (symbolTable[k].externalName == name)
					return symbolTable[k].Value;
			}
			error=1;
			listingFile<<"\nError: Line " << lineNo-1<<": reference to undefined variable\n\n";
			Error();
		}
	}
	else
	{
		error=1;
		listingFile<<"\nError: Line " << lineNo-1<<": reference to undefined variable\n\n";
		Error();
	}
}

string NextToken()
{
	//cout << "string NextToken()" << endl;
  token = "";
  while (token == "")
  {
    if(charac=='{')
    {

	  NextChar();
      while(charac!= '$'&& charac !='}')
		NextChar();
      if(charac=='$')
	  {
		  error=1;
          listingFile<<"\nError: Line " << lineNo-1<<": unexpected end of file\n\n";
		  Error();
	 }
      else
      	NextChar();
    }

    else if(charac=='}')
	{
		error=1;
        listingFile<<"\nError: Line " << lineNo-1<<": '}' cannot begin token\n\n";
		Error();
    }

    else if(isspace(charac))
      NextChar();

    else if(isalpha(charac))
    {
		token = charac;
		if (isupper(charac))
		{
			error = 1;
			listingFile << "\nError: Line " << lineNo-1 << ": uppercase letter is invalid\n\n";
			Error();
		}
      while(isalpha(NextChar()) || isdigit(charac) || charac =='_')
	  {
		if (isupper(charac))
		{
			error = 1;
			listingFile << "\nError: Line " << lineNo-1 << ": uppercase letter is invalid\n\n";
			Error();
		}

		token+=charac;
		if(charac =='_')
			underScore=true;
		else
			underScore=false;
		}

      if(underScore==true)
	  {
		error=1;
        listingFile <<"\nError: Line " << lineNo-1<<": '_' cannot end token\n\n";
	    Error();
	  }
    }

    else if(isdigit(charac))
    {
      token=charac;
      while(isdigit(NextChar()))
        token+=charac;
    }

    else if(charac=='$')
      token=charac;

    else if(charac == '=' || charac ==':' || charac == ',' || charac == ';' || charac == '.' || charac == '+' || charac == '-'|| charac == '*'|| charac == '('|| charac == ')'|| charac == '<'|| charac == '>')
    {
		if(charac==':')
		{
			token=charac;
			if(NextChar()=='=')
			{
				token+=charac;
				NextChar();
			}

		}

		else if(charac=='<')
		{
			token = charac;
			if(NextChar()=='>' || charac=='=')
			{
				token+=charac;
				NextChar();
			}
		}

		else if(charac=='>')
		{
			token = charac;
			if(NextChar()=='=')
			{
				token+=charac;
				NextChar();
			}
		}

		else
		{
			token = charac;
			NextChar();
		}
    }

    else
	{
		error=1;
        listingFile<<"\nError: Line " << lineNo-1<<": illegal symbol\n\n";
		Error();
	}
  }
	if(token=="else")
	{
		countelse++;
		if(countelse>countif)
		{
			error=1;
			listingFile << "\nError: Line " << lineNo-1<<": improper nesting of if statement\n\n";
			Error();

		}

	}
  return token;
}


char NextChar()
{
	
	if(sourceFile.eof())
	{
		charac='$';
		sourceFile.close();
	}
	else
	{

	//if(charac=='$')
	//	return charac;

	if(charac == '\n'&& finish ==false )
	{
		listingFile <<setw(5)<<right<< lineNo << '|';
		lineNo++;
	}
	sourceFile.get(charac);
	}
	if(charac!='$')
		listingFile<<charac;

	//cout << "char NextChar()" << charac << endl;
	return charac;
}

bool isNonKeyID(string x)
{
	//cout << "bool isNonKeyID(" << x << ")" << endl;
	//isNonKeyID can take in a parameter and compare it token by token
	//non key ids cannot begin with a number or contain special symbols not
	//including underscore
	//they may not be keywords such as program, begin, end, var, const, integer
	//cout << x[0] << " " << islower(x[0]) << endl;
	//x = token;

	if (x == "program" ||
			x == "begin" ||
			x == "end" ||
			 x == "var" ||
			  x == "const" ||
				 x == "integer" ||
				 x == "boolean" ||
				 x == "true" ||
				 x == "false" ||
				 x == "not" ||
				  x == "mod"||
					x == "div" ||
					x == "and"||
					 x == "or" ||
					  x == "read"||
						 x == "write" ||
						  x == "if" ||
							 x == "then"||
							 x == "else" ||
							  x == "repeat" ||
								x == "while" ||
								 x == "do"||
								 x == "until") {
		//cout << "keyword" << endl;
		return false;
	}
	else if (isdigit(x[0]) == true) {
		//cout << "digit" << endl;
		return false;
	}
	else if (isalpha(x[0])) {
		//cout << "alpha" << endl;
		return true;
	}

	else
	{
		//cout << "WHAT THE FUCK" << endl;
		return false;
	}
}

string GenInternalName(storeType x) {
	//cout << "string GenInternalName(" << x << ")" << endl;
	static int numBool = 0;
	static int numInt = 0;

	ostringstream oss;

	string internal;

	if (x == BOOLEAN) {
		oss << "B" << numBool;
		numBool += 1;
	}

	if (x == INTEGER) {
		oss << "I" << numInt;
		numInt += 1;
	}

	if (x == PROG_NAME)
	{
		oss << "P0";
	}

	internal =oss.str();
	return internal;
}

bool isInteger(string x)
{
	//cout << "bool isInteger(" << x << ")" << endl;
	for(int i=0; i<x.length(); i++)
	{
		if(isdigit(x[i])==false)
			return false;
	}

	return true;
}

void PushOperator(string name)
{
	//cout << "void PushOperator(" << name << ")" << endl;
	operatorStack.push(name);
}

void PushOperand(string name)
{
	//cout << "void PushOperand(" << name << ")" << endl;
	string n=name, temp, v;
	static bool found;
	found=false;
	//if name is a literal and has no symbol table entry



	if(name=="true" || name=="false")
	{
		if(name=="true") {
			n="tRuE";
			v = "1";
		}
		else if(name=="false") {
			n="fAlSe";
			v = "0";
		}

		for(int k=0; k<symbolTable.size(); k++)
		{
			if (symbolTable[k].Value == n && symbolTable[k].dataType==BOOLEAN)
			{
				found=true;
				n=symbolTable[k].internalName;
				operandStack.push(n);
			}
		}
		if(found==false) {
			Insert(n, BOOLEAN, CONSTANT, v, YES, 1);
			operandStack.push(name);
			found = true;
		}
	}


	if(isInteger(name) || name[0] == '-')
	{
		for(int k=0; k<symbolTable.size(); k++)
		{
			if (symbolTable[k].Value == name && symbolTable[k].dataType==0)
			{
				found=true;
				n=symbolTable[k].internalName;
				operandStack.push(n);
			}
		}

		if(found==false) {
			Insert(name, WhichType(name), CONSTANT, n, YES, 1);
			operandStack.push(name);
			found = true;
		}
	}

	else if (name[0]=='T' && isdigit(name[1]))
	{

		found=true;
		operandStack.push(name);

	}

	else if (name[0]=='L' && isdigit(name[1]))
	{

		found=true;
		operandStack.push(name);

	}

	else if(isNonKeyID(name))
	{
		//cout << "ITS A NON KEY ID BOI" << endl;
		for(int k=0; k<symbolTable.size(); k++)
		{
			if (symbolTable[k].externalName == name)
			{
				found=true;
				operandStack.push(symbolTable[getIndex(name)].internalName);
			}
		}
		if (found == false) {
			error=1;
			listingFile<<"\nError: Line " << lineNo-1<<": reference to undefined variable\n\n";
			Error();
		}
	}

	if(found==false)
	{
		Insert(GenInternalName(WhichType(name)), WhichType(name), CONSTANT, n, YES, 1);
		operandStack.push(name);
	}


	n="";
	temp="";
}


bool isLit(string name)
{
	//cout << "bool isLit("<< name << ")" << endl;
	if(isdigit(name[0])|| name[0]=='+' || name[0]=='-')
	{
		for(int i=1; i<name.length(); i++)
		{
			if(!isdigit(name[i]))
				return false;
		}

		return true;
	}

	else if(name=="true" || name=="false" || name=="not true"|| name=="not false" )
		return true;

	else
		return false;

}

string PopOperator()
{
	//cout << "string PopOperator()" << endl;
	string temp="";
	if(!operatorStack.empty())
	{
		temp=operatorStack.top();
		operatorStack.pop();
		return temp;
	}

	else
	{
		error=1;
        listingFile<<"\nError: Line " << lineNo-1<<": operator stack underflow\n\n";
		Error();
	}

}

string PopOperand()
{
	//cout << "string PopOperand()" << endl;
	string temp="";
	if(!operandStack.empty())
	{
		temp=operandStack.top();
		operandStack.pop();
		return temp;
	}

	else
	{
		error=1;
        listingFile<<"\nError: Line " << lineNo-1<<": operand stack underflow\n\n";
		Error();
	}

}

void EmitAdditionCode(string op1, string op2) {
	//cout << "void EmitAdditionCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1) == INTEGER && WhichType(op2) == INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}

		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}

		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", getInternalName(op2),"", "");
			AssignRegister(op2, "A");
			printCode("", "IAD", getInternalName(op1),"", getExternalName(op2) + " + " + getExternalName(op1));
		}

		else if (ARegister == op1) {
			printCode("", "IAD", getInternalName(op2),"", getExternalName(op2) + " + " + getExternalName(op1));
		}
		else if (ARegister == op2) {
			printCode("", "IAD", getInternalName(op1),"", getExternalName(op2) + " + " + getExternalName(op1));
		}

		if (op1[0] == 'T') {
			FreeTemp();
		}
		if (op2[0] == 'T'){
			FreeTemp();
		}

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);
	}

	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": cannot add boolean\n\n";
		Error();
	}
}

void EmitSubtractionCode(string op1, string op2) {
	//cout << "void EmitSubtractionCode(" << op1 << ", " << op2 << ")" << endl;
	if (symbolTable[getIndex((getInternalName(op1)))].dataType == INTEGER && symbolTable[getIndex((getInternalName(op2)))].dataType == INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op2) {
			printCode("", "LDA", op2,"", "");

			AssignRegister(op2, "A");
		}

		printCode("", "ISB", getInternalName(op1),"", getExternalName(op2) + " - " + getExternalName(op1));

		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);
	}

	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": cannot subtract boolean\n\n";
		Error();
	}
}

void EmitDivisionCode(string op1, string op2) {
	//cout << "void EmitDivisionCode(" << op1 << ", " << op2 << ")" << endl;
	if (symbolTable[getIndex((getInternalName(op1)))].dataType == INTEGER && symbolTable[getIndex((getInternalName(op2)))].dataType == INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op2) {
			printCode("", "LDA", getInternalName(op2),"", "");
			AssignRegister(op2, "A");
		}

		printCode("", "IDV", getInternalName(op1),"", getExternalName(op2) + " div " + getExternalName(op1));

		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);
	}
	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}
}

void EmitMultiplicationCode(string op1, string op2) {
	//cout << "void EmitMultiplicationCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1) == INTEGER && WhichType(op2) == INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != getInternalName(op2))
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != getInternalName(op2)){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", getInternalName(op2),"", "");
			AssignRegister(op2, "A");
			printCode("", "IMU", getInternalName(op1),"", getExternalName(op2) + " * " + getExternalName(op1));
		}
		else if (ARegister == op1) {
			printCode("", "IMU", getInternalName(op2),"", getExternalName(op2) + " * " + getExternalName(op1));
		}
		else if (ARegister == op2) {
			printCode("", "IMU", getInternalName(op1),"", getExternalName(op2) + " * " + getExternalName(op1));
		}



		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);
	}

	else {
		error = 1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": cannot multiply boolean\n\n";
		Error();
	}
}

void EmitAndCode(string op1, string op2) {
	//cout << "void EmitAndCode(" << op1 << ", " << op2 << ")" << endl;
	if (symbolTable[getIndex((getInternalName(op1)))].dataType == BOOLEAN && symbolTable[getIndex((getInternalName(op2)))].dataType == BOOLEAN) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}

		printCode("", "IMU", op2,"", symbolTable[getIndex(op2)].externalName + " and " + symbolTable[getIndex(op1)].externalName);

		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}

	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}
}

void EmitOrCode(string op1, string op2) {
	//cout << "void EmitOrCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1) == BOOLEAN && WhichType(op2) == BOOLEAN) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
			printCode("", "IAD", getInternalName(op1),"", getExternalName(op2) + " or " + getExternalName(op1));
		}

		else if (ARegister == op1) {
			printCode("", "IAD", getInternalName(op2),"", getExternalName(op2) + " or " + getExternalName(op1));
		}
		else if (ARegister == op2) {
			printCode("", "IAD", getInternalName(op1),"", getExternalName(op2) + " or " + getExternalName(op1));
		}

		NextLine();
		printCode("", "AZJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "TRUE" ,"", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);

		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}

	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}
}

void EmitWriteCode(string op1)
{
	//cout << "void EmitWriteCode(" << op1 << ")" << endl;
	if (!doesExist(op1)){
		error=1;
		listingFile<<"\nError: Line " << lineNo-1<<": reference to undefined variable\n\n";
		Error();
	}

	if(WhichType(op1)!=INTEGER && WhichType(op1) != BOOLEAN)
	{
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}

	printCode("", "PRI", getInternalName(op1),"", "write("+getExternalName(op1)+")");
}

void EmitLessThanCode(string op1, string op2) {
	//cout << "void EmitLessThanCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1)==INTEGER && WhichType(op2)==INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				//listingFile << right << setw(9) << "STA " << ARegister << endl;
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}

		printCode("", "ISB", getInternalName(op1),"", symbolTable[getIndex(op2)].externalName + " < " + symbolTable[getIndex(op1)].externalName);

		NextLine();
		printCode("", "AMJ", currentLineLabel ,"", "");
		printCode("", "LDA", "FALS" ,"", "");
		if (!doesExist("FALSE"))
			Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		printCode("", "UNJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "TRUE", "", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}

	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Incompatible types\n\n";
		Error();
	}

}


void EmitLessEqCode(string op1, string op2) {
	//cout << "void EmitLessEqCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1)==INTEGER && WhichType(op2)==INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				//listingFile << right << setw(9) << "STA " << ARegister << endl;
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}

		printCode("", "ISB", op1,"", symbolTable[getIndex(op2)].externalName + " <= " + symbolTable[getIndex(op1)].externalName);

		NextLine();
		printCode("", "AMJ", currentLineLabel ,"", "");
		printCode("", "AZJ", currentLineLabel ,"", "");
		printCode("", "LDA", "FALS" ,"", "");
		if (!doesExist("FALSE"))
			Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		printCode("", "UNJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "TRUE", "", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Incompatible types\n\n";
		Error();
	}

}


void EmitGreaterThanCode(string op1, string op2) {
	//cout << "void EmitGreaterThanCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1)==INTEGER && WhichType(op2)==INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}

		printCode("", "ISB", getInternalName(op1),"",symbolTable[getIndex(op2)].externalName + " > " + symbolTable[getIndex(op1)].externalName);

		NextLine();
		printCode("", "AMJ", currentLineLabel ,"", "");
		printCode("", "AZJ", currentLineLabel, "", "");
		printCode("", "LDA", "TRUE" ,"", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		printCode("", "UNJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "FALS", "", "");
		if (!doesExist("FALSE"))
			Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(getInternalName((ARegister)))].dataType=BOOLEAN;
		PushOperand(ARegister);
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Incompatible types\n\n";
		Error();
	}

}


void EmitGreaterEqCode(string op1, string op2) {
	//cout << "void EmitGreaterEqCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1)==INTEGER && WhichType(op1)==INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}

		printCode("", "ISB", getInternalName(op1),"", symbolTable[getIndex(op2)].externalName + " >= " + symbolTable[getIndex(op1)].externalName);
		NextLine();
		printCode("", "AMJ", currentLineLabel ,"", "");
		//printCode("", "AZJ", currentLineLabel ,"+1", "");
		printCode("", "LDA", "TRUE" ,"", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		printCode("", "UNJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "FALS", "", "");
		if (!doesExist("FALSE"))
			Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Incompatible types\n\n";
		Error();
	}

}

void EmitNotEqualsCode(string op1, string op2) {
	//cout << "void EmitNotEqualsCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1)==WhichType(op2)) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}
		printCode("", "ISB", op1,"", symbolTable[getIndex(op2)].externalName + " <> " + symbolTable[getIndex(op1)].externalName);
		NextLine();
		printCode("", "AZJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "TRUE" ,"", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Incompatible types\n\n";
		Error();
	}

}

void EmitEqualsCode(string op1, string op2) {
	//cout << "void EmitEqualsCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1)==WhichType(op2)) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1 && ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op1 && ARegister != op2) {
			printCode("", "LDA", op2,"", "");
			AssignRegister(op2, "A");
		}
		printCode("", "ISB", op1,"", symbolTable[getIndex(op2)].externalName + " = " + symbolTable[getIndex(op1)].externalName);
		NextLine();
		printCode("", "AZJ", currentLineLabel ,"", "");
		printCode("", "LDA", "FALS" ,"", "");
		if (!doesExist("FALSE"))
			Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		printCode("", "UNJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "TRUE", "", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Incompatible types\n\n";
		Error();
	}

}

void EmitModCode(string op1, string op2) {
	//cout << "void EmitModCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op1) == INTEGER && WhichType(op2) == INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister != op2)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1 && ARegister != op2){
				AssignRegister("", "A");
			}
		}
		if (ARegister != op2) {
			printCode("", "LDA", getInternalName(op2),"", "");
			AssignRegister(op2, "A");
		}
		printCode("", "IDV", getInternalName(op1),"", symbolTable[getIndex(op2)].externalName + " mod " + symbolTable[getIndex(op1)].externalName);
		if (op1[0] == 'T')
			FreeTemp();
		if (op2[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		printCode("", "STQ", ARegister,"", "store remainder in memory");
		printCode("", "LDA", ARegister,"", "load remainder from memory");
		symbolTable[getIndex(ARegister)].alloc=YES;
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);
	}
	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}
}

void EmitNotCode(string op1) {
	//cout << "void EmitNotCode(" << op1 << ")" << endl;
	if (WhichType(op1) == BOOLEAN) {
		if (ARegister[0] == 'T') {
			if (ARegister != op1)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1){
				AssignRegister("", "A");
			}
		}

		if (ARegister != op1) {
			printCode("", "LDA", op1,"", "");
			AssignRegister(op1, "A");
		}

		NextLine();
		printCode("", "AZJ", currentLineLabel ,"", "not "+getExternalName(op1));
		printCode("", "LDA", "FALS" ,"", "");
		if (!doesExist("FALSE"))
			Insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		printCode("", "UNJ", currentLineLabel ,"+1", "");
		printCode(currentLineLabel, "LDA", "TRUE", "", "");
		if (!doesExist("TRUE"))
			Insert("true", BOOLEAN, CONSTANT, "1", YES, 1);
		if (op1[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=BOOLEAN;
		PushOperand(ARegister);
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}

}

void EmitEndCode()
{
	//cout << "void EmitEndCode()" << endl;
		listingFile<<endl;
		CreateListingTrailer();
		listingFile.close();

		printCode("", "HLT", "","", "");
		printCondensedSymbolTable();
		printCode("", "END", "STRT","", "");
		objectFile.close();
		exit(0);
}

void EmitAssignCode(string op1, string op2)
{
	//cout << "void EmitAssignCode(" << op1 << ", " << op2 << ")" << endl;
	if (WhichType(op2) == WhichType(op1))
	{
		//cout << "if (WhichType(op2) == WhichType(op1)) " << "True" << endl;
		//cout << symbolTable[getIndex(op2)].mode <<" "<< getIndex(op2) <<" "<< op2 <<" "<< VARIABLE << endl;
		//cout << getExternalName(op2) << endl;
		if (symbolTable[getIndex(op2)].mode == VARIABLE)
		{
			//cout << "if (symbolTable[getIndex((getInternalName(op2)))].mode == VARIABLE)" << "True" << endl;
			if (op1 == op2)
			{
				return;
			}
			else if (ARegister != op1)
			{
				printCode("", "LDA", getInternalName(op1), "", "");
			}
			printCode("", "STA", getInternalName(op2), "", symbolTable[getIndex(op2)].externalName+" := " +symbolTable[getIndex(op1)].externalName);
			AssignRegister(op2, "A");

			if (op1[0] == 'T')
				FreeTemp();
			if (op2[0] == 'T')
				FreeTemp();
		}
		else
		{
			error=1;
			//cout << "EmitAssignCode " + op1 + " " + op2 << endl;
			listingFile << "\n Error: Line " << lineNo - 1 << ": symbol on left-hand side of assignment must have a storage mode of VARIABLE\n\n";
			Error();
		}
	}
	else {
		error=1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": incompatible types\n\n";
		Error();
	}
}

void EmitNegCode(string op1) {
	//cout << "void EmitNegCode(" << op1 << ")" << endl;
	if (symbolTable[getIndex((getInternalName(op1)))].dataType == INTEGER) {
		if (ARegister[0] == 'T') {
			if (ARegister == op1)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1){
				AssignRegister("", "A");
			}
		}
		NextLine();
		printCode("", "LDA", "ZERO","", "");
		if (!doesExist("ZERO"))
			Insert("ZERO", INTEGER, CONSTANT, "0", YES, 1);
		printCode("", "ISB", op1,"", "-"+getExternalName(op1));
		if (op1[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);
	}

	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}
}

void EmitPosCode(string op1) {
	//cout << "void EmitPosCode(" << op1 << ")" << endl;
	if (symbolTable[getIndex(getInternalName(op1))].dataType == INTEGER) {

		if (ARegister[0] == 'T') {
			if (ARegister != op1)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1)
				AssignRegister("", "A");
		}
		if (ARegister != op1) {
			printCode("", "LDA", op1,"", "");
			AssignRegister(op1, "A");
		}
		NextLine();
		//printCode("", "AMJ", currentLineLabel ,"", "+"+getExternalName(op1)+"still here");
		//printCode(currentLineLabel, "LDA", "TRUE" ,"", "");
		if (op1[0] == 'T')
			FreeTemp();

		ARegister = GetTemp();
		symbolTable[getIndex(ARegister)].dataType=INTEGER;
		PushOperand(ARegister);


	}

	else {
		error=1;
        listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}
}

void EmitThenCode(string op1)
{
	//cout << "void EmitThenCode(" << op1 << ")" << endl;
	string tempLabel;
	NextLine();
	tempLabel=currentLineLabel;
		if (ARegister[0] == 'T') {
			if (ARegister != op1)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1)
				AssignRegister("", "A");
		}
		if (ARegister != op1) {
			printCode("", "LDA", getInternalName(op1),"", "");
			AssignRegister(op1, "A");
		}
	printCode("", "AZJ", tempLabel ,"","if false jump to "+tempLabel);
	PushOperand(tempLabel);
	tempLabel="";

	if(op1[0]=='T')
		FreeTemp();

	AssignRegister("", "A");

}

void EmitElseCode(string op1)
{
	//cout << "void EmitElseCode(" << op1 << ")" << endl;
	string tempLabel;
	NextLine();
	tempLabel=currentLineLabel;
	printCode("", "UNJ", tempLabel ,"","jump to end if");
	printCode(op1, "NOP", "" ,"","else");
	PushOperand(tempLabel);
	tempLabel="";
	AssignRegister("", "A");

}

void EmitPostIfCode(string op1)
{
	//cout << "void EmitPostIfCode(" << op1 << ")" << endl;
	printCode(op1, "NOP", "" ,"","end if");
	AssignRegister("", "A");

}

void EmitWhileCode()
{
//cout << "void EmitWhileCode()" << endl;
	NextLine();
	label=currentLineLabel;
	printCode(label, "NOP", "" ,"","while");
	PushOperand(label);
	AssignRegister("", "A");

}

void EmitDoCode(string op1)
{
	//cout << "void EmitDoCode(" << op1 << ")" << endl;
	string tempLabel;
	NextLine();
	tempLabel=currentLineLabel;
		if (ARegister[0] == 'T') {
			if (ARegister != op1)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1)
				AssignRegister("", "A");
		}
		if (ARegister != op1) {
			printCode("", "LDA", getInternalName(op1),"", "");
			AssignRegister(op1, "A");
		}

	printCode("", "AZJ", tempLabel ,"","do");
	PushOperand(tempLabel);
	tempLabel="";

	if(op1[0]=='T')
		FreeTemp();

	AssignRegister("", "A");

}

void EmitPostWhileCode(string op1, string op2)
{
	//cout << "void EmitPostWhileCode(" << op1 << ", " << op2 << ")" << endl;
	printCode("", "UNJ", op2 ,"","end while");
	printCode(op1, "NOP", "" ,"","");
	AssignRegister("", "A");

}

void EmitRepeatCode()
{
	//cout << "void EmitRepeatCode()" << endl;
	string tempLabel;
	NextLine();
	tempLabel=currentLineLabel;
	printCode(tempLabel, "NOP", "" ,"","repeat");
	PushOperand(currentLineLabel);
	AssignRegister("", "A");

}

void EmitUntilCode(string op1, string op2)
{
	//cout << "void EmitUntilCode(" << op1 << ", " << op2 << ")" << endl;
		if (ARegister[0] == 'T') {
			if (ARegister != op1)
			{
				printCode("", "STA", ARegister,"", "deassign AReg");
				symbolTable[getIndex(ARegister)].alloc = YES;
				AssignRegister("", "A");
			}
		}
		else if(ARegister[0] != 'T') {
			if (ARegister != op1)
				AssignRegister("", "A");
		}
		if (ARegister != op1) {
			printCode("", "LDA", getInternalName(op1),"", "");
			AssignRegister(op1, "A");
		}

	printCode("", "AZJ", op2 ,"","until");
	if(op1[0]=='T')
		FreeTemp();

	AssignRegister("", "A");

}

void EmitReadCode(string op1)
{
	//cout << "void EmitReadCode(" << op1 << ")" << endl;
	if (!doesExist(op1)){
		error=1;
		listingFile<<"\nError: Line " << lineNo-1<<": reference to undefined variable\n\n";
		Error();
	}

	if (symbolTable[getIndex(getInternalName(op1))].mode == CONSTANT) {
		error=1;
		listingFile<<"\nError: Line " << lineNo-1<<": reading in of read-only location '" << op1 << "'\n\n";
		Error();
	}

	if (WhichType(op1) != 0) {
		error = 1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": Illegal type\n\n";
		Error();
	}

	printCode("", "RDI", getInternalName(op1),"", "read("+getExternalName(op1)+")");
}

void NextLine()
{
	//cout << "void NextLine()" << endl;
	static int currentLine = -1;
	currentLine++;
	ostringstream oss;
	oss << "L" << currentLine;
	currentLineLabel =oss.str();
}

void FreeTemp()
{
	//cout << "void FreeTemp()" << endl;
	currentTempNo--;
	if (currentTempNo < -1)
		listingFile<<"\nError: Line " << lineNo-1<<": compile error, "<< currentTempNo << "should be >= -1\n\n";
}

string GetTemp()
{
	//cout << "string GetTemp()" << endl;
	string temp;
	ostringstream oss;
	currentTempNo++;
	oss << "T" << currentTempNo;
	temp = oss.str();

	if (currentTempNo > maxTempNo)
	{
		Insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
		maxTempNo++;
	}

	return temp;
}

void AssignRegister(string x, string reg){
	//cout << "void AssignRegister(" << x << ", " << reg << ")" << endl;
//AssignmentRegister takes to arguments.
//The string to be stored in the register.
//And the register to store it in.

	if (reg == "A") {
		ARegister = x;
	}
	else if (reg == "Q") {
		QRegister = x;
	}
	else {
		error = 1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": No register " << reg << "\n\n";
		Error();
	}
}

void printCode(string location, string operation, string address, string signNum, string comment)
{
	//cout << "void printCode(" << location << ", " << operation << ", " << address << ", " << signNum << ", " << comment << ")" << endl;
	if (operation=="DEC" || operation=="BSS") {
		if (address[0] == '-')
			objectFile<<setw(4) << left <<location<<"  "<<setw(4)<<operation<<"-"<<setw(4)<<setw(3)<<setfill('0')<<right<<address.substr(1,string::npos)<<left<<setw(5)<<setfill(' ')<<signNum<<comment<<endl;
		else
			objectFile<<setw(4) << left <<location<<"  "<<setw(4)<<operation<<setw(4)<<setfill('0')<<right<<address<<left<<setw(5)<<setfill(' ')<<signNum<<comment<<endl;
	}
	else
		objectFile<<setw(4) << left <<location<<"  "<<setw(4)<<operation<<setw(4)<<address<<setw(5)<<signNum<<comment<<endl;
}

void Code(string optr)
{
	//cout << "void Code(" << optr << ")" << endl;
	if (optr == "read" 	||
		optr == "write" 	||
		optr == "not" 		||
		optr == "neg"		||
		optr == "then"		||
		optr == "else"		||
		optr == "postIf"	||
		optr == "do")
	{
		string opand = PopOperand();
		if (optr == "read")
			EmitReadCode(opand);
		else if (optr == "write")
			EmitWriteCode(opand);
		else if (optr == "not")
			EmitNotCode(opand);
		else if (optr == "neg")
			EmitNegCode(opand);
		else if (optr == "pos")
			EmitPosCode(opand);
		else if (optr == "then")
			EmitThenCode(opand);
		else if (optr == "else")
			EmitElseCode(opand);
		else if (optr == "postIf")
			EmitPostIfCode(opand);
		else if (optr == "do")
			EmitDoCode(opand);
	}
	else if (optr == ":="			||
			 optr == "+"			||
			 optr == "-"			||
			 optr == "*"			||
			 optr == "div"			||
			 optr == "mod"			||
			 optr == "="			||
			 optr == "<>"			||
			 optr == ">"			||
			 optr == "<"			||
			 optr == ">="			||
			 optr == "<="			||
			 optr == "and"			||
			 optr == "or"			||
			 optr == "postWhile"	||
			 optr == "until")
	{
		string opand1 = PopOperand();
		string opand2 = PopOperand();
		if (optr == ":=")
			EmitAssignCode(opand1,opand2);
		else if (optr == "+")
			EmitAdditionCode(opand1,opand2);
		else if (optr == "-")
			EmitSubtractionCode(opand1,opand2);
		else if (optr == "*")
			EmitMultiplicationCode(opand1,opand2);
		else if (optr == "div")
			EmitDivisionCode(opand1,opand2);
		else if (optr == "mod")
			EmitModCode(opand1,opand2);
		else if (optr == "=")
			EmitEqualsCode(opand1,opand2);
		else if (optr == "<>")
			EmitNotEqualsCode(opand1,opand2);
		else if (optr == ">")
			EmitGreaterThanCode(opand1,opand2);
		else if (optr == "<")
			EmitLessThanCode(opand1,opand2);
		else if (optr == ">=")
			EmitGreaterEqCode(opand1,opand2);
		else if (optr == "<=")
			EmitGreaterEqCode(opand1,opand2);
		else if (optr == "and")
			EmitAndCode(opand1,opand2);
		else if (optr == "or")
			EmitOrCode(opand1,opand2);
		else if (optr == "postWhile")
			EmitPostWhileCode(opand1,opand2);
		else if (optr == "until")
			EmitUntilCode(opand1,opand2);
	}
	else if (optr == "while")
		EmitWhileCode();
	else if (optr =="program")
		printCode("STRT", "NOP", "", "", symbolTable[0].Value + "NAME");
	else if (optr == "repeat")
		EmitRepeatCode();
	else if (optr == "end")
		EmitEndCode();
	else
	{
		error = 1;
		listingFile << "\n Error: Line " << lineNo - 1 << ": undefined operation\n\n";
		Error();
	}
}

int getIndex(string x)
{
	//cout << "int getIndex("<< x << ")" << endl;
	for (int i = 0; i < symbolTable.size(); i++) {
		if (x == symbolTable[i].internalName || x == symbolTable[i].externalName)
			return i;
	}
	return 0;
}

bool doesExist(string x)
{
	//cout << "bool doesExist("<< x << ")" << endl;
	for (int i = 0; i < symbolTable.size(); i++) {
		if (x == symbolTable[i].externalName || x == symbolTable[i].internalName)
			return true;
	}
	return false;
}

string getExternalName(string x)
{
	//cout << "string getExternalName("<< x <<")" << endl;
	for (int i = 0; i < symbolTable.size(); i++) {
		if (x == symbolTable[i].internalName){
			return symbolTable[i].externalName;
		}
	}
	return x;
}

string getInternalName(string x) {
	//cout << "string getInternalName("<< x << ")"<< endl;
	for (int i = 0; i < symbolTable.size(); i++) {
		if (x == symbolTable[i].externalName){
			return symbolTable[i].internalName;
		}
	}
	return x;
}
