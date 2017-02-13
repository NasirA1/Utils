#pragma once
#include <chrono>


//Before including diagnostics, define SCOPE_TIMER_T_TRACE as a function/macro that takes variadic arguments 
//and outputs them to where you want
#ifndef SCOPE_TIMER_T_TRACE
#define SCOPE_TIMER_T_TRACE(...)		printf(__VA_ARGS__)
#endif


struct scope_timer_t
{
	scope_timer_t(const char* const label = nullptr) 
		: label_(label)
		, start_(chrono::steady_clock::now()) 
	{}

	~scope_timer_t()
	{
		const auto end = chrono::steady_clock::now();
		const auto diff = end - start_;
		SCOPE_TIMER_T_TRACE("%s took %f ms\n", label_, chrono::duration<double, milli>(diff).count());
	}
	
	const char* const label_;
	const chrono::time_point<chrono::steady_clock> start_;
};
