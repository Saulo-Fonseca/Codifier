#ifndef GUARD_Set
#define GUARD_Set

// Title: Codifier (Set class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Set a value to a variable
#include "codifier.hpp"
#include "var.hpp"

class Set
{
private:
	Codifier *codInstance;
	bool boolTrue = true;
	
public:
	// Constructor
	Set(Codifier *c) {codInstance = c;}
	
	// Change boolTrue;
	void setBool(bool b) {boolTrue = b;}
	
	// Run the command
	void run(bool show=false)
	{
		// Choose left parameter
		if (codInstance->nextIsOverflow()) return;
		Var leftVar(codInstance);
		leftVar.searchLeft();
		if (codInstance->isOverflow()) return;
		int &left = *leftVar.getPtr();
		if (codInstance->getError()) return;

		// Choose operation: =, +, -
		string opStr;
		if (codInstance->nextIsOverflow()) return;
		int op = codInstance->getInst()%6;
		if (op == 0)
			opStr = " = ";
		else if (op == 1)
			opStr = " += ";
		else if (op == 2)
			opStr = " -= ";
		else if (op == 3)
			opStr = " *= ";
		else if (op == 4)
			opStr = " /= ";
		else if (op == 5)
			opStr = " %= ";

		// Choose right parameter
		if (codInstance->nextIsOverflow()) return;
		Var rightVar(codInstance);
		rightVar.searchRight();
		if (codInstance->isOverflow()) return;
		int &right = *rightVar.getPtr();
		if (codInstance->getError()) return;

		// Check division by zero
		if ((opStr == " /= " || opStr == " %= ") && right == 0)
		{
			codInstance->setError();
			return;
		}

		// Print the command
		if (show)
		{
			string str;
			str += leftVar.getStr() + opStr + rightVar.getStr();
			str += ";";
			codInstance->addCode(str);
		}

		// Choose operation and run command
		int oldLeft = left;
		int oldRight = right;
		bool overflow = false;
		int min = numeric_limits<int>::min();
		if (boolTrue)
		{
			if (opStr == " = ")
			{
				left = right;
			}
			else if (opStr == " += ")
			{
				left += right;
				if ((oldLeft>0 && oldRight>0 && left<oldLeft) ||
				    (oldLeft<0 && oldRight<0 && left>oldLeft))
					overflow = true;
			}
			else if (opStr == " -= ")
			{
				left -= right;
				if ((oldLeft>0 && oldRight<0 && left<oldLeft) ||
				    (oldLeft<0 && oldRight>0 && left>oldLeft))
					overflow = true;
			}
			else if (opStr == " *= ")
			{
				left *= right;
				if ((left == min && oldRight == -1) ||
				    (oldRight != 0 && oldLeft != left / oldRight))
					overflow = true;
			}
			else if (opStr == " /= ")
			{
				if (oldLeft == min && oldRight == -1)
					overflow = true;
				if (!overflow)
					left /= right;
			}
			else if (opStr == " %= ")
			{
				if (oldLeft == min && oldRight == -1)
					overflow = true;
				if (!overflow)
					left %= right;
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

