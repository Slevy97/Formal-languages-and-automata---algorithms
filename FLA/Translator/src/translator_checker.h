#ifndef TRANSLATOR_CHECKER_H_INCLUDED
#define TRANSLATOR_CHECKER_H_INCLUDED

#include "translator.h"

BEGIN_TRANSLATOR_NAMESPACE

class TranslatorChecker
{
public:
	TranslatorChecker(TranslatorPtr translator) : m_translator(translator) {}

	bool ContainsLambdaCycle();

private:
	bool CheckLambdaCycle(State);

	TranslatorPtr m_translator;
	std::unordered_map<State, bool> m_marked;
};

END_TRANSLATOR_NAMESPACE

#endif // !TRANSLATOR_CHECKER_H_INCLUDED
