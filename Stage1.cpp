// Trevor Smith & Seokhee Han
// CS 4301
// Stage 1

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stage1.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <stack>
#include <cctype>

using namespace std;

Compiler::Compiler(char **argv)
{
	sourceFile.open(argv[1]);
	listingFile.open(argv[2]);
	objectFile.open(argv[3]);
}

Compiler::~Compiler()
{
	sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
	time_t now = time (NULL);
	listingFile << left << "STAGE0:  Trevor Smith, Seokhee Han\t" << ctime(&now) << endl;
	listingFile << "LINE NO." << setw(31) << right << "SOURCE STATEMENT" << endl << endl;
}

void Compiler::parser()
{
   
   nextChar();
   
   if (nextToken() != "program")
   {
      processError("keyword \"program\" expected");
   }
   
   prog();
}

void Compiler::createListingTrailer()
{
	listingFile << "\nCOMPILATION TERMINATED        " << errorCount << " ERROR ENCOUNTERED" << endl;
} 

void Compiler::processError(string err)
{
	listingFile << "\nError on line " << lineNo  <<' '<< err << endl;
	exit(EXIT_FAILURE);
}

void Compiler::prog() 
{
	if (token != "program")
   {
      processError("keyword \"program\" expected");
   }
	
	progStmt();	   
	
	if (token == "const") 
   {
		consts();
   }
   
	if (token == "var")
   {
		vars();
   }
   
	if (token != "begin")
   {
		processError("keyword \"begin\" expected");
   }
   
	beginEndStmt();
   
	if (sourceFile.eof())
   {
		processError("no text may follow \"end\"");
   }
}

void Compiler::progStmt() 
{
	string x;
	if (token != "program")
   {
		processError("keyword \"program\" expected");
   }
   
	x = nextToken();
   
	if (!isNonKeyId(x))
   {
		processError("program name expected\n");
   }
   
	if (nextToken() != ";")
   {
		processError("semicolon expected\n");
   }
   
	nextToken();
   
	code("program", x);
   
	insert(x, PROG_NAME,CONSTANT, x, NO, 0);  
}

void Compiler::consts()
{
	if (token != "const")
   {
		processError("keyword \"const\" expected");
   }
   
	if (!isNonKeyId(nextToken()))
   {
		processError("non-keyword identifier must follow \"const\"");
   }
   
	constStmts();
}

void Compiler::vars()
{
	if (token != "var")
   {
		processError("keyword \"var\" expected");
   }
   
	if (!isNonKeyId(nextToken()))
   {
		processError("non-keyword identifier must follow \"var\"");
   }
   
	varStmts();
}

void Compiler::beginEndStmt() 
{
	if (token != "begin")
   {
		processError("keyword \"begin\" expected");
   }   
   
   execStmts();
   
	if (token != "end")
   {
		processError("keyword \"end\" expected");
   }
   
	if (nextToken() != ".")
   {
		processError("period expected");
   }
   
	code("end",".");
}

void Compiler::constStmts()   
{
	string x,y;
	if (!isNonKeyId(token))
   {
		processError("non-keyword identifier expected");
   }
   
	x = token;
   
	if (nextToken() != "=")
   {   
		processError("\"=\" expected");
   }  
   
	y = nextToken();
   
	if (y != "+" && y != "-" && y != "not" && !isNonKeyId(y) && y != "true" && y != "false" && !isInteger(y))
   {
		processError("token to right of \"=\" illegal");
   }
   
	if (y == "+" || y == "-")
	{
		if (!isInteger(nextToken())) {
			processError("integer expected after sign");
      }
		y = y + token;
	}
   
	if (y == "not")
	{
		if (!isBoolean(nextToken())) {
			processError("boolean expected after \"not\"");
      }
		if (token == "true") {
			y = "false";
      }
		else {
			y = "true";
      }
	} 
   
	if (nextToken() != ";")
   {
		processError("semicolon expected");
   }
   
	map<string, SymbolTableEntry>::iterator it;
	it = symbolTable.find(y);
   
	if (!isBoolean(y) && !isInteger(y))
   {      
		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
   }
   
	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
	x = nextToken();
   
	if (x != "begin" && x != "var" && !isNonKeyId(x))
   {
		processError("non-keyword identifier, \"begin\", or \"var\" expected"); 
   }
   
	if (isNonKeyId(x))
   {
		constStmts();
   }
}	

void Compiler::varStmts()  
{
	string x,y;
	if (!isNonKeyId(token))
	{
		processError("non-keyword identifier expected");
	}
   
	x = ids();
	if (token != ":")
	{
		processError("\":\" expected");
	}
   
	y = nextToken();
   
	if (y != "integer" && y != "boolean")
   {
		processError("illegal type follows \":\"");
   }
   
	if (nextToken() != ";")
   {
		processError("semicolon expected");
   }
   
	insert(x, whichType(y), VARIABLE, "", YES, 1);
   
	if (nextToken() != "begin" && !isNonKeyId(token))
	{
		processError("non-keyword identifier or \"begin\" expected");
	}
   
	if (isNonKeyId(token) && token != "begin")
	{
		varStmts();
	}
}    

