#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
using namespace std;

/********************DO NOT EDIT**********************/
// Function prototype. Defined later.
void read_opinions(string filename); // reads file into opinions vector and updates total_nodes as needed
void read_edges(string filename); // reads file into sparse incoming-neighbour lists
void build_adj_matrix(); // finalize adjacency list

int total_nodes = 0; // We keep track of the total number of nodes based on largest node id.


/****************************************************************/

/******** Create sparse adjacency list and vector of opinions */
// simple vector to hold each node's opinion (0 or 1)
std::vector<int> opinions;

// in_neighbors[target] contains the nodes that influence target.
// This sparse representation uses O(V + E) space instead of O(V^2).
std::vector<std::vector<int>> in_neighbors;

int ones = 0;

void build_adj_matrix()
{
    // (1) make sure all nodes have an adjacency-list slot
    if ((int)in_neighbors.size() < total_nodes)
    {
        in_neighbors.resize(total_nodes);
    }

    // (2) match adjacency-matrix semantics: duplicate edges should count once.
    for (size_t i = 0; i < in_neighbors.size(); i++)
    {
        sort(in_neighbors[i].begin(), in_neighbors[i].end());
        in_neighbors[i].erase(
            unique(in_neighbors[i].begin(), in_neighbors[i].end()),
            in_neighbors[i].end());
    }
}

double calculate_fraction_of_ones()
{
    // (3) Calculate the fraction of nodes with opinion 1 and return it.
    if (total_nodes == 0)
    {
        return 0.0;
    }

    return (double)ones / total_nodes;
}

// For a given node, count majority opinion among its neighbours. Tie -> 0.
int get_majority_friend_opinions(int node)
{
    // (4) Count the number of neighbours with opinion 0 and opinion 1.
    // Return the majority (0 or 1). If tie, return 0.

    int zeros = 0;
    int ones = 0;

    for (size_t i = 0; i < in_neighbors[node].size(); i++)
    {
        int nbr = in_neighbors[node][i];
        if (opinions[nbr] == 0)
        {
            zeros++;
        }
        else
        {
            ones++;
        }
    }

    if (ones > zeros)
    {
        return 1;
    }

    return 0; // return 0 for ties or when 0 is the majority
}

// Calculate new opinions for all voters and return if anyone's opinion changed
bool update_opinions()
{
    // (5) For each node, calculate the majority opinion among its neighbours
    // and update the node's opinion.
    // Return true if any node's opinion changed, false otherwise.

    vector<int> next = opinions;
    bool changed = false;
    int next_ones = 0;

    for (int i = 0; i < total_nodes; i++)
    {
        next[i] = get_majority_friend_opinions(i);
        if (next[i] == 1)
        {
            next_ones++;
        }

        if (next[i] != opinions[i])
        {
            changed = true;
        }
    }

    opinions = next;
    ones = next_ones;
    return changed;
}

int main() {
    // no preallocation; vectors grow on demand

    // Read input files
    read_opinions("opinions.txt"); 
    read_edges("edge_list.txt");

    // finalize sparse adjacency structure once we know total_nodes
    build_adj_matrix();
    
    cout << "Total nodes: " << total_nodes << endl;
    
    // Run simulation
    int max_iterations = 30;
    int iteration = 0;
    bool opinions_changed = true;
    
    // Print initial state
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << calculate_fraction_of_ones() << endl;
    
    /// (6)  //////////////////////////////////////////////
    while (iteration < max_iterations && opinions_changed)
    {
        double current_fraction = calculate_fraction_of_ones();

        // stop if consensus already reached
        if (current_fraction == 0.0 || current_fraction == 1.0)
        {
            break;
        }

        opinions_changed = update_opinions();
        iteration++;

        cout << "Iteration " << iteration << ": fraction of 1's = "
             << calculate_fraction_of_ones() << endl;
    }
    ////////////////////////////////////////////////////////
    
    // Print final result
    double final_fraction = calculate_fraction_of_ones();
    cout << "Iteration " << iteration << ": fraction of 1's = " 
         << final_fraction << endl;
    
    if(final_fraction == 1.0)
        cout << "Consensus reached: all 1's" << endl;
    else if(final_fraction == 0.0)
        cout << "Consensus reached: all 0's" << endl;
    else
        cout << "No consensus reached after " << iteration << " iterations" << endl;
    
    return 0;
}


/*********** Functions to read files **************************/ 

// Read opinion vector from file.
void read_opinions(string filename)
{
    ifstream file(filename);
    int id, opinion;
    while(file >> id >> opinion)
    {
        if(id >= total_nodes) total_nodes = id+1;
        if(id >= (int)opinions.size()) opinions.resize(id + 1, 0);

        if(opinions[id] == 1) ones--;
        opinions[id] = opinion;
        if(opinion == 1) ones++;
    }
    file.close();
}

// Read edges from file and update total nodes as needed.
void read_edges(string filename)
{
    ifstream file(filename);
    int source, target;
    
    while(file >> source >> target)
    {
        if(source >= total_nodes) total_nodes = source+1;
        if(target >= total_nodes) total_nodes = target+1;
        if(target >= (int)in_neighbors.size()) in_neighbors.resize(target + 1);

        in_neighbors[target].push_back(source);
    }
    if((int)opinions.size() < total_nodes) opinions.resize(total_nodes, 0);
    file.close();
}

/********************************************************************** */
