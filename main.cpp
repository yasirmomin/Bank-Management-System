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

struct Transaction {
    string type;     // deposit,withdrawal,transfer 
    double amount;   
    double balanceAfter; 
    string details;
};

class Account
{
protected:
    long long accountNumber;
    double balance;
    vector<Transaction> transactions; 
public:
    Account(long long accNo, double bal) : accountNumber(accNo), balance(bal) {}
    // Common function for all accounts
    void deposit(double amount,long long fromAcc)
    {
        balance += amount;
        cout << "Amount Deposited: " << amount << "\n";
        string method="Self Deposit";
        if(fromAcc!=0)
        method="Received From " + to_string(fromAcc);
        transactions.push_back({"Deposit", amount, balance, method});
    }
    // Pure virtual function (abstract)
    virtual void withdraw(double amount, long long AccNo) = 0;
    // Getter
    double getBalance() const
    {
        return balance;
    }
    long long getAccountNumber() const
    {
        return accountNumber;
    }
    void showTransactions() const {
    cout << "\nTransaction History for Account " << accountNumber << ":\n";
    for (const auto &t : transactions) {
        cout << t.type << " | Amount: " << t.amount << " | Balance after: " << t.balanceAfter << " | Details: "<< t.details <<"\n";
    }
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
    void withdraw(double amount,long long AccNo) override
    {
        if (amount <= balance)
        {
            balance -= amount;
            cout << "Withdrawn: " << amount << " | New Balance: " << balance << endl;
            string method="Self Withdrawn";
            if(AccNo!=0)
            method = "Transferred to " + to_string(AccNo);
            transactions.push_back({"Withdraw", amount, balance, method});
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
    void withdraw(double amount, long long AccNo) override
    {
        if (amount <= balance + overdraftLimit)
        {
            balance -= amount;
            cout << "Withdrawn: " << amount << " | New Balance: " << balance << endl;
            string method="Self Withdrawn";
            if(AccNo!=0)
            method = "Transferred to " + to_string(AccNo);
            transactions.push_back({"Withdraw", amount, balance,method});
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
        if (amount <= 0) {
            cout << "❌ Deposit amount must be positive!\n";
            return;
        }
        if (!validateCustomerAccount(customerId, accNo))
            return;

        accounts[accNo]->deposit(amount, 0);
    }

    void withdraw(int customerId, long long accNo, double amount)
    {
        if (amount <= 0) {
            cout << "❌ Withdrawal amount must be positive!\n";
            return;
        }
        if (!validateCustomerAccount(customerId, accNo))
            return;
        accounts[accNo]->withdraw(amount,0);
    }

    void checkBalance(int customerId, long long accNo)
    {
        if (!validateCustomerAccount(customerId, accNo))
            return;
        cout << "Balance for Account " << accNo
             << " is: " << accounts[accNo]->getBalance() << endl;
    }

    bool getAccountTransactions(int customerId, long long accNo) {
        if (!validateCustomerAccount(customerId, accNo))
            return false;

        accounts[accNo]->showTransactions();
        return true;
    }   

    string getCustomerName(int customerId) {
        if(customers.find(customerId) != customers.end()) {
            return customers[customerId]->getName();
        }
        return "";
    }

    bool transferFunds(int customerId, long long fromAcc, long long toAcc, double amount) {
        if (amount <= 0) {
            cout << "❌ Transfer amount must be positive!\n";
            return false;
        }
        if (!validateCustomerAccount(customerId, fromAcc)) 
            return false;

        if (fromAcc == toAcc) {
            cout << "❌ Cannot transfer to the same account!\n";
            return false;
        }
        if (accounts.find(toAcc) == accounts.end()) {
            cout << "❌ Target account not found!\n";
            return false;
        }


        Account* from = accounts[fromAcc];
        Account* to = accounts[toAcc];

        if (from->getBalance() >= amount) {
            from->withdraw(amount, toAcc);
            to->deposit(amount, fromAcc);

            cout << "✅ Transferred " << amount
                << " from Account " << fromAcc
                << " to Account " << toAcc << endl;
            return true;
        } else {
            cout << "❌ Insufficient funds!\n";
            return false;
        }
    }
        void listCustomerAccounts(int customerId) {
            if (customers.find(customerId) == customers.end()) {
                cout << "❌ Customer not found!\n";
                return;
            }
            const auto &accs = customers[customerId]->getAccounts();
            if (accs.empty()) {
                cout << "❌ No accounts found .\n";
                return;
            }
            cout << "\n--- Your Accounts ---\n";
            for (auto accNo : accs) {
                cout << "Account Number: " << accNo
                    << " | Balance: " << accounts[accNo]->getBalance() << "\n";
            }
        }

        ~Bank() {
            for (auto &p : customers) {
                delete p.second;
            }
            for (auto &p : accounts) {
                delete p.second;
            }
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
        cout << "5. View Transactions\n";
        cout << "6. Transfer Funds\n";
        cout << "7. View Your Accounts\n";
        cout << "8. Logout\n";
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
            case 5: {
                long long accNo;
                cout << "Enter account number: ";
                cin >> accNo;
                bank.getAccountTransactions(customerId,accNo);
                break;

            }
            case 6: {
                long long fromAcc, toAcc;
                double amt;
                cout << "Enter Your Account Number: ";
                cin >> fromAcc;
                cout << "Enter Recipient Account Number: ";
                cin >> toAcc;
                cout << "Enter Amount to Transfer: ";
                cin >> amt;
                bank.transferFunds(customerId,fromAcc, toAcc, amt);
                break;
            }
            case 7: {
                bank.listCustomerAccounts(customerId);
                break;
            }
            case 8:
                cout << "Logging out...\n";
                break;

            default:
                cout << "❌ Invalid choice!\n";
        }

    } while (choice != 8);
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


        case 3:{
            cout << "Exiting...\n";
            break;
        }
        default:
            cout << "❌ Invalid choice! Try again.\n";
        }

    } while (choice != 3);

    return 0;
}
