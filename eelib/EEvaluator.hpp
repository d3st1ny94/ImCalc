// EEvaluator.h - Wraps Expression_Evaluator
#pragma once
#ifdef MATHLIBRARY_EXPORTS
#define MATHLIBRARY_API __declspec(dllexport)
#else
#define MATHLIBRARY_API __declspec(dllimport)
#endif
#include <string>
extern "C" {
	class ExpressionEvaluator;
	class MATHLIBRARY_API EEvaluator {
		ExpressionEvaluator* eval;
	public:
		EEvaluator();
		~EEvaluator();
		void evaluate(char*const buffer, size_t size);
		// these methods throw exceptions
		double evaluateDouble(char const* const buffer, size_t size);
		float evaluateFloat(char const* const buffer, size_t size);
	};
}
// mono/C# wrappers
extern "C" MATHLIBRARY_API EEvaluator * CreateEEClass();

extern "C" MATHLIBRARY_API void DisposeEEClass(
	EEvaluator * pObject);

extern "C" MATHLIBRARY_API void CallEvaluateAscii(
	EEvaluator * pObject, char* const buffer, size_t size);

// these methods throw exceptions
extern "C" MATHLIBRARY_API float CallEvaluateAsciiFloat(
	EEvaluator * pObject, char const* const buffer, size_t size);
extern "C" MATHLIBRARY_API double CallEvaluateAsciiDouble(
	EEvaluator * pObject, char const* const buffer, size_t size);