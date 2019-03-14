#include "pch.h"
#include "translator_base.h"

BEGIN_TRANSLATOR_NAMESPACE

std::istream& operator>>(std::istream& stream, TransitionInfo& transitionInfo)
{
	stream >> transitionInfo.fromState >> transitionInfo.toState;

	stream >> transitionInfo.hasInput;

	if (transitionInfo.hasInput)
		stream >> transitionInfo.transitionInput;
	else
		transitionInfo.transitionInput = {};

	stream >> transitionInfo.hasOutput;

	if (transitionInfo.hasOutput)
		stream >> transitionInfo.transitionOutput;
	else
		transitionInfo.transitionOutput = {};

	return stream;
}

END_TRANSLATOR_NAMESPACE