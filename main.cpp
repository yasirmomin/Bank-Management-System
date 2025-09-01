#include <bits/stdc++.h>
using namespace std;

class Customer
{
private:
    int customerID;
    string name;
    string email;
    string phone;
    string passwordHash;              // for now store plain text, later hash it
    vector<long long> accountNumbers; // list of account numbers

public:
    // Constructor to initialize a customer
    Customer(int id, const string &n, const string &e,
             const string &p, const string &ph)
        : customerID(id), name(n), email(e),
          passwordHash(p), phone(ph) {}
    // Getters
    int getID() const { return customerID; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    // Password check
    bool checkPassword(const string &plain) const
    {
        return plain == passwordHash;
    }
    // Link new account
    void addAccount(long long accNo)
    {
        accountNumbers.push_back(accNo);
    }
    // Get list of accounts
    const vector<long long> &getAccounts() const
    {
        return accountNumbers;
    }
};

class Account
{
protected:
    long long accountNumber;
    double balance;

public:
    Account(long long accNo, double bal) : accountNumber(accNo), balance(bal) {}
    // Common function for all accounts
    void deposit(double amount)
    {
        balance += amount;
        cout << "Amount Deposited: " << amount << "\n";
    }
    // Pure virtual function (abstract)
    virtual void withdraw(double amount) = 0;
    // Getter
    double getBalance() const
    {
        return balance;
    }
    long long getAccountNumber() const
    {
        return accountNumber;
    }
    // Virtual destructor (important for abstract base)
    virtual ~Account() {}
};

class SavingsAccount : public Account
{
public:
    // Constructor
    SavingsAccount(long long accNo, double bal)
        : Account(accNo, bal) {}
    // Withdraw rule: no overdraft
    void withdraw(double amount) override
    {
        if (amount <= balance)
        {
            balance -= amount;
            cout << "Withdrawn: " << amount << " | New Balance: " << balance << endl;
        }
        else
        {
            cout << "Insufficient balance!" << endl;
        }
    }
};

class CurrentAccount : public Account
{
private:
    double overdraftLimit = 100000; // extra money allowed to withdraw beyond balance

public:
    // Constructor
    CurrentAccount(long long accNo, double bal)
        : Account(accNo, bal) {}
    // Withdraw rule: overdraft allowed
    void withdraw(double amount) override
    {
        if (amount <= balance + overdraftLimit)
        {
            balance -= amount;
            cout << "Withdrawn: " << amount
                 << " | New Balance: " << balance << endl;
        }
        else
        {
            cout << "Withdrawal denied! Overdraft limit exceeded." << endl;
        }
    }
};

class Bank
{
private:
    unordered_map<int, Customer *> customers;
    unordered_map<long long, Account *> accounts;
    unordered_map<string, int> emailToID;
    unordered_map<string, int> phoneToID;
    int nextCustomerID = 1;
    long long nextAccountNumber = 10000001;

public:
    // Add customer with uniqueness check
    int addCustomer(const string &name, const string &email,
                    const string &phone, const string &password)
    {

        if (emailToID.find(email) != emailToID.end())
        {
            cout << "❌ Email already registered!\n";
            return -1;
        }
        if (phoneToID.find(phone) != phoneToID.end())
        {
            cout << "❌ Phone number already registered!\n";
            return -1;
        }

        int id = nextCustomerID++;
        Customer *cust = new Customer(id, name, email, password, phone);
        customers[id] = cust;
        emailToID[email] = id;
        phoneToID[phone] = id;

        cout << "✅ Customer registered successfully. ID: " << id << "\n";
        return id;
    }

    int login(const string &email, const string &password)
    {
        if (emailToID.find(email) == emailToID.end())
        {
            cout << "❌ Email not registered!\n";
            return -1;
        }
        int id = emailToID[email];
        if (!customers[id]->checkPassword(password))
        {
            cout << "❌ Wrong password!\n";
            return -1;
        }
        cout << "✅ Login successful! Welcome " << customers[id]->getName() << "\n";
        return id;
    }

