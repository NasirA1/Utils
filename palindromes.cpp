#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <map>
#include <algorithm>
#include <cctype>
#include <sstream>
#include <functional>
#include <iomanip>

using namespace std;



static inline string& triml(string& str)
{
	str.erase(0, str.find_first_not_of(' '));
	return str;
}

static inline string& trimr(string& str)
{
	str.erase(str.find_last_not_of(' ')+1, str.size());
	return str;
}

static inline string& trim(string& str)
{
	return triml(trimr((str)));
}

static inline string& toupper(string& str)
{
	transform(str.begin(), str.end(), str.begin(), std::toupper);
	return str;
}

static inline string& tolower(string& str)
{
	transform(str.begin(), str.end(), str.begin(), std::tolower);
	return str;
}

static inline string& stripchars(string& str, const string& chars)
{
	ostringstream oss;
	
	for_each(str.begin(), str.end(), [&](const auto ch) {
		if (chars.find(ch) == string::npos)
			oss << ch;
	});

	str = oss.str();
	return str;
}

template<class FUNC>
static inline string& filter(string& str, FUNC& predicate)
{
	ostringstream oss;

	for_each(str.begin(), str.end(), [&](const auto ch) {
		if (predicate(ch))
			oss << ch;
	});

	str = oss.str();
	return str;
}

bool isPalindrome(const string& str)
{
	string x = str;
	if (!trim(x).length()) return false;

	string y = tolower(filter(x, [](char ch) { return std::isalnum(ch); }));
	reverse(y.begin(), y.end());
	return x == y;
}

struct bool_to_yesno 
{
	bool_to_yesno(bool value): value_(value) { }
	void operator() (std::ostream& out) const { out << (value_ ? "Yes" : "No"); };
	const bool value_;
};

ostream& operator<< (ostream& out, const bool_to_yesno& yesno) {
	yesno(out);
	return out;
}

void palindrome_demo(const string& input, const size_t tab_count)
{
	string tabs(tab_count, '\t');
	cout << input << tabs << bool_to_yesno(isPalindrome(input)) << endl;
}



int main()
{

	cout << endl;
	cout << "Phrase					 Palindrome?" << endl;
	cout << "==========================================================" << endl;
	palindrome_demo("Mom", 6);
	palindrome_demo("Able was I, ere I saw Elba", 3);
	palindrome_demo("Madam, I'm Adam", 5);
	palindrome_demo("A lad named E.Mandala", 4);
	palindrome_demo("A man, a plan, a canal: Panama!", 3);
	palindrome_demo("A nut for a jar of tuna.", 3);
	palindrome_demo("Nasir", 6);
	palindrome_demo("To be or not to be...", 4);
	cout << endl << "Press 'Q' or 'q' for exit" << endl;

	cout << endl << endl;
	string input;
	
	while (true) 
	{
		cout << "Enter phrase: ";
		getline(cin, input);
		if (input == "q" || input == "Q") break;
		else if (!trim(input).size()) continue;

		if (isPalindrome(input)) cout << "\"" << input << "\" is a palindrome." << endl;
		else cout << "\"" << input << "\" is a not palindrome." << endl;

		cout << endl;
	}

	cout << endl;
}
