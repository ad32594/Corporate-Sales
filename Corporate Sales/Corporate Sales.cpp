// Corporate Sales.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <stdexcept>

using namespace std;

class DivSales {
private:
    double* sales;       // Dynamically allocated 1D array of sales for this division
    int numPeriods;      // Number of Periods
    static double corpSales; // Total Corporate Sales

public:
    // Default constructor (needed for creating the 1D array of DivSales objects)
    DivSales() : sales(nullptr), numPeriods(0) {}

    // Destructor so it doesn't Overstay its Welcome.
    ~DivSales() {
        delete[] sales;
    }

    // Copy Constructor (It do go Down (Deep))
    DivSales(const DivSales& other) : numPeriods(other.numPeriods) {
        if (other.sales != nullptr && numPeriods > 0) {
            sales = new double[numPeriods];
            for (int i = 0; i < numPeriods; ++i) {
                sales[i] = other.sales[i];
            }
        }
        else {
            sales = nullptr;
        }
    }

    // Copy Assignment Operator (Deep Copy + Self-Assignment Check)
    DivSales& operator=(const DivSales& other) {
        if (this != &other) { // Prevents self-assignment
            delete[] sales;   // Free up existing memory

            numPeriods = other.numPeriods;
            if (other.sales != nullptr && numPeriods > 0) {
                sales = new double[numPeriods];
                for (int i = 0; i < numPeriods; ++i) {
                    sales[i] = other.sales[i];
                }
            }
            else {
                sales = nullptr;
            }
        }
        return *this;
    }

    // Allocates the dynamic array based on the runtime period count
    void allocatePeriods(int periods) {
        if (periods <= 0) {
            throw invalid_argument("Get those Periods Up my Friend... Hold On that came out wrong...");
        }
        delete[] sales; // Cleans things up if they were previously allocated
        numPeriods = periods;
        sales = new double[numPeriods] {0.0};
    }

    // Bounds-checked subscript operator (non-const for setting/modifying)
    double& operator[](int index) {
        if (sales == nullptr || index < 0 || index >= numPeriods) {
            throw out_of_range("You get back in Bounds RIGHT THIS SECOND YOUNG MAN!!!");
        }
        return sales[index];
    }

    // Bounds-checked subscript operator (const for reading)
    const double& operator[](int index) const {
        if (sales == nullptr || index < 0 || index >= numPeriods) {
            throw out_of_range("You get back in Bounds RIGHT THIS SECOND YOUNG MAN!!!");
        }
        return sales[index];
    }

    // Helper to add a value to static corporate sales
    static void addToCorpSales(double amount) {
        corpSales += amount;
    }

    // Static getter for total corporate sales
    static double getCorpSales() {
        return corpSales;
    }

    // Resets corporate sales (useful when processing multiple files in one run)
    static void resetCorpSales() {
        corpSales = 0.0;
    }

    // Calculates the total sales for this specific division
    double getDivisionTotal() const {
        double total = 0.0;
        for (int i = 0; i < numPeriods; ++i) {
            total += sales[i];
        }
        return total;
    }

    int getNumPeriods() const {
        return numPeriods;
    }
};

// Initialize static member variable
double DivSales::corpSales = 0.0;

