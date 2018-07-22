#ifndef GUARD_Codifier
#define GUARD_Codifier

// Title: Codifier
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Artificial Inteligence algorithm to develop code

#include <iostream>
#include <iomanip> // Because of setfill(), setw()
#include <vector>
#include <string>

using namespace std;
using namespace search;

// Codifier class
class Codifier
{
private:
	vector<string> mvars;      // Collect vars status over runs
	vector<string> code;       // Contains the generated code
	int *instructions;         // Instructions forming the program
	vector<int> src;           // Clone of one vector of source returned as result of run
	vector<int> var;           // Variables available to be used by the code
	int *heads;                // Position of the beginning of each instruction
	bool error;                // Inform if code contains error
	int index;                 // Pointer to instruction vector
	int i;                     // Iteration variable for "for" loops
	
public:
	// Static vars
	static vector<vector<int>> source;    // Vector with source values
	static vector<vector<int>> target;    // Vector with target values
	static int instances;                 // Count how many was generated
	static int size;                      // Number max of entries on vector
	
	// Constructor
	Codifier()
	{
		// Setup all vars
		instructions = new int[size];
		heads = new int[size];
		clearHeads();
		clear();
		instances++;
	}

	// Copy constructor
	Codifier(const Codifier &e)
	{
		clear();
		instructions = new int[size];
		heads = new int[size];
		for (int i=0; i<size; i++)
		{
			instructions[i] = e.instructions[i];
			heads[i] = e.heads[i];
		}
		instances++;
	}

	// Copy operator
	Codifier &operator=(const Codifier &e)
	{
		clear();
		delete [] instructions;
		delete [] heads;
		instructions = new int[size];
		heads = new int[size];
		for (int i=0; i<size; i++)
		{
			instructions[i] = e.instructions[i];
			heads[i] = e.heads[i];
		}
		instances++;
		return *this;
	}

	// Destructor
	~Codifier()
	{
		delete [] instructions;
		delete [] heads;
	}
	
	// Small Methods
	void addHead(int idx, int c) {if (heads[idx] == 0) heads[idx] = c;}
	bool isOverflow() {return (index >= size) ? true : false;}
	void clearHeads() {fill(heads,heads+size,0);}
	void addCode(string s) {code.push_back(s);}

	// Sets
	void setIndex(int n) {index = n;}
	void setError() {error = true;}
	
	// Gets
	int getInst() {return instructions[index];}
	int *getRefSrcElem(int n) {return &src[n];}
	int getSrcSize() {return (int)src.size();}
	int getVarSize() {return (int)var.size();}
	int getCounter() {return heads[index];}
	int *getRefVar(int n) {return &var[n];}
	int getSrc(int n) {return src[n];}
	bool getError() {return error;}
	int getIndex() {return index;}
	int getSize() {return size;}
	int *getRefI() {return &i;}
	int getValI() {return i;}

	// Send status to stdout
	void statusAll()
	{
		for (int i=0; i<(int)mvars.size(); i++)
			cout << mvars[i] << endl;
	}

	// Clear all variables
	void clear()
	{
		error = false;
		src.clear();
		var.clear();
		index = 0;
		i = 0;
	}
	
	// Mark killed instruction with -1 plus -2 until next instruction
	// -1,-2,-2,-2,-2,-2,-2... - Avoid being interpreted
	// Killed instructions are not removed from heads
	void kill(int n)
	{
		// Search for the nth head
		int count = -1;
		int idx;
		for (idx=0; idx<size; idx++)
		{
			if (heads[idx] != 0)
				count++;
			if (count == n)
				break;
		}

		// Kill instruction on idx
		if (idx != size && instructions[idx] != -1)
		{
			instructions[idx] = -1;
			while (idx+1 < size && heads[idx+1] == 0)
			{
				idx++;
				instructions[idx] = -2;
			}
			run(); // Update error
		}
	}
	
	// Count the number of heads
	int getSizeHeads()
	{
		int count = 0;
		for (int i=0; i<size; i++)
			if (heads[i] != 0)
				count++;
		return count;
	}
	
	// Go to next instructions and check overflow
	bool nextIsOverflow()
	{
		if (error)
			return true;
		index++;
		return isOverflow();
	}
	
	// Advances instructions until another than 0 or overflow is found
	bool nextInstruction()
	{
		if (error)
			return true;
		do
			index++;
		while (!isOverflow() && instructions[index] == -2);
		return isOverflow();
	}

