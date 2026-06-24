// Code for Project 1
// Report poll results

#include <iostream>
using namespace std;

int main()
{
	int numberSurveyed;
	int approve;
	int disapprove;

	cout << "How many people were surveyed? ";
	cin >> numberSurveyed;
	cout << "How many of them approve of the way the President is handling his job? ";
	cin >> approve;
	cout << "How many of them disapprove of the way the President is handling his job? ";
	cin >> disapprove;

	double pctApprove = approve / numberSurveyed * 100;
	double pctDisapprove = 100 * disapprove / numberSurveyed;

	cout.setf(ios::fixed);
	cout.precision(1);

	cout << endl;
	cout << pctApprove << "% say they approve." << endl;
	cout << pctDisapprove << "% say they disapprove." << endl;

	if (approve > disapprove)
		cout << "More people approve than disapprove." << endl;
	else
		cout << "More people disapprove than approve." << endl;
}