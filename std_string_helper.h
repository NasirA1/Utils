///////////  Generic string helper functions that work on both std::string and std::wstring  ////////////////////
#pragma once
#include <string>
#include <stdarg.h>
#include <tchar.h>
#include <iostream>
#include <locale>
#include <windows.h>
#include <array>
#include <vector>
#include <sstream>
#include <codecvt>


//Returns C-style formatted std string
#define _F(...)		format(##__VA_ARGS__)
#define _FN(...)	format(##__VA_ARGS__).append(1, '\n')


using namespace std;


#pragma region format() Helpers
//Helper function used in format()
//Base Template for CHARTYPEs
template<class CHARTYPE>
static inline int my_vsnprintf(
	_Out_writes_(_BufferCount)    CHARTYPE*       const _Buffer,
	_In_                          size_t          const _BufferCount,
	_In_z_ _Printf_format_string_ CHARTYPE const* const _Format,
	va_list           _ArgList
)
{
	std::cerr << "ERROR: Unknown CHARTYPE!! Provide template specialisation!" << std::endl;
	return -1;
}

//Helper function used in format()
//Template Specialisation for ASCII strings (CHARTYPE = char)
template<>
static inline int my_vsnprintf(
	_Out_writes_(_BufferCount)    char*       const _Buffer,
	_In_                          size_t      const _BufferCount,
	_In_z_ _Printf_format_string_ char const* const _Format,
	va_list           _ArgList
)
{
	return vsnprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

//Helper function used in format()
//Template Specialisation for Unicode strings (CHARTYPE = TCHAR)
template<>
static inline int my_vsnprintf(
	_Out_writes_(_BufferCount)    TCHAR*          const _Buffer,
	_In_                          size_t          const _BufferCount,
	_In_z_ _Printf_format_string_ TCHAR const*    const _Format,
	va_list           _ArgList
)
{
	return vswprintf(_Buffer, _BufferCount, _Format, _ArgList);
}

#pragma endregion


//C-style string formatting on arbitrary-length input
//Supply larger InitialBufferSize for larger strings for better performance
template<size_t InitialBufferSize = 1024u, class CHARTYPE>
static inline std::basic_string<CHARTYPE> format(const CHARTYPE* fmt, ...)
{
	va_list args;

	std::basic_string<CHARTYPE> result;
	int rc = -1;
	size_t length = InitialBufferSize;

	while (true)
	{
		va_start(args, fmt);
		result.resize(length);
		rc = my_vsnprintf(const_cast<CHARTYPE*>(result.data()), length, fmt, args);
		va_end(args);

		if (rc > -1 && static_cast<unsigned int>(rc) < length)
		{//all ok
			result.resize(rc);
			return result;
		}

		if (rc > -1)
			//needed size returned, allocate 1 byte for the null terminator
			length = rc + 1;
		else
			//Error occurred. Try doubling buffer size
			length *= 2;
	}

	return result;
}



#if 0
//Trim the given string from the left and return reference
template<class CHARTYPE>
inline static basic_string<CHARTYPE>& triml(basic_string<CHARTYPE>& s)
{
	s.erase(0, s.find_first_not_of(' '));
	return s;
}

//Trim the given string from the right and return reference
template<class CHARTYPE>
inline static basic_string<CHARTYPE>& trimr(basic_string<CHARTYPE>& s)
{
	s.erase(s.find_last_not_of(' ') + 1, s.size());
	return s;
}

//Trim the given string from both sides and return reference
template<class CHARTYPE>
inline static basic_string<CHARTYPE>& trim(basic_string<CHARTYPE>& s)
{
	return trimr(triml(s));
}
#endif

//Trim the given string from the left and return reference
//By default SPACE characters are trimmed. Custom characters can be provided in trim_chars parameter.
template<class CHARTYPE = char, size_t ARRSIZE = 1u>
inline static basic_string<CHARTYPE>& triml(basic_string<CHARTYPE>& s, const std::array<CHARTYPE, ARRSIZE>& trim_chars = { _T(' ') })
{
	auto pos = 0;

	for (auto ch : s)
	{
		if (std::any_of(trim_chars.begin(), trim_chars.end(), [&](auto delim) { return ch == delim; }))
			pos++;
		else
			break;
	}

	if (pos > 0)
	{
		s.erase(0, pos);
	}

	return s;
}

//Trim the given string from the right and return reference
//By default SPACE characters are trimmed. Custom characters can be provided in trim_chars parameter.
template<class CHARTYPE = char, size_t ARRSIZE = 1u>
inline static basic_string<CHARTYPE>& trimr(basic_string<CHARTYPE>& s, const std::array<CHARTYPE, ARRSIZE>& trim_chars = { _T(' ') })
{
	if (s.empty()) return s;

	auto pos = s.length() - 1;

	for (auto i = s.length() - 1; i >= 0; i--)
	{
		if (std::any_of(trim_chars.begin(), trim_chars.end(), [&](auto delim) { return s[i] == delim; }))
			pos--;
		else
			break;
	}

	if (pos < s.length() - 1)
	{
		s.erase(pos + 1, s.length() - pos);
	}

	return s;
}

//Trim the given string from both sides and return reference
//By default SPACE characters are trimmed. Custom characters can be provided in trim_chars parameter.
template<class CHARTYPE = char, size_t ARRSIZE = 1u>
inline static basic_string<CHARTYPE>& trim(basic_string<CHARTYPE>& s, const std::array<CHARTYPE, ARRSIZE>& trim_chars = { _T(' ') })
{
	return trimr(triml(s, trim_chars), trim_chars);
}


template<class CHARTYPE = char>
inline static bool starts_with(const basic_string<CHARTYPE>& subject, const basic_string<CHARTYPE>& prefix)
{
	if (prefix.length() > subject.length()) return false;
	return subject.substr(0, prefix.length()) == prefix;
}

template<class CHARTYPE = char>
inline static bool starts_with(const CHARTYPE* const subject, const CHARTYPE* const prefix)
{
	const basic_string<CHARTYPE> s_subject(subject);
	const basic_string<CHARTYPE> s_prefix(prefix);
	return starts_with(s_subject, s_prefix);
}

template<class CHARTYPE = char>
inline static bool starts_with(const basic_string<CHARTYPE>& subject, const CHARTYPE* const prefix)
{
	const basic_string<CHARTYPE> s_prefix(prefix);
	return starts_with(subject, s_prefix);
}


template<class CHARTYPE = char>
inline static bool ends_with(const basic_string<CHARTYPE>& subject, const basic_string<CHARTYPE>& suffix)
{
	if (suffix.length() > subject.length()) return false;
	return subject.substr(subject.length() - suffix.length(), suffix.length()) == suffix;
}

template<class CHARTYPE = char>
inline static bool ends_with(const CHARTYPE* const subject, const CHARTYPE* const suffix)
{
	const basic_string<CHARTYPE> s_subject(subject);
	const basic_string<CHARTYPE> s_suffix(suffix);
	return ends_with(s_subject, s_suffix);
}

template<class CHARTYPE = char>
inline static bool ends_with(const basic_string<CHARTYPE>& subject, const CHARTYPE* const suffix)
{
	const basic_string<CHARTYPE> s_suffix(suffix);
	return ends_with(subject, s_suffix);
}


template<class CHARTYPE = char>
inline static basic_string<CHARTYPE> copy(const basic_string<CHARTYPE>& s)
{
	return s;
}

template<class CHARTYPE = char>
inline static basic_string<CHARTYPE> copy(const CHARTYPE* const s)
{
	return s;
}

template<class CHARTYPE = char, size_t ARRSIZE = 1>
inline static auto split(const basic_string<CHARTYPE>& subject, const CHARTYPE delim = '\n', const bool ignore_empty = false)
{
	std::vector<std::basic_string<CHARTYPE>> output;
	std::basic_istringstream<CHARTYPE> iss(subject);

	for (std::basic_string<CHARTYPE> token; std::getline(iss, token, delim);)
	{
		if (ignore_empty && token.empty())
			continue;
		output.push_back(std::move(token));
	}

	return output;
}


template<class CHARTYPE = char, size_t ARRSIZE = 1>
inline static auto join(const std::vector<std::basic_string<CHARTYPE>>& tokens, const CHARTYPE delim = '\n')
{
	std::basic_ostringstream<CHARTYPE> oss;

	for (auto i = 0u; i < tokens.size(); ++i)
	{
		oss << tokens[i] << delim;
	}

	return oss.str();
}


static inline std::string wstring_to_string(const std::wstring& wstr)
{
	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::string astr = converter.to_bytes(wstr);
	return astr;
}


static inline std::wstring string_to_wstring(const std::string& astr)
{
	//use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
	using convert_type = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_type, wchar_t> converter;
	std::wstring wstr = converter.from_bytes(astr);
	return wstr;
}