	// Dump the code vector
	void showCode()
	{
		int tab = 0;
		for (int i=0; i<(int)code.size(); i++)
		{
			if (code[i][0] == ';') // Remove NOP instructions
				continue;
			if (code[i][0] == '}')
				tab--;
			for (int i=0; i<tab; i++)
				cout << "\t";
			cout << code[i] << endl;
			if (code[i] == "{")
				tab++;
		}		
	}

	// Crate summary of variables values
	string getVars()
	{
		string str = "// i:"+to_string(i);
		str += ", src:";
		for (int i=0; i<(int)src.size(); i++)
			str += to_string(src[i])+",";
		str += " var:";
		for (int i=0; i<(int)var.size(); i++)
			str += to_string(var[i])+",";
		return str;
	}

	// Show instructions
	void show()
	{
		// Runs generated code
		run(true);

		// Print code
		showCode();
		statusAll();
	}

	// Genetic methods

	// Combine two instructions set
	void cross(Codifier &best, Codifier *crossVector, int crossIdx)
	{
		int counter = 0;
		bool error;
		do
		{
			// Reset neu instances
			for (int i=crossIdx; i<crossIdx+2; i++)
			{
				crossVector[i].clear();
				crossVector[i].clearHeads();
			}

			// Choose point where vector will change between parents
			int change = rand()%(size-1)+1;

			// Combine best with this instance
			for (int i=0; i<change; i++)
				crossVector[crossIdx+0].instructions[i] = best.instructions[i];
			for (int i=change; i<size; i++)
				crossVector[crossIdx+0].instructions[i] = instructions[i];

			// Combine this instance with best
			for (int i=0; i<change; i++)
				crossVector[crossIdx+1].instructions[i] = instructions[i];
			for (int i=change; i<size; i++)
				crossVector[crossIdx+1].instructions[i] = best.instructions[i];

			// Test errors in the instances
			counter++;
			error = false;
			for (int i=crossIdx; i<crossIdx+2; i++)
			{
				crossVector[i].run();
				if (crossVector[i].error)
					error = true;
			}

			// Interrupt in case of unsolvable
			if (counter > size)
			{
				setError();
				return;
			}
		}
		while (error);
	}

	// Calculate heuristics for this instructions set
	int heuristic(bool show=false)
	{
		int diff = run(show);
		if (show)
			cout << "// Heuristic: " << setfill(' ') << setw(4) << diff << " ";
		return diff;
	}

	// Fill instructions vector with randon data
	void generateRnd()
	{
		do
		{
			for (int i=0; i<size; i++)
			{
				instructions[i] = rand()%100;
				heads[i] = 0;
			}
			run();
		} while (error);
	}

	// Include random mutations
	void mutation()
	{
		Codifier clone;
		int counter = 0;
		do
		{
			// Create a clone of this instance
			clone = *this;
			
			// Mutation rate about 0.01 (depends on instructions length)
			if (rand()%2==0)
			{
				int idx = rand()%size;
				int val = rand()%100;
				clone.instructions[idx] = val;
				clone.clearHeads();
				clone.run();
			}
			counter++;

			// Interrupt in case of unsolvable
			if (counter > size)
			{
				error = false;
				setError();
				return;
			}
		}
		while (clone.error);
		
		// Copy clone back
		*this = clone;
		clear();
		clearHeads();
	}
	
	// Verify if solution is found
	bool solved()
	{
		if (error)
			return false;
		if (heuristic() == 0)
			return true;
		return false;
	}
	
	// Run methods
	int run(bool show = false);
};

// Dependencies
#include "set.hpp"
#include "opr.hpp"
#include "inc.hpp"
#include "if.hpp"
#include "for.hpp"
#include "while.hpp"

