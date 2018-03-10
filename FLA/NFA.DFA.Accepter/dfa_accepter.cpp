/** DFA ACCEPTER **/

#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <vector>
#include <map>
#include <bitset>
#define NMAX 1001

using namespace std;

vector <int> *dfa; // DFA representation
/** AVL that maps each symbols to an integer **/
map <char, int> asoc;
map <char, int> :: iterator it;
/** **/
bitset <NMAX> mark; /** final states **/
int nr_states, alf_dim = -1;

void build_DFA () {

	ifstream in("DFA.in", ifstream::binary);
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

	/** Allocate memory for the dfa representation **/
	dfa = new vector<int> [nr_states];

	for (int i = 0; i < nr_states; i++)
		dfa[i].resize(alf_dim);

	/** Initialize the dfa with -1 for abort case **/
	for (int i = 0; i < nr_states; i++)
	{
		for (int j = 0; j < alf_dim; j++)
			dfa[i][j] = -1;
	}

	/** **/

	in.seekg(read_at, in.beg);

	for (int i = 1; i <= n; i++)
	{
		in >> from >> alf >> to;
		dfa[from][asoc[alf]] = to;
	}

	in.close();
}

bool evaluate (const char* word) {

	unsigned int length = strlen(word);
	int cur_state = 0;

	if (length == 0) 
	{
		if (mark.test(cur_state))
			return true;
		return false;
	}

	for (unsigned int i = 0; i < length && cur_state != -1; i++)
	{
		it = asoc.find(word[i]);
		if (it == asoc.end())
			cur_state = -1;
		else
			cur_state = dfa[cur_state][asoc[word[i]]];
	}

	if (cur_state == -1 || !mark.test(cur_state))
		return false;

	return true;
}

int main (int argc, char* argv[]) {

	build_DFA();
	char *word = new char[NMAX + 1];
	cin.getline(word, NMAX);

	if (evaluate(word))
		cout << "Word is accepted!\n";
	else
		cout << "Word is not accepted!\n";

	delete[] dfa;
	delete[] word;

	return 0;
}