string Compiler::ids()   
{
	string temp, tempString;
	if (!isNonKeyId(token))
   {
		processError("non-keyword identifier expected");
   }
   
	tempString = token;
	temp = token;
   
	if (nextToken() == ",")
	{
		if (!isNonKeyId(nextToken())) {
			processError ("non-keyword identifier expected");
      }
		tempString = temp + "," + ids();
	}		
	return tempString;
}	

void Compiler::execStmts()
{
	nextToken();
	if (isNonKeyId(token) || token == "read" || token == "write" ||  token == ";" ||  token == "begin")
   {
		execStmt();
   }
}

void Compiler::execStmt() 
{
	if (isNonKeyId(token)) {
		assignStmt();
   }
	else if (token == "read") {
		readStmt();
   }
	else if (token == "write") {
		writeStmt();
   }
	else if (token == "begin") {
		beginEndStmt();
   }
	execStmts();
}

void Compiler::assignStmt()
{
	if (!isNonKeyId(token))
   {
		processError("non-keyword identifier expected");
   }
   
	pushOperand(token);
	if (nextToken() != ":=")
   {
		processError("\":=\" expected");
   }
   
	pushOperator(token);
	express();
   
	if (token != ";")
   {
		processError("\";\" expected");
   }
   
	string one = popOperand();
	string two = popOperand();
	code(popOperator(), one, two);
}

void Compiler::readStmt() 
{
	string emp = "";
	if (nextToken() != "(")
   {
		processError("\"(\" required after read");
	}
   
	nextToken();
	string usedId = ids();
   
	for (unsigned int i = 0; i < usedId.length(); i++)
	{
		if (usedId[i] == ',')
		{
			code("read", emp);
			emp = "";
		}
		else
      {
			emp += usedId[i];
      }
	}
   
	code("read" , emp);
   
	if (token != ")") {
		processError ( "expected \")\"");
   }
   
	if (nextToken() != ";") {
		processError ( "expected \";\"");
	}
}

void Compiler::writeStmt() 
{
	string emp = "";
	if (nextToken() != "(") 
   {
		processError("expected \"(\" after write");
   }
   
	nextToken();
	string usedId = ids();
	
	for (unsigned int i = 0; i < usedId.length(); i++)
	{
		if (usedId[i] == ',')
		{
			code("write", emp);
			emp = "";
		}
		else
      {
			emp += usedId[i];
      }
	}
   
	code("write" , emp);
	
	if (token != ")") {
		processError ( "expected \")\"");
   }
	if (nextToken() != ";") {
		processError ( "expected \";\"");
   }
}

void Compiler::express() 
{
	nextToken();
	if (!(token == "not" || token == "true" || token == "false" || token == "-" || token == "+" || token == "(" || isInteger(token) || isNonKeyId(token) || token == "mod"))
   {
		processError("passed token cannot be expressed properly");
   }
   
	term();
	expresses();
}

void Compiler::expresses()
{
	if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		pushOperator(token);
		nextToken();
		term();
		string one = popOperand();
		string two = popOperand();
		code(popOperator(), one, two);
		expresses();
	}
}

void Compiler::term()     
{
	if (!(token == "not" || token == "true" || token == "false" || token == "-" || token == "+" || token == "(" || isInteger(token) || isNonKeyId(token) || token == "mod"))
   {
		processError("improper terms");
   }
   
	factor();
	terms();
}

void Compiler::terms()     
{
	if (token == "+" || token == "-" || token == "or")
	{
		pushOperator(token);
		nextToken();
		factor();
		string one = popOperand();
		string two = popOperand();
		code(popOperator(), one, two);
		terms();
	}
}

void Compiler::factor()    
{
	if (!(token == "not" || token == "true" || token == "false" || token == "-" || token == "+" || token == "(" || isInteger(token) || isNonKeyId(token) || token == "mod"))
   {
		processError("improper terms");
   }
   
	part();
	factors();
}

void Compiler::factors()   
{
	if (token== "*" || token == "div" || token== "mod" || token== "and")
	{
		pushOperator(token);
		nextToken();
		part();
		string one = popOperand();
		string two = popOperand();
		code(popOperator(), one, two);
		factors();
	}
}

