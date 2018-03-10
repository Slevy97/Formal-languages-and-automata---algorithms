#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "grammar.h"

using namespace std;

void check_for_spaces(const string& filename) {

	fstream file(filename, ios::in);
	string buffer;
	string big_buffer = "";

	while (getline(file, buffer))
		big_buffer += (buffer + '\n');
	file.close();

	for (int i = 0; i < (signed)big_buffer.size(); i++)
		if (big_buffer[i] == ' ')
		{
			big_buffer.erase(big_buffer.begin() + i);
			i--;
		}

	file.open(filename, ios::out);
	file << big_buffer;
	file.close();
}

Grammar g;

int main() {

	srand(time(NULL));
	check_for_spaces("cnf.in");
	
	/** read grammar from file **/
	ifstream in("cnf.in");
	in >> g;
	in.close();
	/** **/
	try {
		
		g.convert_to_cnf();
		cout << g << "\n";
	}
	catch (exception *excep) {

		cerr << excep->what() << "\n";
	}
	catch (...) {
		
		cerr << "Unknown error !\n";
	}
	return 0;
}