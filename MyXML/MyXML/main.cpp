/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	A very simple minimalistic XML parser in C++11
	Currently only able to recognise XML elements (without header/attributes)

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
		string str;

		t.seekg(0, std::ios::end);
		str.reserve(static_cast<unsigned int>(t.tellg()));
		t.seekg(0, std::ios::beg);

		str.assign((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		return str; //C++11: move the string out
	}
}


namespace xml
{
	//Recognisable tokens
	enum TokenType
	{
		//Invalid token
		TOKEN_INVALID,
		//End of file/input
		TOKEN_EOF,
		//ElementStart name
		TOKEN_ENAME,
		//ElementStart value
		TOKEN_EVALUE,
		//Less than character
		TOKEN_LTHAN,
		//Forward Slash character
		TOKEN_FSLASH,
		//Greater than character
		TOKEN_GTHAN,
		//Double-quote character
		TOKEN_DQUOTE,
		//Equals character
		TOKEN_EQUAL,
		//Attribute name
		TOKEN_ANAME,
		//Attribute value
		TOKEN_AVALUE
	};

	const string tokenNames[] = { "n/a", "<EOF>", "ENAME", "EVALUE", "LTHAN", "FSLASH", "GTHAN", "DQUOTE", "EQUAL", "ANAME", "AVALUE" };

	string getTokenName(const TokenType id)
	{
		return tokenNames[static_cast<int>(id)];
	}


	///Represents a Lexer token
	///A token has a type and a string description
	struct Token
	{
		TokenType type;
		string text;

		Token(const TokenType ty = TOKEN_INVALID, const string& tx = "")
			: type(ty)
			, text(tx)
		{}

		string toString() const
		{
			ostringstream ss;
			ss << "['" << text << "'," << getTokenName(type) << "]";
			return ss.str();
		}
	};


	///Lexer class
	///Tokenises input (lexical analysis)
	class Lexer
	{
		string input;
		int p = 0;
		char c;

	public:
		Lexer(const string& in)
			: input(in)
			, p(0)
			, c(in.length() > 0? in[p]: EOF)
		{
		}


	private:
		//Consume a single character and move cursor to next char (if not EOF)
		void consume()
		{
			p++;

			if (static_cast<size_t>(p) >= input.length())
				c = EOF;
			else 
				c = input[p];
		}

		//Match the given char, consume if successful or throw if fails
		void match(char x)
		{
			if (c == x)
				consume();
			else
				THROW("ERROR: Expecting '" << x << "', Found '" << c << "'")
		}

		//Consume all whitespace
		void WS()
		{
			while (c == ' ' || c == '\t' || c == '\n' || c == '\r')
				consume();
		}

		//Consume element name
		Token ENAME()
		{
			ostringstream ss;
			do {
				ss << c;
				consume();
			} while (::isalnum(c) || c == '_');

			return Token(TOKEN_ENAME, ss.str().c_str());
		}

		//Consume element value
		Token EVALUE()
		{
			ostringstream ss;
			do {
				ss << c;
				consume();
			} while (c != '<');

			return Token(TOKEN_EVALUE, ss.str().c_str());
		}

		//Consume attribute name
		Token ANAME()
		{
			ostringstream ss;
			do {
				ss << c;
				consume();
			} while (::isalnum(c) || c == ':');

			return Token(TOKEN_ANAME, ss.str().c_str());
		}

		//Consume attribute value
		Token AVALUE()
		{
			ostringstream ss;
			do {
				ss << c;
				consume();
			} while (c != '\"');

			return Token(TOKEN_AVALUE, ss.str().c_str());
		}


	public:
		///This performs the lexical analysis
		///Returns result in a Token object
		Token nextToken()
		{
			static enum class States
			{
				Normal,
				ElementStart,
				Attribute,
				AttributeValue,
				ElementValue
			} state = States::Normal;

			while (c != EOF)
			{
				switch (c)
				{
				case ' ': case '\t': case '\n': case '\r':
					WS();
					continue;

				case '<':
					consume();
					state = States::ElementStart;
					return Token(TOKEN_LTHAN, "<");

				case '>':
					consume();
					if(p != input.length() - 1)
						state = States::ElementValue;
					return Token(TOKEN_GTHAN, ">");

				case '/':
					consume();
					state = States::Normal;
					return Token(TOKEN_FSLASH, "/");

				case '=':
					consume();
					state = States::AttributeValue;
					return Token(TOKEN_EQUAL, "=");

				case '"':
					consume();
					return Token(TOKEN_DQUOTE, "\"");

				default:
					if (state == States::ElementValue)
					{
						return EVALUE();
					}
					else if (::isalnum(c) || c == ':' || c == '_') //TODO recognise understores and other allowable characters (See XML spec.)
					{
						if (state == States::ElementStart)
						{
							state = States::Attribute;
							return ENAME();
						}
						else if (state == States::Attribute)
						{
							state = States::AttributeValue;
							return ANAME();
						}
						else if (state == States::AttributeValue)
						{
							state = States::Attribute;
							return AVALUE();
						}
						else
						{
							state = States::Normal;
							return ENAME();
						}
					}
					else if (static_cast<size_t>(p) >= input.length() - 1)
					{
						state = States::Normal;
						c = EOF; //End of input
					}
					else
					{
						THROW("ERROR: Invalid character '" << c << "'")
					}
				}
			}

			return Token(TOKEN_EOF, "<EOF>");
		}

	};


