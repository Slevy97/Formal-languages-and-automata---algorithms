#include "pch.h"
#include "translator.h"
#include "translator_factory.h"
#include "translator_checker.h"
#include <iostream>

class Solution
{
public:
	static translator::TranslatorPtr BuildTranslator(const std::string& translatorConfigFileName)
	{
		translator::TranslatorPtr translator = nullptr;

		std::ifstream translatorConfigFile(translatorConfigFileName);

		if (translatorConfigFile.is_open())
		{
			translator = translator::TranslatorFactory::BuildFromData(translatorConfigFile);
			translatorConfigFile.close();
		}

		return translator;
	}

	static void TranslateInputs(translator::TranslatorPtr translator, const std::string& dataFileName)
	{
		std::ifstream dataFile(dataFileName);

		if (dataFile.is_open())
		{
			std::string input;

			while (getline(dataFile, input))
			{
				std::cout << "For input: " << input << "\nOutputs: ";
					
				auto outputs = translator->TranslateInput(input);

				if (outputs.empty())
					std::cout << "No output\n";
				else 
				{
					for (const auto& output : outputs)
						std::cout << output << " ";

					std::cout << "\n";
				}

				std::cout << "\n";
			}

			dataFile.close();
		}
	}
};

int main()
{
	auto translator = Solution::BuildTranslator("translator.in");

	if (translator::TranslatorChecker(translator).ContainsLambdaCycle())
		std::cout << "Lambda cycle detected !\n";
	else
		Solution::TranslateInputs(translator, "data.in");

	std::cin.get();
	return 0;
}