void Compiler::part()     
{
	map<string, SymbolTableEntry>::iterator it;
   
	if (token == "not")
	{
		nextToken();
		if (token == "(")
		{
			express();
         
			if (token != ")") {
				processError(" \")\" expected");
         }
         
			nextToken();
			string notthis= popOperand();
			code("not", notthis);
		}
		else if (isBoolean(token))
		{
			if (whichValue(token) == "true") {
				pushOperand("false");
         }
			else {
				pushOperand("true");
         }
         
			nextToken();
		}
		else if (isNonKeyId(token))
		{
			it = symbolTable.find(token);
			code("not", it->second.getInternalName());
			nextToken();
		}
		else
      {
			processError("illegal token following \"not\"");
      }
	}
	else if (token == "+")
	{
		nextToken();
		if (token == "(")
		{
			express();
			if (token != ")") {
				processError("\")\" expected");
         }
         
			nextToken();
		}
		else if (isInteger(token) || isNonKeyId(token))
		{
			pushOperand(token);
			nextToken();
		}
		else
      {
			processError("illegal token");
      }
	} 
	else if (token == "-")
	{
		nextToken();
		if (token == "(")
		{
			express();
			if (token != ")") {
				processError("\")\" expected");
         }
         
			nextToken();
			code("neg", popOperand());
		}
		else if (isInteger(token))
		{
			pushOperand('-' + token);
			nextToken();
		}
		else if (isNonKeyId(token))
		{
			it = symbolTable.find(token);
			code("neg", it->second.getInternalName());
			nextToken();
		}
		else
      {
			processError("illegal token");
      }
	}
	else if (token == "(")
	{
		express();
			if (token != ")") {
				processError("\")\" expected");
         }
         
			nextToken();
	}
	else if (isInteger(token))
	{
		pushOperand(token);
		nextToken();
	}
	else if(isBoolean(token))
	{
		pushOperand(token);
		nextToken();
	}
	else if (isNonKeyId(token) || token == "mod")
	{
		pushOperand(token);
		nextToken();
	}
	else
   {
		processError("illegal item");
   }
}

void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	string name = "";
   
	for(uint i = 0; i <=externalName.length(); i++)
	{
		if(externalName[i] != ',' && i !=externalName.length())
		{
			name += externalName[i];
		}
		else
		{
			if(symbolTable.find(name) != symbolTable.end()) {
				processError("multiple name definition");
         }
			else if(isKeyword(name)) {
				processError("illegal use of keyword");
         }
			else
			{
				if(isupper(externalName[0])) {
					symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(name,inType, inMode, inValue, inAlloc, inUnits)));
            }
				else {
					symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType),inType, inMode, inValue, inAlloc, inUnits)));
            }
			}
			name = "";
		}
		
	}
}

storeTypes Compiler::whichType(string name) 
{
	if(name == "boolean" || isBoolean(name) || name[0] == 'B')
   {
		return BOOLEAN;
   }
	else if(name == "integer" || isInteger(name)|| name[0] == 'I')
   {
		return INTEGER;
   }
	else
	{
		map<string, SymbolTableEntry>::iterator it;
		it = symbolTable.find(name);
      
		if(it->second.getDataType() == 0) {
			return INTEGER;
      }
		if(it->second.getDataType() == 1) {
			return BOOLEAN;
      }
	}
   
	return INTEGER;
}

string Compiler::whichValue(string name)
{
   if (isLiteral(name))
   {
      return name;
   }
    else
	{
      if (symbolTable.count(name)) {
         return symbolTable.at(name).getValue();
      }
      else {
         processError("reference to undefined constant");
      }
   }
   
	return name;
}

void Compiler::code(string op, string operand1 , string operand2)
{
	operand1 = operand1.substr(0, 15);
	operand2 = operand2.substr(0, 15);
   
	if (op == "program")
		emitPrologue(operand1);
	else if (op == "end")
		emitEpilogue();
	else if (op == "read")
		emitReadCode(operand1, operand2);
	else if(op == "write")
		emitWriteCode(operand1, operand2);
	else if(op == "+")
		emitAdditionCode(operand1, operand2);
	else if (op == "-")
		emitSubtractionCode(operand1, operand2);
	else if(op == "neg")
		emitNegationCode(operand1, operand2);
	else if(op == "not")
		emitNotCode(operand1, operand2);
	else if(op == "*")
		emitMultiplicationCode(operand1, operand2);
	else if(op == "div")
		emitDivisionCode(operand1, operand2);
	else if(op == "mod")
		emitModuloCode(operand1, operand2);
	else if(op == "and")
		emitAndCode(operand1, operand2);
	else if(op == "or")
		emitOrCode(operand1, operand2);
	else if(op == "<")
		emitLessThanCode(operand1, operand2);
	else if(op == ">")
		emitGreaterThanCode(operand1, operand2);
	else if(op == "<=")
		emitLessThanOrEqualToCode(operand1, operand2);
	else if(op == ">=")
		emitGreaterThanOrEqualToCode(operand1, operand2);
	else if(op == "<>")
		emitInequalityCode(operand1, operand2);
	else if(op == "=")
      emitEqualityCode(operand1, operand2);
	else if(op == ":=")
		emitAssignCode(operand1, operand2);
	else
		processError("compiler error since function code should not be called with illegal arguments");
}

