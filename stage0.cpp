
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
   listingFile << left << "STAGE0:\t" << "Trevor Smith, Seokhee Han" << ctime(&now) << endl;
   listingFile << left << setw(15) "LINE NO:" << "SOURCE STATEMENT" << endl;
   
}

void parser()
{
   
}

void createListingTrailer()
{
   
}


// Methods implementing the grammar productions
void prog()           // stage 0, production 1
{
   
}

void progStmt()       // stage 0, production 2
{
   
}

void consts()         // stage 0, production 3
{
   
}

void vars()           // stage 0, production 4
{
   
}

void beginEndStmt()   // stage 0, production 5
{
   
}

void constStmts()     // stage 0, production 6
{
   
}

void varStmts()       // stage 0, production 7
{
   
}

string ids()          // stage 0, production 8
{
   
}

