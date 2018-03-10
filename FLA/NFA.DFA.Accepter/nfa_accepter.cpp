/** NFA ACCEPTER **/

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <bitset>
#define NMAX 1001

using namespace std;

vector <list<int> > *nfa; // NFA representation
/** AVL that maps each symbols to an integer **/
map <char, int> asoc;
map <char, int> :: iterator it;
/** **/
bitset <NMAX> mark; /** final states **/
int nr_states, alf_dim = -1;

void build_NFA () {

	ifstream in("NFA.in", ifstream::binary);
	int n, from, to, nr_acc, x, read_at, val;
	char alf;

	in >> nr_states >> n >> nr_acc;

	for (int i = 1; i <= nr_acc; i++)
	{
		in >> x;
		mark.set(x);
	}

	in.get();

	read_at = in.tellg();

	for (int i = 1; i <= n; i++)
	{
		in >> from >> alf >> to;

		it = asoc.find(alf); /** search for a letter in the map **/
		if (it == asoc.end()) /** if it was not found **/
			asoc.insert(make_pair(alf, ++alf_dim)); /** map the letter with an integer **/
	}

	alf_dim++;

	/** Allocate memory for the nfa representation **/
	nfa = new vector<list<int> > [nr_states];

	for (int i = 0; i < nr_states; i++)
		nfa[i].resize(alf_dim);

	/** **/

	in.seekg(read_at, in.beg);

	for (int i = 1; i <= n; i++)
	{
		in >> from >> alf >> to;
		nfa[from][asoc[alf]].push_back(to);
	}

	in.close();
}

bool evaluate (const char* word) {

	unsigned int length = strlen(word);
	int cur_state = 0;

	list <int> cur_states1, cur_states2;

	cur_states1.push_back(cur_state);

	list<int>::iterator it_beg, it_end;

	if (length == 0) 
	{
		if (mark.test(cur_state))
			return true;
		return false;
	}

	for (unsigned int i = 0; i < length; i++)
	{
		if (!cur_states1.empty()) {

			/** Checks all the current states **/

			while (!cur_states1.empty())
			{
				cur_state = cur_states1.front();

				if (asoc.find(word[i]) != asoc.end())
				{
					it_beg = nfa[cur_state][asoc[word[i]]].begin();
					it_end = nfa[cur_state][asoc[word[i]]].end();

					/** finds all the states connected to the current state **/
					for (; it_beg != it_end; it_beg++)
						cur_states2.push_back(*it_beg);
				}

				cur_states1.pop_front();
			}


		} else if (!cur_states2.empty()) {

			/** Checks all the current states **/

			while (!cur_states2.empty())
			{
				cur_state = cur_states2.front();

				if (asoc.find(word[i]) != asoc.end())
				{
					it_beg = nfa[cur_state][asoc[word[i]]].begin();
					it_end = nfa[cur_state][asoc[word[i]]].end();

					/** finds all the states connected to the current state **/
					for (; it_beg != it_end; it_beg++)
						cur_states1.push_back(*it_beg);
				}

				cur_states2.pop_front();
			}

		} else break;
	}

	if (!cur_states1.empty())
	{
		/** Checks for final states to see whether 
			the word was accepted or not **/

		while (!cur_states1.empty())
		{
			if (mark.test(cur_states1.front()))
				return true;

			cur_states1.pop_front();
		}
	}

	if (!cur_states2.empty())
	{
		/** Checks for final states to see whether 
			the word was accepted or not **/
		
		while (!cur_states2.empty())
		{
			if (mark.test(cur_states2.front()))
				return true;

			cur_states2.pop_front();
		}
	}
	/** **/
	return false;
}

int main (int argc, char* argv[]) {

	build_NFA();

	char *word = new char[NMAX + 1];
	cin.getline(word, NMAX);
	
	if (evaluate(word) == true)
		cout << "Word is accepted!\n";
	else
		cout << "Word is not accepted!\n";

	delete[] nfa;
	delete[] word;

	return 0;
}
