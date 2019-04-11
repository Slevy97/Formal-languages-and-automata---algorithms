#ifndef PARSER_BASE_H_INCLUDED
#define PARSER_BASE_H_INCLUDED

#include "grammar/grammar_base.h"

BEGIN_GRAMMAR_NAMESPACE

#define BEGIN_PARSER_NAMESPACE namespace parser {
#define END_PARSER_NAMESPACE }

BEGIN_PARSER_NAMESPACE

struct InstantaneousDescription
{
	InstantaneousDescription(const Word& toParse, 
							 const Symbols& parsingConfig);

	Word toParse;
	Symbols parsingConfig;
};

END_PARSER_NAMESPACE

END_GRAMMAR_NAMESPACE

#endif // !PARSER_BASE_H_INCLUDED