// Prepare to run
// Returned value will be the heuristic
int Codifier::run(bool show)
{
	// Add initialization as code
	bool oldShow = show;
	code.clear();
	mvars.clear();
	if (show)
	{
		addCode("// Description: Dynamic generated code");
		addCode("// Author: Codifier - The Artificial Intelligence C Programmer");
		addCode("#include <stdio.h>");
		addCode("");
		addCode("// Function created by Codifier");
		addCode("int codifier(int *src, int *var)");
		addCode("{");
		addCode("int i = 0;");
	}

	// Define quantity of temporary variables
	int varSize = instructions[0] % 10 + 1; // 1 to 10 temp vars

	// Calculate the heuristics for the vectors inside source
	int diff = 0;
	for (int i=0; i<(int)source.size(); i++)
	{
		// Show only code for the first run
		if (i != 0)
			show = false;

		// Fill var vector with zeros
		clear();
		for (int j=0; j<varSize; j++)
			var.push_back(0);

		// Copy source vector to src
		for (int j=0; j<(int)source[i].size(); j++)
			src.push_back(source[i][j]);

		// Run instructions on main()
		index = 0; // will start interpreting at index 1
		while (!nextInstruction())
		{
			int instruction = getInst()%7;
			if (instruction == -1)
			{
				// Instructions -1 means the instruction was killed.
				addHead(index,1);
				if (show)
					addCode("; // Killed");
			}
			else if (instruction == 0)
			{
				// NOP - Ignore entry.
				// This is important to permit different quantity of
				// commands in the instructions set.
				addHead(index,1);
				if (show)
					addCode(";");
			}
			else if (instruction == 1)
			{
				// Attribution
				addHead(index,1);
				Set inst(this);
				inst.run(show);
			}
			else if (instruction == 2)
			{
				// Operation
				addHead(index,1);
				Opr inst(this);
				inst.run(show);
			}
			else if (instruction == 3)
			{
				// Increment
				addHead(index,1);
				Inc inst(this);
				inst.run(show);
			}
			else if (instruction == 4)
			{
				// For loop
				For inst(this);
				int idx = index;
				int cnt = inst.def();
				if (isOverflow()) break;
				addHead(idx,cnt);
				inst.run(show);
			}
			else if (instruction == 5)
			{
				// If statement
				If inst(this);
				int idx = index;
				int cnt = inst.def();
				if (isOverflow()) break;
				addHead(idx,cnt);
				inst.run(show);
			}
			else if (instruction == 6)
			{
				// While statement
				While inst(this);
				int idx = index;
				int cnt = inst.def();
				if (isOverflow()) break;
				addHead(idx,cnt);
				inst.run(show);
			}
		}

		// Check for errors
		if (error)
			return 999999999;

		// Collect var status to show later
		mvars.push_back(getVars());

		// Calculate partial heuristics
		for (int j=0; j<(int)src.size(); j++)
		{
			// Avoid sum overflow
			if (src[j] < -999999999 || src[j] > 999999999)
			{
				setError();
				return 999999999;
			}

			// Find diferences between src and target vectors
			if (src[j] > target[i][j])
				diff += src[j] - target[i][j];
			else
				diff += target[i][j] - src[j];

			// Check integer overflow
			if (diff < 0)
			{
				setError();
			}
		}
	}

	// Close function
	show = oldShow;
	if (show)
	{
		addCode("return i;");
		addCode("}");
		addCode("");
		addCode("int main()");
		addCode("{");
		addCode("// Define source vector");
		string srcStr = "int source[]["+to_string(source[0].size())+"] = {";
		for (int i=0; i<(int)source.size(); i++)
		{
			srcStr += "{";
			for (int j=0; j<(int)source[i].size(); j++)
			{
				srcStr += to_string(source[i][j]);
				if (j != (int)source[i].size()-1)
					srcStr += ",";
			}
			srcStr += "}";
			if (i != (int)source.size()-1)
				srcStr += ",";
		}
		srcStr += "};";
		addCode(srcStr);
		addCode("");
		addCode("// Run codifier function for each source vector");
		addCode("int m = 0, n = 0;");
		addCode("for (m=0; m<"+to_string(source.size())+"; m++)");
		addCode("{");
		addCode("// Initialize variables and run");
		srcStr = "int var[] = {";
		for (int i=0; i<varSize; i++)
		{
			srcStr += "0";
			if (i != varSize-1)
				srcStr += ",";
		}
		srcStr += "};";
		addCode(srcStr);
		addCode("int i = 0;");
		addCode("i = codifier(source[m],var);");
		addCode("");
		addCode("// Print status");
		addCode("printf(\"// i:%d, src:\",i);");
		addCode("for (n=0; n<"+to_string(source[0].size())+"; n++)");
		addCode("	printf(\"%d,\",source[m][n]);");
		addCode("printf(\" var:\");");
		addCode("for (n=0; n<"+to_string(varSize)+"; n++)");
		addCode("	printf(\"%d,\",var[n]);");
		addCode("printf(\"\\n\");");
		addCode("}");
		addCode("return 0;");
		addCode("}");
	}
	return diff;
}
#endif

