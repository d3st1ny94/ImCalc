// EEvaluator.h - Wraps Expression_Evaluator
#pragma once
#include <string>
extern "C" {
	class ExpressionEvaluator;
	class EEvaluator {
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
extern "C" EEvaluator * CreateEEClass();

extern "C" void DisposeEEClass(
	EEvaluator * pObject);

extern "C" void CallEvaluateAscii(
	EEvaluator * pObject, char* const buffer, size_t size);

// these methods throw exceptions
extern "C" float CallEvaluateAsciiFloat(
	EEvaluator * pObject, char const* const buffer, size_t size);
extern "C" double CallEvaluateAsciiDouble(
	EEvaluator * pObject, char const* const buffer, size_t size);