void Compiler::pushOperator(string op)
{
	operatorStk.push(op);
}

string Compiler::popOperator()
{
	if(operatorStk.empty())
   {
		processError("compiler error; operator stack underflow");
   }
   
   string returnItem = operatorStk.top();
   operatorStk.pop();

   
	return returnItem;
}

void Compiler::pushOperand(string operand)
{
	if (operand == "true") {
		operand = "TRUE";
   }
	if (operand == "false") {
		operand = "FALSE";
   }
	if ((isBoolean(operand)|| isInteger(operand)) && symbolTable.count(operand) == 0 ) {
		insert(operand, whichType(operand), CONSTANT, whichValue(operand), YES,1);
	}
   
	operandStk.push(operand);
}

string Compiler::popOperand()
{
	if (operandStk.empty())
   {
		processError("compiler error; operand stack underflow");
   }
   
   string returnItem = operandStk.top();
	operandStk.pop();
   
   return returnItem;
}

void Compiler::emit(string label, string instruction, string operands,string comment)
{
	objectFile << left << setw(8) << label << setw(8)  << instruction << setw(24) << operands << comment << endl; 
}

void Compiler::emitPrologue(string progName, string operand2) 
{
	time_t now = time(NULL); 
   
	objectFile << "; Trevor Smith, Seokhee Han\t" << ctime(&now) << endl;
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program" + progName);
   objectFile << endl;
	emit("_start:");
}

void Compiler::emitEpilogue(string, string)
{
	emit("","Exit", "{0}", "\n");
	emitStorage();
}

void Compiler::emitStorage() 
{
	map<string, SymbolTableEntry>::iterator it;
	
	emit("SECTION", ".data");
	for (it = symbolTable.begin(); it != symbolTable.end(); it++)
	{
		if(it->second.getInternalName().at(0) == 'B'&& it->second.getValue() == "false" ) {
			emit(it->second.getInternalName(), "dd", "0", "; " + it->first);
      }
		else if(it->second.getInternalName().at(0) == 'B' && it->second.getValue() == "true" ) {
			emit(it->second.getInternalName(), "dd", "-1", "; " + it->first);
      }
		else if (it->second.getInternalName() == "TRUE") {
			emit(it->second.getInternalName(), "dd", "-1", "; " + it->first);
      }
		else if (it->second.getInternalName() == "FALSE") {
			emit(it->second.getInternalName(), "dd", "0", "; " + it->first);
      }
		else if(it->second.getAlloc() == YES && it->second.getMode() == CONSTANT) {
			emit(it->second.getInternalName(), "dd", it->second.getValue(), "; " + it->first);
		}
	}
	
	emit("\nSECTION", " .bss");
	for (it = symbolTable.begin(); it != symbolTable.end(); it++)
	{
		if(it->second.getAlloc() == YES && it->second.getMode() == VARIABLE)
		{
			emit(it->second.getInternalName(), "resd", "1", "; " + it->first);
		}
	}
}

void Compiler::emitReadCode(string operand, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	it = symbolTable.find(operand);
   
	if(symbolTable.count(operand) == 0) {
		processError("reference to undefined operand");
   }
	if(it->second.getDataType() != INTEGER) {
		processError("can't read variables of this type");
   }
	if(it->second.getMode() != VARIABLE) {
		processError("attempting to read to a read-only location");
   }
   
	emit("","call", "ReadInt", "; read int; value placed in eax");
	emit(" ","mov", '[' + it->second.getInternalName() + "],eax", "; store eax at "+ operand);
	contentsOfAReg = operand;
}

