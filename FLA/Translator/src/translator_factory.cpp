#include "pch.h"
#include "translator_factory.h"

BEGIN_TRANSLATOR_NAMESPACE

std::shared_ptr<Translator> TranslatorFactory::BuildFromData(const std::string& data)
{
	std::stringstream stream(data);
	return TranslatorFactory::BuildFromData(stream);
}

std::shared_ptr<Translator> TranslatorFactory::BuildFromData(std::istream& stream)
{
	stream.exceptions(std::istream::failbit | std::istream::badbit);

	try 
	{
		State startingState;
		uint32_t nrTransitions, nrFinalStates;

		std::vector<TransitionInfo> transitions;
		std::vector<State> finalStates;

		stream >> nrTransitions;

		for (uint32_t i = 1; i <= nrTransitions; ++i)
		{
			TransitionInfo transitionInfo;
			stream >> transitionInfo;

			transitions.push_back(std::move(transitionInfo));
		}
		
		stream >> startingState >> nrFinalStates;

		for (uint32_t i = 1; i <= nrFinalStates; ++i)
		{
			State finalState;
			stream >> finalState;
			
			finalStates.push_back(finalState);
		}

		return std::make_shared<Translator>(transitions, startingState, finalStates);
	}
	catch (...)
	{
		return nullptr;
	}
}

END_TRANSLATOR_NAMESPACE