#ifndef TRANSLATOR_FACTORY_H_INCLUDED
#define TRANSLATOR_FACTORY_H_INCLUDED

#include "translator.h"

BEGIN_TRANSLATOR_NAMESPACE

class TranslatorFactory final
{
public:
	// Factory methods
	static std::shared_ptr<Translator> BuildFromData(const std::string& data);
	static std::shared_ptr<Translator> BuildFromData(std::istream& stream);
	
	// non-instantiable class
	TranslatorFactory() = delete;
	TranslatorFactory(const TranslatorFactory&) = delete;
	TranslatorFactory& operator= (const TranslatorFactory&) = delete;
};

END_TRANSLATOR_NAMESPACE

#endif // !TRANSLATOR_FACTORY_H_INCLUDED