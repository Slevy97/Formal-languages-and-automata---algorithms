#include "pch.h"
#include "grammar/grammar.h"

BEGIN_GRAMMAR_NAMESPACE

BEGIN_CF_GRAMMAR_NAMESPACE

CFGrammar::CFGrammar()
	: m_StartingSymbol(UNDEFINED_SYMBOL),
	  m_NrProductionRules(0)
{
}

NonTerminal CFGrammar::GetStartingSymbol() const
{
	return m_StartingSymbol;
}

void CFGrammar::SetStartingSymbol(NonTerminal startingSymbol)
{
	auto rhs_it = m_CFGrammar.find(startingSymbol);

	if (rhs_it != m_CFGrammar.end())
	{
		m_StartingSymbol = startingSymbol;
	}
}

void CFGrammar::AddProductionRule(const ProductionRule& productionRule)
{
	NonTerminal nonTerminal = productionRule.first;
	Symbols rhs = RemoveLambdas(productionRule.second);

	auto rhs_it = m_CFGrammar.find(nonTerminal);
	
	if (rhs_it == m_CFGrammar.end())
	{
		decltype(m_CFGrammar)::value_type grammarEntry(nonTerminal, {});
		auto insertResult = m_CFGrammar.insert(std::move(grammarEntry));
		
		rhs_it = insertResult.first;
	}

	rhs_it->second.emplace_back(++m_NrProductionRules, rhs);
}

std::vector<std::pair<size_t, ProductionRule>> CFGrammar::GetProductionRules() const
{
	std::vector<std::pair<size_t, ProductionRule>> productionRules;

	for (auto grammar_it = m_CFGrammar.cbegin(); grammar_it != m_CFGrammar.cend(); ++grammar_it)
	{
		NonTerminal nonTerminal = grammar_it->first;

		for (const std::pair<size_t, Symbols> productionRhs : grammar_it->second)
		{
			size_t nrProductionRule = productionRhs.first;
			const Symbols& rhs = productionRhs.second;

			productionRules.emplace_back(nrProductionRule, ProductionRule(nonTerminal, rhs));
		}
	}

	return productionRules;
}

const CFGrammar::CFGrammarData& CFGrammar::GetGrammarData() const
{
	return m_CFGrammar;
}

END_CF_GRAMMAR_NAMESPACE

END_GRAMMAR_NAMESPACE