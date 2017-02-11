#pragma once
#include "CppEvent.h"
#include <string>


//Declare a bindable class with properties and Observer event-style support
/*
	For Example:

	BEGIN_BINDABLE_MODEL(Employee)
	DEFINE_READONLY_PROPERTY(Id,unsigned long)
	DEFINE_PROPERTY(FullName,std::string)
	END_BINDABLE_MODEL()

	Will produce class "Employee" containing the properties as defined, as follows:

	class Employee
	{
	public:
		unsigned long GetId() const 
		{
			return m_Id;
		}

		std::string GetFullName() const 
		{ 
			return m_FullName; 
		}
		
		void SetFullName(const std::string& value)
		{
			if(m_FullName != value)
			{
				m_FullName = value;
				PropertyChanged.Trigger(*this, PropertyChangedEventArgs("FullName"));
			}
		}

	protected:
		unsigned long m_Id;
		std::string m_FullName;
	};


	Employee emp;
	emp.SetFullName("John Doe");
	
	will trigger a PropertyChanged event which client code can listen to as follows:

	class EmployeView...
	{
	...
		EmployeView()
		{ //subscribe to the event upon construction
			emp.PropertyChanged.Attach(*this, &EmployeeView::OnPropertyChanged);
		}

		~EmployeView()
		{ //unsubscribe upon destruction
			emp.PropertyChanged.Detach(*this, &EmployeeView::OnPropertyChanged);
		}

		//event handler
		void OnPropertyChanged(Employee& sender, PropertyChangedEventArgs& e)
		{
			cout << e.PropertyName << " was modified" << endl;
		}
		...
	};

*/


#define BEGIN_BINDABLE_MODEL(CLASS)	\
	class CLASS	\
	{	public:	\
			PropertyChangedEvent<CLASS> PropertyChanged;


#define DEFINE_READONLY_PROPERTY(PropertyName,DataType) \
	protected: \
		DataType m_##PropertyName; \
	public: \
		DataType Get##PropertyName() const { return m_##PropertyName; }


#define DEFINE_PROPERTY(PropertyName,DataType) \
	protected: \
		DataType m_##PropertyName; \
	public: \
		DataType Get##PropertyName() const { return m_##PropertyName; } \
		void Set##PropertyName(const DataType& value)	\
		{	\
			if (m_##PropertyName != value)	\
			{	\
				m_##PropertyName = value;	\
				PropertyChanged.Trigger(*this, PropertyChangedEventArgs(#PropertyName));	\
			}	\
		}


#define END_BINDABLE_MODEL()	};



struct PropertyChangedEventArgs
{
	PropertyChangedEventArgs(const std::string propertyName)
		: PropertyName(propertyName)
	{}

	const std::string PropertyName;
};


template<class T>
using PropertyChangedEvent = Event<T&, PropertyChangedEventArgs&>;
