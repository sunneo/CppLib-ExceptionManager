// ExceptionManager.cpp : 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include "ExceptionManager.h"
ExceptionManager __ExceptionManager;

ExceptionDataChecker::ExceptionDataChecker() :needTrigger(true), handled(0)
{

}

ExceptionDataChecker::~ExceptionDataChecker()
{
	if (handled)
	{
		return;
	}
	
	if (needTrigger)
	{
		needTrigger = false;
		__ExceptionManager.FINALLY();
	}
	handled = 1;
}
ExceptionStatus::ExceptionStatus()
{
}
void ExceptionStatus::printTraceDump(FILE* outputStream)
{
	bool startDump = false;
	char* dumpBuf=new char[4096];
	for (auto i = Parent->stack.crbegin(); i!=Parent->stack.crend(); ++i)
	{
		ExceptionStatus* ex = *i;
		if (ex == this)
		{
			startDump = true;
			getTrace(dumpBuf, 4096);
			fprintf(outputStream, "%s", dumpBuf);
			
			continue;
		}
		if (startDump)
		{
			fprintf(outputStream, "%s (From Try-%s:%d)\n", ex->function_try, ex->file_try, ex->line_try);
		}
	}
	fprintf(outputStream, "_________________________________\n");
	fprintf(outputStream, "\n");
	delete[] dumpBuf;
}
void ExceptionStatus::printTraceDump()
{
	printTraceDump(stderr);
}
void ExceptionStatus::getTrace(char* buf,int size){
	if (thrown){
		if (byType){
			sprintf_s(buf, size, "thrown from [%s:%d] %s\nError=%d\n", 
				filename, line,function,  errorType);
		}
		else{
			sprintf_s(buf, size, "thrown from [%s:%d] %s\nMessage=%s\n", 
				filename, line, function, MSG);
		}
	}
	else{
		sprintf_s(buf, size, "(NOT THROWN)\n");
	}
}
ExceptionStatus* ExceptionManager::NEWBACK(const char* file, int line, const char* fnc, ExceptionDataChecker* checker ){
	ExceptionStatus* ex = new ExceptionStatus();
	memset(ex, 0, sizeof(ExceptionStatus));
	stack.push_back(ex);
	ex->jmpbufStatus = 0;
	ex->file_try = (char*)file;
	ex->function_try = (char*)fnc;
	ex->line_try = line;
	ex->jmpbufStatus = 1;
	ex->Parent = this;
	ex->mChecker = checker;
	return ex;
}
bool ExceptionManager::TRY(const char* file,int line,const char* fnc){
	if (0 == setjmp(NEWBACK(file,line,fnc)->jbf)){
		return true;
	}
	return false;
}
int ExceptionStatus::what(){
	return this->errorType;
}
const char* ExceptionStatus::message(){
	return this->MSG;
}
bool  ExceptionManager::CATCH(ExceptionStatus** _ex){
	if (this->stack.empty()) return false;
	ExceptionStatus* top = stack.back();
	if (!top->thrown)
	{
		return false;
	}
	top->handled = true;
	*_ex = top;
	return true;
}
bool ExceptionManager::CATCH(ExceptionStatus** _ex, int type){
	if (this->stack.empty()) return false;
	ExceptionStatus* top = stack.back();
	if (!top->thrown) return false;
	if (top->byType && top->errorType != type)
	{
		return false;
	}
	top->handled = true;
	*_ex = top;
	return true;
}
void ExceptionManager::THROW_Impl(const char* message, int type, const char* file, int line, const char* fnc, bool throwType)
{
	if (this->stack.empty()){
		fprintf(stderr, "Unhandled Exception:\n");
		if (throwType){
			fprintf(stderr, "thrown from [%s:%d] %s\nError=%d\n",
				file, line, fnc, type);
		}
		else{
			fprintf(stderr, "thrown from [%s:%d] %s\nMessage=%s\n",
				file, line, fnc, message);
			//exit(-1);
		}
		return;
	}
	ExceptionStatus* ex = stack.back();
	if (ex->handled){
		// re throw
		RemoveLast();
		if (this->stack.empty()) return;
		ex = stack.back();
		if (ex == NULL) return;
	}
	strcpy_s(ex->MSG, 1024, message);
	ex->thrown = true;
	ex->function = (char*)fnc;
	ex->line = line;
	ex->filename = (char*)file;
	ex->byType = throwType;
	ex->errorType = type;
	longjmp(ex->jbf, 1);
}
void ExceptionManager::THROW(const char* message, const char* file, int line, const char* fnc){
	THROW_Impl(message, -1, file, line, fnc, false);
}
void ExceptionManager::THROW(int type, const char* message, const char* file, int line, const char* fnc){
	THROW_Impl(message, type, file, line, fnc, true);
}
void ExceptionManager::RemoveLast(){
	if (this->stack.empty()) return;
	current = stack.back();
	stack.pop_back();
	if (current != NULL)
	{
		free(current);
	}
	if (this->stack.empty())
	{
		current = NULL;
	}
	else
	{
		current = stack.back();
	}
}
bool  ExceptionManager::FINALLY(){
	if (this->stack.empty()) return false;
	ExceptionStatus* ex = stack.back();
	if (ex->mChecker != NULL)
	{	
		// if finally is presented, disable the checker
		ex->mChecker->needTrigger = false;
		ex->mChecker->handled = 1;
		ex->mChecker = NULL;
	}
	if (ex->thrown &&  !ex->handled)
	{
		stack.pop_back();
		THROW_Impl(ex->MSG, ex->errorType, ex->filename, ex->line, ex->function, ex->byType);
	}
	else
	{
		free(ex);
		stack.pop_back();
		if (stack.empty())
		{
			current = NULL;
		}
	}
	return true;
}

ExceptionManager::~ExceptionManager(){
	for (auto i = stack.crbegin(); i != stack.crend(); ++i)
	{
		ExceptionStatus* ex = *i;
		if (ex->handled) continue;
		fprintf(stderr, "Unhandled Exception:\n");
		ex->printTraceDump();
	}
	
	
}