// Function to load, validate, and display sales data from a file
bool processSalesFile(const string& filename, DivSales divisions[], int numDivisions) {
    ifstream inFile(filename);

    // 1. File-open Validation
    if (!inFile.is_open()) {
        cerr << "Erm, Are you Sure this File: " << filename << " Exists?" << endl;
        return false;
    }

    // Reset corporate sales before processing a new file
    DivSales::resetCorpSales();

    int periods = 0;
    // Read and validate the period count (first value in file)
    if (!(inFile >> periods) || periods <= 0) {
        cerr << "Erm, Are you Sure this File: " << filename << " Has a Period Count?" << endl;
        inFile.close();
        return false;
    }

    // Allocate periods for all 6 divisions
    for (int d = 0; d < numDivisions; ++d) {
        divisions[d].allocatePeriods(periods);
    }

    // Nested loop file loading using operator[]
    for (int d = 0; d < numDivisions; ++d) {
        for (int p = 0; p < periods; ++p) {
            double val;
            inFile >> val;

            // 2. Data Count Validation (premature EOF)
            if (inFile.eof() && inFile.fail()) {
                cerr << "Not Enough Numbers to work with this File: " << filename
                    << " Gotta Have at Least " << (numDivisions * periods) << " Sales Figures.";
                inFile.close();
                return false;
            }

            // 3. Numeric Value Validation (non-numeric characters encountered)
            if (inFile.fail()) {
                cerr << "Just so you know, There's a Faulty Value in File: " << filename
                    << " Should be Division " << (d + 1) << " and Period " << (p + 1) << "." << endl;
                inFile.close();
                return false;
            }

            // 4. Positive Value Validation (negative sales check)
            if (val < 0.0) {
                cerr << "We got a Negative Sales Value (" << val << ") In File: " << filename
                    << " Division " << (d + 1) << " and Period " << (p + 1) << " I Believe." << endl;
                inFile.close();
                return false;
            }

            // Store via bounds-checked operator[] and update corporate total
            divisions[d][p] = val;
            DivSales::addToCorpSales(val);
        }
    }

    // Check if extra trailing data exists in the file
    double extraVal;
    if (inFile >> extraVal) {
        cerr << "Found Some Extra Data Trailing Around in File: " << filename << " We Ignored it For You Though!" << endl;
    }

    inFile.close();

    // Displays the Sales Report
    cout << "Sales Report for: " << filename << " (" << periods << " periods)\n";
    cout << fixed << setprecision(2);

    for (int d = 0; d < numDivisions; ++d) {
        cout << "Division " << (d + 1) << " Total: $" << setw(10) << divisions[d].getDivisionTotal() << endl;
    }
    cout << "Corporate Total:  $" << setw(10) << DivSales::getCorpSales() << endl;

    return true;
}

// Function to verify deep copies via Copy Constructor and Assignment Operator
void runIndependenceTests(const DivSales& original) {
    cout << "Deep Copy Independence Tests Coming Through!!!" << endl;
    if (original.getNumPeriods() == 0) {
        cout << "Nah, Nevermind, There's no Periods to Work With here." << endl;
        return;
    }

    double originalFirstPeriod = original[0];

    // 1. Test Copy Constructor
    DivSales copyConstructed(original);
    copyConstructed[0] += 5000.00; // Modify the copy

    cout << "Copy Constructor Test: " << endl;
    cout << "  Original Period 1: $" << original[0] << " (Expected: $" << originalFirstPeriod << ")" << endl;
    cout << "  Copy     Period 1: $" << copyConstructed[0] << " (Modified +$5000.00)" << endl ;
    cout << "  Status: " << (original[0] == originalFirstPeriod ? "PASSED (Independent)" : "FAILED (Shallow Copy)") << endl;

    // 2. Test Copy Assignment Operator
    DivSales copyAssigned;
    copyAssigned = original;
    copyAssigned[0] += 9999.00; // Modify the assigned object

    // Test Self-Assignment safety
    copyAssigned = copyAssigned;

    cout << "Copy Assignment Operator Test:" << endl;
    cout << "  Original Period 1: $" << original[0] << " (Expected: $" << originalFirstPeriod << ")" << endl;
    cout << "  Assigned Period 1: $" << copyAssigned[0] << " (Modified +$9999.00 + Self-Assigned)" << endl;
    cout << "  Status: " << (original[0] == originalFirstPeriod ? "PASSED (Independent)" : "FAILED (Shallow Copy)") << endl;
}

//This is where it All Comes Together
int main() {
    const int NUM_DIVISIONS = 6;
    DivSales divisions[NUM_DIVISIONS]; // 1D array of 6 DivSales objects

    // Process Quarterly Sales File
    if (processSalesFile("Corporate_Sales_Quarterly_Test_Data.txt", divisions, NUM_DIVISIONS)) {
        runIndependenceTests(divisions[0]);
    }

    // Process Monthly Sales File
    if (processSalesFile("Corporate_Sales_Monthly_Test_Data.txt", divisions, NUM_DIVISIONS)) {
        runIndependenceTests(divisions[0]);
    }

    return 0;
}
