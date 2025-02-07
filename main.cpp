#include <iostream>
#include <fstream> // For File handling
#include <string>
#include <map> // For storing credentials
#include <cstdlib> // For rand()
#include <ctime> // For time()

using namespace std;

class PasswordManager {
private:
    map<string, string> credentials;  // Stores username-password pairs
    string masterPassword;            // Stores the master password for authentication
    string filename;                  // File to save encrypted credentials
    string masterPassFile;            // File to save the encrypted master password
    string encryptionKey;             // Key used for XOR encryption and decryption

public:
    // Initialise file paths and encryption key, load master password and existing credentials
    PasswordManager() 
        : filename("encrypted_credentials.txt"), // Save the encrypted passwords
          masterPassFile("master_password.txt"), // Save the encrypted master password
          encryptionKey("my_top_secret_key_encryption")  // This key is used in XOR encryption;
    {
        loadMasterPassword();  // Load master password from file or prompt to set a new one
        loadFromFile();        // Load saved credentials from file if available
    }

    // Function to set a new master password and save it encrypted
    void setMasterPassword() {
        cout << "Set a new master password: ";
        cin >> masterPassword; //store user input password
        saveMasterPassword();  // Save encrypted master password to file
        cout << "✅ Master password set successfully!\n"; // Feedback to the user
    }

    // Function to authenticate user using the master password
    bool authenticate() {
        string inputPassword;
        cout << "Enter master password: ";
        cin >> inputPassword;

        if (inputPassword == masterPassword) {  // Check if the input matches the stored master password
            cout << "✅ Authentication successful!\n";
            return true;
        } else {
            cout << "❌ Incorrect password. Access denied!\n";
            return false;
        }
    }

    // Function to add or update a password for a given username
    void addCredential(const string& username) {
        string password = generateRandomPassword();  // Automatically generate a strong password
        credentials[username] = password;  // Save the generated password linked to the username
        cout << "✅ Password generated for " << username << ": " << password << endl;
    }

    // Function to retrieve and display a password for a given username
    void getCredential(const string& username) {
        if (credentials.find(username) != credentials.end()) {  // Check if the username exists
            cout << "✅ Password for " << username << ": " << credentials[username] << endl;
        } else {
            cout << "❌ No credentials found for " << username << endl;
        }
    }

    // Function to delete stored credentials for a given username
    void deleteCredential(const string& username) {
        if (credentials.erase(username)) {  // Check if the username exist the procede deletion
            cout << "✅ Credential for " << username << " deleted successfully.\n";
        } else {
            cout << "❌ No credentials found for " << username << endl;
        }
    }

    // Function to save encrypted credentials to a file
    void saveToFile() {
        ofstream file(filename, ios::out);  // Open file in text mode for writing
        if (!file) {
            cout << "❌ Error: Could not save credentials to file.\n";
            return;
        }

        // Encrypt each username and password before writing to the file
        for (const auto& entry : credentials) {
            string encryptedUsername = xorEncrypt(entry.first);
            string encryptedPassword = xorEncrypt(entry.second);
            file << encryptedUsername << " " << encryptedPassword << "\n";
        }
        file.close();
        cout << "✅ Credentials saved securely to file.\n";
    }

    // Function to load and decrypt credentials from a file
    void loadFromFile() {
        ifstream file(filename);  // Open file for reading
        if (!file) {
            cout << "ℹ️ No existing credential file found. Starting fresh.\n";
            return;
        }

        credentials.clear();  // Clear current credentials to avoid duplicates
        string encryptedUsername, encryptedPassword;
        while (file >> encryptedUsername >> encryptedPassword) {  // Read encrypted username and password pairs
            string username = xorDecrypt(encryptedUsername);
            string password = xorDecrypt(encryptedPassword);
            credentials[username] = password;  // Store decrypted credentials in the map
        }
        file.close();
        cout << "✅ Credentials loaded successfully!\n";
    }

    // Function to save the master password to a file after encryption
    void saveMasterPassword() {
        ofstream file(masterPassFile);
        if (!file) {
            cout << "❌ Error: Could not save master password.\n";
            return;
        }
        file << xorEncrypt(masterPassword);  // Encrypt and save the master password
        file.close();
    }

    // Function to load and decrypt the master password from file
    void loadMasterPassword() {
        ifstream file(masterPassFile);
        if (!file) {
            cout << "⚠️ No master password found. Please set one.\n";
            setMasterPassword();  // Prompt the user to set a new master password if none exists
            return;
        }
        string encryptedPassword;
        getline(file, encryptedPassword);  // Read the encrypted master password
        masterPassword = xorDecrypt(encryptedPassword);  // Decrypt it for authentication
        file.close();
    }

    // Function to generate a random password of 12 length 
    string generateRandomPassword(int length = 12) {
        const string chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%^&*()-_=+[]{}|;:,.<>?";
        string password;

        // srand(time(0));  // Seed the random number generator to ensure different outputs on each run

        for (int i = 0; i < length; i++) {
            password += chars[rand() % chars.size()];  // Randomly select 12 characters from the set chars
        }
        return password; //save the random chosen 12 chars
    }

    // XOR encryption function: Encrypts input using the encryption key
    string xorEncrypt(const string& input) {
        string output = input;
        for (size_t i = 0; i < input.size(); i++) {
            output[i] ^= encryptionKey[i % encryptionKey.size()];  // XOR each character with the key
        }
        return output;
    }

    // XOR decryption function
    string xorDecrypt(const string& input) {
        return xorEncrypt(input);  // XOR again to decrypt
    }
};

// Main fuction
int main() {
    PasswordManager pm; // Create PasswordManager object

    if (!pm.authenticate()) {  // Authenticate before allowing access to password management
        return 0;  // Exit if authentication fails
    }

    int choice;
    string username;

    do {
        // Display the password manager menu
        cout << "\n=========== PASSWORD MANAGER MENU ===========\n";
        cout << "1. Add/Update Credential\n";
        cout << "2. Get Credential\n";
        cout << "3. Delete Credential\n";
        cout << "4. Save to File\n";
        cout << "5. Load from File\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice; // Store user choice

        if (cin.fail()) {  // Handle invalid input (non-numeric)
            cin.clear();  // Clear the error flag
            cin.ignore(1000, '\n');  // Ignore invalid input
            cout << "❌ Invalid input. Please enter a number between 1 and 6.\n"; // Feedback to user
            continue;
        }

        // Handle user's choice using a switch statement
        switch (choice) {
            case 1: // Add/Update Credential
                cout << "Enter username: ";
                cin >> username;
                pm.addCredential(username);
                break;
            case 2: // Get Credential
                cout << "Enter username: ";
                cin >> username;
                pm.getCredential(username);
                break;
            case 3: // Delete Credetial
                cout << "Enter username: ";
                cin >> username;
                pm.deleteCredential(username);
                break;
            case 4: // Save to file
                pm.saveToFile();  // Save current credentials to file
                break;
            case 5: // Load from File
                pm.loadFromFile();  // Reload credentials from file
                break;
            case 6: // Exit The Program
                cout << "✅ Exiting Password Manager....\n";
                break;
            default:
                cout << "❌ Invalid choice. Please enter a number between 1 and 6.\n";
        }
    } while (choice != 6);  // Keeps looping until the user chooses to exit

    return 0;
}
