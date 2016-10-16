/*
$Id: tunumxx.cpp $

Description: Test file

$Log: $
*/

#include <iostream> // cout, endl

#include "unumxx.h"

using namespace std;

int main(int argc, char** argv)
{
#if 1
	set_uenv(3, 4); /* make sure the environment is set first */
	ubnd_c a, b, c, d(2);
	short sa;

	a = 1234;
	b = "-5678";
	c = a + b;
	cout << "sum is " << c << endl;
	cout << "absolute value is " << abs(c) << endl;
	cout << "square root of 2 value is " << sqrt(d) << endl;
	sa = 3;
	c = a - sa;
	cout << "a - sa is " << c << endl;
	c = sa + a;
	cout << "sa + a is " << c << endl;
	if (a < b) cout << "a < b" << endl;
	else cout << "a >= b" << endl;
	if (a > sa) cout << "a > sa" << endl;
	else cout << "a <= sa" << endl;
	cout << "-c is " << -c << endl;
	d += a;
	d /= 2;
	cout << "d is " << d << endl;
#endif
	set_uenv(4, 6);
	ubnd_c sum = 0.0;
	for (int i = 0; i < 10; i++) {
		sum += ubnd_c("0.1");
		cout << "sum is " << sum << endl;
		uview_ub(sum.get_ubnd_t()); cout << endl;
	}
	return EXIT_SUCCESS;
}
