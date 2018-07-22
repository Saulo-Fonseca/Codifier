// Title: Codifier
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Artificial Inteligence algorithm to develop code
// Please compile using: g++ -std=c++11 main.cpp -o codifier

#include <iostream>
#include "search.hpp"
#include "codifier.hpp"
using namespace search;
using namespace std;

// Initialize static variables
int Codifier::size = 100;
int Codifier::instances = 0;

// srand(64983) & size=50
 vector<vector<int>> Codifier::source = {{2,1,3},{7,0,4},{1,9,2}};
 vector<vector<int>> Codifier::target = {{3,1,2},{4,0,7},{2,9,1}};

// srand(78079) & size = 75
// vector<vector<int>> Codifier::source = {{2,1,3},{ 7, 0, 4},{ 1, 9, 2}};
// vector<vector<int>> Codifier::target = {{6,6,6},{11,11,11},{12,12,12}};

// srand(60426) & size = 75
// vector<vector<int>> Codifier::source = {{2,1,3},{7,0,4},{1,9,2}};
// vector<vector<int>> Codifier::target = {{1,2,3},{0,4,7},{1,2,9}};

// srand(1153489657) & size=100
// vector<vector<int>> Codifier::source = {{2,1,3},{7,0,4},{1,9,2},{2,4,8},{9,2,1},{3,7,5},{2,0,2},{1,1,1}};
// vector<vector<int>> Codifier::target = {{1,2,3},{0,4,7},{1,2,9},{2,4,8},{1,2,9},{3,5,7},{0,2,2},{1,1,1}};

// Search for a code that transforms "source" in "target"
int main(int argc, char **argv)
{
	cout << "// Size: " <<  Codifier::size << endl;
	int t = (int)time(NULL) % 86400;
	cout << "// srand(" << t << ")" << endl; 
	srand(t);
	genetic(Codifier()); // Pass only the class type as argument
	return 0;
}

