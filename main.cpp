#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <limits>
#include <algorithm>
#include <conio.h>

using namespace std;

// --- DATA STRUCTURES ---
struct User {
    string username;
    string password;
    string role;
};

struct MedicalRecord {
    string username;
    string visitNum;
    string diseaseDesc;
    string remediation;
};

// --- FUNCTION PROTOTYPES ---
void doctorMenu(string loggedInDoctor);
void patientMenu(string loggedInPatient);
void addPatientAndRecord(string doctorID);
void viewOwnRecord(string username);
bool userExists(string username);
bool updatePassword(string username, string newPassword); // Changed from void to bool

string getMaskedPassword() {
    string password = "";
    char ch;

    while (true) {
        ch = _getch(); // Reads character without echoing to console

        // If user presses Enter (ASCII 13)
        if (ch == 13) {
            cout << endl;
            break;
        }
        // If user presses Backspace (ASCII 8)
        else if (ch == 8) {
            if (password.length() > 0) {
                password.pop_back(); // Remove last char from string
                cout << "\b \b";     // Erase asterisk from console
            }
        }
        // Standard typed characters
        else {
            password += ch;
            cout << '*';
        }
    }
    return password;
}

int main() {
    string inputUser, inputPass;
    bool authenticated = false;
    string fileRole;

    cout << "====================================\n";
    cout << "  SECURED CLINICAL DATABASE SYSTEM  \n";
    cout << "====================================\n";

    while (true) {
        authenticated = false;

        cout << "\nUsername: ";
        getline(cin >> ws, inputUser);

        ifstream userFile("Users.txt");
        string line;
        bool foundUser = false;
        string currentPass = "";

        while (getline(userFile, line)) {
            if (line.empty()) continue;

            stringstream ss(line);
            string fUser, fPass, fRole;
            getline(ss, fUser, ',');
            getline(ss, fPass, ',');
            getline(ss, fRole, ',');

            if (inputUser == fUser) {
                foundUser = true;
                currentPass = fPass;
                fileRole = fRole;
                break;
            }
        }
        userFile.close();
if (foundUser) {
            if (currentPass == "UNSET") {
                cout << "Account found. Password hasn't been created yet.\n";
                cout << "Create new password: ";
                inputPass = getMaskedPassword();

                // --- NEW LOGIC: Only log in if the password file actually saved ---
                if (updatePassword(inputUser, inputPass)) {
                    authenticated = true;
                    cout << "[Success] Password created successfully. Logging you in...\n";
                } else {
                    cout << "[Failed] Password could not be saved to disk. Check file permissions.\n";
                }
                // ------------------------------------------------------------------

            } else {
                cout << "Password: ";
                inputPass = getMaskedPassword();
                if (inputPass == currentPass) {
                    authenticated = true;
                } else {
                    cout << "[ERROR] Invalid password.\n";
                }
            }
        } else {
            cout << "[ERROR] Username not found.\n";
        }

        if (authenticated) {
            if (fileRole == "Doctor") {
                doctorMenu(inputUser);
            } else if (fileRole == "Patient") {
                patientMenu(inputUser);
            }
        }
    }
    return 0;
}

