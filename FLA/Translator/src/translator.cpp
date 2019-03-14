#include "pch.h"
#include "translator.h"
#include <iostream>

BEGIN_TRANSLATOR_NAMESPACE

Translator::Translator(const std::vector<TransitionInfo>& transitions, State startingState, const std::vector<State>& finalStates)
{
	// set starting state
	this->m_startingState = startingState;

	// add transitions
	for (const TransitionInfo& transitionInfo : transitions)
		this->AddTransition(transitionInfo);

	// set final states
	m_finalStates.insert(finalStates.begin(), finalStates.end());
}

void Translator::AddTransition(const TransitionInfo& transitionInfo)
{
	TransitionTrieVertexPtr trieVertex = nullptr;

	// initialize entry in m_transitions if not existing
	auto vertex_it = this->m_transitions.find(transitionInfo.fromState);

	if (vertex_it == this->m_transitions.end())
	{
		trieVertex = std::make_shared<TransitionTrieVertex>();
		this->m_transitions[transitionInfo.fromState] = trieVertex;
	}
	else trieVertex = vertex_it->second;

	// descend through the tree
	for (size_t indx = 0; indx < transitionInfo.transitionInput.length(); ++indx)
	{
		Letter letter = transitionInfo.transitionInput[indx];

		if (trieVertex->edges.find(letter) == trieVertex->edges.end())
			trieVertex->edges[letter] = std::make_shared<TransitionTrieVertex>();

		trieVertex = trieVertex->edges[letter];
	}

	// update the trie vertex
	trieVertex->endTransitionInfos.emplace_back(transitionInfo.toState, transitionInfo.transitionOutput);
}

std::vector<std::string> Translator::TranslateInput(const std::string& input)
{
	std::queue<InstantaneousDescription> instantDescriptions;
	std::vector<std::string> results = {};

	instantDescriptions.push({ this->m_startingState, input, {} });

	while (!instantDescriptions.empty())
	{
		// get current instantaneous description
		InstantaneousDescription instantDescription = instantDescriptions.front();

		/*std::cout << instantDescription.state << "-" << instantDescription.translatorInput << "-"
			<< instantDescription.translatorOutput << "\n";
*/
		// check for final state
		if (instantDescription.translatorInput.empty())
		{
			auto final_it = this->m_finalStates.find(instantDescription.state);

			if (final_it != this->m_finalStates.end())
				results.emplace_back(instantDescription.translatorOutput);
		}

		// generate new instantaneous descriptions
		auto vertex_it = this->m_transitions.find(instantDescription.state);

		if (vertex_it != this->m_transitions.end())
		{
			// get the trie for the transitions of the current state
			auto trieVertex = vertex_it->second;

			do
			{
				// create new descriptions based on the end points saved in this vertex
				for (const EndTransitionInfo& transitionInfo : trieVertex->endTransitionInfos)
				{
					instantDescriptions.push({ transitionInfo.toState, 
											   instantDescription.translatorInput,
											   instantDescription.translatorOutput + transitionInfo.transitionOutput
											});
				}

				// if there are any letters left in the input of the description
				// try to go one step down in the trie, if that configuration exists
				if (!instantDescription.translatorInput.empty())
				{
					Letter letter = instantDescription.translatorInput.front();
					instantDescription.translatorInput.erase(instantDescription.translatorInput.begin());

					auto edge_it = trieVertex->edges.find(letter);
					trieVertex = (edge_it != trieVertex->edges.end()) ? edge_it->second : nullptr;
				}
				else trieVertex = nullptr;

			} while (trieVertex);
		}
		
		instantDescriptions.pop();
	}

	return std::move(results);
}

END_TRANSLATOR_NAMESPACE