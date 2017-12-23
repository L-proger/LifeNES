#include <list>

#define COMBINE(a,b)		COMBINE1(a,b)
#define COMBINE1(a,b)		a##b

#define COMBINE3(a,b,c)		COMBINE3(a,b,c)
#define COMBINE31(a,b,c)		a##b##c

#define I_DELEGATE			COMBINE(Delegate, SUFFIX)
#define I_EVENT				COMBINE(Event, SUFFIX)
#define C_STATIC_DELEGATE	COMBINE(StaticDelegate, SUFFIX)
#define C_METHOD_DELEGATE	COMBINE(MethodDelegate, SUFFIX)

template<class TRet TEMPLATE_PARAMS>
class I_DELEGATE
{
public:
	virtual ~I_DELEGATE(){}
	virtual TRet Invoke(PARAMS) = 0;
	virtual bool Compare(I_DELEGATE<TRet TEMPLATE_ARGS>* other) = 0;
};

template<class TRet TEMPLATE_PARAMS>
class C_STATIC_DELEGATE : public I_DELEGATE<TRet TEMPLATE_ARGS>
{
public:
	typedef TRet (*Func)(PARAMS);
	C_STATIC_DELEGATE(Func func)
	{
		pFunc = func;
	}
	virtual TRet Invoke(PARAMS)
	{
		return pFunc(ARGS);
	}
	virtual bool Compare(I_DELEGATE<TRet TEMPLATE_ARGS>* other)
	{
		return true;
	}
private:
	Func pFunc;
};

template<class TBase, class TRet TEMPLATE_PARAMS>
class C_METHOD_DELEGATE : public I_DELEGATE<TRet TEMPLATE_ARGS>
{
public:
	typedef TRet (TBase::*Func)(PARAMS);

	C_METHOD_DELEGATE(TBase* base, Func func)
	{
		pBase = base;
		pFunc = func;
	}
	virtual TRet Invoke(PARAMS)
	{
		return (pBase->*pFunc)(ARGS);
	}
	virtual bool Compare(I_DELEGATE<TRet TEMPLATE_ARGS>* other)
	{
		return true;
	}
private:
	Func pFunc;
	TBase* pBase;
};

template<class TRet TEMPLATE_PARAMS>
__forceinline I_DELEGATE<TRet TEMPLATE_ARGS>* NewDelegate(TRet (*pFunc)(PARAMS))
{
	return new C_STATIC_DELEGATE<TRet TEMPLATE_ARGS>(pFunc);
}

template<class TBase, class TRet TEMPLATE_PARAMS>
__forceinline I_DELEGATE<TRet TEMPLATE_ARGS>* NewDelegate(TBase* base, TRet (TBase::*pFunc)(PARAMS))
{
	return new C_METHOD_DELEGATE<TBase, TRet TEMPLATE_ARGS>(base,pFunc);
}


template<class TRet TEMPLATE_PARAMS>
class I_EVENT
{
public:
	typedef I_DELEGATE<TRet TEMPLATE_ARGS> IDelegate;
	typedef std::list<IDelegate*> DelegateList;

	DelegateList delegates;

	void Clear()
	{
		for(DelegateList::iterator it = delegates.begin(); it != delegates.end(); it++)
		{
			delete (*it);
		}
		delegates.clear();
	}

	void AddDelegate(IDelegate* pDelegate)
	{
		if(pDelegate)
		{
			delegates.push_back(pDelegate);
		}
	}

	void Invoke(PARAMS)
	{
		for(DelegateList::iterator it = delegates.begin(); it != delegates.end(); it++)
		{
			(*it)->Invoke(ARGS);
		}
	}

	I_EVENT<TRet TEMPLATE_ARGS>& operator =(IDelegate* pDelegate)
	{
		Clear();
		AddDelegate(pDelegate);
		return *this;
	}
	I_EVENT<TRet TEMPLATE_ARGS>& operator +=(IDelegate* pDelegate)
	{
		AddDelegate(pDelegate);
		return *this;
	}

};


