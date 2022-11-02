
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
	
	if (token != "integer"  && token  != "boolean")
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
    		else
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

storeTypes whichType(string name) // tells which data type a name has
{
	storeTypes datatype;
	if (isLiteral(name))
 		if (isLiteral(name) && isBoolean(name))
 			datatype = BOOLEAN;
 		else
 			datatype = INTEGER;
 	else 
		if (symbolTable.count(name) > 0)
 			datatype = symbolTable.at(name).getDataType();
 		else
 			processError("reference to undefined constant");
 	return datatype;
}

string whichValue(string name) // tells which value a name has
{
	string value;
 	if (isLiteral(name))
   	{
		if (name == "false")
      			value = "0";
    		else if (name == "true")
      			value = "-1"; 
    		else value = name;
   	}
 	else 
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

void code(string op, string operand1 = "", string operand2 = "")
{
	if (op == "program")
   		emitPrologue(operand1);
 	else if (op == "end")
    		emitEpilogue();
 	else
    		processError("compiler error since function code should not be called with  illegal arguments");
}

void emit(string label = "", string instruction = "", string operands = "",
            string comment = "")
{
	objectFile << left << setw(8) << label;
	objectFile << left <<  setw(8) << instruction;
	objectFile << left << setw(24) << operands;
	objectFile << left << setw(8) << comment; 
}

void emitPrologue(string progName, string = "")
{
	time_t now = time (NULL);
	
	objectFile <<"; Trevor Smith, Seokhee Han" << ctime(&now) << endl;
	objectFile << "&INCLUDE\"Along32.inc\" " << endl 
	<< "&INCLUDE\"Marcos_Along.inc\"" << endl;
	emit("SECTION", ".text");
    emit("global", "_start", "", "; program" + progName);
    emit("_start:");
}

void emitEpilogue(string = "", string = "")
{
    emit("","Exit", "{0}");
    emitStorage();
}

void emitStorage()
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

char nextChar() // returns the next character or END_OF_FILE marker
{
	sourceFile.get(ch);
	
	ch = (sourceFile.eof()) ? END_OF_FILE : ch;

	listingFile << ch;

	return ch;
}

string nextToken() // returns the next token or END_OF_FILE marker
{
	char nxtch;
	token = "";
	while (token == "")
	{
			if (ch == '{')
			{
				nxtch = nextChar();
				// process comment
				while (nxtch != END_OF_FILE && nxtch != '}') {
					nxtch = nextChar();
				}
				
				if (ch == END_OF_FILE)
					processError("unexpected end of file");
				else
					nextChar();
			}
			else if (ch == '}')
			{
				processError("'}' cannot begin token");
			}
			else if (isspace(ch))
			{
				nextChar();
			}
			else if (isSpecialSymbol(ch ))
			{
				token = ch;
				nextChar();
			}
			else if (islower(ch))
			{
				token = ch;

				nxtch = nextChar();
				while ((islower(nxtch) || isdigit(nxtch) || nxtch == '_') && nxtch != END_OF_FILE )
				{
					token += ch;
					nxtch = nextChar();
				}
				if (ch == END_OF_FILE)
					processError("unexpected end of file");
			}
			else if (isdigit(ch))
			{
				token = ch;
				nxtch = nextChar();
				while (isdigit(nxtch) && nxtch != END_OF_FILE)
				{
				     token += ch;
				}
		
				if (ch == END_OF_FILE)
					processError("unexpected end of file");
			}
			else if (ch == END_OF_FILE)
			{
				cout << "END OF FILE " << endl;
				token = ch;
			}
			else
			{
				processError("illegal symbol");
			}
	}
	
	cout << token << endl;
	return token;
}

string genInternalName(storeTypes stype) const
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

void processError(string err)
{
	listingFile << err << endl;
	errorCount++;
	exit(0);
}
