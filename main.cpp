#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <limits>

using namespace std;

// User class
class User {
private:
    string username;
    string password;
    string fullName;
    string email;
    bool isAdmin;
    string walletId;
    double balance;

    // Simple password hash function
    string hashPassword(const string& pwd) const {
        string hash = "";
        for(int i = 0; i < pwd.length(); i++) {
            hash += pwd[i] + 1; // Simple shift by 1
        }
        return hash;
    }

    // Generate simple wallet ID
    string generateWalletId() const {
        string id = "W";
        for(int i = 0; i < 6; i++) {
            id += '0' + (rand() % 10);
        }
        return id;
    }

public:
    User(const string& uname, const string& pwd, 
         const string& name, const string& mail, const double& balnc ,bool admin = false) {
        username = uname;
        password = pwd;
        fullName = name;
        email = mail;
        isAdmin = admin;
        balance = balnc;
        walletId = generateWalletId();
    }

    // Getters
    string getUsername() const { return username; }
    string getFullName() const { return fullName; }
    string getPassword() const { return password; }
    string getEmail() const { return email; }
    bool getIsAdmin() const { return isAdmin; }
    string getWalletId() const { return walletId; }
    double getBalance() const { return balance; }

    // Password verification
    bool verifyPassword(const string& pwd) const {
        return password == pwd;
    }

    // Change password
    void changePassword(const string& newPwd) {
        password = newPwd;
    }

    // Wallet operations
    bool transferPoints(double amount) {
        if (amount <= 0) return false;
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    void addPoints(double amount) {
        if (amount > 0) {
            balance += amount;
        }
    }
    
    void printUserInfo() {
    	cout << "username: " +  username + "\n";
    	cout << "fullname: " + fullName + "\n";
    	cout << "email: " + email + "\n";
    	cout << "balance: " << balance << "\n";
	}
};

// System Manager class
class SystemManager {
private:
    User* users[100]; // Array of user pointers
    int userCount;
    double masterBalance;

    // Save data to file
    bool saveToFile() {
        ofstream file("users.dat");
        if (!file.is_open()) {
            cout << "Error: Cannot open file for writing\n";
            return false;
        }

        for(int i = 0; i < userCount; i++) {
            file << users[i]->getUsername() << ","
            	 << users[i]->getPassword() << ","
                 << users[i]->getFullName() << ","
                 << users[i]->getEmail() << ","
                 << users[i]->getWalletId() << ","
                 << users[i]->getBalance() << "\n";
        }
        return true;
    }

    // Load data from file
    bool loadFromFile() {
        ifstream file("users.dat");
        if (!file.is_open()) {
            cout << "No existing user data found. Starting with empty system.\n";
            return false;
        }

        string line;
        while (getline(file, line) && userCount < 100) {
            string username, password, fullName, email, walletId;
            double balance;
            int pos = 0;
            
            // Parse CSV line
            pos = line.find(',');
            if (pos == string::npos) continue;
            username = line.substr(0, pos);
            line = line.substr(pos + 1);
            
             pos = line.find(',');
            if (pos == string::npos) continue;
            username = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            pos = line.find(',');
            if (pos == string::npos) continue;
            fullName = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            pos = line.find(',');
            if (pos == string::npos) continue;
            email = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            pos = line.find(',');
            if (pos == string::npos) continue;
            walletId = line.substr(0, pos);
            line = line.substr(pos + 1);
            
            balance = atof(line.c_str());
            
            users[userCount++] = new User(username, username, fullName, email, balance);
        }
        return true;
    }

public:
    SystemManager() : userCount(0), masterBalance(0.0) {
        srand(time(0)); // Initialize random seed
        loadFromFile();
    }

    ~SystemManager() {
        for(int i = 0; i < userCount; i++) {
            delete users[i];
        }
    }

