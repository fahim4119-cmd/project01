#include <iostream>
#include <fstream>
#include <regex>
#include <cctype>
#include <string>
#include <limits>
using namespace std;

struct studentData
{
    string name;
    string regNumber;
};

// Function to validate integer input and return as string
string getValidatedInt(const string &prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        cin >> value;

        if (cin.fail())
        {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear(); // Clear the fail state
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        else
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // clear newline
            return to_string(value);
        }
    }
}

// Write student data to file
void writeIn(int sportChoice, string dateFileName, studentData players[], int ttchoice)
{
    string sportName;
    int l;
    if (sportChoice == 1)
    {
        sportName = "basketball";
        l = 10;
    }
    else if (sportChoice == 2)
    {
        sportName = "volleyball";
        l = 12;
    }
    else if (sportChoice == 3)
    {
        sportName = "table tennis";
        l = (ttchoice == 1) ? 2 : 4;
    }
    else if (sportChoice == 4)
    {
        sportName = "futsal";
        l = 10;
    }

    for (int i = 0; i < l; i++)
    { // convert names to lowercase
        for (int j = 0; j < players[i].name.length(); j++)
        {
            players[i].name[j] = tolower(players[i].name[j]);
        }
    }

    ofstream outFile(dateFileName, ios::app);
    if (outFile.is_open())
    {
        for (int i = 0; i < l; i++)
        {
            outFile << players[i].name << "|" << players[i].regNumber << "|" << sportName << "\n";
        }
    }
    outFile.close();
}

// Check if a time slot is available
bool checkTimeSlot(string dateFileName, int sportChoice)
{
    string sportName;
    switch (sportChoice)
    {
    case 1:
        sportName = "basketball";
        break;
    case 2:
        sportName = "volleyball";
        break;
    case 3:
        sportName = "table tennis";
        break;
    case 4:
        sportName = "futsal";
        break;
    default:
        cout << "Invalid sport choice!\n";
        return false;
    }

    string timeSlot;
    regex timePattern(R"(^(0[0-9]|1[0-9]|2[0-3]):00-(0[0-9]|1[0-9]|2[0-3]):00$)");

    while (true)
    {
        cout << "Enter the booking time slot for one hour (HH:00-HH:00, 24-hour format): ";
        getline(cin >> ws, timeSlot);

        if (!regex_match(timeSlot, timePattern))
        {
            cout << "Invalid time format. Please use HH:00-HH:00.\n";
            continue;
        }

        int startHour = stoi(timeSlot.substr(0, 2));
        int endHour = stoi(timeSlot.substr(6, 2));
        if (endHour - startHour != 1)
        {
            cout << "Invalid time slot! Booking should be for one hour only.\n";
            continue;
        }

        ifstream inFile(dateFileName);
        bool conflict = false;
        string line;

        while (getline(inFile, line))
        {
            if (line == timeSlot + sportName)
            {
                cout << "Time slot already booked! Choose another.\n";
                conflict = true;
                break;
            }
        }
        inFile.close();

        if (conflict)
            continue;

        ofstream outFile(dateFileName, ios::app);
        outFile << timeSlot + sportName << "\n";
        outFile.close();

        cout << "Time slot is available!\n";
        break;
    }

    return true;
}

// Book a sport
void bookSport(int sportChoice, string date)
{
    string dateFileName = date + ".txt";
    int l = 0;

    if (sportChoice == 1)
    {
        cout << "Booking Basketball on " << date << "\n";
        l = 10;
    }
    else if (sportChoice == 2)
    {
        cout << "Booking Volleyball on " << date << "\n";
        l = 12;
    }
    else if (sportChoice == 3)
    {
        cout << "Booking Table Tennis on " << date << "\n";
        cout << "1. Two players\n2. Four players\nEnter your choice: ";
        int ttChoice;
        cin >> ttChoice;

        bool isAvailable = checkTimeSlot(dateFileName, sportChoice);
        if (isAvailable)
        {
            studentData players[(ttChoice == 1) ? 2 : 4];
            for (int i = 0; i < ((ttChoice == 1) ? 2 : 4); i++)
            {
                cout << "Player " << i + 1 << " Name: ";
                getline(cin >> ws, players[i].name);
                players[i].regNumber = getValidatedInt("Player " + to_string(i + 1) + " Registration Number: ");
            }
            writeIn(sportChoice, dateFileName, players, ttChoice);
        }
        return;
    }
    else if (sportChoice == 4)
    {
        cout << "Booking Futsal on " << date << "\n";
        l = 10;
    }
    else
    {
        cout << "Invalid sport choice!\n";
        return;
    }

    bool isAvailable = checkTimeSlot(dateFileName, sportChoice);
    if (isAvailable)
    {
        studentData players[l];
        cout << "Enter " << l << " players' names and registration numbers:\n";
        for (int i = 0; i < l; i++)
        {
            cout << "Player " << i + 1 << " Name: ";
            getline(cin >> ws, players[i].name);
            players[i].regNumber = getValidatedInt("Player " + to_string(i + 1) + " Registration Number: ");
        }
        writeIn(sportChoice, dateFileName, players, 0);
    }
}

