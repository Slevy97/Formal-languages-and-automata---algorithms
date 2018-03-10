#ifndef GRAMMAR_H_INCLUDED
#define GRAMMAR_H_INCLUDED
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <bitset>
#include <exception>
#define ALPHA_SIZE 26
#define UNDEF -1

using namespace std;

class Grammar {

	private:
		/** Grammar data **/
		char start_symbol;
		vector<char> non_t;
		vector<vector<string> > grammar;
		map<char, int> asoc;
		bitset<ALPHA_SIZE> alphabet;
		bool accepts_null;
		
		/** Utility functions **/
		void dfs_grammar(char);
		char get_free_nont();
		void erase_nullprod(char);
		void generate(const string&, char);
		void move_prod(char, char);

	public:
		/** Grammar constructors - destructor - assignment operator **/
		Grammar();
		Grammar(const Grammar&) = default;
		~Grammar();
		Grammar& operator= (const Grammar&) = default;

		/** Grammar methods **/
		void add_production(char, const string&);
		void erase_production(char, const string&);
		void erase_nont(char);
		char get_startsymbol() const;
		void set_startsymbol(char);

		/** <Grammar conversion to CNF> functions **/
		void erase_redundant();
		void erase_nullprod();
		void erase_unitprod();
		void replace_longprod();
		void replace_combined();
		void convert_to_cnf();

		/** Grammar functions + Friend functions **/
		friend istream& operator>> (istream&, Grammar&);
		friend ostream& operator<< (ostream&, Grammar&);
		static bool check_lhs(const string&);
		static bool check_rhs(const string&);
		static bool terminal(char);
		static bool nonterminal(char);
		static string eliminate_null(string);
};

class GrammarException : public std::exception {

	string excep_msg;
public:
	GrammarException() {
		excep_msg = "Error ! Not enought non-terminal letter symbols exists !!\n";
	}
	const char* what() const noexcept {

		return excep_msg.c_str();
	}
};

#endif // !GRAMMAR_H_INCLUDED