	///Class 'Recogniser' does the actual parsing after input has been tokenised
	///Uses a lookahead circular buffer of size(2) 
	class Recogniser
	{
	public:
		typedef void(*OnMatch) (const Token&);

		//.ctor
		Recogniser(Lexer& in, OnMatch onMatchCallback = nullptr)
			: input(in)
			, k(2)
			, p(0)
			, onMatch(onMatchCallback)
		{
			lookaheads.reserve(k);
			for (int i = 1; i <= k; ++i)
			{
				lookaheads.push_back(input.nextToken());
				p = (p + 1) % k;
			}
		}

		~Recogniser() { onMatch = nullptr; }


		//Parse the root 
		void parse() { xmlnode(); }


	private:
		//Consume next token
		void consume()
		{
			lookaheads[p] = input.nextToken();
			p = (p + 1) % k;
		}

		//Return lookahead token (1 or 2)
		Token LT(int i) { return lookaheads[(p + i - 1) % k]; }

		//Return TokenType of LT (1 or 2)
		TokenType LA(int i) { return LT(i).type; }

		//Match the given token-type and call OnMatch callback handler
		void match(const TokenType x)
		{
			if (LA(1) == x) 
			{
				//cout << "matched: " << (x == TOKEN_EVALUE? LT(1).toString(): getTokenName(x)) << endl;
				if (onMatch) onMatch(LT(1));
				consume();
			}
			else
			{
				THROW("PARSER: Expecting " << getTokenName(x) << " Found: " << LT(1).toString())
			}
		}

		//Match Start-tag
		void starttag()
		{
			match(TOKEN_LTHAN);
			match(TOKEN_ENAME);

			//parse attributes (if any)
			while (LA(1) == TokenType::TOKEN_ANAME)
				attribute();

			match(TOKEN_GTHAN);
		}

		//Match attribute
		void attribute()
		{
			match(TOKEN_ANAME);
			match(TOKEN_EQUAL);
			match(TOKEN_DQUOTE);
			match(TOKEN_AVALUE);
			match(TOKEN_DQUOTE);
		}

		//Match end-tag
		void endtag()
		{
			match(TOKEN_LTHAN);
			match(TOKEN_FSLASH);
			match(TOKEN_ENAME);
			match(TOKEN_GTHAN);
		}

		//Match value
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

		//Match a single XML node
		void xmlnode()
		{
			starttag();
			value();
			if (LA(1) != TOKEN_EOF)
				endtag();
			//else cout << "<EOF>" << endl;			
		}


	private:
		Lexer& input;		//Lexer object
		const int k;		//size of lookahead buffer 
		int p;					//index of current buffer element
		vector<Token> lookaheads; //Lookahead buffer
		OnMatch onMatch;		//Callback called on every match
	};

}//end of xml namespace



void OnMatch(const xml::Token& token)
{
	cout << "matched: " << token.toString() << endl;
}


int main(int argc, char* argv[])
{
	//test rig..
#if 1
	{
		auto raw = util::readAllText("contacts2.xml");
		//string raw = util::readAllText("test.xml");
		cout << raw << endl;

		try
		{
			xml::Lexer lexer(raw);
			xml::Recogniser parser(lexer, &OnMatch);

			//test Lexer only
			//{
			//	xml::Token t = lexer.nextToken();

			//	while (t.type != xml::TOKEN_EOF)
			//	{
			//		cout << t.toString() << endl; 
			//		t = lexer.nextToken();
			//	}
			//}
			parser.parse();
			cout << "Finished successfully" << endl;
		}
		catch (const string& s)
		{
			cerr << s << endl;
		}
	}
#endif

	cin.get();
}