    // Register new user
    bool registerUser(const string& username, const string& password,
                     const string& fullName, const string& email) {
        if (userCount >= 100) {
            cout << "Error: Maximum number of users reached\n";
            return false;
        }
        
        // Validate input
        if (username.empty() || password.empty() || fullName.empty() || email.empty()) {
            cout << "Error: All fields are required\n";
            return false;
        }
        
        // Check if username exists
        for(int i = 0; i < userCount; i++) {
            if (users[i]->getUsername() == username) {
                cout << "Error: Username already exists\n";
                return false;
            }
        }
        
        users[userCount++] = new User(username, password, fullName, email, 10.0);
        if (!saveToFile()) {
            cout << "Error: Failed to save user data\n";
            return false;
        }
        return true;
    }

    // Login
    bool login(const string& username, const string& password) {
        if (username.empty() || password.empty()) {
            cout << "Error: Username and password are required\n";
            return false;
        }

        for(int i = 0; i < userCount; i++) {
            if (users[i]->getUsername() == username) {
                return users[i]->verifyPassword(password);
            }
        }
        return false;
    }
    
    User* findUser(const string& userName) {
    	User* user = NULL;
    	
    	for(int i = 0; i < userCount; i++) {
            if (users[i]->getUsername() == userName) {
                user = users[i];
            }
        }
    	return user;
	}

    // Transfer points
    bool transferPoints(const string& fromUsername, const string& toUsername,
                       double amount) {
        if (amount <= 0) {
            cout << "Error: Amount must be positive\n";
            return false;
        }

        User* fromUser = NULL;
        User* toUser = NULL;
        
       
        for(int i = 0; i < userCount; i++) {
            if (users[i]->getUsername() == fromUsername) {
                fromUser = users[i];
            }
            if (users[i]->getUsername() == toUsername) {
                toUser = users[i];
            }
        }
        
        if (!fromUser || !toUser) {
            cout << "Error: One or both users not found\n";
            return false;
        }
        
        if (fromUser->transferPoints(amount)) {
            toUser->addPoints(amount);
            if (!saveToFile()) {
                cout << "Error: Failed to save transaction\n";
                return false;
            }
            return true;
        }
        cout << "Error: Insufficient balance\n";
        return false;
    }

    // Get user balance
    double getUserBalance(const string& username) const {
        for(int i = 0; i < userCount; i++) {
            if (users[i]->getUsername() == username) {
                return users[i]->getBalance();
            }
        }
        return -1.0;
    }

