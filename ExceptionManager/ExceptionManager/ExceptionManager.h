#ifndef EXCEPTION_MANAGER_H_
#define EXCEPTION_MANAGER_H_
#include <list>
#include <stdio.h>
#include <direct.h>
#include <limits.h>
#include <stdlib.h>
#include <setjmp.h>

class ExceptionDataChecker;
class ExceptionStatus{
private:
	friend class ExceptionManager;
	bool hasPassCatch;
	ExceptionManager* Parent;
	int jmpbufStatus;
	bool thrown;
	bool handled;
	int errorType;
	char* file_try;
	char* function_try;
	int line_try;
	int line;
	char* filename;
	char* function;
	bool byType;
	char MSG[1024];
public:
	ExceptionDataChecker* mChecker;
	jmp_buf jbf;
	ExceptionStatus();
	int what();
	const char* message();
	void getTrace(char* buf,int bufSize);
	void printTraceDump(FILE* outputStream);
	void printTraceDump();
};
class ExceptionManager{
private:
	friend class ExceptionStatus;
	std::list<ExceptionStatus*> stack;
	void RemoveLast();
	void THROW_Impl(const char* message, int type = -1, const char* file = "", int line = 0, const char* fnc = "", bool throwType = false,bool isRethrow=false);
	void UnhandledExceptionChecker(const char* message, int type, const char* file, int line, const char* fnc, bool throwType);
public:
	ExceptionStatus* NEWBACK(const char* file, int line, const char* fnc, ExceptionDataChecker* checker=NULL);
	ExceptionStatus* current;
	bool TRY(const char* file, int line, const char* fnc);
	bool CATCH(ExceptionStatus** _ex);
	bool CATCH(ExceptionStatus** _ex, int type);
	void THROW(const char* message, const char* file = "", int line = 0, const char* fnc = "");
	void THROW(int type, const char* msg, const char* file = "", int line = 0, const char* fnc = "");
	bool FINALLY();
	virtual ~ExceptionManager();
};
class ExceptionDataChecker{
public:
	int handled;
	bool needTrigger;
	ExceptionDataChecker();
	~ExceptionDataChecker();
};
extern ExceptionManager __ExceptionManager;
#define __try  ExceptionDataChecker ____private_##__LINE__##exceptionDataChecker; if (0 == setjmp(__ExceptionManager.NEWBACK(__FILE__,__LINE__,__FUNCTION__,&____private_##__LINE__##exceptionDataChecker)->jbf))
#define __catch_s(X) if(__ExceptionManager.CATCH(&__ExceptionManager.current)) if(ExceptionStatus* X=(__ExceptionManager.current))
#define __catch_t(ERROR_TYPE) if(__ExceptionManager.CATCH(&__ExceptionManager.current,(ERROR_TYPE)))
#define __catch_st(X,ERROR_TYPE) if(__ExceptionManager.CATCH(&__ExceptionManager.current,(ERROR_TYPE))) if(ExceptionStatus* X=(__ExceptionManager.current))
#define __throw_s(MSG) __ExceptionManager.THROW((MSG),__FILE__,__LINE__,__FUNCTION__)
#define __throw_t(TYPE) __ExceptionManager.THROW((TYPE),"", __FILE__,__LINE__,__FUNCTION__)
#define __throw_st(MSG,TYPE) __ExceptionManager.THROW((TYPE),(MSG), __FILE__,__LINE__,__FUNCTION__)
#define __finally if(__ExceptionManager.FINALLY())
#endif
