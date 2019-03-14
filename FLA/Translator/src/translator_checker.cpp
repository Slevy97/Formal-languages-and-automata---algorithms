#include "pch.h"
#include "translator_checker.h"

BEGIN_TRANSLATOR_NAMESPACE

bool TranslatorChecker::ContainsLambdaCycle()
{
	auto reset_marked = [this]() {

		this->m_marked.clear();

		for (const auto& entry : this->m_translator->m_transitions)
			this->m_marked[entry.first] = false;
	};

	bool lambdaCycle = false;

	for (const auto& entry : this->m_translator->m_transitions)
	{
		reset_marked();
		lambdaCycle |= this->CheckLambdaCycle(entry.first);
	}

	return lambdaCycle;
}

bool TranslatorChecker::CheckLambdaCycle(State state)
{
	auto vertex_it = this->m_translator->m_transitions.find(state);

	if (vertex_it != this->m_translator->m_transitions.end())
	{
		this->m_marked[state] = true;
		
		TransitionTrieVertexPtr trieVertex = vertex_it->second;
		bool lambdaCycle = false;

		for (const EndTransitionInfo& transitionInfo : trieVertex->endTransitionInfos)
		{
			if (!this->m_marked[transitionInfo.toState])
				lambdaCycle |= this->CheckLambdaCycle(transitionInfo.toState);
			else 
				lambdaCycle = true;
		}

		return lambdaCycle;
	}
	
	return false;
}

END_TRANSLATOR_NAMESPACE