void Compiler::emitWriteCode(string operand, string operand2)
{
	string bounce1, bounce2;
   string name;
   static bool definedStorage = false;
   name = operand;
   
   if (name != ""){
      map<string, SymbolTableEntry>::iterator it;
      it = symbolTable.find(name);
      
      if (symbolTable.count(name) == 0) {
         processError("reference to undefined symbol");
      }
      if (name != contentsOfAReg) {
         emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; load " + name +" in eax");
      }
      
      contentsOfAReg = name;
      
      if(it->second.getDataType() == INTEGER || it->second.getDataType() == BOOLEAN) {
         emit("", "call", "WriteInt", "; write int in eax to standard out");
      }
      else{
         emit(" ","cmp", "eax,0", "; compare to 0");
         bounce1 = getLabel();
         emit(" ","je", bounce1, "; jump if equal to print FALSE");
         emit("","mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
         bounce2 = getLabel();
         emit(" ","jmp", bounce2, "; unconditionally jump to "+ bounce2);
         emit(bounce1 + ':', "","", "");
         emit(" ","mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
         emit(bounce2 + ':', "","", "");
         emit(" ","call", "WriteString", "; write string to standard out");
         
         if (definedStorage == false)
         {
            definedStorage = true;
            objectFile << endl;
            emit("SECTION", ".data");
            emit("TRUELIT", "db", "'TRUE',0","; literal string TRUE");
            emit("FALSLIT", "db", "'FALSE',0","; literal string FALSE");
            objectFile << endl;
            emit("SECTION", ".text");
         }
      }
      emit(" ","call", "Crlf", "; write \\r\\n to standard out");
   }
}

void Compiler::emitAssignCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	it=symbolTable.find(operand2);
   
	if (whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if (it->second.getMode() != VARIABLE) {
		processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE");
   }
	if (operand1 == operand2) {
		return;
   }
	if (operand1 != contentsOfAReg)
	{
		it=symbolTable.find(operand1);
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = " + operand1);
	}
   
	it=symbolTable.find(operand2);
	emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; " + operand2+ " = AReg");
   contentsOfAReg = operand2;
   
   if (isTemporary(operand1)) {
		freeTemp();
   }
}     

void Compiler::emitAdditionCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
   
	if (whichType(operand1) !=INTEGER || whichType(operand2) != INTEGER) {
		processError("illegal type");
   }
   
	it = symbolTable.find(operand1);
	if (contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+"],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
   
	if (contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","add", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand1 + " + " + operand2);
	}
	else if (contentsOfAReg == operand2)
	{
		it = symbolTable.find(operand1);
		emit(" ","add", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " + " + operand1);
	}
	else
	{
		if (contentsOfAReg != operand2 && contentsOfAReg != operand1 &&!isTemporary(contentsOfAReg)) {
			contentsOfAReg = "";
      }
      
		it = symbolTable.find(operand2);
		if (contentsOfAReg != operand2 && contentsOfAReg != operand1) {
			emit(" ","mov", "eax,["+it->second.getInternalName()+ "]", "; AReg = " + operand2);
      }
      
		it = symbolTable.find(operand1);
		emit(" ","add", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " + " + operand1);
	}

	if(isTemporary(operand1)) {
		freeTemp();
   }
	if(isTemporary(operand2)) {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}  

void Compiler::emitSubtractionCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	if(whichType(operand1) !=INTEGER || whichType(operand2) != INTEGER) {
		processError("illegal type");
   }
   
	it = symbolTable.find(contentsOfAReg);
	if(contentsOfAReg != operand2  && isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov",'['+it->second.getInternalName()+"],eax","; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
   
	if (contentsOfAReg != operand2 && contentsOfAReg != operand1 &&!isTemporary(contentsOfAReg)) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 ) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2);
   }
   
	it = symbolTable.find(operand1);
	emit(" ","sub", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " - " + operand1);
	if(isTemporary(operand1)) {
		freeTemp();
   }
	if(isTemporary(operand2)) {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}  
 
void Compiler::emitMultiplicationCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it; 
	if(whichType(operand1) != INTEGER|| whichType(operand2) != INTEGER) {
		processError("illegal type");
   }
   
	if(contentsOfAReg != operand2  && contentsOfAReg != operand1 && isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", "eax,["+contentsOfAReg+']', ";mov5");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","imul", "dword ["+it->second.getInternalName()+']', "; AReg = "+ operand1 + " * " + operand2);
	}
	else if(contentsOfAReg == operand2)
	{
		it = symbolTable.find(operand1);
		emit(" ","imul", "dword ["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " * " + operand1);
	}
	else
	{
		if (contentsOfAReg != operand2 && contentsOfAReg != operand1 &&!isTemporary(contentsOfAReg)) {
			contentsOfAReg = "";
      }
      
		it = symbolTable.find(operand2);
		if(contentsOfAReg != operand2 && contentsOfAReg != operand1)
		{
			emit(" ","mov", "eax,["+it->second.getInternalName()+ "]", "; AReg = "+ operand2);
			it = symbolTable.find(operand1);
		}
		emit(" ","imul", "dword ["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " * " + operand1);
	}
	if(isTemporary(operand1)) {
		freeTemp();
   }
	if(isTemporary(operand2)) {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}

void Compiler::emitDivisionCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	if(whichType(operand1) != INTEGER|| whichType(operand2) != INTEGER) {
		processError("illegal type");
   }
   
	if(contentsOfAReg != operand2 && isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov",'['+contentsOfAReg+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg)) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2&&contentsOfAReg != operand1) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2);
   }
   
	it = symbolTable.find(operand1);
	emit(" ","cdq", " ", "; sign extend dividend from eax to edx:eax");
	emit(" ","idiv", "dword ["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " div " +operand1);

	if(isTemporary(operand1)) {
		freeTemp();
   }
	if(isTemporary(operand2)) {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}  