void doctorMenu(string loggedInDoctor) {
    int choice = 0;
    while (choice != 2) {
        cout << "\n--- DOCTOR PORTAL (" << loggedInDoctor << ") ---\n";
        cout << "1. Add Patient & Medical Record\n";
        cout << "2. Logout\n";
        cout << "Select option: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[Error] Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            addPatientAndRecord(loggedInDoctor);
        } else if (choice == 2) {
            cout << "Logging out securely...\n";
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void patientMenu(string loggedInPatient) {
    int choice = 0;
    while (choice != 2) {
        // FIXED: Added the missing patient menu UI so the user knows what to type
        cout << "\n--- PATIENT PORTAL (" << loggedInPatient << ") ---\n";
        cout << "1. View Own Record\n";
        cout << "2. Logout\n";
        cout << "Select option: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "[Error] Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == 1) {
            viewOwnRecord(loggedInPatient);
        } else if (choice == 2) {
            cout << "Logging out securely...\n";
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void addPatientAndRecord(string doctorID) {
    string fullName, disease, remediation, visitNum;

    cout << "\n--- ADD MEDICAL RECORD ---\n";
    cout << "Patient name (Fullname): ";
    // FIXED: Swapped cin.ignore() for cin >> ws to safely clear the buffer
    getline(cin >> ws, fullName);

    string username = fullName;
    replace(username.begin(), username.end(), ' ', '_');

    if (!userExists(username)) {
        ofstream userFile("Users.txt", ios::app);
        if (userFile.is_open()) {
            // FIX: Add \n at the START of the string to guarantee a new line
            userFile << "\n" << username << ",UNSET,Patient\n";
            cout << "[System] New patient account created. Username: " << username << "\n";
        }
        userFile.close();
    }

    cout << "Visit Number (e.g., 1, 2, 3): ";
    getline(cin, visitNum);

    cout << "Description of disease: ";
    getline(cin, disease);
    replace(disease.begin(), disease.end(), ',', '-');

    cout << "Remediation: ";
    getline(cin, remediation);
    replace(remediation.begin(), remediation.end(), ',', '-');

    ofstream recordFile("MedicalRecords.txt", ios::app);
    if (recordFile.is_open()) {
        recordFile << username << "," << visitNum << "," << disease << "," << remediation << "\n";
        cout << "\n[Success] Medical record saved for " << fullName << "!\n";
    } else {
        cout << "\n[Error] Could not open MedicalRecords.txt\n";
    }
    recordFile.close();
}

void viewOwnRecord(string username) {
    ifstream recordFile("MedicalRecords.txt");
    string line;
    bool foundRecord = false;

    cout << "\n--- YOUR MEDICAL DETAILS ---\n";
    while (getline(recordFile, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string rUser, rVisit, rDisease, rRemediation;

        getline(ss, rUser, ',');
        getline(ss, rVisit, ',');
        getline(ss, rDisease, ',');
        getline(ss, rRemediation, ',');

        if (rUser == username) {
            cout << "--------------------------------\n";
            cout << "Visit       : " << rVisit << "\n";
            cout << "Disease     : " << rDisease << "\n";
            cout << "Remediation : " << rRemediation << "\n";
            foundRecord = true;
        }
    }
    recordFile.close();
    cout << "--------------------------------\n";

    if (!foundRecord) {
        cout << "No medical records found on file.\n";
    }
}

bool userExists(string username) {
    ifstream userFile("Users.txt");
    string line, fUser;
    while (getline(userFile, line)) {
        stringstream ss(line);
        getline(ss, fUser, ',');
        if (fUser == username) {
            userFile.close();
            return true;
        }
    }
    userFile.close();
    return false;
}

bool updatePassword(string username, string newPassword) {
    vector<string> lines;
    string line;
    ifstream userFileIn("Users.txt");

    while (getline(userFileIn, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        string fUser, fPass, fRole;
        getline(ss, fUser, ',');
        getline(ss, fPass, ',');
        getline(ss, fRole, ',');

        if (fUser == username) {
            lines.push_back(fUser + "," + newPassword + "," + fRole);
        } else {
            lines.push_back(line);
        }
    }
    userFileIn.close();

    ofstream userFileOut("Users.txt", ios::trunc);

    if (!userFileOut.is_open()) {
        cout << "\n[CRITICAL ERROR] Lacking write permissions to overwrite Users.txt!\n";
        cout << "To fix this in your terminal, run: chmod 666 Users.txt\n";
        return false; // <-- ADD THIS: Tell main() the operation failed
    }

    for (const auto& l : lines) {
        userFileOut << l << "\n";
    }
    userFileOut.close();

    return true; // <-- ADD THIS: Tell main() the operation succeeded
}
