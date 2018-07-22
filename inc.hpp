#ifndef GUARD_Inc
#define GUARD_Inc

// Title: Codifier (Inc class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Increment / Decrement a variable
#include "codifier.hpp"
#include "var.hpp"

class Inc
{
private:
	Codifier *codInstance;
	bool boolTrue = true;
	
public:
	// Constructor
	Inc(Codifier *c) {codInstance = c;}
	
	// Change boolTrue;
	void setBool(bool b) {boolTrue = b;}

	// Run the command
	void run(bool show=false)
	{
		// Choose variable
		if (codInstance->nextIsOverflow()) return;
		Var usedVar(codInstance);
		usedVar.searchLeft();
		if (codInstance->isOverflow()) return;
		int &var = *usedVar.getPtr();
		if (codInstance->getError()) return;

		// Choose operation: ++, --
		string opStr;
		if (codInstance->nextIsOverflow()) return;
		int op = codInstance->getInst()%2;
		if (op == 0)
			opStr = "++";
		else if (op == 1)
			opStr = "--";

		// Print the command
		if (show)
			codInstance->addCode(usedVar.getStr() + opStr + ";");

		// Choose operation and run command
		int oldVar = var;
		bool overflow = false;
		int min = numeric_limits<int>::min();
		int max = numeric_limits<int>::max();
		if (boolTrue)
		{
			if (opStr == "++")
			{
				if (var == max)
					overflow = true;
				var++;
			}
			else if (opStr == "--")
			{
				if (var == min)
					overflow = true;
				var--;
			}
		}

		// Check integer overflow
		if (overflow)
		{
			codInstance->setError();
		}
	}
};
#endif

