#include <gtest/gtest.h>
#include <memory>
#include <vector>


#define DEBUGGING
#ifdef DEBUGGING
#define LPRINTF(...)	printf(__VA_ARGS__)
#else
#define LPRINTF(...)	/* no-op */
#endif



/**
 Demonstrates a simple CoW (Copy-on-Write) buffer written TDD style
*/
class CoWBuffer
{
public:
	//Construct the buffer with the given data and initialise ref count to 1
	CoWBuffer(const char* const data)
		: size_(data? strlen(data): 0lu)
		, data_(size_? new char[size_ + 1]: nullptr)
		, ref_count_(size_? new int(1): nullptr)
	{
		LPRINTF("ctor\n");
		if (data) 
		{
			strcpy(data_, data);
			data_[size_] = '\0';
		}
	}

	//.dctor decrements the counter
	//if counter reaches 0, memory is deallocated
	~CoWBuffer() 
	{
		LPRINTF("dtor\n");
		release();
	}

	//Copy .ctor does a shallow copy and increments the reference count
	CoWBuffer(const CoWBuffer& other)
		: size_(other.size_)
		, data_(other.data_)
		, ref_count_(other.ref_count_)
	{
		if(ref_count_)
			(*ref_count_)++;
		LPRINTF("copy ctor\n");
	}

	//Helper swap function
	friend void swap(CoWBuffer& a, CoWBuffer& b)
	{
		std::swap(a.size_, b.size_);
		std::swap(a.data_, b.data_);
		std::swap(a.ref_count_, b.ref_count_);
	}

	//Move .ctor
	CoWBuffer(CoWBuffer&& other)
		: size_(0)
		, data_(nullptr)
		, ref_count_(nullptr)
	{
		LPRINTF("move ctor\n");
		swap(*this, other);
	}

	//Move assign
	//Release self and swap with rhs
	CoWBuffer& operator=(CoWBuffer&& rhs)
	{
		//no self-assignment
		if (this == &rhs)
			return *this;

		LPRINTF("move assign\n");
		release();
		swap(*this, rhs);
		return *this;
	}


	//Copy assignment operator does the actual CoW (Copy-on-write)
	//New buffer is created and ref count is reset to 1
	CoWBuffer& operator= (const CoWBuffer& rhs)
	{
		//no self-assignment
		if (this == &rhs)
			return *this;

		LPRINTF("copy assign\n");
		if (*ref_count_ == 1)
		{
			delete[] data_;
			delete ref_count_;
		}

		size_ = rhs.size_;
		if (size_)
		{
			data_ = new char[size_ + 1];
			ref_count_ = new int(1);
			strcpy(data_, rhs.data_);
			data_[size_] = '\0';
			*ref_count_ = 1;
		}
		else
		{
			data_ = nullptr;
			ref_count_ = nullptr;
		}

		return *this;
	}
	
	size_t size() const { return size_; }
	const char* const data() { return data_; }
	int ref_count() const { return ref_count_ ? *ref_count_ : 0; }


private:
	//decerments ref count and releases memory when the count reaches zero
	void release()
	{
		if (ref_count_)
		{
			(*ref_count_)--;

			if (*ref_count_ <= 0)
			{
				delete[] data_;
				delete ref_count_;
			}
		}
		data_ = nullptr;
		ref_count_ = nullptr;
		size_ = 0;
	}


private:
	size_t size_;
	char* data_;
	int* ref_count_;
};




TEST(CoWBuffer, Initialisation)
{
	CoWBuffer buff("hello");
	ASSERT_EQ(0, strcmp("hello", buff.data()));
	ASSERT_EQ(5, buff.size());
	ASSERT_EQ(1, buff.ref_count());
}


TEST(CoWBuffer, Initialisation_NullPointer_ReferenceNotIncremented)
{
	CoWBuffer buff(nullptr);
	ASSERT_EQ(nullptr, buff.data());
	ASSERT_EQ(0, buff.size());
	ASSERT_EQ(0, buff.ref_count());
}


TEST(CoWBuffer, CopyConstructor_PerformsShallowCopy)
{
	CoWBuffer buff("hello");
	
	//Call copy .ctor
	auto buff2 = buff;

	ASSERT_EQ(2, buff.ref_count());
	ASSERT_EQ(2, buff2.ref_count());
	ASSERT_EQ(buff.size(), buff2.size());
	ASSERT_EQ(buff.data(), buff2.data());
}


TEST(CoWBuffer, CopyConstructor_NullPointer_ReferenceNotIncremented)
{
	CoWBuffer buff(nullptr);

	//Call copy .ctor
	auto buff2 = buff;

	ASSERT_EQ(0, buff.ref_count());
	ASSERT_EQ(0, buff2.ref_count());
	ASSERT_EQ(buff.size(), buff2.size());
	ASSERT_EQ(buff.data(), buff2.data());
}


TEST(CoWBuffer, CopyConstructor_MultipleCopies_PerformsShallowCopy)
{
	CoWBuffer buff("hello");
	{
		std::vector<CoWBuffer> elems;
		for (auto i = 0; i < 3; ++i)
			elems.push_back(buff);
		ASSERT_EQ(4, buff.ref_count());

		for (auto i = 0u; i < elems.size(); ++i)
			ASSERT_EQ(buff.data(), elems[i].data());
	}
	ASSERT_EQ(1, buff.ref_count());
}


TEST(CoWBuffer, Assignment_PerformsDeepCopy)
{
	CoWBuffer buff("hello");
	CoWBuffer buff2("hello");

	//Call copy assignment operator
	buff2 = buff;

	ASSERT_NE(buff2.data(), buff.data());
	ASSERT_EQ(0, strcmp(buff2.data(), buff.data()));
	ASSERT_EQ(1, buff.ref_count());
	ASSERT_EQ(1, buff2.ref_count());
}


TEST(CoWBuffer, MoveAssignment_MovesTheRHS)
{
	CoWBuffer buff("hello");
	CoWBuffer buff2("goodbye");

	//Call move assignment operator
	buff = std::move(buff2);

	ASSERT_EQ(nullptr, buff2.data());
	ASSERT_EQ(0, buff2.ref_count());
	ASSERT_EQ(0, buff2.size());
	ASSERT_EQ(7, buff.size());
	ASSERT_EQ(0, strcmp("goodbye", buff.data()));
}



int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
