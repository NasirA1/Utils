//The functor
struct Functor
{
	Functor(const int n) : N(n) {}
	int operator() (const int a) { return N + a; }
	int N;
};

//The function that takes the functor as argument
template<typename Iterator, typename Func>
void FunctorRecepient(Iterator begin, Iterator end, Func func)
{
	for (Iterator it = begin; it != end; ++it)
	{
		*it = func(*it);
	}
}

//Usage
int xs[] = { 1, 1, 1, 1, 2 };
FunctorRecepient(xs, xs + 4, Functor(5));


