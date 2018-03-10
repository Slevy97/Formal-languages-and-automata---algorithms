/**
*	LFA - Lab
*	LNFA to NFA converter
*	Author: Slevy
**/

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <map>
#include <cstring>
#include <algorithm>
#define NMAX 1001

using namespace std;

vector <vector<int> > *lnfa; // LNFA representation
vector <vector<int> > *nfa; // NFA representation
vector<int> *lclosure;

/** AVL that maps each symbols to an integer **/
map <char, int> asoc;
map <char, int> ::iterator it;
char *alphabet;

/** final states **/
bitset <NMAX> mark; 
bitset <NMAX> mark_nfa;

int nr_states, alf_dim = -1;

void build_LNFA() {

	try {

		ifstream in("LNFA.in", ios::binary);
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
		lnfa = new vector<vector<int> >[nr_states];
		lclosure = new vector<int>[nr_states];
		nfa = new vector<vector<int> >[nr_states];
		alphabet = new char[alf_dim];

		for (int i = 0; i < nr_states; i++)
		{
			lnfa[i].resize(alf_dim);
			nfa[i].resize(alf_dim);
		}
		/** **/

		in.seekg(read_at, in.beg);

		for (int i = 1; i <= nr_edges; i++)
		{
			in >> from >> alf >> to;
			lnfa[from][asoc[alf]].push_back(to);
			alphabet[asoc[alf]] = alf;
		}

		in.close();

	} catch (...) {
		
		cerr << "Error !\n";
		exit(1);
	}
}

void build_lambda_closure() {

	int lambda, where;

	lambda = (*it).second;

	for (int i = 0; i < nr_states; i++) // for each state, find the direct states where you can get in with only one lambda transition
	{
		lclosure[i].push_back(i);
		for (unsigned int k = 0; k < lnfa[i][lambda].size(); k++)
			lclosure[i].push_back(lnfa[i][lambda][k]);
	}
	for (int i = 0; i < nr_states; i++) // for each state, compose the lambda transitions until you get the lambda closure of all states
	{
		for (unsigned int j = 1; j < lclosure[i].size(); j++)
		{
			where = lclosure[i][j];
			for (unsigned int k = 0; k < lclosure[where].size(); k++)
				if (find(lclosure[i].begin(), lclosure[i].end(), lclosure[where][k]) == lclosure[i].end()) {

					lclosure[i].push_back(lclosure[where][k]);
				}
		}
	}
}

void build_NFA() {

	int lambda, where;
	int from;

	lambda = (*it).second;

	for (int i = 0; i < nr_states; i++)
	{
		for (unsigned int j = 0; j < lclosure[i].size(); j++)
		{
			where = lclosure[i][j]; // for each state in the lclosure of state i, find where you can go
			for (int sym = 0; sym < alf_dim; sym++) // with sym letter and add the lclosure of the 
			{										// destination to the nfa representation
				if (sym != lambda) {

					for (unsigned int k = 0; k < lnfa[where][sym].size(); k++)
					{
						from = lnfa[where][sym][k];
						for (unsigned int l = 0; l < lclosure[from].size(); l++)
						{
							if (find(nfa[i][sym].begin(), nfa[i][sym].end(), lclosure[from][l]) == nfa[i][sym].end()) {

								nfa[i][sym].push_back(lclosure[from][l]);
							}
						}
					}
				}
			}
		}
	}
}

void check_finals() {

	// Mark all the states that enter in a final-state in the LNFA ,
	// with a lambda transition, as being final in the NFA.
	for (int i = 0; i < nr_states; i++)
	{
		for (unsigned int j = 0; j < lclosure[i].size(); j++)
		{
			if (mark.test(lclosure[i][j]))
			{
				mark_nfa.set(i);
			}
		}
	}
}

void print_nfa() {

	ofstream out("NFA.out");
	int nr_edges = 0;
	/** count how many edges are in the resulted NFA **/
	for (int i = 0; i < nr_states; i++)
		for (int j = 0; j < alf_dim; j++)
			for (unsigned int k = 0; k < nfa[i][j].size(); k++)
				nr_edges++;

	out << nr_states << " " << nr_edges << " " << mark_nfa.count() << "\n";

	/** print the final states in the NFA **/
	for (int i = 0; i < nr_states; i++)
		if (mark_nfa.test(i))
			out << i << " ";
	out << "\n";

	/** print the edges in the NFA **/
	for (int i = 0; i < nr_states; i++)
	{
		for (int j = 0; j < alf_dim; j++)
		{
			for (unsigned int k = 0; k < nfa[i][j].size(); k++)
			{
				out << i << " " << alphabet[j] << " " << nfa[i][j][k] << "\n";
			}
		}
	}
	out.close();
}

void copy_file() {

	ifstream in("LNFA.in");
	ofstream out("NFA.out");
	char ch;
	while (!in.eof())
	{
		ch = in.get();
		if (!in.eof())
			out << ch;
	}
	in.close();
	out.close();
}

void free_mem() {

	try {

		delete[] lnfa;
		delete[] nfa;
		delete[] lclosure;
		delete[] alphabet;
	
	} catch (...) {

		cerr << "Error !\n";
		exit(1);
	}
}

int main() {

	build_LNFA();
	
	try {

		it = asoc.find('_');
		if (it != asoc.end()) // if there are lambda transitions in the lnfa
		{
			build_lambda_closure();
			build_NFA();
			check_finals();
			print_nfa();
		}
		else
			copy_file();

	} catch (...) {

		cerr << "Error !\n";
		exit(1);
	}
	free_mem();
	return 0;
}