// Search by Registration Number
void searchByRegNumber(const string &regNumber, const string &date)
{
    string fileName = date + ".txt";
    ifstream readLines(fileName);
    if (!readLines)
    {
        cout << "File could not be opened.\n";
        return;
    }

    string line, currentTimeSlot = "";
    bool found = false;
    int i = 1;

    while (getline(readLines, line))
    {
        if (line.find("|") == string::npos && line.find(":") != string::npos)
        {
            currentTimeSlot = line;
            string currentTimeSlotOnly = currentTimeSlot.substr(0, 5) + " to " + currentTimeSlot.substr(6, 5);
            currentTimeSlot = currentTimeSlotOnly;
            continue;
        }

        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|", pos1 + 1);
        if (pos1 == string::npos || pos2 == string::npos)
            continue;

        string name = line.substr(0, pos1);
        string regNum = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string sport = line.substr(pos2 + 1);

        if (regNum == regNumber)
        {
            cout << i << ". Name: " << name
                 << ", Registration Number: " << regNum
                 << ", Sport: " << sport
                 << ", Time Slot: " << currentTimeSlot << "\n";
            i++;
            found = true;
        }
    }

    if (!found)
        cout << "No records found for Registration Number: " << regNumber << "\n";
}

// Search by Name
void searchByName(const string &searchName, const string &date)
{
    string fileName = date + ".txt";
    ifstream readLines(fileName);
    if (!readLines)
    {
        cout << "File not found.\n";
        return;
    }

    string line, currentTimeSlot = "";
    bool found = false;
    int i = 1;

    while (getline(readLines, line))
    {
        if (line.find("|") == string::npos && line.find(":") != string::npos)
        {
            currentTimeSlot = line;
            string currentTimeSlotOnly = currentTimeSlot.substr(0, 5) + " to " + currentTimeSlot.substr(6, 5);
            currentTimeSlot = currentTimeSlotOnly;
            continue;
        }

        size_t pos1 = line.find("|");
        size_t pos2 = line.find("|", pos1 + 1);
        if (pos1 == string::npos || pos2 == string::npos)
            continue;

        string name = line.substr(0, pos1);
        string regNum = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string sport = line.substr(pos2 + 1);

        if (name == searchName)
        {
            cout << i << ". Name: " << name
                 << ", Registration Number: " << regNum
                 << ", Sport: " << sport
                 << ", Time Slot: " << currentTimeSlot << "\n";
            i++;
            found = true;
        }
    }

    if (!found)
        cout << "No records found for Name: " << searchName << "\n";
}

// Sports Complex Menu
void sportsComplexMenu(const string date)
{
    int choice;

    do
    {
        cout << "\n--- SPORTS COMPLEX SECTION ---\n";
        cout << "1. Book a Sport\n";
        cout << "2. View Student Usage History\n";
        cout << "0. Back to Main Menu\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1)
        {
            int choiceSport;
            cout << "\n--- Book a Sport ---\n";
            cout << "Select a sport:\n1. Basketball\n2. Volleyball\n3. Table Tennis\n4. Futsal\n0. Go back\nEnter your choice: ";
            cin >> choiceSport;
            if (choiceSport >= 1 && choiceSport <= 4)
                bookSport(choiceSport, date);
        }
        else if (choice == 2)
        {
            string dateSearch;
            regex datePattern(R"(^\d{2}-\d{2}-\d{4}$)");
            do
            {
                cout << "Enter date to search usage history (dd-mm-yyyy): ";
                cin >> dateSearch;
                if (!regex_match(dateSearch, datePattern))
                    cout << "Invalid date format.\n";
            } while (!regex_match(dateSearch, datePattern));

            cout << "1. Search by Registration Number\n2. Search by Name\nEnter choice: ";
            string searchChoice = getValidatedInt("");

            if (searchChoice == "1")
            {
                string regNumber = getValidatedInt("Enter Registration Number: ");
                searchByRegNumber(regNumber, dateSearch);
            }
            else if (searchChoice == "2")
            {
                cout << "Enter the name of the student: ";
                string name;
                getline(cin >> ws, name);
                for (auto &c : name)
                    c = tolower(c);
                searchByName(name, dateSearch);
            }
            else
            {
                cout << "Invalid choice!\n";
            }
        }
        else if (choice == 0)
        {
            cout << "Returning to main menu...\n";
        }
        else
        {
            cout << "Invalid option!\n";
        }

    } while (choice != 0);
}

int main()
{
    int choice;
    string date;
    regex datePattern(R"(^\d{2}-\d{2}-\d{4}$)");

    do
    {
        cout << "\n====================================\n";
        cout << "  SPORTS COMPLEX MANAGEMENT SYSTEM\n";
        cout << "====================================\n";
        do
        {
            cout << "Enter date (dd-mm-yyyy): ";
            cin >> date;
            if (!regex_match(date, datePattern))
                cout << "Invalid date format.\n";
        } while (!regex_match(date, datePattern));

        cout << "1. Sports Complex Section\n";
        cout << "0. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1)
            sportsComplexMenu(date);
        else if (choice == 0)
            cout << "Thank you for using the system!\n";
        else
            cout << "Invalid choice! Try again.\n";

    } while (choice != 0);

    return 0;
}
