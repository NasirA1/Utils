#pragma once
#include <sstream>

namespace my_cstring_formatter 
{
	static thread_local CString __s_buff;
	static CString& with_nl() { __s_buff.AppendChar('\n'); return __s_buff; }
}

//Returns C-style formatted CString
#define _F(...)			(my_cstring_formatter::__s_buff.Format(##__VA_ARGS__), my_cstring_formatter::__s_buff)
#define _FN(...)		(my_cstring_formatter::__s_buff.Format(##__VA_ARGS__), my_cstring_formatter::with_nl())



#define DBOUTA( s )           \
{                             \
   std::ostringstream os_;    \
   os_ << s << std::endl;                   \
   ::OutputDebugStringA( os_.str().c_str() );  \
}


#define DBOUTW( s )           \
{                             \
   std::wostringstream os_;    \
   os_ << s << std::endl;                   \
   ::OutputDebugStringW( os_.str().c_str() );  \
}



#define DBPRINTFW(...)					::OutputDebugStringW( _FN(__VA_ARGS__).GetBuffer() )
#define DBPRINTFA(...)					::OutputDebugStringA( _FN(__VA_ARGS__).GetBuffer() )


#ifdef _UNICODE
#define DBOUT( s )						DBOUTW( s )
#define DBPRINTF(...)					DBPRINTFW(__VA_ARGS__)
#else
#define DBOUT( s )						DBOUTA( s )
#define DBPRINTF(...)					DBPRINTFA(__VA_ARGS__)
#endif
