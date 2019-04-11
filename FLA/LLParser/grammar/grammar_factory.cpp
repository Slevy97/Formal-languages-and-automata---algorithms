#include "pch.h"
#include "grammar/grammar_factory.h"

BEGIN_GRAMMAR_NAMESPACE

cf_grammar::CFGrammarPtr GrammarFactory::BuildCFGrammarFromData(const std::string& data)
{
	std::stringstream stream(data);
	return GrammarFactory::BuildCFGrammarFromData(stream);
}

cf_grammar::CFGrammarPtr GrammarFactory::BuildCFGrammarFromData(std::istream& stream)
{
	std::string buffer;
	cf_grammar::CFGrammarPtr cfGrammar = cf_grammar::BuildCFGrammar();

	bool firstProduction = true;

	while (std::getline(stream, buffer))
	{
		buffer.erase(		
			std::remove_if(buffer.begin(), buffer.end(), std::isspace),
			buffer.end()
		);
		
		size_t indx = buffer.find("->");
		std::string lhs = buffer.substr(0, indx);

		if (cf_grammar::IsLeftSideOfProductionRule(lhs)) 
		{
			NonTerminal nonTerminal = lhs[0];
			buffer = buffer.substr(indx + 2);
		
			while (buffer.length())
			{
				indx = buffer.find("|");
				std::string rhs = buffer.substr(0, indx);

				if (cf_grammar::IsRightSideOfProductionRule(rhs))
				{
					cfGrammar->AddProductionRule({ nonTerminal, rhs });

					if (firstProduction)
					{
						cfGrammar->SetStartingSymbol(nonTerminal);
						firstProduction = false;
					}
				}

				buffer = (indx != std::string::npos) ? buffer.substr(indx + 1) : std::string();
			}
		}
	}

	return cfGrammar;
}

END_GRAMMAR_NAMESPACE