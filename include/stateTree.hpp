#pragma once

#include <iostream>
#include <vector>
using namespace std;

struct Edge {
	int src;
	int dest;
	int weight;
};

typedef pair<int, int> Pair;

// StateTree is a weighted graph represented by an adjacency list
class StateTree
{
public:
	// vector of vectors of Pairs used to represent an adjacency list
	vector<vector<Pair>> adjList;

	// Constructor
	StateTree(vector<Edge> const &edges, int numNodes);
};
