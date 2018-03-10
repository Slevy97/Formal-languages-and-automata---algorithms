#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <map>
#include <bitset>
#include <queue>
#include <cstdlib>
#define NMAX 1001
#define UNDEF -1

using namespace std;

vector <int> *dfa; // DFA representation
/** AVL that maps each symbols to an integer **/
map <char, int> asoc;
map <char, int> ::iterator it;
char* alphabet;
/** **/
bitset <NMAX> mark; /** final states **/
int nr_states, alf_dim = -1;
/** **/
bool **node_cmpr;
/**Disjoint data-set **/
int *father, *height;
/** Min - DFA **/
vector <vector<int> > min_DFA;
vector <int> repr;
bitset <NMAX> mark_minDFA; // final states in the min_DFA
/** **/
bitset <NMAX> mark_dfs;
vector <vector<vector<int> > > NFA;

void build_DFA() {

	try {

		ifstream in("DFA.out", ifstream::binary);
		int nr_edges, from, to, nr_fin, x, read_at;
		char alf;

		in >> nr_states >> nr_edges >> nr_fin;

		for (int i = 1; i <= nr_fin; i++)
		{
			in >> x;
			mark.set(x);
		}

		read_at = in.tellg();

		for (int i = 1; i <= nr_edges; i++)
		{
			in >> from >> alf >> to;

			it = asoc.find(alf); /** search for a letter in the map **/
			if (it == asoc.end()) /** if it was not found **/
			{
				asoc.insert(make_pair(alf, ++alf_dim)); /** map the letter with an integer **/
			}
		}

		alf_dim++;

		/** Allocate memory for the dfa representation **/
		dfa = new vector<int>[nr_states];
		alphabet = new char[alf_dim];
		node_cmpr = new bool*[nr_states];
		father = new int[nr_states];
		height = new int[nr_states];
		for (int i = 0; i < nr_states; i++)
		{
			node_cmpr[i] = new bool[nr_states];
			memset(node_cmpr[i], 0, sizeof(bool) * nr_states);
		}
		for (int i = 0; i < nr_states; i++)
			dfa[i].resize(alf_dim);

		/** Initialize the dfa with -1 for abort case **/
		for (int i = 0; i < nr_states; i++)
		{
			for (int j = 0; j < alf_dim; j++)
				dfa[i][j] = UNDEF;
		}

		/** **/

		in.seekg(read_at, in.beg);

		for (int i = 1; i <= nr_edges; i++)
		{
			in >> from >> alf >> to;
			dfa[from][asoc[alf]] = to;
			alphabet[asoc[alf]] = alf;
		}

		in.close();

	} catch (...) {

		cerr << "Error !\n";
		exit(1);
	}
}

/** Disjoint data-set functions **/

int det_father(int x) {

	int root = x, aux;
	while (root != father[root])
		root = father[root];
	while (x != father[x])
	{
		aux = x;
		x = father[x];
		father[aux] = root;
	}
	return root;
}

void unite(int x, int y)
{
	if (height[x] > height[y])
		father[y] = x;
	else
		father[x] = y;
	if (height[x] == height[y])
		height[y]++;
}

/** **/

void build_Indis_table() {

	/** Mark the pairs of states that are similar in the Indistinguishability table **/
	int ok = 0, x, y;
	for (int i = 1; i < nr_states; i++)
		for (int j = 0; j < i; j++)
			node_cmpr[i][j] = mark.test(i) ^ mark.test(j);

	/** The same as before, but this time using letters of the alphabet until 
	no more changes are made **/
	while (!ok)
	{
		ok = 1;
		for (int i = 1; i < nr_states; i++)
			for (int j = 0; j < i; j++)
				if (node_cmpr[i][j] == 0)
				{
					for (int sym = 0; sym < alf_dim; sym++)
					{
						x = dfa[i][sym];
						y = dfa[j][sym];
						if (x != y)
						{
							if (x != UNDEF && y != UNDEF)
							{
								if (x < y)
									swap(x, y);
								if (node_cmpr[x][y])
									node_cmpr[i][j] = node_cmpr[x][y], ok = 0;
							}
							else
								node_cmpr[i][j] = true;
						}
					}
				}
	}
}

