#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <stage0.h>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <ctime>

using namespace std;

Compiler::Compiler(char **argv) // constructor
{
	this->sourceFile.open(argv[1], ios::in);
	this->listingFile.open(argv[2], fstream::app);
	this->objectFile.open(argv[3], fstream::app);
}

  Compiler::~Compiler() // Destructor
  {
	this->sourceFile.close();
	this->listingFile.close();
	this->objectFile.close();
  }

  void Compiler::createListingHeader()
  {
  	time_t now = time (NULL);

	// print "STAGE0:", name(s), DATE, TIME OF DAY
	this->listingFile << setw(35) << left << "STAGE0: Seokhee Han, " << ctime(&now) << endl;

	//  print "LINE NO:", "SOURCE STATEMENT"
	this->listingFile << setw(22) << left << "LINE NO." << "SOURCE STATEMENT" << endl << endl;

  }


  void Compiler::parser()
  {
	// TODO
	nextChar();
   //ch must be initialized to the first character of the source file
         if (nextToken() != "program")
         {
            	 processError("keyword program expected");  
         }
   //         	             //a call to nextToken() has two effects
   //         	                   // (1) the variable, token, is assigned the value of the next token
   //         	                        // (2) the next token is read from the source file in order to make
   //         	                            // the assignment. The value returned by nextToken() is also
   //         	                               // the next token.
   //    
   	prog();
   //         	                                          //parser implements the grammar rules, calling first rule
  }

  void Compiler::createListingTrailer()
  {
	this->listingFile << endl << setw(28) << left << "COMPILATION TERMINATED" << this->errorCount << " ERROR ENCOUNTERED" << endl;
  }


  // Methods implementing the grammar productions
  void Compiler::prog()           // stage 0, production 1
  {
	if (this->token != "program")
		this->processError("keyword \"program\" expected");
	
	this->progStmt();

	if (this->token == "const")
		this->consts();

	if (this->token == "var")
		this->vars();

	if (this->token != "begin")
		this->processError("keyword \"begin\" expected");

	this->beginEndStmt();
	
	cout << "check end " << token << endl;
	if (this->token != "$")
		this->processError("no text may follow \"end\"");
  }

  void Compiler::progStmt()       // stage 0, production 2
  {
	string x;
	if (this->token != "program")
		this->processError("keyword \"program\" expected");

	x = this->nextToken();

	if (!this->isNonKeyId(this->token))
		this->processError("program name expected");

	if (this->nextToken() != ";")
		this->processError("semicolon expected");

	this->nextToken();

	this->code("program", x, "");

	// TODO
	this->insert(x,PROG_NAME,CONSTANT,x,NO,0);
  }

  void Compiler::consts()         // stage 0, production 3
  {
	if (this->token != "const")
 		this->processError("keyword \"const\" expected");

	if (!this->isNonKeyId(this->nextToken()))
 		this->processError("non-keyword identifier must follow \"const\"");

 	this->constStmts();
  }

  void Compiler::vars()           // stage 0, production 4
  {
	static int c = 0; 
	cout << "vars " << ++c << ":  " << this->token << endl; 
 	if (this->token != "var")
 		this->processError("keyword \"var\" expected");

	if (!this->isNonKeyId(this->nextToken()))
 		this->processError("non-keyword identifier must follow \"var\"");
 	this->varStmts();
  }

  void Compiler::beginEndStmt()   // stage 0, production 5
  {
	 if (token != "begin")
 		processError("keyword \"begin\" expected");

 	if (nextToken() != "end")
 		processError("keyword \"end\" expected");

 	if (nextToken() != ".")
 		processError("period expected");
	
	
	nextToken();
 	code("end", ".", "");
  }

  void Compiler::constStmts()     // stage 0, production 6
  {
	static int c = 0; 
	cout << "constStmt " << ++c << ":  " << this->token << endl; 
	string x,y;
 	if (!this->isNonKeyId(this->token))
 		this->processError("non-keyword identifier expected");
 	x = token;
 	if (nextToken() != "=")
 		processError("\"=\" expected");
	y = nextToken();

	if (y != "+" && y != "-" &&  y != "not" && !isNonKeyId(y) && y != "true" && y != "false" && !isInteger(y))
 		processError("token to right of \"=\" illegal");

	if (y == "+" || y == "-")
 	{
 		if (!isInteger(nextToken()))
 			processError("integer expected after sign");
 		y = y + token;
 	}
 	if (y == "not")
 	{
		if (!isBoolean(nextToken()))
 			processError("boolean expected after \"not\"");
 		if (token == "true")
 			y = "false";
 		else
 			y = "true";
 	}
 	if (nextToken() != ";")
 		processError("semicolon expected");

	if (!isInteger(y) && !isBoolean(y))
 		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");

	// TODO whichtype
	storeTypes _type = whichType(y);
	string val = whichValue(y);
 	insert(x,_type,CONSTANT, val,YES,1);
 	x = nextToken();

	if (x != "begin" && x != "var" && !isNonKeyId(x))
 		processError("non-keyword identifier, \"begin\", or \"var\" expected");

	//if (isNonKeyId(x))
	if (isNonKeyId(x) && x != "var")
 		constStmts();

  }

  void Compiler::varStmts()       // stage 0, production 7
  {
	string x,y;
	static int c=0;
	cout << "VARSTMTS " << ++c << endl;
 	if (!this->isNonKeyId(this->token))
 		this->processError("non-keyword identifier expected");

 	x = this->ids();
	cout << ">> VARSTMTS ids " << x << endl;
	if (this->token != ":")
 		this->processError("\":\" expected");
	
	cout << ">>VARSTMTS token " << token << endl;

	this->nextToken();
	//if (this->nextToken() != "integer"  && this->nextToken()  != "boolean")
	if (this->token != "integer" && this->token  != "boolean")
 		this->processError("illegal type follows \":\"");

	cout << ">>VARSTMTS token " << token << endl;
	y = this->token; 

	if (this->nextToken() != ";")
 		this->processError("semicolon expected");
		
	cout << ">>VARSTMTS token1 " << token << endl;
	// TODO
	//storeTypes _type = whichType(y);
	
 	this->insert(	
		x,	
		(y == "integer") ? INTEGER : BOOLEAN,
		VARIABLE,
		"",
		YES, 
		1	
	);

	this->nextToken();
	//if (this->nextToken() != "begin" && !this->isNonKeyId(this->nextToken()))
	if (this->token != "begin" && !this->isNonKeyId(this->token))
	{
		cout << ">>VARSTMTS token2 " << token << endl;
 		this->processError("non-keyword identifier or \"begin\" expected");
	}

 	//if (this->isNonKeyId(this->token))
 	if (this->isNonKeyId(this->token) && this->token != "begin")
 		this->varStmts();

  }

  string Compiler::ids()          // stage 0, production 8
  {
	// Get list of non key id strings with , as a delimiter
	string temp, tempString;
 	if (!this->isNonKeyId(this->token))
 		processError("non-keyword identifier expected");
 	tempString = token;
 	temp = token;
 	if (nextToken() == ",")
 	{
 		if (!this->isNonKeyId(this->nextToken()))
 			processError("non-keyword identifier expected");
		 tempString = temp + "," + ids();
 	}
 	return tempString;
  }

  // Helper functions for the Pascallite lexicon
  bool Compiler::isKeyword(string s) const  // determines if s is a keyword
  {
	return s == "program" || s == "begin" || s == "end" || s == "var" || s == "const" || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not";
  }

  bool Compiler::isSpecialSymbol(char c) const // determines if c is a special symbol
  {
	return c == '=' || c == ':' || c == ',' || c == ';' || c == '.' || c ==  '+' || c == '-';
  }

  bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id
  {
	return s == "_" || s == "" || islower(s.at(0)) || isdigit(s.at(0));
  }

  bool Compiler::isInteger(string s) const  // determines if s is an integer
  {
	// TODO
	if (symbolTable.count(s) > 0)
	{
		cout << "isInt " << s << endl;
		return symbolTable.at(s).getDataType() == INTEGER;
	}
	else
		try
		{
			stoi(s);
		}
		catch (...)
		{
			return false;
		}
	return true;
  }

  bool Compiler::isBoolean(string s) const  // determines if s is a boolean
  {
	if (symbolTable.count(s) > 0)
	{
		return symbolTable.at(s).getDataType() == BOOLEAN;
	}
	return s == "true" || s == "false";
  }
	

  bool Compiler::isLiteral(string s) const  // determines if s is a literal
  {
	// TODO
	return this->isInteger(s) || this->isBoolean(s) || s == "+" || s == "-";
  }

  // Action routines
  void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
  {
	// a,b
	// d
	string name = externalName.substr(0, externalName.find(','));
	externalName = externalName.substr(externalName.find(',')+1, externalName.length() - externalName.find(',') - 1);

	int count = 0;
 	while (name != "")
  	{	
	
		cout << "insert name " << name << " - " << count << endl;
    		if (symbolTable.count(name) > 0)
		{
			cout << "Mult name def error: " << name << endl;
			processError("\nmultiple name definition\n");
		}
    		else if (isKeyword(name))
    		{
	   		processError("illegal use of keyword");	
		}   
    		else //create table entry
    		{
    			if (isupper(externalName.at(0)))
				symbolTable.insert(pair<string, SymbolTableEntry>(externalName, SymbolTableEntry(name,inType, inMode, inValue, inAlloc, inUnits)));
   			else
				symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType),inType, inMode, inValue, inAlloc, inUnits)));
    		}

		if (name == externalName.substr(0, externalName.find(',')))
            		break;

		name = externalName.substr(0, externalName.find(','));
		externalName = externalName.substr(externalName.find(',')+1, externalName.length() - externalName.find(',') - 1);
  	}

  }

  storeTypes Compiler::whichType(string name) // tells which data type a name has
  {	
	// TODO
	storeTypes datatype;
	if (isLiteral(name))
 		if (isLiteral(name) && isBoolean(name))
 			datatype = BOOLEAN;
 		else
 			datatype = INTEGER;
 	else //name is an identifier and hopefully a constant
		//if (symbolTable.find(name) != symbolTable.end())
		if (symbolTable.count(name) > 0)
 			datatype = symbolTable.at(name).getDataType();
 		else
 			processError("reference to undefined constant");
 	return datatype;
  }

  string Compiler::whichValue(string name) // tells which value a name has
  {
	string value;
 	if (isLiteral(name))
   	{
		if (name == "false")
      			value = "0";
	// TODO
    		else if (name == "true")
      			value = "-1"; 
    		else value = name;
   	}
 	else //name is an identifier and hopefully a constant
   		if (symbolTable.find(name) != symbolTable.end())
   		{
	 		value = symbolTable.at(name).getValue();
   		}    
   		else
   		{
	 		processError("reference to undefined constant");  
   		} 
   	return value;   
  }

  void Compiler::code(string op, string operand1 , string operand2 )
  {
	if (op == "program")
   		emitPrologue(operand1);
 	else if (op == "end")
    		emitEpilogue();
 	else
    		processError("compiler error since function code should not be called with  illegal arguments");
  }


  // Emit Functions
  void Compiler::emit(string label , string instruction ,string operands , string comment )
  {
	 objectFile << left << setw(8) << label;
	objectFile << left <<  setw(8) << instruction;
	objectFile << left << setw(24) << operands;
	objectFile << left << setw(8) << comment;   
  }

  void Compiler::emitPrologue(string progName, string operand2)
  {
	time_t now = time (NULL);
	
	objectFile <<"; Seokhee Han and        " << ctime(&now) << endl;
	objectFile << "%INCLUDE\"Along32.inc\" " << endl 
	objectFile << "%INCLUDE\"Macros_Along.inc\"" << endl;
	emit("SECTION", ".text");
    emit("global", "_start", "", "; program" + progName);
    emit("_start:");
  }

  void Compiler::emitEpilogue(string a, string b)
  {
	 emit("","Exit", "{0}");
    emitStorage();
  }

  void Compiler::emitStorage()
  {
	 map<string, SymbolTableEntry>::iterator itr;
    emit("SECTION", ".data", "", "");
	for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++) {
       if(itr->second.getAlloc() == YES && itr->second.getMode() == CONSTANT){
		emit(itr->second.getInternalName(), "dd",itr->second.getValue(),"; " +itr->first); 
	   }
	}			
    emit("SECTION", ".bss", "", "");
    for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++) {
	 if(itr->second.getAlloc() == YES && itr->second.getMode() == VARIABLE){
		emit(itr->second.getInternalName(), "resd",itr->second.getValue(),"; " +itr->first);
	   }
	}	
  }


  // Lexical routines
  char Compiler::nextChar() // returns the next character or END_OF_FILE marker
  {
	this->sourceFile.get(this->ch);
	
	this->ch = (this->sourceFile.eof()) ? END_OF_FILE : this->ch;

	// print to listing file
	this->listingFile << this->ch;

	return this->ch;
  }

  string Compiler::nextToken() // returns the next token or END_OF_FILE marker
  {
	char nxtch;
	this->token = "";
	while (this->token == "")
	{
			if (this->ch == '{')
			{
				nxtch = this->nextChar();
				// process comment
				while (nxtch != END_OF_FILE && nxtch != '}') {
					nxtch = this->nextChar();
				}
				
				if (this->ch == END_OF_FILE)
					this->processError("unexpected end of file");
				else
					this->nextChar();
			}
			else if (this->ch == '}')
			{
				this->processError("'}' cannot begin token");
			}
			else if (isspace(this->ch))
			{
				this->nextChar();
			}
			else if (this->isSpecialSymbol(this->ch ))
			{
				this->token = this->ch;
				this->nextChar();
			}
			else if (islower(this->ch ))
			{
				this->token = this->ch;

				nxtch = this->nextChar();
				while ((islower(nxtch) || isdigit(nxtch) || nxtch == '_') && nxtch != END_OF_FILE )
				{
					this->token += this->ch;
					nxtch = this->nextChar();
				}
				if (this->ch == END_OF_FILE)
					this->processError("unexpected end of file");
			}
			else if (isdigit(this->ch ))
			{
				this->token = this->ch;
				nxtch = this->nextChar();
				while (isdigit(nxtch) && nxtch != END_OF_FILE)
				{
					this->token += this->ch;
				}
		
				if (this->ch == END_OF_FILE)
					this->processError("unexpected end of file");
			}
			else if (this->ch == END_OF_FILE)
			{
				cout << "END OF FILE " << endl;
				this->token = this->ch;
			}
			else
			{
				this->processError("illegal symbol");
			}
	}
	
	cout << token << endl;
	return this->token;
  }


  // Other routines
  string Compiler::genInternalName(storeTypes stype) const
  {
	int count = 0;
	string iname;
	for (auto i: symbolTable)
	{
		if (i.second.getDataType() == stype)
			count++;
	}

	if (stype == INTEGER)
		iname = "I";
	else if (stype == BOOLEAN)
		iname = 'B';
	else if (stype == PROG_NAME)
		iname = 'P';

	return iname + to_string(count);
  }

  void Compiler::processError(string err)
  {
	this->listingFile << err << endl;
	errorCount++;
	exit(0);
  }
