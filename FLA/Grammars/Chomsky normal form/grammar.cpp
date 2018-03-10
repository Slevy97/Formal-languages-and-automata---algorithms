#include "grammar.h"
#include <algorithm>
#include <queue>
#define NMAX 101

bitset<NMAX> reachable;
bitset<NMAX> terminating;
bitset<NMAX> nullable;
queue<char> to_del;
queue<string> to_add;
queue<pair<char, string> > to_add_prod;
queue<pair<char, string> > to_del_prod;

Grammar::Grammar() {
	
	this->accepts_null = false;
}

Grammar::~Grammar() {}

bool Grammar::nonterminal(char symbol) {

	if ((symbol >= 'A' && symbol <= 'Z'))
		return true;
	return false;
}

bool Grammar::terminal(char symbol) {

	if ((symbol >= 'a' && symbol <= 'z') || (symbol == '_'))
		return true;
	return false;
}

bool Grammar::check_lhs(const string& to_check) {

	if (to_check.length() == 1 && nonterminal(to_check[0]))
		return true;
	return false;
}

bool Grammar::check_rhs(const string& to_check) {

	if (to_check.size() > 0) {

		for (unsigned int i = 0; i < to_check.size(); i++)
			if (!Grammar::terminal(to_check[i]) && !Grammar::nonterminal(to_check[i]))
				return false;
		return true;
	}

	return false;
}

char Grammar::get_startsymbol() const {

	return this->start_symbol;
}

void Grammar::set_startsymbol(char symbol) {

	if (asoc.find(symbol) != asoc.end()) {

		this->start_symbol = symbol;
	}
}

void Grammar::dfs_grammar(char symbol) {

	char sym_where;
	map<char, int>::iterator it = asoc.find(symbol);
	if (it != asoc.end()) {
	
		int index = asoc[symbol];
		reachable.set(index);
		for (unsigned int i = 0; i < grammar[index].size(); i++) {

			for (unsigned int j = 0; j < grammar[index][i].size(); j++) {

				sym_where = grammar[index][i][j];
				if (Grammar::terminal(sym_where))
				{
					terminating.set(asoc[symbol]);
					if (sym_where == '_')
						nullable.set(asoc[symbol]);
				}

				if (Grammar::nonterminal(sym_where) && asoc.find(sym_where) != asoc.end())
				{
					if (!reachable.test(asoc[sym_where]))
					{
						dfs_grammar(sym_where);
						if (terminating.test(asoc[sym_where]) == true)
							terminating.set(asoc[symbol]);
					}
				}
			}
			unsigned int nr = 0;
			for (unsigned int j = 0; j < grammar[index][i].size(); j++) {

				sym_where = grammar[index][i][j];
				if (Grammar::nonterminal(sym_where) && asoc.find(sym_where) != asoc.end())
				{
					if (nullable.test(asoc[sym_where]) == true)
						nr++;
				}
			}
			if (nr == grammar[index][i].length())
				nullable.set(asoc[symbol], true);
		}
	}
}

char Grammar::get_free_nont() {

	vector<char> aux;
	for (unsigned int i = 0; i < ALPHA_SIZE; i++)
		if (alphabet.test(i) == false)
			aux.push_back(i);
	if (aux.size() > 0)
	{
		random_shuffle(aux.begin(), aux.end());
		return *aux.begin() + 'A';
	}
	return char(UNDEF);
}

string Grammar::eliminate_null(string obj) {

	if (obj.size() > 1)
	{
		for (int i = 0; i < (signed)obj.size(); i++)
			if (obj[i] == '_') {

				obj.erase(obj.begin() + i);
				i--;
			}
	}
	return obj;
}

void Grammar::generate(const string& obj, char symbol) {

	vector<int> pos;
	string aux;
	for (unsigned int i = 0; i < obj.size(); i++)
		if (obj[i] == symbol)
			pos.push_back(i);
	for (unsigned int i = 1; i <= (1U << pos.size()) - 1; i++)
	{
		aux = obj;
		for (unsigned int j = 0; (1U << j) <= i; j++)
		{
			if (i & (1 << j))
				aux[pos[(1 << j) - 1]] = '_';
		}
		to_add.push(aux);
	}
}


