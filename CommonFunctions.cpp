#include "common.h"

const wchar_t* convert(const char* input){
	#define MAXSIZE 1024
	size_t origsize = strlen(input) + 1;
	static wchar_t wcstring[MAXSIZE];
	mbstowcs(wcstring, input, origsize);
	wcscat(wcstring, L"");
	return wcstring;
	#undef MAXSIZE
}

char* convert(wchar_t* input){
	char* tmp=new char();
	wcstombs(tmp,input,0);
	return tmp;
}