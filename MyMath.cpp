/*

MyMath consists of math helper functions
For simplicity of computing some function

*/

#ifndef __MYLIB_MYMATH__

#define __MYLIB_MYMATH__

#include <algorithm>

using namespace std;

namespace MyMath {

	int log2(int);
	long long log2(long long);

};

int MyMath::log2(int x) {
	return 31 - __builtin_clz(x);
}
long long MyMath::log2(long long x) {
	return 63 - __builtin_clzll(x);
}

#endif