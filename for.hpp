#ifndef GUARD_For
#define GUARD_For

// Title: Codifier (For class)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: For loop
#include "codifier.hpp"

class For
{
private:
	Codifier *codInstance;
	int start, end, step, counter;
	bool boolTrue;
	
public:
	// Constructor
	For(Codifier *c) {codInstance = c;}
	
	// Define main variables
	int def(bool it=true)
	{
		// Define the start and end of loop
		if (codInstance->nextIsOverflow()) return 0;
		start = codInstance->getInst() % codInstance->getSrcSize();
		if (codInstance->nextIsOverflow()) return 0;
		end = codInstance->getInst() % codInstance->getSrcSize();
		step = 1;
		if (start > end)
			step = -1;

		// If inside an "if", verifies if "if" is true
		boolTrue = it;
		
		// Define how many commands will be inside "for"
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
#include "while.hpp"

// Run the "for" loop
void For::run(bool show)
{
	// Print opening the command
	if (show)
	{
		string forStr = "for (i=";
		forStr += to_string(start);
		forStr += "; i";
		if (step == 1)
			forStr += "<";
		else
			forStr += ">";
		forStr += to_string(end+step);
		forStr += "; ";
		if (step == 1)
			forStr += "i++";
		else
			forStr += "i--";
		forStr += ")";
		codInstance->addCode(forStr);
		codInstance->addCode("{");
	}

	// Save old values
	int oldIndex = codInstance->getIndex();
	bool oldShow = show;

	// Main "for" loop
	int &i = *codInstance->getRefI();
	for (i=start; i!=end+step; i+=step)
	{
		// Retrieve old starting point
		codInstance->setIndex(oldIndex);
		
		// Run the commands inside "for"
		// "for" has its own loop
		int cloneCounter = counter;
		while (cloneCounter > 0)
		{
			// Set context variables
			if (codInstance->getValI() != start)
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
			else if (instruction == 5)
			{
				// If statement
				If inst(codInstance);
				int idx = codInstance->getIndex();
				int cnt = inst.def(boolTrue,true);
				if (codInstance->isOverflow()) break;
				codInstance->addHead(idx,cnt);
				inst.run(show);
			}
			else if (instruction == 6)
			{
				// While statement
				While inst(codInstance);
				int idx = codInstance->getIndex();
				int cnt = inst.def(boolTrue,true);
				if (codInstance->isOverflow()) break;
				codInstance->addHead(idx,cnt);
				inst.run(show);
			}

			// End if instruction was incomplete
			if (codInstance->isOverflow()) break;
		}
	}
		
	// Recover original show value
	show = oldShow;
		
	// Print closing "for"
	if (show)
		codInstance->addCode("}");
};
#endif

