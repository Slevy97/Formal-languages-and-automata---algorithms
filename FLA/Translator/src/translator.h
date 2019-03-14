#ifndef TRANSLATOR_H_INCLUDED
#define TRANSLATOR_H_INCLUDED

#include "translator_base.h"

BEGIN_TRANSLATOR_NAMESPACE

class TranslatorChecker;

class Translator
{
public:
	Translator(const std::vector<TransitionInfo>& transitions,
			   State startingState,
			   const std::vector<State>& finalStates);

	std::vector<std::string> TranslateInput(const std::string& input);

private:
	Translator(const Translator&) = delete;
	Translator& operator=(const Translator&) = delete;

	void AddTransition(const TransitionInfo& transitionInfo);

	State m_startingState;
	std::unordered_set<State> m_finalStates;
	std::unordered_map<State, TransitionTrieVertexPtr> m_transitions;
	
	friend class TranslatorChecker;
};

using TranslatorPtr = std::shared_ptr<Translator>;

END_TRANSLATOR_NAMESPACE

#endif // !TRANSLATOR_H_INCLUDED