void Grammar::erase_redundant() {

	char symbol;
	dfs_grammar(get_startsymbol());
	map<char, int>::iterator it;
	for (int i = 0; i < (signed)non_t.size(); i++)
		if (reachable.test(i) == false || terminating.test(i) == false) {

			this->erase_nont(non_t[i]);
			i--;
		}
	for (unsigned int i = 0; i < non_t.size(); i++)
		for (unsigned int j = 0; j < grammar[i].size(); j++)
			for (unsigned int k = 0; k < grammar[i][j].size(); k++)
			{
				symbol = grammar[i][j][k];
				it = asoc.find(symbol);
				if (Grammar::nonterminal(symbol) && it == asoc.end())
					to_del.push(symbol);
			}
	while (!to_del.empty())
	{
		this->erase_nont(to_del.front());
		to_del.pop();
	}
}

void Grammar::erase_nullprod(char symbol) {

	for (unsigned int i = 0; i < non_t.size(); i++)
		for (unsigned int j = 0; j < grammar[i].size(); j++)
			if (grammar[i][j].find(symbol) != string::npos)
			{
				generate(grammar[i][j], symbol);
				while (!to_add.empty()) {

					this->add_production(non_t[i], Grammar::eliminate_null(to_add.front()));
					to_add.pop();
				}
			}
}

void Grammar::erase_nullprod() {

	this->dfs_grammar(this->get_startsymbol());
	for (int i = 0; i < (signed)non_t.size(); i++)
		if (nullable.test(i) == true)
			erase_nullprod(non_t[i]);
	
	for (int i = 0; i < (signed)non_t.size(); i++)
		if (nullable.test(i) == true)
		{
			if (non_t[i] == get_startsymbol())
				this->accepts_null = true;
			this->erase_production(non_t[i], "_");
		}
}

void Grammar::move_prod(char symbol1, char symbol2) {

	if (symbol1 != symbol2) {
	
		bool ok1 = false, ok2 = false;
		if (Grammar::nonterminal(symbol1) && asoc.find(symbol1) != asoc.end())
			ok1 = true;
		if (Grammar::nonterminal(symbol2) && asoc.find(symbol2) != asoc.end())
			ok2 = true;
		if (ok1 && ok2) {

			unsigned int index = asoc[symbol2];
			for (unsigned int i = 0; i < grammar[index].size(); i++)
			{
				if (Grammar::check_lhs(grammar[index][i]) == true)
				{
					if (symbol2 != grammar[index][i][0])
						move_prod(symbol1, grammar[index][i][0]);
				}
				else
					to_add_prod.push(make_pair(symbol1, grammar[index][i]));
			}
		}
	}
}

void Grammar::erase_unitprod() {

	pair<char, string> elem;
	for (unsigned int i = 0; i < non_t.size(); i++)
		for (unsigned int j = 0; j < grammar[i].size(); j++)
			if (Grammar::check_lhs(grammar[i][j]) == true) {

				move_prod(non_t[i], grammar[i][j][0]);
				to_del_prod.push(make_pair(non_t[i], grammar[i][j]));
			}
	while (!to_add_prod.empty()) {

		elem = to_add_prod.front();
		this->add_production(elem.first, elem.second);
		to_add_prod.pop();
	}
	while (!to_del_prod.empty()) {

		elem = to_del_prod.front();
		this->erase_production(elem.first, elem.second);
		to_del_prod.pop();
	}
}

map<string, char> asoc_long;

