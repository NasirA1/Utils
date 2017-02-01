/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	A very simple minimalistic XML parser in C++11
	Currently only able to recognise XML elements (without header/attributes)

	Warning: Be wary of performance implications when back-porting to old C++ (e.g. C++98), as the code relies 
	heavily on 'Move Semantics'

	Nasir Ahmad
	2015.11.02
*/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>



using namespace std;


///Neat macro technique from throwing string exceptions
///Takens stringstream arguments in the form of: THROW("error: "<<errno<<"desc: ..."<<endl)
#define THROW(ssargs) {	ostringstream ss; ss << ssargs; throw ss.str(); }



namespace util
{
	///Reads all text from the given text file and returns it in a string 
	string readAllText(const char* const filename)
	{
		std::ifstream t(filename);
		std::string str;

		t.seekg(0, std::ios::end);
		str.reserve(static_cast<unsigned int>(t.tellg()));
		t.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		return str; //C++11: move the string
	}
}


namespace xml
{
	//Recognisable tokens
	enum TokenType
	{
		TOKEN_INVALID,
		TOKEN_EOF,
		TOKEN_ENAME,   /*Element name */
		TOKEN_EVALUE,  /*Element value*/
		TOKEN_LTHAN,
		TOKEN_FSLASH,
		TOKEN_GTHAN
	};

	const string tokenNames[] = { "n/a", "<EOF>", "ENAME", "EVALUE", "LTHAN", "FSLASH", "GTHAN" };

	string getTokenName(const TokenType id)
	{
		return tokenNames[static_cast<int>(id)];
	}


	///Represents a lexer token
	///A token has a type and a string description
	struct token
	{
		TokenType type;
		string text;

		token(const TokenType ty = TOKEN_INVALID, const string& tx = "")
			: type(ty)
			, text(tx)
		{}

		string toString()
		{
			ostringstream ss;
			ss << "['" << text << "'," << getTokenName(type) << "]";
			return ss.str();
		}
	};


	///lexer class
	///Tokenises input (lexical analysis)
	class lexer
	{
		string input;
		int p = 0;
		char c;

	public:
		lexer(const string& in)
			: input(in)
			, p(0)
			, c(in.length() > 0? in[p]: EOF)
		{
		}

	private:
		void consume()
		{
			p++;
			if (static_cast<size_t>(p) >= input.length()) c = EOF;
			else c = input[p];
		}


		void match(char x)
		{
			if (c == x)
				consume();
			else
				THROW("ERROR: Expecting '" << x << "', Found '" << c << "'")
		}


		void WS()
		{
			while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
				consume();
		}


		token ENAME()
		{
			ostringstream ss;
			do {
				ss << c;
				consume();
			} while (::isalnum(c));

			return token(TOKEN_ENAME, ss.str().c_str());
		}

		token EVALUE()
		{
			ostringstream ss;
			do {
				ss << c;
				consume();
			} while (c != '<');

			return token(TOKEN_EVALUE, ss.str().c_str());
		}

	public:
		///This performs the lexical analysis
		///Returns result in a token object
		token nextToken()
		{
			static bool valuemode = false;

			while (c != EOF)
			{
				switch (c)
				{
				case ' ': case '\t': case '\n': case '\r':
					WS();
					continue;
				case '<':
					consume();
					valuemode = false;
					return token(TOKEN_LTHAN, "<");
				case '>':
					consume();
					if(p != input.length() - 1)
						valuemode = true;
					return token(TOKEN_GTHAN, ">");
				case '/':
					consume();
					valuemode = false;
					return token(TOKEN_FSLASH, "/");
				default:
					if (valuemode)
						return EVALUE();
					else if (::isalnum(c))
						return ENAME();
					else if (static_cast<size_t>(p) >= input.length() - 1)
						c = EOF; //End of input
					else
						THROW("ERROR: Invalid character '" << c << "'")
				}
			}

			return token(TOKEN_EOF, "<EOF>");
		}

	};


	///Class 'parser' does the actual parsing
	///After input has been tokenised
	///Uses a lookahead circular buffer of size(2) 
	class parser
	{
		lexer& input;
		const int k; //size of lookahead buffer
		int p; //index of current buffer element
		vector<token> lookaheads;

	public:
		parser(lexer& in)
			: input(in)
			, k(2)
			, p(0)
		{
			lookaheads.reserve(k);
			for (int i = 1; i <= k; ++i)
			{
				lookaheads.push_back(input.nextToken());
				p = (p + 1) % k;
			}
		}

	private:
		void consume()
		{
			lookaheads[p] = input.nextToken();
			p = (p + 1) % k;
		}

		token LT(int i) { return lookaheads[(p + i - 1) % k]; }
		TokenType LA(int i) { return LT(i).type; }

		void match(const TokenType x)
		{
			if (LA(1) == x) {
				//cout << "matched: " << (x == TOKEN_EVALUE? LT(1).toString(): getTokenName(x)) << endl;
				if (x == TOKEN_EVALUE)
					cout << "matched: " << LT(1).toString() << endl;
				consume();
			}
			else
			{
				THROW("PARSER: Expecting " << getTokenName(x) << " Found: " << LT(1).toString())
			}
		}


		void starttag()
		{
			match(TOKEN_LTHAN);
			match(TOKEN_ENAME);
			match(TOKEN_GTHAN);
		}


		void endtag()
		{
			match(TOKEN_LTHAN);
			match(TOKEN_FSLASH);
			match(TOKEN_ENAME);
			match(TOKEN_GTHAN);
		}


		void value()
		{
			if (LA(1) == TOKEN_EVALUE)
			{
				match(TOKEN_EVALUE);
			}
			else 
			{
				//parse children
				while (LA(1) == TOKEN_LTHAN)
				{
					if (LA(2) == TOKEN_FSLASH)
						endtag();
					else
						xmlnode();
				}
			}
		}

	public:
		///Parsing starts here
		///on the root element
		void xmlnode()
		{
			starttag();
			value();
			if (LA(1) != TOKEN_EOF)
				endtag();
			/*else
				cout << "<EOF>" << endl;
			*/
		}

	};

}//end of xml namespace




int main(int argc, char* argv[])
{
	//test rig..
#if 1
	{
		string raw = util::readAllText("contacts.xml");
		//string raw = util::readAllText("test.xml");
		cout << raw << endl;

		try
		{
			xml::lexer lex(raw);
			xml::parser par(lex);

			//test lexer only
			//{
			//	xml::token t = lex.nextToken();

			//	while (t.type != xml::TOKEN_EOF)
			//	{
			//		cout << t.toString() << endl;
			//		t = lex.nextToken();
			//	}
			//}
			par.xmlnode();
		}
		catch (const string& s)
		{
			cerr << s << endl;
		}
	}
#endif

	cin.get();
}
