#ifndef GUARD_Search
#define GUARD_Search

// Title: Codifier (Search Namespace)
// Author: Saulo Fonseca <fonseca@astrotown.de>
// Description: Search Functions

#include <iostream>
#include <chrono>  // Because of high_resolution_clock
#include <limits>  // Because of numeric_limits
#include <vector>
using namespace std;
using namespace std::chrono;

namespace search
{
	// Clear the code using a variation of Hill Climbing Search algorithm.
	// Used to remove useless instructions.
	template<class T>
	static void clearCode(T &elem)
	{
		// Run clean 3 times
		int size = elem.getSizeHeads();
		for (int i=0; i<3; i++)
		{
			for (int j=0; j<size; j++)
			{
				T clone = elem;
				clone.kill(j);
				if (clone.solved())
				{
					elem = clone;
				}
			}
		}
	}

	// Genetic Search function
	template<class T>
	static void genetic(T)
	{
		// Initial time
		int highestHeur = numeric_limits<int>::max();
		auto start = high_resolution_clock::now();
		int oldBest = numeric_limits<int>::max();
		int generation = 0;
		T::instances = 0;

		// Generate four instances
		T instances[4];
		T crossInstances[4];
		for (int i=0; i<4; i++)
			instances[i].generateRnd();

		// Start genetic search
		while(true)
		{
			// Verify if solution is found
			generation++;
			for (int i=0; i<4; i++)
			{
				if (instances[i].solved())
				{
					// Clear useless instructions
					auto end = high_resolution_clock::now(); // Before show()
					clearCode(instances[i]);
					instances[i].show();

					// Final time
					cout << "// Generations: " << generation << endl;
					cout << "// Instances:   " << T::instances << endl;
					auto elapsed = duration_cast<milliseconds>(end-start).count();
					elapsed /= 1000;
					cout << "// Time: " << elapsed	<< " sec" << endl;
					if (elapsed >= 1)
						cout << "// Performance: " << T::instances / elapsed
							<< " inst/sec" << endl;
					return;
				}
			}

			// Identify best and worst instance
			int best=numeric_limits<int>::max(), worst=0;
			int bestIdx=-1, worstIdx=-1;
			for (int i=0; i<4; i++)
			{
				int h = instances[i].heuristic();
				if (h < best)
				{
					best = h;
					bestIdx = i;
				}
				if (h > worst)
				{
					worst = h;
					worstIdx = i;
				}
			}

			// Avoid problems if all are the same
			if (bestIdx == worstIdx)
				worstIdx++;

			// Show heuristic progress (and avoid repeat)
			if (best != oldBest)
			{
				if (highestHeur == numeric_limits<int>::max())
					highestHeur = best;
				oldBest = best;
				instances[bestIdx].heuristic(true);

				// Draw graph
				int upperLimit = (highestHeur > 60 ? 60 : highestHeur);
				int pos = upperLimit*best/highestHeur;
				for (int i=0; i<pos; i++)
					cout << " ";
				cout << "*" << endl;
			}

			// Cross best and other instances (delete worst)
			int crossIdx = 0;
			for (int i=0; i<4; i++)
			{
				if (i != bestIdx && i != worstIdx)
				{
					instances[i].cross(instances[bestIdx],crossInstances,crossIdx);
					crossIdx += 2;
				}
			}
			for (int i=0; i<4; i++)
				instances[i] = crossInstances[i];

			// Mutation
			for (int i=0; i<4; i++)
				instances[i].mutation();
		}
	}
};
#endif

