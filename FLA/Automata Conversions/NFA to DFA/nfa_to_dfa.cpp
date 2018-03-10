/**
*	LFA - Lab
*	NFA to DFA converter
*	Author: Slevy
**/

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <cstring>
#include <algorithm>
#include <queue>
#include <exception>
#define UNDEF -1
#define NMAX 1001

using namespace std;

vector <vector<int> > *nfa; // NFA representation
vector <vector<int> > dfa; // DFA representation
vector <vector<int> > repr;
/** AVL that maps each symbols to an integer **/
map <char, int> asoc;
map <char, int> ::iterator it;
char *alphabet;

/** final states **/
bitset <NMAX> mark;
bitset <NMAX> mark_dfa;
int nr_states, alf_dim = -1;

void build_NFA() {

	try {

		ifstream in("NFA.out", ios::binary);
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

		/** Allocate memory for the nfa representation **/
		nfa = new vector<vector<int> >[nr_states];
		alphabet = new char[alf_dim];

		for (int i = 0; i < nr_states; i++)
			nfa[i].resize(alf_dim);
	
		/** **/

		in.seekg(read_at, in.beg);

		for (int i = 1; i <= nr_edges; i++)
		{
			in >> from >> alf >> to;
			nfa[from][asoc[alf]].push_back(to);
			alphabet[asoc[alf]] = alf;
		}

		in.close();

	}
	catch (...) {

		cerr << "Error !\n";
		exit(1);
	}
}

void build_DFA() {

	vector <int> aux;
	int indx, indx_find, where, ok;
	queue <int> q;

	/** **/
	q.push(0);
	repr.push_back(vector<int>(1, 0));
	dfa.push_back(vector<int>(alf_dim));
	fill(dfa[dfa.size() - 1].begin(), dfa[dfa.size() - 1].end(), UNDEF);
	/** **/

	while (!q.empty())
	{
		indx = q.front();
		q.pop();

		for (int sym = 0; sym < alf_dim; sym++)
		{
			aux.clear();
			for (unsigned int j = 0; j < repr[indx].size(); j++)
			{
				where = repr[indx][j];
				for (unsigned int k = 0; k < nfa[where][sym].size(); k++)
				{
					if (find(aux.begin(), aux.end(), nfa[where][sym][k]) == aux.end())
						aux.push_back(nfa[where][sym][k]);
				}
			}

			if (aux.size() != 0) {

				ok = 1;
				for (unsigned int j = 0; j < repr.size(); j++)
					if (repr[j] == aux)
					{
						ok = 0;
						indx_find = j;
						break;
					}

				if (ok)
				{
					repr.push_back(aux);
					dfa.push_back(vector<int>(alf_dim));
					fill(dfa[dfa.size() - 1].begin(), dfa[dfa.size() - 1].end(), UNDEF);
					dfa[indx][sym] = repr.size() - 1;
					q.push(repr.size() - 1);

				}
				else {
					dfa[indx][sym] = indx_find;
				}
			}
		}
	}
}

void free_mem() {

	try {

		delete[] nfa;
		delete[] alphabet;
	
	} catch (...) {

		cerr << "Error !\n";
		exit(1);
	}
}

void mark_final() {

	for (unsigned int i = 0; i < repr.size(); i++)
	{
		for (unsigned int j = 0; j < repr[i].size(); j++)
		{
			if (mark.test(repr[i][j]))
				mark_dfa.set(i);
		}
	}
}

void print_dfa() {

	ofstream out("DFA.out");
	int nr_edges = 0;

	for (unsigned int i = 0; i < dfa.size(); i++)
		for (unsigned int j = 0; j < dfa[i].size(); j++)
			if (dfa[i][j] != UNDEF)
				nr_edges++;

	out << repr.size() << " " << nr_edges << " " << mark_dfa.count() << "\n";
	for (unsigned int i = 0; i < repr.size(); i++)
		if (mark_dfa.test(i))
			out << i << " ";
	out << "\n";

	for (unsigned int i = 0; i < dfa.size(); i++)
		for (unsigned int j = 0; j < dfa[i].size(); j++)
			if (dfa[i][j] != UNDEF)
				out << i << " " << alphabet[j] << " " << dfa[i][j] << "\n";
	out.close();
}

int main() {

	try {

		build_NFA();
		build_DFA();
		mark_final();
		print_dfa();
		free_mem();
	
	} catch (...) {

		cerr << "Error !\n";
		exit(1);
	}

	return 0;
}