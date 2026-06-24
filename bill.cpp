#include <iostream>
#include <string>
using namespace std;

int main()
{
	double initialMeter;
	double finalMeter;
	int month;
	string name;
	cout << "Initial meter reading: ";
	cin >> initialMeter;
	cout << "Final meter reading: ";
	cin >> finalMeter;
	cin.ignore(10000, '\n');
	cout << "Customer name: ";
	getline(cin, name);
	cout << "Month number (1=Jan, 2=Feb, etc.): ";
	cin >> month;
	cout << "---" << endl;
	double meterDiff = (finalMeter - initialMeter);
	double firstTierRate = 5.41; // first tier rate
	double highTierRate = 9.89; // second tier rate for high season
	double lowTierRate = 7.77; // second tier rate for low season
	double bill = 0; // total for bill
	if ((month <= 10) && (month >= 4)) // april thru oct, high season
	{
		if (meterDiff <= 23)
			bill = meterDiff * firstTierRate;
		else
			bill = (23 * firstTierRate) + ((meterDiff - 23) * highTierRate);
	}
	else
	{
		if (meterDiff <= 15)
			bill = meterDiff * firstTierRate;
		else
			bill = (15 * firstTierRate) + ((meterDiff - 15) * lowTierRate);
	}

	cout.setf(ios::fixed);
	cout.precision(2);
	if (initialMeter < 0)
		cout << "The initial meter reading must not be negative.";
	else if (finalMeter < initialMeter)
		cout << "The final meter reading must not be less than the initial reading.";
	else if (name.empty()) // checks if name is empty
		cout << "You must enter a customer name.";
	else if (month < 1 || month > 12)
		cout << "The month number must be in the range 1 through 12.";
	else
		cout << "The bill for " << name << " is $" << bill << endl;
}