void Grammar::replace_longprod() {

	bool ok_long = false;

	while (ok_long == false) {

		ok_long = true;

		string aux;
		string::iterator it_b, it_e;
		pair<char, string> elem;
		char nont;
		for (unsigned int i = 0; i < non_t.size(); i++)
			for (unsigned int j = 0; j < grammar[i].size(); j++)
				if (grammar[i][j].size() > 2) {

					aux = grammar[i][j].substr(1);
					it_b = grammar[i][j].begin() + 1;
					it_e = grammar[i][j].end();
					ok_long = false;

					if (asoc_long.find(aux) == asoc_long.end()) {

						nont = this->get_free_nont();
						if (nont != char(UNDEF)) {

							this->alphabet.set(nont - 'A', true);
							grammar[i][j].replace(it_b, it_e, string(1, nont));
							to_add_prod.push(make_pair(nont, aux));
							asoc_long[aux] = nont;
						}
						else throw new GrammarException();
					}
					else
					{
						nont = asoc_long[aux];
						grammar[i][j].replace(it_b, it_e, string(1, nont));
						to_add_prod.push(make_pair(nont, aux));
					}
				}

		while (!to_add_prod.empty()) {

			elem = to_add_prod.front();
			this->add_production(elem.first, elem.second);
			to_add_prod.pop();
		}
	}
	asoc_long.clear();
}

void Grammar::replace_combined() {

	string aux;
	string::iterator it_b, it_e;
	pair<char, string> elem;
	char nont;
	for (unsigned int i = 0; i < non_t.size(); i++)
		for (unsigned int j = 0; j < grammar[i].size(); j++)
			if (grammar[i][j].size() == 2) {

				if (Grammar::terminal(grammar[i][j][0])) {

					aux = grammar[i][j][0];
					it_b = grammar[i][j].begin();
					it_e = grammar[i][j].begin() + 1;

					if (asoc_long.find(aux) == asoc_long.end()) {

						nont = this->get_free_nont();
						if (nont != char(UNDEF)) {

							this->alphabet.set(nont - 'A', true);
							grammar[i][j].replace(it_b, it_e, string(1, nont));
							to_add_prod.push(make_pair(nont, aux));
							asoc_long[aux] = nont;
						}
						else throw new GrammarException();
					}
					else
					{
						nont = asoc_long[aux];
						grammar[i][j].replace(it_b, it_e, string(1, nont));
						to_add_prod.push(make_pair(nont, aux));
					}
				}

				if (Grammar::terminal(grammar[i][j][1])) {

					aux = grammar[i][j][1];
					it_b = grammar[i][j].begin() + 1;
					it_e = grammar[i][j].end();

					if (asoc_long.find(aux) == asoc_long.end()) {

						nont = this->get_free_nont();
						if (nont != char(UNDEF)) {

							this->alphabet.set(nont - 'A', true);
							grammar[i][j].replace(it_b, it_e, string(1, nont));
							to_add_prod.push(make_pair(nont, aux));
							asoc_long[aux] = nont;
						}
						else throw new GrammarException();
					}
					else
					{
						nont = asoc_long[aux];
						grammar[i][j].replace(it_b, it_e, string(1, nont));
						to_add_prod.push(make_pair(nont, aux));
					}
				}
			}

	while (!to_add_prod.empty()) {

		elem = to_add_prod.front();
		this->add_production(elem.first, elem.second);
		to_add_prod.pop();
	}
	asoc_long.clear();
}

void Grammar::convert_to_cnf() {

	this->erase_redundant();
	this->erase_nullprod();
	this->erase_redundant();
	this->erase_unitprod();
	this->erase_redundant();
	this->replace_longprod();
	this->replace_combined();
}

