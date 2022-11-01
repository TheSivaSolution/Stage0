
Compiler(char **argv) // constructor
{
   sourceFile.open(argv[1]);
   listingFile.open(argv[2]);
   objectFile.open(argv[3]);
}

~Compiler()           // destructor
{
   sourceFile.close();
   listingFile.close();
   objectFile.close();
}

void createListingHeader()
{
   time_t now = time (NULL);
   listingFile << left << "STAGE0:  Trevor Smith, Seokhee Han" << ctime(&now) << endl;
   listingFile << left << setw(15) "LINE NO:" << "SOURCE STATEMENT" << endl;
   
}

void parser()
{
   
   nextChar();
   
   if (nextToken() != "program")
   {
      processError("keyword program expected");
   }
   
   prog();
}

void createListingTrailer()
{
   listingFile << endl << setw(28) << left << "COMPILATION TERMINATED" << errorCount << " ERROR ENCOUNTERED" << endl;
}


// Methods implementing the grammar productions
void prog()           // stage 0, production 1
{
   if (token != "program")
		processError("keyword \"program\" expected");
	
	progStmt();

	if (token == "const")
		consts();

	if (token == "var")
		vars();

	if (token != "begin")
		processError("keyword \"begin\" expected");

	beginEndStmt();
	
	cout << "check end " << token << endl;
	if (token != "$")
		processError("no text may follow \"end\"");
}

void progStmt()       // stage 0, production 2
{
   string x;
	if (token != "program")
		processError("keyword \"program\" expected");

	x = nextToken();

	if (!isNonKeyId(token))
		processError("program name expected");

	if (nextToken() != ";")
		processError("semicolon expected");

	nextToken();

	code("program", x, "");

	// TODO
	insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

void consts()         // stage 0, production 3
{
   if (token != "const")
 		processError("keyword \"const\" expected");

	if (!isNonKeyId(this->nextToken()))
 		processError("non-keyword identifier must follow \"const\"");

 	constStmts();
}

void vars()           // stage 0, production 4
{
   tatic int c = 0; 
	cout << "vars " << ++c << ":  " << token << endl; 
 	if (token != "var")
 		processError("keyword \"var\" expected");

	if (!isNonKeyId(nextToken()))
 		processError("non-keyword identifier must follow \"var\"");
 	varStmts();
}

void beginEndStmt()   // stage 0, production 5
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

void constStmts()     // stage 0, production 6
{
   static int c = 0; 
	cout << "constStmt " << ++c << ":  " << token << endl; 
	string x,y;
 	if (!isNonKeyId(token))
 		processError("non-keyword identifier expected");
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

	storeTypes _type = whichType(y);
	string val = whichValue(y);
 	insert(x,_type,CONSTANT, val,YES,1);
 	x = nextToken();

	if (x != "begin" && x != "var" && !isNonKeyId(x))
 		processError("non-keyword identifier, \"begin\", or \"var\" expected");

	if (isNonKeyId(x) && x != "var")
 		constStmts();
}

void varStmts()       // stage 0, production 7
{
   string x,y;
	static int c=0;
	cout << "VARSTMTS " << ++c << endl;
 	if (!->isNonKeyId(token))
 		this->processError("non-keyword identifier expected");

 	x = ids();
	cout << ">> VARSTMTS ids " << x << endl;
	if (token != ":")
 		processError("\":\" expected");
	
	cout << ">>VARSTMTS token " << token << endl;

	nextToken();
	
	if (token != "integer"  &&token  != "boolean")
 		processError("illegal type follows \":\"");

	cout << ">>VARSTMTS token " << token << endl;
	y = token; 

	if (nextToken() != ";")
 		processError("semicolon expected");
		
	cout << ">>VARSTMTS token1 " << token << endl;
	
 	insert(	
		x,	
		(y == "integer") ? INTEGER : BOOLEAN,
		VARIABLE,
		"",
		YES, 
		1	
	);

	nextToken();
   
	if (token != "begin" && !isNonKeyId(token))
	{
		cout << ">>VARSTMTS token2 " << token << endl;
 		processError("non-keyword identifier or \"begin\" expected");
	}

 	if (isNonKeyId(token) && token != "begin")
 		varStmts();
}

string ids()          // stage 0, production 8
{
   string temp, tempString;
 	if (!isNonKeyId(token))
 		processError("non-keyword identifier expected");
 	tempString = token;
 	temp = token;
 	if (nextToken() == ",")
 	{
 		if (!isNonKeyId(nextToken()))
 			processError("non-keyword identifier expected");
		 tempString = temp + "," + ids();
 	}
 	return tempString;
}

void insert(string externalName, storeTypes inType, modes inMode,
              string inValue, allocation inAlloc, int inUnits)
{
	
}

storeTypes whichType(string name) // tells which data type a name has
{
	
}

string whichValue(string name) // tells which value a name has
{
	
}

void code(string op, string operand1 = "", string operand2 = "")
{
	
}

void emit(string label = "", string instruction = "", string operands = "",
            string comment = "")
{
	
}

void emitPrologue(string progName, string = "")
{
	
}

void emitEpilogue(string = "", string = "")
{
	
}

void emitStorage()
{
	
}

char nextChar() // returns the next character or END_OF_FILE marker
{
	
}

string nextToken() // returns the next token or END_OF_FILE marker
{
	
}

string genInternalName(storeTypes stype) const
{
	
}

void processError(string err)
{
	
}
