#include "pch.h"
#include "parser/parser.h"

BEGIN_GRAMMAR_NAMESPACE

BEGIN_PARSER_NAMESPACE

LLParser::LLParser(cf_grammar::CFGrammarPtr cfGrammar, size_t lookahead)
	: m_CFGrammar(cfGrammar)
{
	m_Lookahead = std::max<size_t>(lookahead, 1);
	m_IsStrongLLGrammar = false;
}

void LLParser::BuildParsingTable()
{
	if (!m_CFGrammar || m_CFGrammar->IsEmpty())
		return;

	const auto& followLookahead = BuildFollowLookahead();
	const auto& productionRulesWithIndx = m_CFGrammar->GetProductionRules();
	
	m_IsStrongLLGrammar = true;

	for (const auto& productionRuleWithIndx : productionRulesWithIndx)
	{
		size_t nrProductionRule = productionRuleWithIndx.first;
		const cf_grammar::ProductionRule& productionRule = productionRuleWithIndx.second;

		NonTerminal lhs = cf_grammar::GetLhs(productionRule);
		const Symbols& rhs = cf_grammar::GetRhs(productionRule);

		auto table_it = m_ParsingTable.find(lhs);

		if (table_it == m_ParsingTable.end())
		{
			decltype(m_ParsingTable)::value_type tableEntry(lhs, ParsingLookaheads());
			auto insertResult = m_ParsingTable.insert(std::move(tableEntry));

			table_it = insertResult.first;
		}

		const std::set<Word> lookaheads = FirstLookahead(Concatenate({ rhs }, followLookahead.at(lhs)));

		for (const Word& lookahead : lookaheads)
		{
			ParsingLookaheads& parsingLookaheads = table_it->second;

			auto lookaheads_it = parsingLookaheads.find(lookahead);

			if (lookaheads_it == parsingLookaheads.end())
			{
				parsingLookaheads[lookahead] = std::pair<size_t, Symbols>(nrProductionRule, rhs);
			}
			else 
			{
				m_IsStrongLLGrammar = false;
				m_ParsingTable.clear();
				break;
			}
		}
	}
}

LLParser::ParsingResult LLParser::ParseWord(const Word& word)
{
	if (m_IsStrongLLGrammar == false || m_ParsingTable.empty())
		return { false, {} };

	ParsingResult result = { true, {} };

	bool parsingFailed = false;
	InstantaneousDescription instantDesc{ RemoveLambdas(word), {m_CFGrammar->GetStartingSymbol()} };

	while (!parsingFailed)
	{
		Word& toParse = instantDesc.toParse;
		Symbols& parsingConfig = instantDesc.parsingConfig;

		if (parsingConfig.empty())
		{
			if (toParse.empty())
				break;
			else
				parsingFailed = true;
		}
		else
		{
			const Symbol parsingSymbol = parsingConfig.front();

			if (IsTerminal(parsingSymbol))
			{
				if (!toParse.empty() && toParse.front() == parsingSymbol)
				{
					toParse.erase(toParse.begin());
					parsingConfig.erase(parsingConfig.begin());
				}
				else
					parsingFailed = true;
			}
			else
			{
				auto table_it = m_ParsingTable.find(parsingSymbol);

				if (table_it != m_ParsingTable.end())
				{
					const ParsingLookaheads& parsingLookaheads = table_it->second;
					const Word& lookahead = toParse.substr(0, m_Lookahead);

					auto lookaheads_it = parsingLookaheads.find(lookahead);

					if (lookaheads_it != parsingLookaheads.end())
					{
						const std::pair<size_t, Symbols>& nextProductionInfo = lookaheads_it->second;

						parsingConfig.erase(parsingConfig.begin());
						parsingConfig = nextProductionInfo.second + parsingConfig;
						result.second += std::to_string(nextProductionInfo.first);
					}
					else
						parsingFailed = true;
				}
			}
		}	
	}

	if (parsingFailed)
	{
		result.first = false;
		result.second.clear();
	}

	return result;
}

std::unordered_map<NonTerminal, std::set<Word>> LLParser::BuildFollowLookahead()
{
	if (!m_CFGrammar || m_CFGrammar->IsEmpty())
		return {};

	std::unordered_map<NonTerminal, std::set<Word>> followLookahead = {};

	for (const auto& grammarData : m_CFGrammar->GetGrammarData())
	{
		NonTerminal nonTerminal = grammarData.first;
		followLookahead[nonTerminal] = {};
	}

	followLookahead[m_CFGrammar->GetStartingSymbol()].insert(Word());

	bool modifiedTable = true;
	const auto& productionRulesWithIndx = m_CFGrammar->GetProductionRules();

	while (modifiedTable)
	{
		modifiedTable = false;

		for (const auto& productionRuleWithIndx : productionRulesWithIndx)
		{
			const cf_grammar::ProductionRule& productionRule = productionRuleWithIndx.second;

			NonTerminal lhs = cf_grammar::GetLhs(productionRule);
			const Symbols& rhs = cf_grammar::GetRhs(productionRule);

			const auto& followLhs = followLookahead[lhs];

			for (size_t rhsIndex = 0; rhsIndex < rhs.length(); ++rhsIndex)
			{
				Symbol symbol = rhs[rhsIndex];

				if (IsNonTerminal(symbol))
				{
					std::set<Word> updateFollow = FirstLookahead(Concatenate({ rhs.substr(rhsIndex + 1) }, followLhs));
					auto& followSymbol = followLookahead[symbol];

					size_t initialSize = followSymbol.size();
					followSymbol.insert(updateFollow.cbegin(), updateFollow.cend());

					modifiedTable = (initialSize != followSymbol.size()) ? true : modifiedTable;
				}
			}
		}
	}
	return followLookahead;
}

std::set<Word> LLParser::FirstLookahead(const std::set<Symbols>& symbolsSet)
{
	if (!m_CFGrammar || m_CFGrammar->IsEmpty())
		return {};

	const auto& cfGrammarData = m_CFGrammar->GetGrammarData();

	std::queue<Symbols> resolve;
	std::set<Word> result;

	std::for_each(symbolsSet.begin(), symbolsSet.end(), [&resolve](const Symbols& symbols) {
		resolve.push(RemoveLambdas(symbols));
	});

	while (!resolve.empty())
	{
		const Symbols& symbols = resolve.front();

		auto symbols_it = std::find_if(
			symbols.cbegin(),
			symbols.cend(),
			IsNonTerminal
		);

		size_t firstNonTerminalIndx = std::distance(symbols.cbegin(), symbols_it);

		if (firstNonTerminalIndx >= m_Lookahead || symbols_it == symbols.cend())
		{
			result.insert(symbols.substr(0, m_Lookahead));
		}
		else
		{
			NonTerminal nonTerminal = symbols[firstNonTerminalIndx];
			auto rhs_it = cfGrammarData.find(nonTerminal);

			if (rhs_it != cfGrammarData.end())
			{
				for (const std::pair<size_t, Symbols>& productionRhs : rhs_it->second)
				{
					resolve.push(
						symbols.substr(0, firstNonTerminalIndx) +
						productionRhs.second +
						symbols.substr(firstNonTerminalIndx + 1)
					);
				}
			}
		}
		resolve.pop();
	}
	return result;
}

END_PARSER_NAMESPACE

END_GRAMMAR_NAMESPACE