void Compiler::emitModuloCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	if(whichType(operand1) != INTEGER|| whichType(operand2) != INTEGER) {
		processError("illegal type");
   }
	if(contentsOfAReg != operand2 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '[' +it->second.getInternalName()+"],eax","; deassign AReg"); 
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}

	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg)) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = " + operand2);
   }
   
	emit(" ","cdq", " ", "; sign extend dividend from eax to edx:eax");
	it = symbolTable.find(operand1);
	emit(" ","idiv", "dword ["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " div " +operand1);
	emit(" ","xchg", "eax,edx", "; exchange quotient and remainder");

	if(isTemporary(operand1)) {
		freeTemp();
   }
	if(isTemporary(operand2)) {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}  
  
void Compiler::emitNegationCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
   if(whichType(operand1) != INTEGER) {
      processError("illegal type");
   }
   if(contentsOfAReg != operand1 )
   {
      it = symbolTable.find(contentsOfAReg);
      emit(" ","mov","eax,["+operand1+']', "; Areg = " + operand1);
      it->second.setAlloc(YES);
      contentsOfAReg = "";
   }
   if(contentsOfAReg != operand1 && !isTemporary(contentsOfAReg)) {
      contentsOfAReg = "";
   }
   if(contentsOfAReg != operand1) {
      contentsOfAReg = operand1;
   }
   
   emit(" ","neg","eax" ,"; AReg = -AReg");
   pushOperand(contentsOfAReg);
}     

void Compiler::emitNotCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	if(whichType(operand1) != BOOLEAN) {
		processError("illegal type");
   }
   
	if(contentsOfAReg != operand1 && isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov","eax,["+contentsOfAReg+']', "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand1 && !isTemporary(contentsOfAReg)) {
		contentsOfAReg = "";
   }
	if(contentsOfAReg != operand1)
	{
		emit(" ","mov", "eax,["+operand1+']',"; AReg = !AReg");
		contentsOfAReg = operand1;
	}
   
	emit(" ","not", "eax", "; AReg = !AReg");
	pushOperand(contentsOfAReg);
}     

