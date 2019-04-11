#ifndef GRAMMAR_H_INCLUDED
#define GRAMMAR_H_INCLUDED

#include "grammar/grammar_base.h"

BEGIN_GRAMMAR_NAMESPACE

BEGIN_CF_GRAMMAR_NAMESPACE

class CFGrammar
{
public:
	using CFGrammarData = std::unordered_map<
		/*
		 *	@brief
		 *	lhs of production rule
		 */
		NonTerminal,
		/*
		 *	@brief
		 *	[(index of production rule, rhs of production rule)]
		 */
		std::vector<std::pair<size_t, Symbols>> 
	>;

	CFGrammar();
	
	/*
	 *	@brief
	 *	Checks if there are any production rules added
	 */
	bool IsEmpty() const { return m_CFGrammar.empty(); }

	/*
	 *	@brief
	 *	Getter/setter for m_StartingSymbol
	 */
	NonTerminal GetStartingSymbol() const;
	void SetStartingSymbol(NonTerminal startingSymbol);
	
	/*
	 *	@brief
	 *	Adds new production rule in grammar
	 */
	void AddProductionRule(const ProductionRule& productionRule);

	/*
	 *	@brief
	 *	Return type: [(index of production rule, production rule)]
	 */
	std::vector<std::pair<size_t, ProductionRule>> GetProductionRules() const;

	/*
	 *	@brief
	 *	Return type: Const reference of data inside
	 */
	const CFGrammarData& GetGrammarData() const;

private:
	CFGrammarData m_CFGrammar;
	NonTerminal m_StartingSymbol;
	size_t m_NrProductionRules;

	CFGrammar(const CFGrammar&) = delete;
	CFGrammar& operator=(const CFGrammar&) = delete;
};

using CFGrammarPtr = std::shared_ptr<CFGrammar>;
inline CFGrammarPtr BuildCFGrammar() { return std::make_shared<CFGrammar>(); }

END_CF_GRAMMAR_NAMESPACE

END_GRAMMAR_NAMESPACE

#endif // !GRAMMAR_H_INCLUDED