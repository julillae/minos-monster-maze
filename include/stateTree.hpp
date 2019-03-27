#pragma once

#include <iostream>
#include <vector>
using namespace std;

// Enum for character states
// set StateMAX = last item in enum, needed to know how many states are available
enum State { idle, running, jumping, rising, falling, landing, frozen, thawing, reviving, following, dead, StateMAX = dead};

struct Edge {
	State src;
	State dest;
	int weight;
};

typedef pair<State, int> DestWeightPair;

// boolean for whether state change was successful or not
// int for cost of change
typedef pair<bool, int> ChangeCost;

// StateTree is a weighted graph represented by an adjacency list
class StateTree
{
public:
	// vector of vectors of Pairs used to represent an adjacency list
	vector<vector<DestWeightPair>> adjList;

	// Constructor creates a StateTree given all edges and the starting node
	StateTree(vector<Edge> const &edges, State startingNode);

	vector<DestWeightPair> getChildren();

	// returns true if state successfully changed, false otherwise
	bool changeState(State newState);

	// gets ChangeCost for hypothetically changing to newState
	// does not actually change state
	ChangeCost getStateChangeCost(State newState);

	void resetStateTree();

	State currentState;

private:
	State root;
};