void build_minDFA() {

	int ok, x, y;
	for (int i = 0; i < nr_states; i++)
	{
		father[i] = i;
		height[i] = 0;
	}
	/** Using the Disjoint data-set, build the sets of similar states **/
	for (int i = 1; i < nr_states; i++)
		for (int j = 0; j < i; j++)
			if (node_cmpr[i][j] == 0)
			{
				x = det_father(i);
				y = det_father(j);
				if (x != y)
					unite(x, y);
			}

	int indx, indx_find, where;
	
	/** Build the min_DFA graph using the new states obtained from the sets built before **/
	queue <int> q;
	q.push(0);
	repr.push_back(father[0]);
	min_DFA.push_back(vector<int>(alf_dim));
	fill(min_DFA[min_DFA.size() - 1].begin(), min_DFA[min_DFA.size() - 1].end(), UNDEF);

	while (!q.empty())
	{
		indx = q.front();
		q.pop();

		for (int sym = 0; sym < alf_dim; sym++)
		{
			where = UNDEF;
			if (dfa[repr[indx]][sym] != UNDEF)
				where = father[dfa[repr[indx]][sym]];

			if (where != UNDEF) {

				ok = 1;
				for (unsigned int j = 0; j < repr.size(); j++)
					if (repr[j] == where)
					{
						ok = 0;
						indx_find = j;
						break;
					}

				if (ok)
				{
					repr.push_back(where);
					min_DFA.push_back(vector<int>(alf_dim));
					fill(min_DFA[min_DFA.size() - 1].begin(), min_DFA[min_DFA.size() - 1].end(), UNDEF);
					min_DFA[indx][sym] = repr.size() - 1;
					q.push(repr.size() - 1);

				}
				else {
					min_DFA[indx][sym] = indx_find;
				}
			}
		}
	}

	/** mark the new final states in the min_DFA **/
	for (unsigned int i = 0; i < min_DFA.size(); i++)
		if (mark.test(repr[i]))
			mark_minDFA.set(i);

}

void dfs(int node) {

	mark_dfs.set(node);
	for (int sym = 0; sym < alf_dim; sym++)
		for (unsigned int i = 0; i < NFA[node][sym].size(); i++)
			if (!mark_dfs.test(NFA[node][sym][i]))
				dfs(NFA[node][sym][i]);
}


void erase_badstates() {

	/** Delete Dead-end states from the minDFA **/
	NFA.resize(min_DFA.size());
	for (unsigned int i = 0; i < NFA.size(); i++)
		NFA[i].resize(alf_dim);

	/** Build an NFA that represents the transpose of the min_DFA graph **/
	for (unsigned int i = 0; i < min_DFA.size(); i++)
		for (int sym = 0; sym < alf_dim; sym++)
			if (min_DFA[i][sym] != UNDEF)
				NFA[min_DFA[i][sym]][sym].push_back(i);

	/** Do a DFS traversal from every final node from the min_DFA **/
	for (unsigned int i = 0; i < NFA.size(); i++)
		if (mark_minDFA.test(i))
			dfs(i);

	/** Set the deleted states as non - final in the resulted min_DFA **/
	for (unsigned int i = 0; i < NFA.size(); i++)
		if (mark_dfs.test(i) == 0)
			mark_minDFA.set(i, 0);

	/** In the min_DFA representation, clear the edges to deleted states **/
	for (unsigned int i = 0; i < min_DFA.size(); i++)
		for (int sym = 0; sym < alf_dim; sym++)
			if (min_DFA[i][sym] != UNDEF && mark_dfs.test(min_DFA[i][sym]) == 0)
				min_DFA[i][sym] = UNDEF;
	
}


void free_mem() {

	try {

		delete[] dfa;
		delete[] alphabet;
		for (int i = 0; i < nr_states; i++)
			delete[] node_cmpr[i];
		delete[] node_cmpr;
		delete[] father;
		delete[] height;
	}
	catch (...) {

		cerr << "Error !\n";
		exit(1);
	}
}

void print_minDFA() {

	ofstream out("minDFA.out");
	int nr_edges = 0;

	for (unsigned int i = 0; i < min_DFA.size(); i++)
		for (unsigned int j = 0; j < min_DFA[i].size(); j++)
			if (min_DFA[i][j] != UNDEF)
				nr_edges++;

	out << mark_dfs.count() << " " << nr_edges << " " << mark_minDFA.count() << "\n";
	for (unsigned int i = 0; i < min_DFA.size(); i++)
		if (mark_minDFA.test(i))
			out << i << " ";
	out << "\n";

	for (unsigned int i = 0; i < min_DFA.size(); i++)
		for (unsigned int j = 0; j < min_DFA[i].size(); j++)
			if (min_DFA[i][j] != UNDEF)
				out << i << " " << alphabet[j] << " " << min_DFA[i][j] << "\n";
	out.close();
}

int main() {

	try {

		build_DFA();
		build_Indis_table();
		build_minDFA();
		erase_badstates();
		print_minDFA();
	}
	catch (...) {

		cerr << "Error !\n";
		exit(1);
		
	}

	free_mem();
	return 0;
}
