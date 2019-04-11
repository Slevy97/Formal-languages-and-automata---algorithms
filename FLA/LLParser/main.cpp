#include "pch.h"
#include "grammar/grammar.h"
#include "grammar/grammar_factory.h"
#include "parser/parser.h"

class Settings
{
public:
	using SettingKey = std::string;
	using SettingValue = std::string;

	Settings(int argc, char* argv[])
	{
		std::string buffer;

		for (int argIndex = 1; argIndex < argc; ++argIndex)
		{
			buffer = argv[argIndex];

			size_t indx = buffer.find("=");

			SettingKey settingKey = buffer.substr(0, indx);
			SettingValue settingValue = buffer.substr(indx + 1);

			m_Settings.emplace(settingKey, settingValue);
		}
	}

	const SettingValue& GetSettingValue(const SettingKey& settingKey) const
	{
		return m_Settings.at(settingKey);
	}

private:
	std::unordered_map <SettingKey, SettingValue> m_Settings;
};

class Solution
{
public:
	static grammar::cf_grammar::CFGrammarPtr BuildCFGrammar(const std::string& grammarFileName)
	{
		grammar::cf_grammar::CFGrammarPtr cfGrammar = nullptr;

		std::ifstream grammarFile(grammarFileName);

		if (grammarFile.is_open())
		{
			cfGrammar = grammar::GrammarFactory::BuildCFGrammarFromData(grammarFile);
			grammarFile.close();
		}

		return cfGrammar;
	}

	static void ParseInputs(grammar::parser::LLParserPtr llParser, const std::string& grammarInputFileName)
	{
		std::ifstream grammarInputFile(grammarInputFileName);
		std::string buffer;

		if (grammarInputFile.is_open())
		{
			while (getline(grammarInputFile, buffer))
			{
				const auto& parsingResult = llParser->ParseWord(buffer);

				if (parsingResult.first)
				{
					std::cout << "Accepted ! Production rules used are: " << parsingResult.second << "\n";
				}
				else
				{
					std::cout << "Not accepted !\n";
				}
			}
			
			grammarInputFile.close();
		}
	}
};

int main(int argc, char* argv[])
{
	const Settings settings(argc, argv);
	
	Settings::SettingValue grammarFileName{};
	Settings::SettingValue grammarInputFileName{};
	size_t lookahead{};

	try
	{
		grammarFileName = settings.GetSettingValue("--grammar");
		grammarInputFileName = settings.GetSettingValue("--grammar-input");
		
		const Settings::SettingValue& parserLookahead = settings.GetSettingValue("--lookahead");
		lookahead = std::stoi(parserLookahead);
	}
	catch (...)
	{
		std::cerr << "Error: Invalid settings !\n";
		return 1;
	}

	auto cfGrammar = Solution::BuildCFGrammar(grammarFileName);

	if (cfGrammar)
	{
		auto llParser = grammar::parser::BuildLLParser(cfGrammar, lookahead);

		llParser->BuildParsingTable();

		if (llParser->IsStrongLLGrammar())
		{
			Solution::ParseInputs(llParser, grammarInputFileName);
		}
		else
		{
			std::cout << "Grammar is not LL(" << lookahead << ") strong !\n";
		}
	}
	else
	{
		std::cout << "Grammar is empty !\n";
	}

	std::cin.get();
	return 0;
}