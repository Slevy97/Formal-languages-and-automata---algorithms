#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "parser/parser_base.h"
#include "grammar/grammar.h"

BEGIN_GRAMMAR_NAMESPACE

BEGIN_PARSER_NAMESPACE

class LLParser
{
public:
	using ParsingLookaheads = std::unordered_map<
		/*
		 *	@brief
		 *	Lookahead
		 */
		Word,
		/*
		 *	@brief
		 *	(index of production rule, rhs of production rule)
		 */
		std::pair<size_t, Symbols>
	>;
	using ParsingTable = std::unordered_map<NonTerminal, ParsingLookaheads>;
	
	using ParsingResult = std::pair<
		/*
		 *	@brief
		 *	word accepted
		 */
		bool,
		/*
		 *	@brief
		 *	parsing output
		 */
		std::string
	>;

	LLParser(cf_grammar::CFGrammarPtr cfGrammar, size_t lookahead);
	
	/*
	 *	@brief
	 *	Builds m_ParsingTable needed for ParseWord method
	 *  OnFailure: sets m_IsStrongLLGrammar as false
	 */
	void BuildParsingTable();
	
	/*
	 *	@brief
	 *	Parses word using m_ParsingTable built previously
	 */
	ParsingResult ParseWord(const Word& word);

	/*
	 *	@brief
	 *	Getter for m_IsStrongLLGrammar
	 */
	bool IsStrongLLGrammar() const { return m_IsStrongLLGrammar; }
private:
	ParsingTable m_ParsingTable;
	size_t m_Lookahead;

	cf_grammar::CFGrammarPtr m_CFGrammar;
	bool m_IsStrongLLGrammar;
	
	std::unordered_map<
		/*
		 *	@brief
		 *	Every non-terminal symbol in the grammar
		 */
		NonTerminal,
		/*
		 *	@brief
		 *	Follow(K) for every k non-terminal symbol
		 */
		std::set<Word>> 
		/*
		 *	@brief
		 *	Uses Follow() method used in LLParser algorithm
		 *  Builds the set for every non-terminal in the grammar
		 */
		BuildFollowLookahead();

	/*
	 *	@brief
	 *	First() method used in LLParser algorithm
	 *  Return type: set of words that appear at the beginning
	 *  of any Symbols string derived from symbolsSet
	 */
	std::set<Word> FirstLookahead(const std::set<Symbols>& symbolsSet);
};

using LLParserPtr = std::shared_ptr<LLParser>;

inline LLParserPtr BuildLLParser(cf_grammar::CFGrammarPtr cfGrammar, size_t lookahead)
{ 
	return std::make_shared<LLParser>(cfGrammar, lookahead); 
}

END_PARSER_NAMESPACE

END_GRAMMAR_NAMESPACE

#endif // !PARSER_H_INCLUDED