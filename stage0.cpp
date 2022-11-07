// Trevor Smith & Seokhee Han
// CS 4301
// Stage 0

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
   sourceFile.open(argv[1]);
   listingFile.open(argv[2]);
   objectFile.open(argv[3]);
}

Compiler::~Compiler() // Destructor
{
   sourceFile.close();
	listingFile.close();
	objectFile.close();
}

void Compiler::createListingHeader()
{
   lineNo++;
   time_t now = time (NULL);
   listingFile << left << "STAGE0:  Trevor Smith, Seokhee Han\t" << ctime(&now) << endl;
   listingFile << left << setw(15) << "LINE NO:" << "SOURCE STATEMENT" << endl << endl;
   listingFile << right << setw(5) << lineNo << '|';
}

void Compiler::parser()
{
   
   nextChar();
   
   if (nextToken() != "program")
   {
      processError("keyword \"program\" expected");
   }
   
   prog();  	                                          //parser implements the grammar rules, calling first rule
}

void Compiler::createListingTrailer()
{
    if(errorCount == 1) {
	listingFile << endl << setw(15) << left << "COMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
     }
    else
   listingFile << endl << setw(15) << left << "COMPILATION TERMINATED      " << errorCount << " ERRORS ENCOUNTERED" << endl;
}

void Compiler::prog()           // stage 0, production 1
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
	
	if (token != "$")
   {
		processError("no text may follow \"end\"");
   }
}

void Compiler::progStmt()       // stage 0, production 2
{
   string x;
	if (token != "program")
   {
		processError("keyword \"program\" expected");
   }

	x = nextToken();

	if (!isNonKeyId(token))
   {
		processError("program name expected");
   }

	if (nextToken() != ";")
   {
		processError("semicolon expected");
   }

	nextToken();

	code("program", x, "");

	insert(x, PROG_NAME,CONSTANT, x, NO, 0);
}

void Compiler::consts()         // stage 0, production 3
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

void Compiler::vars()           // stage 0, production 4
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

void Compiler::beginEndStmt()   // stage 0, production 5
{
   if (token != "begin")
   {
      processError("keyword \"begin\" expected");
   }

 	if (nextToken() != "end")
   {
 		processError("keyword \"end\" expected");
   }

 	if (nextToken() != ".")
   {
 		processError("period expected");
   }
   
   nextToken();
 	code("end", ".", "");
}

void Compiler::constStmts()     // stage 0, production 6
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

	if (y != "+" && y != "-" &&  y != "not" && !isNonKeyId(y) && y != "true" && y != "false" && !isInteger(y))
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

	if (!isInteger(y) && !isBoolean(y))
   {
 		processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");
   }
   
	// TODO whichtype
 	insert(x, whichType(y), CONSTANT, whichValue(y), YES, 1);
 	x = nextToken();

   if (x != "begin" && x != "var" && !isNonKeyId(x))
   {
 		processError("non-keyword identifier, \"begin\", or \"var\" expected");
   }

	if (isNonKeyId(x) && x != "var")
   {
 		constStmts();
   }

}

void Compiler::varStmts()       // stage 0, production 7
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

	nextToken();
	
	if (token != "integer"  && token  != "boolean")
   {
 		processError("illegal type follows \":\"");
   }

	y = token; 

	if (nextToken() != ";")
   {
 		processError("semicolon expected");
   }
	
 	insert(x, (y == "integer") ? INTEGER : BOOLEAN, VARIABLE, "", YES, 1);

	nextToken();
   
	if (token != "begin" && !isNonKeyId(token))
	{
 		processError("non-keyword identifier or \"begin\" expected");
	}

 	if (isNonKeyId(token) && token != "begin")
   {
 		varStmts();
   }
}

string Compiler::ids()          // stage 0, production 8
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
 			processError("non-keyword identifier expected");
      }
		 tempString = temp + "," + ids();
 	}
 	return tempString;
}

  // Helper functions for the Pascallite lexicon
bool Compiler::isKeyword(string s) const  // determines if s is a keyword
{
	return s == "program" || s == "begin" || s == "end" || s == "var" || s == "const" || s == "integer" || s == "boolean" || s == "true" || s == "false" || s == "not";
}

bool Compiler::isSpecialSymbol(char c) const
{
   return c == '=' || c == '+' || c == '-' || c == ';' || c == ':' || c ==  '.' || c == ',';
}

bool Compiler::isNonKeyId(string s) const
{
   return s == "_" || s == "" || isdigit(s.at(0)) || islower(s.at(0));
}