void Compiler::emitAndCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	if(whichType(operand1) != BOOLEAN|| whichType(operand2) != BOOLEAN) {
		processError("illegal type");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", "eax, ["+it->second.getInternalName()+']', "; AReg = "+operand2);
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","and", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand1 + " and " + operand2);
	}
	else if(contentsOfAReg == operand2)
	{
		it = symbolTable.find(operand1);
		emit(" ","and", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2 + " and " + operand1);
	}
	else
	{
		if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
			contentsOfAReg = "";
      }
      
		it = symbolTable.find(operand2);
		if(contentsOfAReg != operand1 && contentsOfAReg != operand2) {
			emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = " + operand2);
      }
      
		it = symbolTable.find(operand1);
		emit(" ","and", "eax,["+it->second.getInternalName()+']', "; AReg = " +operand2 + " and " + operand1);
	}
   
	if(operand1[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}
          
void Compiler::emitOrCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	if(whichType(operand1) != BOOLEAN|| whichType(operand2) != BOOLEAN) {
		processError("illegal type");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+"],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand1 && contentsOfAReg != operand2) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = " + operand2);
   }
   
	it = symbolTable.find(operand1);
	emit(" ","or", "eax,["+it->second.getInternalName()+']', "; AReg = " +operand2 + " or " + operand1);
   
	if(operand1[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

void Compiler::emitEqualityCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	string bounce1, bounce2;
	
	if(whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+operand2);
   }
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand1 + " and " +operand2);
	}
	else
	{
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
   
	bounce1 = getLabel();
	emit(" ","je", bounce1, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	emit(" ","mov", "eax,[FALSE]", "; else set eax to FALSE");
	
   if(symbolTable.count("FALSE") == 0) {
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
   }
   
	bounce2 = getLabel();
	emit(" ","jmp", bounce2, "; unconditionally jump");
	emit(bounce1 + ':', "","", "");
	emit(" ","mov", "eax,[TRUE]", "; set eax to TRUE");
	
   if(symbolTable.count("TRUE") == 0) {
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);
   }
   
	emit(bounce2 + ':', "","", "");
	if(operand1[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

void Compiler::emitInequalityCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	string bounce1, bounce2;
	
	if(whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+operand2);
   }
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand1 + " and " +operand2);
	}
	else
	{
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
   
	bounce1 = getLabel();
	emit(" ","jne", bounce1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	emit(" ","mov", "eax,[FALSE]", "; else set eax to FALSE");
	
   if(symbolTable.count("FALSE") == 0) {
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
   }
   
	bounce2 = getLabel();
	emit(" ","jmp", bounce2, "; unconditionally jump");
	emit(bounce1 + ':', "","", "");
	emit(" ","mov", "eax,[TRUE]", "; set eax to TRUE");
	
   if(symbolTable.count("TRUE") == 0) {
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);
   }
   
	emit(bounce2 + ':', "","", "");
	if(operand1[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	string bounce1, bounce2;
	
	if(whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 )
	{
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = " + operand2);
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
	else if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand1 + " and " +operand2);
	}
	else
	{
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
	
   bounce1 = getLabel();
	emit(" ","jl", bounce1, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
   emit(" ","mov", "eax,[FALSE]", "; else set eax to FALSE");
	
   if(symbolTable.count("FALSE") == 0) {
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
   }
   
	bounce2 = getLabel();
	emit(" ","jmp", bounce2, "; unconditionally jump");
	emit(bounce1 + ':', "","", "");
	emit(" ","mov", "eax,[TRUE]", "; set eax to TRUE");
	
   if(symbolTable.count("TRUE") == 0) {
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);
   }
	
   emit(bounce2 + ':', "","", "");
	if(operand1[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	string bounce1, bounce2;
	
	if(whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+operand2);
   }
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand1 + " and " +operand2);
	}
	else
	{
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
   
	bounce1 = getLabel();
	emit(" ","jle", bounce1, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	emit(" ","mov", "eax,[FALSE]", "; else set eax to FALSE");
	
   if(symbolTable.count("FALSE") == 0) {
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
   }
   
	bounce2 = getLabel();
	emit(" ","jmp", bounce2, "; unconditionally jump");
	emit(bounce1 + ':', "","", "");
	emit(" ","mov", "eax,[TRUE]", "; set eax to TRUE");
	
   if(symbolTable.count("TRUE") == 0) {
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);
   }
   
	emit(bounce2 + ':', "","", "");
	if(operand1[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }      
	if(operand2[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	string bounce1, bounce2;
	
	if(whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+operand2);
   }
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand1 + " and " +operand2);
	}
	else
	{
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
   
	bounce1 = getLabel();
	emit(" ","jg", bounce1, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	emit(" ","mov", "eax,[FALSE]", "; else set eax to FALSE");
	
   if(symbolTable.count("FALSE") == 0) {
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
   }
   
	bounce2 = getLabel();
	emit(" ","jmp", bounce2, "; unconditionally jump");
	emit(bounce1 + ':', "","", "");
	emit(" ","mov", "eax,[TRUE]", "; set eax to TRUE");
	
   if(symbolTable.count("TRUE") == 0) {
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);
   }
   
	emit(bounce2 + ':', "","", "");
	if(operand1[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2)
{
	map<string, SymbolTableEntry>::iterator it;
	string bounce1, bounce2;
	
	if(whichType(operand1) != whichType(operand2)) {
		processError("incompatible types");
   }
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1 &&isTemporary(contentsOfAReg))
	{
		it = symbolTable.find(contentsOfAReg);
		emit(" ","mov", '['+it->second.getInternalName()+ "],eax", "; deassign AReg");
		it->second.setAlloc(YES);
		contentsOfAReg = "";
	}
	if(contentsOfAReg != operand2 && !isTemporary(contentsOfAReg) && contentsOfAReg != operand1) {
		contentsOfAReg = "";
   }
   
	it = symbolTable.find(operand2);
	if(contentsOfAReg != operand2 && contentsOfAReg != operand1) {
		emit(" ","mov", "eax,["+it->second.getInternalName()+']', "; AReg = "+ operand2);
   }
	if(contentsOfAReg == operand1)
	{
		it = symbolTable.find(operand2);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand1 + " and " +operand2);
	}
	else
	{
		it = symbolTable.find(operand1);
		emit(" ","cmp", "eax,["+it->second.getInternalName()+']', "; compare " + operand2 + " and " +operand1);
	}
   
	bounce1 = getLabel();
	emit(" ","jge", bounce1, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	emit(" ","mov", "eax,[FALSE]", "; else set eax to FALSE");
	
   if(symbolTable.count("FALSE") == 0) {
		insert("FALSE", BOOLEAN, CONSTANT, "0", YES, 1);
   }
   
	bounce2 = getLabel();
	emit(" ","jmp", bounce2, "; unconditionally jump");
	emit(bounce1 + ':', "","", "");
	emit(" ","mov", "eax,[TRUE]", "; set eax to TRUE");
	
   if(symbolTable.count("TRUE") == 0) {
		insert("TRUE", BOOLEAN, CONSTANT, "-1", YES, 1);
   }
   
	emit(bounce2 + ':', "","", "");
	if(operand1[0] == 'T' && operand2 != "TRUE") {
		freeTemp();
   }
	if(operand2[0] == 'T' && operand1 != "TRUE") {
		freeTemp();
   }
   
	contentsOfAReg = getTemp();
	it = symbolTable.find(contentsOfAReg);
	it->second.setDataType(BOOLEAN);
	pushOperand(contentsOfAReg);
}

char Compiler::nextChar() 
{
	sourceFile.get(ch);
   
	static char lastChar = '\n';
   
	if(sourceFile.eof())
   {
		return END_OF_FILE;
   }
	else
	{
		if (lastChar == '\n')
		{
			lineNo++;
			listingFile << right << setw(5) << lineNo << '|';
		}
		listingFile << ch;
	}
   
	lastChar = ch;
	return ch;
}

string Compiler::nextToken()
{
	char nxtChar;
	token = "";
	while(token == "")
	{
		if(ch == '{') 
      {
			nxtChar = nextChar();
			while (nxtChar != END_OF_FILE && nxtChar != '}') {
				nxtChar = nextChar();
			}
			
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
           }
			else {
				nextChar();
         }
      }
      else if(ch == '}') 
      {
         processError("'}' cannot begin token");
      }
      else if(isspace(ch))
      {
         nextChar();
      }
      else if(isSpecialSymbol(ch)) 
      {
         token += ch;
         nextChar(); 
         
         if(token == "<")
         {
            if(ch == '=' || ch == '>')
            {
               token += ch;
               nextChar();
            }
         }
         else if(token == ">" || token == ":") 
         {
            if (ch == '=') 
            {
               token += ch;
               nextChar();
            }
         }
      }
      else if(islower(ch))
      {
         token += ch;
         while (islower(nextChar()) || isdigit(ch)|| ch == '_')
         {  
            if(token[token.length()-1] == '_') {
               processError("cant have more than one \'_\'");
            }
            token += ch;
         }
         if(token[token.length()-1] == '_') {
            processError("token cant end on \'_\'");
         }
      }
		else if(isdigit(ch)) 
      {
         token += ch;
			while (isdigit(nextChar())) {
				token += ch;
			}
			if (ch == END_OF_FILE) {
				processError("unexpected end of file");
         }
      }
		else if(sourceFile.eof()) {
         cout << "END OF FILE " << endl;
			token = ch;
      }
		else {
			processError("illegal symbol");
      }
   }
   
	return token;
}

string Compiler::genInternalName(storeTypes stype) const
{
	int count = 0;
	string iname;
   
	for (auto i: symbolTable)
	{
		if (i.second.getDataType() == stype) {
			count++;
      }
	}

	if (stype == INTEGER)
   {
		iname = "I";
   }
	else if (stype == BOOLEAN)
   {
		iname = 'B';
   }
	else if (stype == PROG_NAME)
   {
		iname = 'P';
   }

	return iname + to_string(count);
}

bool Compiler::isKeyword(string s) const
{
	return s == "program" || s == "begin" || s == "end" || s == "var" || s == "const" || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not" || s == "mod" || s == "div" || s == "and" || s == "or" || s == "read" || s == "write";
}

bool Compiler::isSpecialSymbol(char c) const
{
	return c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c == '+' || c == '-' ||  c == '*' || c == '<' || c == '>' || c == '(' || c == ')';
}

bool Compiler::isNonKeyId(string s) const
{
	return s == "_" || s == "" || !(isKeyword(s) || isInteger(s));
}

bool Compiler::isInteger(string s) const
{
	for (uint i = 0; i < s.length(); i++)
	{
		if(i == 0)
		{
			if(s[0] == '+' || s[0] == '-')
				i++;
		}
		if(s[i] != '0' &&  s[i] != '1' &&  s[i] != '2' &&  s[i] != '3' &&  s[i] != '4' &&  s[i] != '5' &&  s[i] != '6'&&  s[i] != '7' &&  s[i] != '8' &&  s[i] != '9') 
			return false;
	}
	return true;
}

bool Compiler::isBoolean(string s) const
{
	if (s == "true" || s == "false" || s == "TRUE" || s == "FALSE" )
   {
		return true;
   }
   
	return false;
}

bool Compiler::isLiteral(string s) const
{
	return isInteger(s) || isBoolean(s) || s == "-" || s == "+";
}

void Compiler::freeTemp()
{
	currentTempNo--;
	if(currentTempNo < -1) {
		processError("compiler error, currentTempNo should be >= -1");
   }
}

string Compiler::getTemp()
{
	string temp;
   currentTempNo++;
   temp = "T" + to_string(currentTempNo);
   if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
		maxTempNo++;
	}
   return temp;
}

string Compiler::getLabel()
{
	static int labelNum = -1;
   labelNum ++;
	return ".L" + to_string(labelNum);
}

bool Compiler::isTemporary(string s) const  
{
	if(s[0] == 'T' && s != "TRUE") {
		return true;
   }
	return false;
}	
