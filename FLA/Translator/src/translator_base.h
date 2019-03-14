#ifndef TRANSLATOR_BASE_H_INCLUDED
#define TRANSLATOR_BASE_H_INCLUDED

#define BEGIN_TRANSLATOR_NAMESPACE namespace translator {
#define END_TRANSLATOR_NAMESPACE }

BEGIN_TRANSLATOR_NAMESPACE

using State = uint32_t;
using Letter = char;

using TransitionInput = std::string;
using TransitionOutput = std::string;

using TranslatorInput = std::string;
using TranslatorOutput = std::string;

struct TransitionInfo
{
	State fromState;
	State toState;

	bool hasInput, hasOutput;
	TransitionInput transitionInput;
	TransitionOutput transitionOutput;
};

struct InstantaneousDescription 
{
	InstantaneousDescription(State state, TranslatorInput translatorInput,
		TranslatorOutput translatorOutput)
		: state(state), 
		  translatorInput(translatorInput),
		  translatorOutput(translatorOutput) {}
	
	State state;
	TranslatorInput translatorInput;
	TranslatorOutput translatorOutput;
};

std::istream& operator>>(std::istream& stream, TransitionInfo& transitionInfo);

struct EndTransitionInfo
{
	EndTransitionInfo(State toState, TransitionOutput transitionOutput) 
		: toState(toState), transitionOutput(transitionOutput) {}

	State toState;
	TransitionOutput transitionOutput;
};

struct TransitionTrieVertex
{
	std::unordered_map<Letter, std::shared_ptr<TransitionTrieVertex>> edges;
	std::vector<EndTransitionInfo> endTransitionInfos;
};

using TransitionTrieVertexPtr = std::shared_ptr<TransitionTrieVertex>;

END_TRANSLATOR_NAMESPACE

#endif // !TRANSLATOR_BASE_H_INCLUDED