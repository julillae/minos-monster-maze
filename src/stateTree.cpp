// Header
#include "../include/stateTree.hpp"

StateTree::StateTree(vector<Edge> const & edges, int numNodes)
{
	adjList.resize(numNodes);

	for (auto &edge : edges)
	{
		int src = edge.src;
		int dest = edge.dest;
		int weight = edge.weight;

		// insert at the end
		adjList[src].push_back(make_pair(dest, weight));
	}
}

