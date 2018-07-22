#ifndef GUARD_Var
#define GUARD_Var

// Title: Codifier (Var class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Choose a variable
#include "codifier.hpp"

class Var
{
private:
	Codifier *codInstance;
	string str;
	int *ptr, *ptr2=NULL;
	int val, size=-1;
	
public:
	// Constructor
	Var() {}
	Var(Codifier *c) {codInstance = c;}

	// Small methods
	string getStr() {return str;}
	void setInst (Codifier *c) {codInstance = c;}

	// Return refering pointer
	int *getPtr()
	{
		if (size != -1)
		{
			int &idx = *ptr2;
			if (idx < 0 || idx >= size)
			{
				codInstance->setError();
				return NULL;
			}
			return ptr+idx;
		}
		return ptr;
	}
	
	// Search for reference to a variable
	// You should refer the value using:
	//    Var x(codInstance);
	//    x.searchLeft();
	//    int &left = *x.getPtr();
	// This will return a reference to a variable, for example: str[2]
	// After this, you can change the value refered:
	//    left = 1;
	// This will be the same as:
	//    str[2] = 1;
	void searchLeft()
	{
		int v = codInstance->getInst()%5;
		getVar(v);
	}
	void searchRight()
	{
		int v = codInstance->getInst()%8;
		getVar(v);
	}

	// Select a variable
	void getVar(int &v)
	{
		if (v == 0)
		{
			// Return the element on position "i"
			ptr2 = codInstance->getRefI();
			size = codInstance->getSrcSize();
			int &i = *ptr2;
			if (i < 0 || i >= size)
			{
				codInstance->setError();
				return;
			}
			ptr = codInstance->getRefSrcElem(0);
			str = "src[i]";
		}
		else if (v == 1)
		{
			// The temporary variable var[n] defined by next byte
			if (codInstance->nextIsOverflow()) return;
			int idx = codInstance->getInst()%codInstance->getVarSize();
			ptr = codInstance->getRefVar(idx);
			str = "var["+to_string(idx)+"]";
		}
		else if (v == 2)
		{
			// An element of the src vector defined by next byte
			if (codInstance->nextIsOverflow()) return;
			int idx = codInstance->getInst()%codInstance->getSrcSize();
			ptr = codInstance->getRefSrcElem(idx);
			str = "src["+to_string(idx)+"]";
		}
		else if (v == 3)
		{
			// An element of the src vector defined by var[n]
			if (codInstance->nextIsOverflow()) return;
			int idx = codInstance->getInst()%codInstance->getVarSize();
			ptr2 = codInstance->getRefVar(idx);
			int &var = *ptr2;
			if (var < 0 || var >= codInstance->getSrcSize())
			{
				codInstance->setError();
				return;
			}
			size = codInstance->getSrcSize();
			ptr = codInstance->getRefSrcElem(0);
			str = "src[var["+to_string(idx)+"]]";
		}
		else if (v == 4)
		{
			// An element of the var vector defined by src[n]
			if (codInstance->nextIsOverflow()) return;
			int idx = codInstance->getInst()%codInstance->getSrcSize();
			ptr2 = codInstance->getRefSrcElem(idx);
			int &src = *ptr2;
			if (src < 0 || src >= codInstance->getVarSize())
			{
				codInstance->setError();
				return;
			}
			size = codInstance->getVarSize();
			ptr = codInstance->getRefVar(0);
			str = "var[src["+to_string(idx)+"]]";
		}
		
		// Instructions that cannot be on the left side of an atribution
		else if (v == 5)
		{
			// Return the value of "i"
			// In order to avoid problems in the iteraction process,
			// the variable "i" should not be changed by atribution.
			ptr = codInstance->getRefI();
			str = "i";
		}
		else if (v == 6)
		{
			// The number on position "index"
			if (codInstance->nextIsOverflow()) return;
			int n = codInstance->getInst();
			val = n;
			ptr = &val;
			str = to_string(n);
		}
		else if (v == 7)
		{
			// The size of "src" vector
			val = codInstance->getSrcSize();
			ptr = &val;
			str = to_string(val);
		}
	}
};
#endif

