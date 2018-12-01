#include "stdafx.h"
#include "ExceptionManager.h"

static int counter_fnc1 = 0;

static void fnc1()
{
	if (counter_fnc1 > 101)
	{
		//__throw_s("func1.counter>101");
		__throw_t(1025);
	}
	++counter_fnc1;
}

static int counter_fnc2 = 0;
static void fnc2(){
	__try{
		for (int i = 0; i < 10; ++i)
		{
			fnc1();
		}
	}
	__catch_st(ex,1024)
	{
		ex->printTraceDump();
	}
	__finally;
	if (counter_fnc2 > 5)
	{
		__throw_s("Fnc2.counter>10");
	}
	++counter_fnc2;
}
static void fnc3(){
	__try
	{
		for (int i = 0; i < 15; ++i)
		{
			__try
			{
				fnc2();
			}
			__catch_st(ex,1024)
			{
				ex->printTraceDump();
			}
			__finally;
		}
		fnc1();
	}
	__catch_st(ex,1024)
	{
		ex->printTraceDump();
	}
	__finally;
}
int _tmain(int argc, _TCHAR* argv[])
{
	fnc3();
	getchar();
	return 0;
}