bool Compiler::isInteger(string s) const
{
	if (symbolTable.count(s) > 0)
	{
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
	
bool Compiler::isLiteral(string s) const
{
   return isInteger(s) || isBoolean(s) || s == "-" || s == "+";
}

  // Action routines
void Compiler::insert(string externalName, storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)
{
	string name = externalName.substr(0, externalName.find(','));
	externalName = externalName.substr(externalName.find(',')+1, externalName.length() - externalName.find(',') - 1);

 	while (name != "")
  	{	
    	if (symbolTable.count(name) > 0) {
            processError("multiple name definition");
         }
    	else if (isKeyword(name)) {
	   		processError("illegal use of keyword");	
         }  
    	else
    		{
    			if (isupper(externalName.at(0))) {
               symbolTable.insert(pair<string, SymbolTableEntry>(externalName, SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
            }
   			else {
               symbolTable.insert(pair<string, SymbolTableEntry>(name, SymbolTableEntry(genInternalName(inType),inType, inMode, inValue, inAlloc, inUnits)));
            }
    		}

		if (name == externalName.substr(0, externalName.find(',')))
      {
         break;
      }

		name = externalName.substr(0, externalName.find(','));
		
		externalName = externalName.substr(externalName.find(',')+1, externalName.length() - externalName.find(',') - 1);
  	}

}

storeTypes Compiler::whichType(string name) // tells which data type a name has
{
   storeTypes datatype;
   
	if (isLiteral(name))
   {
 		if (isLiteral(name) && isBoolean(name)) {
 			datatype = BOOLEAN;
      }
 		else {
 			datatype = INTEGER;
      }
   }
 	else 
   {
		if (symbolTable.count(name) > 0) {
 			datatype = symbolTable.at(name).getDataType();
      }
 		else {
 			processError("reference to undefined constant");
      }
   }
 	return datatype;
}

string Compiler::whichValue(string name) // tells which value a name has
{
	string value;
 	if (isLiteral(name))
   	{
         value = name;
      }
 	else 
   {
   	if (symbolTable.find(name) != symbolTable.end())
   	{
         value = symbolTable.at(name).getValue();
   	}    
   	else
  		{
         processError("reference to undefined constant");  
      } 
   }
   	return value;   
}

void Compiler::code(string op, string operand1, string operand2)
{
	if (op == "program")
   {
   	emitPrologue(operand1);
   }
 	else if (op == "end")
   {
    	emitEpilogue();
   }
 	else
   {
    	processError("compiler error since function code should not be called with  illegal arguments");
   }
}

void Compiler::emit(string label, string instruction, string operands,string comment)
{
	objectFile << left << setw(8) << label << setw(8)  << instruction << setw(24) << operands << comment << endl; 
}

void Compiler::emitPrologue(string progName, string operand2)
{
	time_t now = time (NULL);
	
	objectFile << "; Trevor Smith, Seokhee Han\t" << ctime(&now) << endl;
	objectFile << "%INCLUDE \"Along32.inc\"" << endl;
	objectFile << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	emit("SECTION", ".text");
   emit("global", "_start", "", "; program " + progName);
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
   map<string, SymbolTableEntry>::iterator itr;
   emit("SECTION", ".data", "", "");
   emit("", "", "", "");
	for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++) 
   {
      if(itr->second.getAlloc() == YES && itr->second.getMode() == CONSTANT){
         emit(itr->second.getInternalName(), "dd",itr->second.getValue(),"; " +itr->first); 
	   }
	}
   
   emit("SECTION", ".bss", "", "");
   for (itr = symbolTable.begin(); itr != symbolTable.end(); itr++) 
   {
      if(itr->second.getAlloc() == YES && itr->second.getMode() == VARIABLE){
         emit(itr->second.getInternalName(), "resd",itr->second.getValue(),"; " +itr->first);
	   }
	}	
}

char Compiler::nextChar() // returns the next character or END_OF_FILE marker
{
	sourceFile.get(ch);
   
	ch = (sourceFile.eof()) ? END_OF_FILE : ch;
   
  if (ch == '\n')
   {
      if(nextChar() == '$')
      {
         listingFile << endl;
         return END_OF_FILE;
      }
      else
      {
         lineNo++;
         listingFile << endl;
         listingFile << right << setw(5) << lineNo << '|';
         return ch; // made some changes, don't forget 
      }
   }
   else if (ch == END_OF_FILE)
   {
      return END_OF_FILE;
   }
   else
   {
      listingFile << ch;
      return ch;
   }
   
   
}

string Compiler::nextToken() // returns the next token or END_OF_FILE marker
{
	char nxtChar;
	token = "";
	while (token == "")
	{
			if (ch == '{')
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
			else if (ch == '}')
			{
				processError("'}' cannot begin token");
			}
			else if (isspace(ch))
			{
				nextChar();
			}
			else if (isSpecialSymbol(ch))
			{
				token = ch;
				nextChar();
            
			}
			else if (islower(ch))
			{
				token = ch;

				nxtChar = nextChar();
				while ((islower(nxtChar) || isdigit(nxtChar) || nxtChar == '_') && nxtChar != END_OF_FILE)
				{
					token += ch;
					nxtChar = nextChar();
				}
				if (ch == END_OF_FILE) {
					processError("unexpected end of file");
            }
			}
			else if (isdigit(ch))
			{
				token = ch;
				nxtChar = nextChar();
				while (isdigit(nxtChar) && nxtChar != END_OF_FILE)
				{
				   token += ch;
				}
		
				if (ch == END_OF_FILE) {
					processError("unexpected end of file");
            }
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
   
	return token;
}

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
	listingFile << "\nError: Line " << lineNo << ": " << err << endl;
	errorCount++;
   createListingTrailer();
	exit(EXIT_FAILURE);
}
