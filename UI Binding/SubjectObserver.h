#pragma once
#include <vector>

//Forward declarations
struct ISubject;



//Base class for event params
struct IEventArgs {};


//Views inherit from this class and provide GUI update code in function Update()
struct IObserver
{
	virtual void Update(ISubject* sender, IEventArgs* args = nullptr) = 0;
};


//Models inherit from this class and call Notify() to notify views of any changes
struct ISubject
{
	//Attach or subscribe
	void Attach(IObserver* obj) { observers.push_back(obj); }

	//Detach or unsubscribe
	void Detach(IObserver* obj) 
	{
		for (auto it = observers.begin(); it != observers.end();)
		{
			if (*it == obj) it = observers.erase(it);
			else it++;
		}
	}

	//Notify all attached observers
	void Notify(IEventArgs* args = nullptr)
	{
		for (auto observerObject : observers)
			observerObject->Update(this, args);
	}


private:
	std::vector<IObserver*> observers;
};
