#include "pch.h"
#include "parser/parser_base.h"

BEGIN_GRAMMAR_NAMESPACE

BEGIN_PARSER_NAMESPACE

InstantaneousDescription::InstantaneousDescription(
	const Word& toParse,
	const Symbols& parsingConfig)
{
	this->toParse = toParse;
	this->parsingConfig = parsingConfig;
}

END_PARSER_NAMESPACE

END_GRAMMAR_NAMESPACE