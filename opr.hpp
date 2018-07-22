#ifndef GUARD_Opr
#define GUARD_Opr

// Title: Codifier (Opr class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Perform an operation with two variables
#include "codifier.hpp"
#include "var.hpp"

class Opr
{
private:
	Codifier *codInstance;
	bool boolTrue = true;
	
public:
	// Constructor
	Opr(Codifier *c) {codInstance = c;}
	
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

		// Choose first right parameter
		if (codInstance->nextIsOverflow()) return;
		Var rightVar1(codInstance);
		rightVar1.searchRight();
		if (codInstance->isOverflow()) return;
		int &right1 = *rightVar1.getPtr();
		if (codInstance->getError()) return;

		// Choose operation: +, -, *, /, %
		string opStr;
		if (codInstance->nextIsOverflow()) return;
		int op = codInstance->getInst()%5;
		if (op == 0)
			opStr = " + ";
		else if (op == 1)
			opStr = " - ";
		else if (op == 2)
			opStr = " * ";
		else if (op == 3)
			opStr = " / ";
		else if (op == 4)
			opStr = " % ";

		// Choose second right parameter
		Var rightVar2(codInstance);
		if (codInstance->nextIsOverflow()) return;
		rightVar2.searchRight();
		if (codInstance->isOverflow()) return;
		int &right2 = *rightVar2.getPtr();
		if (codInstance->getError()) return;

		// Check division by zero
		if ((opStr == " / " || opStr == " % ") && right2 == 0)
		{
			codInstance->setError();
			return;
		}

		// Print the command
		if (show)
		{
			string str;
			str += leftVar.getStr() + " = " + rightVar1.getStr();
			str += opStr + rightVar2.getStr();
			str += ";";
			codInstance->addCode(str);
		}

		// Choose operation and run command
		int oldLeft = left;
		int oldRight1 = right1;
		int oldRight2 = right2;
		bool overflow = false;
		int min = numeric_limits<int>::min();
		if (boolTrue)
		{
			if (opStr == " + ")
			{
				left = right1 + right2;
				if ((oldRight1>0 && oldRight2>0 && left<oldRight1) ||
				    (oldRight1<0 && oldRight2<0 && left>oldRight1))
					overflow = true;
			}
			else if (opStr == " - ")
			{
				left = right1 - right2;
				if ((oldRight1>0 && oldRight2<0 && left<oldRight1) ||
				    (oldRight1<0 && oldRight2>0 && left>oldRight1))
					overflow = true;
			}
			else if (opStr == " * ")
			{
				left = right1 * right2;
				if ((left == min && oldRight2 == -1) ||
				    (oldRight2 != 0 && oldRight1 != left / oldRight2))
					overflow = true;
			}
			else if (opStr == " / ")
			{
				if (oldRight1 == min && oldRight2 == -1)
					overflow = true;
				if (!overflow)
					left = right1 / right2;
			}
			else if (opStr == " % ")
			{
				if (oldRight1 == min && oldRight2 == -1)
					overflow = true;
				if (!overflow)
					left = right1 % right2;
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

