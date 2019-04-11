#ifndef GRAMMAR_FACTORY_H_INCLUDED
#define GRAMMAR_FACTORY_H_INCLUDED

#include "grammar/grammar.h"

BEGIN_GRAMMAR_NAMESPACE

class GrammarFactory final
{
public:
	// Factory methods
	static cf_grammar::CFGrammarPtr BuildCFGrammarFromData(const std::string& data);
	static cf_grammar::CFGrammarPtr BuildCFGrammarFromData(std::istream& stream);

	// non-instantiable class
	GrammarFactory() = delete;
	GrammarFactory(const GrammarFactory&) = delete;
	GrammarFactory& operator= (const GrammarFactory&) = delete;
};

END_GRAMMAR_NAMESPACE

#endif // !GRAMMAR_FACTORY_H_INCLUDED