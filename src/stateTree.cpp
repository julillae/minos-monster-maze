// Header
#include "../include/stateTree.hpp"

StateTree::StateTree(vector<Edge> const & edges, State startingNode)
{
	adjList.resize(StateMAX + 1);

	for (auto &edge : edges)
	{
		State src = edge.src;
		State dest = edge.dest;
		int weight = edge.weight;

		// insert at the end
		adjList[src].push_back(make_pair(dest, weight));
	}

	root = startingNode;
	currentState = root;
}

vector<DestWeightPair> StateTree::getChildren()
{
	return adjList[currentState];
}

bool StateTree::changeState(State newState)
{
	vector<DestWeightPair> validNewStates = getChildren();
	for (auto &pair : validNewStates) {
		if (pair.first == newState) {
			currentState = newState;
			return true;
		}
	}
	return false;
}

ChangeCost StateTree::getStateChangeCost(State newState)
{
	vector<DestWeightPair> validNewStates = getChildren();
	for (auto &pair : validNewStates) {
		if (pair.first == newState) {
			return make_pair(true, pair.second);
		}
	}
	return make_pair(false, 0);
}

void StateTree::resetStateTree()
{
	currentState = root;
}

