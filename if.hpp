#ifndef GUARD_If
#define GUARD_If

// Title: Codifier (If class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: If condition
#include "codifier.hpp"

class If
{
private:
	Codifier *codInstance;
	bool boolTrue, insideFor;
	Var leftVar, rightVar;
	int op, counter;
	string opStr;
	
public:
	// Constructor
	If(Codifier *c) {codInstance = c;}
	
	// Define main variables
	int def(bool it=true, bool in=false)
	{
		// Choose left parameter for bool comparison
		if (codInstance->nextIsOverflow()) return 0;
		leftVar.setInst(codInstance);
		leftVar.searchRight();
		if (codInstance->isOverflow()) return 0;
		int &left = *leftVar.getPtr();
		if (codInstance->getError()) return 0;

		// Choose boolean comparison: ==, != , <, <=, >, >=
		if (codInstance->nextIsOverflow()) return 0;
		op = codInstance->getInst()%6;
		if (op == 0)
			opStr = " == ";
		else if (op == 1)
			opStr = " != ";
		else if (op == 2)
			opStr = " < ";
		else if (op == 3)
			opStr = " <= ";
		else if (op == 4)
			opStr = " > ";
		else if (op == 5)
			opStr = " >= ";

		// Choose right parameter for bool comparison
		if (codInstance->nextIsOverflow()) return 0;
		rightVar.setInst(codInstance);
		rightVar.searchRight();
		if (codInstance->isOverflow()) return 0;
		int &right = *rightVar.getPtr();
		if (codInstance->getError()) return 0;
		
		// Verifies if inside a "for"
		insideFor = in;

		// Verifies if "if" is true
		boolTrue = it;
		if (boolTrue) // If parent "if" is true
		{
			if (!((opStr == " == " && left == right) ||
				  (opStr == " != " && left != right) ||
				  (opStr == " < "  && left  < right) ||
				  (opStr == " <= " && left <= right) ||
				  (opStr == " > "  && left >  right) ||
				  (opStr == " >= " && left >= right)))
				boolTrue = false;
		}

		// Define how many commands will be inside "if"
		if (codInstance->nextIsOverflow()) return 0;
		counter = codInstance->getInst()%5+1;
		return counter;
	}
	
	// Run methods
	void run(bool show=false);
};

// Dependencies
#include "set.hpp"
#include "opr.hpp"
#include "inc.hpp"
#include "for.hpp"
#include "while.hpp"

// Prepare to run
void If::run(bool show)
{	
	// Print opening the command
	if (show)
	{
		string ifStr = "if ("+leftVar.getStr()+opStr+rightVar.getStr()+")";
		codInstance->addCode(ifStr);
		codInstance->addCode("{");
	}

	// Run the commands inside "if"
	// "if" has its own loop
	int cloneCounter = counter;
	while (cloneCounter > 0)
	{
		// Read next instruction
		if (codInstance->nextInstruction()) break;
		cloneCounter--; // Count as one used instruction
		int instruction = codInstance->getInst()%7;

		// Run the instruction
		if (instruction == -1)
		{
			// Instructions -1 means the instruction was killed.
			codInstance->addHead(codInstance->getIndex(),1);
			if (show)
				codInstance->addCode("; // Killed");

			// Include instructions of killed block in parent block
			if (codInstance->getCounter() > 1)
				cloneCounter += codInstance->getCounter();
		}
		else if (instruction == 0)
		{
			// NOP - Ignore entry.
			// This is important to permit different quantity of
			// commands in the instructions set.
			codInstance->addHead(codInstance->getIndex(),1);
			if (show)
				codInstance->addCode(";");
		}
		else if (instruction == 1)
		{
			// Attribution
			codInstance->addHead(codInstance->getIndex(),1);
			Set inst(codInstance);
			inst.setBool(boolTrue);
			inst.run(show);
		}
		else if (instruction == 2)
		{
			// Operation
			codInstance->addHead(codInstance->getIndex(),1);
			Opr inst(codInstance);
			inst.setBool(boolTrue);
			inst.run(show);
		}
		else if (instruction == 3)
		{
			// Increment
			codInstance->addHead(codInstance->getIndex(),1);
			Inc inst(codInstance);
			inst.setBool(boolTrue);
			inst.run(show);
		}
		else if (instruction == 4)
		{
			// For loop
			if (insideFor)
			{
				// Here should be the code for another "for".
				// Nested "for" commands are not allowed at this moment.
				// Here there is the need for this placeholder
				// in order to garantee the same kind of instructions
				// here and outside the "for" loop (if it is killed).
				// Because "for" uses three bytes, three instruction
				// will be read to maintain the same lenght of an "for"
				// in the outside loop.
				codInstance->addHead(codInstance->getIndex(),1);
				if (codInstance->nextIsOverflow()) break;
				if (codInstance->nextIsOverflow()) break;
				if (codInstance->nextIsOverflow()) break;
				if (show)
					codInstance->addCode(";");
			}
			else
			{
				int oldI = codInstance->getValI(); // Saves the value of "i"
				For inst(codInstance);
				int idx = codInstance->getIndex();
				int cnt = inst.def(boolTrue);
				if (codInstance->isOverflow()) break;
				codInstance->addHead(idx,cnt);
				inst.run(show);
				if (!boolTrue)
					*codInstance->getRefI() = oldI; // Recovers "i"
			}
		}
		else if (instruction == 5)
		{
			// If statement
			If inst(codInstance);
			int idx = codInstance->getIndex();
			int cnt = inst.def(boolTrue,insideFor);
			if (codInstance->isOverflow()) break;
			codInstance->addHead(idx,cnt);
			inst.run(show);
		}
		else if (instruction == 6)
		{
			// While statement
			While inst(codInstance);
			int idx = codInstance->getIndex();
			int cnt = inst.def(boolTrue,insideFor);
			if (codInstance->isOverflow()) break;
			codInstance->addHead(idx,cnt);
			inst.run(show);
		}

		// End if instruction was incomplete
		if (codInstance->isOverflow()) break;
	}

	// Print closing "if"
	if (show)
		codInstance->addCode("}");
}
#endif

