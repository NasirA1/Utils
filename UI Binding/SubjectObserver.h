#pragma once
#include <set>

//Forward declarations
struct ISubject;



//Base class for event params
struct IEventArgs {};


//Views inherit from this class and provide GUI update code in function Update()
struct IObserver
{
	virtual void Update(ISubject* sender, IEventArgs* args) = 0;
};


//Models inherit from this class and call Notify() to notify views of any changes
struct ISubject
{
	//Attach or subscribe
	void Attach(IObserver* obj) { observers.insert(obj); }

	//Detach or unsubscribe
	void Detach(IObserver* obj) { observers.erase(obj); }

	//Notify all attached observers
	void Notify(IEventArgs* args = nullptr)
	{
		for (auto observerObject : observers)
			observerObject->Update(this, args);
	}


private:
	std::set<IObserver*> observers;
};