    long long openAccount(int customerId, const string &type, double initialBalance)
    {
        if (customers.find(customerId) == customers.end())
        {
            cout << "❌ Customer not found!\n";
            return -1;
        }

        long long accNo = nextAccountNumber++;

        if (type == "savings")
        {
            accounts[accNo] = new SavingsAccount(accNo, initialBalance);
        }
        else if (type == "current")
        {
            accounts[accNo] = new CurrentAccount(accNo, initialBalance);
        }
        else
        {
            cout << "❌ Invalid account type!\n";
            return -1;
        }

        customers[customerId]->addAccount(accNo);
        cout << "✅ Account opened successfully. Account Number: " << accNo << "\n";
        return accNo;
    }

    void deposit(int customerId, long long accNo, double amount)
    {
        if (!validateCustomerAccount(customerId, accNo))
            return;
        accounts[accNo]->deposit(amount);
    }

    void withdraw(int customerId, long long accNo, double amount)
    {
        if (!validateCustomerAccount(customerId, accNo))
            return;
        accounts[accNo]->withdraw(amount);
    }

    void checkBalance(int customerId, long long accNo)
    {
        if (!validateCustomerAccount(customerId, accNo))
            return;
        cout << "Balance for Account " << accNo
             << " is: " << accounts[accNo]->getBalance() << endl;
    }

    string getCustomerName(int customerId) {
    if(customers.find(customerId) != customers.end()) {
        return customers[customerId]->getName();
    }
    return "";
}

private:
    bool validateCustomerAccount(int customerId, long long accNo)
    {
        if (customers.find(customerId) == customers.end())
        {
            cout << "❌ Customer not found!\n";
            return false;
        }
        const auto &accs = customers[customerId]->getAccounts();
        if (find(accs.begin(), accs.end(), accNo) == accs.end())
        {
            cout << "❌ This account does not belong to you!\n";
            return false;
        }
        if (accounts.find(accNo) == accounts.end())
        {
            cout << "❌ Account not found!\n";
            return false;
        }
        return true;
    }
};

void customerMenu(Bank &bank, int customerId) {
    int choice;
    do {
        cout << "\n===== CUSTOMER MENU =====\n";
        cout << "1. Open Account\n";
        cout << "2. Deposit\n";
        cout << "3. Withdraw\n";
        cout << "4. Check Balance\n";
        cout << "5. Logout\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: {
                string type;
                double initialBalance;
                cout << "Enter account type (savings/current): ";
                cin >> type;
                cout << "Enter initial deposit: ";
                cin >> initialBalance;
                bank.openAccount(customerId, type, initialBalance);
                break;
            }

            case 2: {
                long long accNo;
                double amt;
                cout << "Enter account number: ";
                cin >> accNo;
                cout << "Enter deposit amount: ";
                cin >> amt;
                bank.deposit(customerId, accNo, amt);
                break;
            }

            case 3: {
                long long accNo;
                double amt;
                cout << "Enter account number: ";
                cin >> accNo;
                cout << "Enter withdrawal amount: ";
                cin >> amt;
                bank.withdraw(customerId, accNo, amt);
                break;
            }

            case 4: {
                long long accNo;
                cout << "Enter account number: ";
                cin >> accNo;
                bank.checkBalance(customerId, accNo);
                break;
            }

            case 5:
                cout << "Logging out...\n";
                break;

            default:
                cout << "❌ Invalid choice!\n";
        }

    } while (choice != 5);
}


int main()
{
    Bank myBank;
    int choice;

    do
    {
        cout << "\n===== BANK MANAGEMENT SYSTEM =====\n";
        cout << "1. Register Customer\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1: {
    string name, email, phone, password;
    cout << "\nEnter Name: "; cin >> ws; getline(cin, name);
    cout << "Enter Email: "; cin >> email;
    cout << "Enter Phone: "; cin >> phone;
    cout << "Enter Password: "; cin >> password;

    int id = myBank.addCustomer(name, email, phone, password);
    if (id != -1) {
        cout << "Welcome, " << name << "! You have been registered successfully.\n";
    }
    break;
}


        case 2: {
    string email, password;
    cout << "\nEnter Email: ";
    cin >> email;
    cout << "Enter Password: ";
    cin >> password;

    int customerId = myBank.login(email, password);
    if (customerId != -1) {
        cout << "Hello, " << myBank.getCustomerName(customerId) << "! You are now logged in.\n";
        customerMenu(myBank, customerId);  
    }
    break;
}


        case 3:
            cout << "Exiting...\n";
            break;
        default:
            cout << "❌ Invalid choice! Try again.\n";
        }

    } while (choice != 3);

    return 0;
}