void Grammar::erase_nont(char symbol) {

	string aux;
	if (Grammar::nonterminal(symbol)) {

		for (unsigned int i = 0; i < non_t.size(); i++)
		{
			for (int j = 0; j < (signed)grammar[i].size(); j++)
			{
				aux = grammar[i][j];

				for (int k = 0; k < (signed)aux.size(); k++)
				{
					if (aux[k] == symbol)
					{
						aux.erase(aux.begin() + k);
						k--;
					}
				}
				if (grammar[i][j] != aux) {

					grammar[i].erase(grammar[i].begin() + j);
					j--;
					to_add_prod.push(make_pair(non_t[i], aux));
				}
			}
		}

		while (!to_add_prod.empty()) {

			pair<char, string> elem = to_add_prod.front();
			this->add_production(elem.first, elem.second);
			to_add_prod.pop();
		}

		for (int i = 0; i < (signed)non_t.size(); i++)
			if (non_t[i] == symbol)
			{
				non_t.erase(non_t.begin() + i);
				grammar.erase(grammar.begin() + i);
				i--;
			}
		map<char, int>::iterator it = asoc.find(symbol);
		if (it != asoc.end()) {
			asoc.erase(it);
		}
		for (unsigned int i = 0; i < non_t.size(); i++)
			asoc[non_t[i]] = i;

		if (symbol == get_startsymbol()) {

			if (non_t.size() > 0)
				set_startsymbol(non_t[0]);
		}

		for (unsigned int i = 0; i < non_t.size(); i++)
			if (grammar[i].size() == 0)
				this->erase_nont(non_t[i]);

		alphabet.set(symbol - 'A', false);
	}
}

void Grammar::add_production(char symbol, const string& prod) {
	
	vector<char>::iterator it;
	vector<string>::iterator it2;
	if (nonterminal(symbol) && check_rhs(prod)) {

		// add the production
		it = find(non_t.begin(), non_t.end(), symbol);
		if (it == non_t.end()) {
			
			non_t.push_back(symbol);
			alphabet.set(symbol - 'A', true);
			grammar.push_back(vector<string>());
			grammar[grammar.size() - 1].push_back(Grammar::eliminate_null(prod));

			asoc[symbol] = non_t.size() - 1;
			if (grammar.size() == 1)
				this->set_startsymbol(symbol);

		} else {

			unsigned int where = it - non_t.begin();
			it2 = find(grammar[where].begin(), grammar[where].end(), Grammar::eliminate_null(prod));
			if (it2 == grammar[where].end())
				grammar[where].push_back(Grammar::eliminate_null(prod));
		}
	}
}

void Grammar::erase_production(char symbol, const string& prod) {

	vector<char>::iterator it;
	vector<string>::iterator it2;
	if (nonterminal(symbol) && check_rhs(prod)) {

		// erase the production
		it = find(non_t.begin(), non_t.end(), symbol);
		if (it != non_t.end()) {

			unsigned int where = it - non_t.begin();
			it2 = find(grammar[where].begin(), grammar[where].end(), prod);
			if (it2 != grammar[where].end())
				grammar[where].erase(it2);

			if (grammar[asoc[symbol]].size() == 0)
				this->erase_nont(symbol);
		}
	}
}

istream& operator>> (istream& in, Grammar& obj) {

	string buffer;
	char symbol;
	while (getline(in, buffer)) {

		unsigned int indx = buffer.find("->");
		string substr = buffer.substr(0, indx);

		if (Grammar::check_lhs(substr)) {

			symbol = substr[0];
			substr = buffer.substr(indx + 2);

			while (substr.length()) {

				indx = substr.find("|");
				if (indx != string::npos) {

					if (Grammar::check_rhs(substr.substr(0, indx)))
						obj.add_production(symbol, substr.substr(0, indx));
					substr = substr.substr(indx + 1, substr.length());
				}
				else
				{
					if (Grammar::check_rhs(substr.substr(0, indx)))
						obj.add_production(symbol, substr.substr(0, indx));
					substr = "";
				}
			}
		}
	}
	return in;
}

ostream& operator<< (ostream& stream, Grammar& obj) {

	if (obj.accepts_null == true)
	{
		stream << "S1 -> _";
		if (obj.non_t.size() > 0)
			stream << " | " << obj.get_startsymbol() << "\n";
	}
	for (unsigned int i = 0; i < obj.non_t.size(); i++) {

		if (obj.grammar[i].size()) {
			stream << obj.non_t[i] << " -> ";
			for (unsigned int j = 0; j < obj.grammar[i].size() - 1; j++)
				stream << obj.grammar[i][j] << " | ";
			stream << obj.grammar[i][obj.grammar[i].size() - 1] << "\n";
		}
	}
	return stream;
}
