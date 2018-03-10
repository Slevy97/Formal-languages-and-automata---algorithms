#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <stack>
#include <cstdlib>
#define NO_CNF_EXCEP -1

using namespace std;

vector<string> non_t;
vector<vector<string> > grammar;
stack<string> concat;
string ** matrix, word;

bool check_lhs(const string& to_check) {

	if (to_check.length() == 1 && to_check[0] >= 'A' && to_check[0] <= 'Z')
		return true;
	return false;
}

bool check_rhs(const string& to_check) {

	if (to_check.length() == 2 && to_check[0] >= 'A' && to_check[0] <= 'Z' && to_check[1] >= 'A' && to_check[1] <= 'Z')
		return true;
	if (to_check.length() == 1 && to_check[0] >= 'a' && to_check[0] <= 'z')
		return true;
	return false;
}

void read_grammar() {

	string buffer;
	ifstream in("cyk.in");
	while (getline(in, buffer)) {

		unsigned int indx = buffer.find("->");
		string substr = buffer.substr(0, indx);
		
		if (check_lhs(substr)) {

			non_t.push_back(substr);
			grammar.push_back(vector<string>());
			substr = buffer.substr(indx + 2);
	
			while (substr.length()) {

				indx = substr.find("|");
				if (indx != string::npos) {

					if (check_rhs(substr.substr(0, indx)))
						grammar[grammar.size() - 1].push_back(substr.substr(0, indx));
					else throw NO_CNF_EXCEP;
					
					substr = substr.substr(indx + 1, substr.length());
				}
				else
				{
					if (check_rhs(substr.substr(0, indx)))
						grammar[grammar.size() - 1].push_back(substr);
					else throw NO_CNF_EXCEP;

					substr = "";
				}
			}

		}
		else throw NO_CNF_EXCEP;
	}
	in.close();
}

string search_productions(const string& prod) {

	string found = "";
	for (unsigned int i = 0; i < grammar.size(); i++)
	{
		for (unsigned int j = 0; j < grammar[i].size(); j++)
			if (grammar[i][j] == prod)
				found += non_t[i];
	}
	return found;
}

void concat_nont(const string& first, const string& second) {

	string add = "";
	for (unsigned int i = 0; i < first.length(); i++)
		for (unsigned int j = 0; j < second.length(); j++)
			concat.push(add + first[i] + second[j]);
}

void alloc_mem() {

	matrix = new string*[word.length()];
	for (unsigned int i = 0; i < word.length(); i++)
		matrix[i] = new string[word.length()];
}

void free_mem() {

	for (unsigned int i = 0; i < word.length(); i++)
		delete[] matrix[i];
	delete[] matrix;
}

int main() {

	try {
	
		string repr, aux;
		read_grammar();
		cin >> word;
		alloc_mem();
		
		for (unsigned int i = 0; i < word.length(); i++)
			matrix[i][0] = search_productions(string(1, word[i]));

		for (unsigned int j = 1; j < word.length(); j++) {

			for (unsigned int i = 0; i + j < word.length(); i++) {

				repr = "";
				for (unsigned int k = 1; k <= j; k++) {

					concat_nont(matrix[i][k - 1], matrix[i + k][j - k]);

					while (!concat.empty()) {

						aux = search_productions(concat.top());
						for (unsigned int i = 0; i < aux.length(); i++)
							if (repr.find(aux[i]) == string::npos)
								repr += aux[i];

						concat.pop();
					}
				}
				matrix[i][j] = repr;
			}
		}

		if (matrix[0][word.length() - 1].find("S") != string::npos)
			cout << "Word accepted !\n";
		else
			cout << "Word rejected !\n";

		free_mem();
	
	} catch (int excep) {

		free_mem();
		if (excep == NO_CNF_EXCEP)
			cerr << "Input grammar is not in the CNF form !\n";
		else throw;

	}
	catch (...) {

		cerr << "Unknown error !\n";
		free_mem();
	}
	return 0;
}