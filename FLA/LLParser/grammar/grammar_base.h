#ifndef GRAMMAR_BASE_H_INCLUDED
#define GRAMMAR_BASE_H_INCLUDED

#define BEGIN_GRAMMAR_NAMESPACE namespace grammar {
#define END_GRAMMAR_NAMESPACE }

BEGIN_GRAMMAR_NAMESPACE

using Symbol = char;
using NonTerminal = Symbol;
using Terminal = Symbol;

constexpr Symbol UNDEFINED_SYMBOL = CHAR_MAX;

using Symbols = std::basic_string<Symbol>;
using Word = std::basic_string<Terminal>;

// Utilities
bool IsSymbol(char ch);
bool IsNonTerminal(char ch);
bool IsTerminal(char ch);
bool IsLambda(char ch);

Symbols RemoveLambdas(const std::string& data);
std::set<Symbols> Concatenate(const std::set<Symbols>& firstSet, const std::set<Symbols>& secondSet);

#define BEGIN_CF_GRAMMAR_NAMESPACE namespace cf_grammar {
#define END_CF_GRAMMAR_NAMESPACE }

BEGIN_CF_GRAMMAR_NAMESPACE

using ProductionRule = std::pair<NonTerminal, Symbols>;

inline NonTerminal GetLhs(const ProductionRule& productionRule) { return productionRule.first; }
inline const Symbols& GetRhs(const ProductionRule& productionRule) { return productionRule.second; }

bool IsLeftSideOfProductionRule(const std::string& lhs);
bool IsRightSideOfProductionRule(const std::string& rhs);

END_CF_GRAMMAR_NAMESPACE

END_GRAMMAR_NAMESPACE

#endif // !GRAMMAR_BASE_H_INCLUDED