    // Change password
    bool changePassword(const string& username, const string& oldPassword,
                       const string& newPassword) {
        for(int i = 0; i < userCount; i++) {
            if (users[i]->getUsername() == username) {
                if (users[i]->verifyPassword(oldPassword)) {
                    users[i]->changePassword(newPassword);
                    if (!saveToFile()) {
                        cout << "Error: Failed to save password change\n";
                        return false;
                    }
                    return true;
                }
                return false;
            }
        }
        return false;
    }
};

// Show main menu
void showMainMenu() {
    cout << "\n=== ACCOUNT AND REWARDS WALLET MANAGEMENT SYSTEM ===\n";
    cout << "1. Login\n";
    cout << "2. Register new account\n";
    cout << "3. Exit\n";
    cout << "Choose function (1-3): ";
}

// Show user menu
void showUserMenu() {
    cout << "\n=== USER MENU ===\n";
    cout << "1. View Account Information\n";
    cout << "2. Change Password\n";
    cout << "3. View Wallet Balance\n";
    cout << "4. Transfer Points\n";
    cout << "0. Logout\n";
    cout << "Choose function (0-4): ";
}

void showAdminmenu() {
	cout << "\n=== ADMIN MENU ===\n";
    cout << "1. View All Account Information\n";
    cout << "2. Register New Account\n";
    cout << "3. Edit Account Infomation\n";
    cout << "0. Logout\n";
    cout << "Choose function (0-3): ";
}


// Handle registration
void handleRegistration(SystemManager& system) {
    string username, password, fullName, email;
    
    cout << "\n=== NEW ACCOUNT REGISTRATION ===\n";
    
    cout << "Enter username: ";
    cin >> username;
    
    cout << "Enter password: ";
    cin >> password;
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Enter full name: ";
    getline(cin, fullName);
    
    cout << "Enter email: ";
    cin >> email;
    
    if (system.registerUser(username, password, fullName, email)) {
        cout << "Registration successful!\n";
    } else {
        cout << "Registration failed. Please try again.\n";
    }
}

// Handle user menu
void handleUserMenu(SystemManager& system, const string& username, User* loginUser) {
    int choice;
    bool loggedIn = true;
    
    while (loggedIn) {
        showUserMenu();
        
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 0:
                loggedIn = false;
                cout << "Logged out successfully.\n";
                break;
                
            case 1: { // View Account Information
                double balance = system.getUserBalance(username);
                cout << "\n=== ACCOUNT INFORMATION ===\n";
                loginUser->printUserInfo();
                break;
            }
            
            case 2: { // Change Password
                string oldPassword, newPassword;
                cout << "\n=== CHANGE PASSWORD ===\n";
                cout << "Enter current password: ";
                cin >> oldPassword;
                cout << "Enter new password: ";
                cin >> newPassword;
                
                if (system.changePassword(username, oldPassword, newPassword)) {
                    cout << "--> Password changed successfully!\n";
                } else {
                    cout << "--> Current password is incorrect.\n";
                }
                break;
            }
            
            case 3: { // View Wallet Balance
                double balance = system.getUserBalance(username);
                cout << "\n=== WALLET BALANCE ===\n";
                cout << "Current balance: " << balance << " points\n";
                break;
            }
            
            case 4: { // Transfer Points
                string toUsername;
                double amount;
                
                cout << "\n=== TRANSFER POINTS ===\n";
                cout << "Enter recipient username: ";
                cin >> toUsername;
                cout << "Enter amount to transfer: ";
                cin >> amount;
                
                if (system.transferPoints(username, toUsername, amount)) {
                    cout << "Transfer successful!\n";
                    double newBalance = system.getUserBalance(username);
                    cout << "New balance: " << newBalance << " points\n";
                } else {
                    cout << "Transfer failed. Please check the amount and recipient.\n";
                }
                break;
            }
            
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
}

void handleAdminMenu(SystemManager& system, const string& username, User* loginUser) {
	int choice;
    bool loggedIn = true;
    
    while (loggedIn) {
        showAdminmenu();
        
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 0:
                loggedIn = false;
                cout << "Logged out successfully.\n";
                break;
                
            case 1: { // View All Account Information
                double balance = system.getUserBalance(username);
                cout << "\n=== ALL ACCOUNT INFORMATION ===\n";
                
                
                break;
            }
            case 2: { 
                double balance = system.getUserBalance(username);
                cout << "\n=== REGISTER NEW ACCOUNT ===\n";
                handleRegistration(system);
                
                break;
            }
            case 3: {
                cout << "\n=== Edit Account Infomation ===\n";
                
                
                break;
            }
            
            
            
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
    
}

// Handle login
void handleLogin(SystemManager& system) {
    string username, password;
    
    cout << "\n=== LOGIN ===\n";
    
    cout << "Enter username: ";
    cin >> username;
    
    cout << "Enter password: ";
    cin >> password;
    
    if (system.login(username, password)) {
        cout << "Login successful!\n";
        
        User* loginUser = system.findUser(username);
        
		if(username != "admin") {
			handleUserMenu(system, username, loginUser);
		} else {
			handleAdminMenu(system, username, loginUser);
		}
    } else {
        cout << "Login failed. Please check your information.\n";
    }
}

int main() {
    SystemManager system;
    int choice;
    bool running = true;
    
    while (running) {
        showMainMenu();
        
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                handleLogin(system);
                break;
            case 2:
                handleRegistration(system);
                break;
            case 3:
                running = false;
                cout << "Thank you for using the program!\n";
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
        }
    }
    
    return 0;
} 
