#include <bits/stdc++.h>
using namespace std;
int main() {
    int choice;
    do {
        cout << "\n===== Bank Management System =====\n";
        cout << "1. Customer Login\n";
        cout << "2. Admin Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice) {
            case 1:
                cout << "Customer Login selected\n";
                break;
            case 2:
                cout << "Admin Login selected\n";
                break;
            case 3:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Try again.\n";
        }

    } while(choice != 3);

    return 0;
}
