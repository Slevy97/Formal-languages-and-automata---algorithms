#include "pch.h"
#include "grammar/grammar_base.h"

BEGIN_GRAMMAR_NAMESPACE

bool IsSymbol(char ch)
{
	return std::isgraph(ch);
}

bool IsNonTerminal(char ch)
{
	return std::isupper(ch);
}

bool IsTerminal(char ch)
{
	return IsSymbol(ch) && !IsNonTerminal(ch);
}

bool IsLambda(char ch)
{
	return (ch == '_');
};

Symbols RemoveLambdas(const std::string& data)
{
	std::string newData{ data };

	newData.erase(
		std::remove_if(newData.begin(), newData.end(), [](char ch) {
			return IsLambda(ch) || !IsSymbol(ch);
		}),
		newData.end()
	);

	return newData;
}

std::set<Symbols> Concatenate(const std::set<Symbols>& firstSet, const std::set<Symbols>& secondSet)
{
	if (firstSet.empty() || secondSet.empty())
		return firstSet.empty() ? secondSet : firstSet;

	std::set<Symbols> finalSet;

	for (Symbols firstSymbols : firstSet)
		for (Symbols secondSymbols : secondSet)
			finalSet.insert(firstSymbols + secondSymbols);

	return finalSet;
}

BEGIN_CF_GRAMMAR_NAMESPACE

bool IsLeftSideOfProductionRule(const std::string& lhs)
{
	return (lhs.length() == 1) && IsNonTerminal(lhs[0]);
}

bool IsRightSideOfProductionRule(const std::string& rhs)
{
	if (rhs.length() > 0)
		return std::all_of(rhs.cbegin(), rhs.cend(), IsSymbol);

	return false;
}

END_CF_GRAMMAR_NAMESPACE

END_GRAMMAR_NAMESPACE