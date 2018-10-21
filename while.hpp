#ifndef GUARD_While
#define GUARD_While

// Title: Codifier (While class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: While loop
#include "codifier.hpp"

class While
{
private:
	Codifier *codInstance;
	bool boolTrue, insideFor;
	Var leftVar, rightVar;
	int counter;
	string opStr;
	
public:
	// Constructor
	While(Codifier *c) {codInstance = c;}

	// Analysis of bool operation
	// Type is bool only to check if getPtr found an error
	bool checkBool(bool bt)
	{
		boolTrue = bt;
		int &left  = *leftVar.getPtr();
		if (codInstance->getError()) return true;
		int &right = *rightVar.getPtr();
		if (codInstance->getError()) return true;
		if (boolTrue) // If parent "if" is true
			// If "while" is not true
			if   (!((opStr == " == " && left == right) ||
				(opStr == " != " && left != right) ||
				(opStr == " < "  && left  < right) ||
				(opStr == " <= " && left <= right) ||
				(opStr == " > "  && left >  right) ||
				(opStr == " >= " && left >= right)))
				boolTrue = false;
		return false;
	}
	
	// Define main variables
	int def(bool bt=true, bool in=false)
	{
		// Choose left parameter for bool comparison
		if (codInstance->nextIsOverflow()) return 0;
		leftVar.setInst(codInstance);
		leftVar.searchRight();
		if (codInstance->isOverflow()) return 0;

		// Choose boolean comparison: ==, != , <, <=, >, >=
		if (codInstance->nextIsOverflow()) return 0;
		int op = codInstance->getInst()%6;
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
		
		// Update boolTrue variable
		if (checkBool(bt)) return 0;

		// Verifies if inside a "for"
		insideFor = in;

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
#include "if.hpp"
#include "for.hpp"

// Prepare to run
void While::run(bool show)
{	
	// Print opening the command
	if (show)
	{
		string wStr = "while ("+leftVar.getStr()+opStr+rightVar.getStr()+")";
		codInstance->addCode(wStr);
		codInstance->addCode("{");
	}

	// Save old values
	int oldIndex = codInstance->getIndex();
	bool oldShow = show;

	// Main "while" loop
	int loopCount = 0;

	// "While" runs the block at least 1 time, so that the
	// instructions can be shown if boolTrue false
	while (boolTrue || loopCount == 0)
	{
		// Retrieve old starting point
		codInstance->setIndex(oldIndex);

		// Run the commands inside "while"
		// "while" has its own loop
		int cloneCounter = counter;
		while (cloneCounter > 0)
		{
			// Set context variables
			if (loopCount != 0)
				show = false;

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

		// Check if loop is infinit
		loopCount++;
		if (loopCount > codInstance->getSize()*codInstance->getSrcSize())
		{
			codInstance->setError();
			break;
		} 

		// Update boolTrue variable
		if (checkBool(boolTrue)) break;
	}
		
	// Recover original show value
	show = oldShow;
		
	// Print closing "while"
	if (show)
		codInstance->addCode("}");
};
#endif

