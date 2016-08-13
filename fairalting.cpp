#include <iostream>
#include <thread>
#include <mutex>

using namespace std;


//C++11
//Fair ALT'ing through the use of condition variable


mutex mu;
condition_variable cond;
bool ready = false;


void shared_print(const bool toggle)
{
	static int msg1_count = 0;
	static int msg2_count = 0;

	lock_guard<mutex> guard(mu);

	if (toggle)
	{
		cout << msg1_count << ": Those who live by the sword, die by the sword!" << endl;
		msg1_count++;
	}
	else
	{
		cout << msg2_count << ": To be or not to be, that is the question." << endl;
		msg2_count++;
	}
}


void fair_shared_print(const bool toggle)
{
	static int msg1_count = 0;
	static int msg2_count = 0;

	unique_lock<mutex> lock(mu);

	if (toggle)
	{
		cond.wait(lock, []() { return ready; });
		cout << msg1_count << ": Those who live by the sword, die by the sword!" << endl;
		msg1_count++;
		ready = false;
		cond.notify_one();
	}
	else
	{
		cond.wait(lock, []() { return !ready; });
		cout << msg2_count << ": To be or not to be, that is the question." << endl;
		msg2_count++;
		ready = true;
		cond.notify_one();
	}
}




void do_work(const bool toggle)
{
	while (true)
	{
		shared_print(toggle);
	}
}


void fair_do_work(const bool toggle)
{
	while (true)
	{
		fair_shared_print(toggle);
	}
}


int main(int argc, char* argv[])
{
#if 0
	thread t1(do_work, true);
	thread t2(do_work, false);
#else
	thread t1(fair_do_work, true);
	thread t2(fair_do_work, false);
#endif

	t1.join();
	t2.join();

	cout << "All done!\n";
	cin.get();

	return EXIT_SUCCESS;
}

