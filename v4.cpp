/*
==========================================================
        MAINTENANCE MANAGEMENT SYSTEM
==========================================================

STREAMS USED IN THIS PROGRAM:

1) ifstream  → used for reading files
   - duties.csv → getDutyMember()
   - complaints.csv → deleteComplaint(), markcomplete(), viewMyComplaints()
   - stock.csv → checkAvailability(), updateStock()
   - srd_records.csv → viewPreviousItems()

2) ofstream  → used for writing files
   - complaints.csv → saveComplaint()
   - records.csv → saveRecord()
   - stock.csv → updateStock(), updatestock()
   - srd_records.csv → requestItem()

3) stringstream → used for splitting CSV data
   - used wherever we read CSV rows and separate values
   
4)vector → used for temporary storage of file data
deleteComplaint() → store remaining complaints after deletion
markcomplete() → store pending complaints after removing completed one
updateStock() → store updated stock data before rewriting file

5) <stdexcept> → used for exception handling
used in try-catch blocks to handle runtime errors
used when file is not opening or invalid input occurs

6) <cctype> → used for character checking functions
used in markcomplete() and deleteComplaint()
helps to check whether a character is a digit or not

NOTE:
- stoi() is protected using try-catch to avoid crash
- stock check will NOT stop completion (only warning)
==========================================================
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>
#include <cctype>
using namespace std;


// ================= FILE OPERATIONS =================
namespace FileOperation {

    // This function returns duty member for given date and day
    string getDutyMember(string date, string day) {
        ifstream file("duties.csv");

        string line, fileDate, fileDay, member;
        getline(file, line); // skipping header

        while (getline(file, line)) {
            stringstream ss(line);
            getline(ss, fileDate, ',');
            getline(ss, fileDay, ',');
            getline(ss, member, ',');

            if (fileDate == date && fileDay == day)
                return member;
        }
        return "Not Found";
    }

    // Save complaint into complaints.csv
    void saveComplaint(string name,string repairType,int repairId,double cost,
                       string day,string dutyMember,string address) {

        ofstream file("complaints.csv", ios::app);

        file<<name<<","<<repairType<<","<<repairId<<","<<cost<<","
            <<day<<","<<dutyMember<<","<<address<<endl;

        cout<<"Complaint saved successfully!\n";
    }

    // Save completed complaint into records.csv
    void saveRecord(string name,string type,int id,double cost,
                    string day,string duty,string room){

        ofstream file("records.csv", ios::app);

        file<<name<<","<<type<<","<<id<<","<<cost<<","
            <<day<<","<<duty<<","<<room<<endl;

        cout<<"Saved into records file\n";
    }

    // Delete completed complaint from complaints.csv
    void deleteComplaint(int repairId,string memberName){

        ifstream inFile("complaints.csv");
        vector<string> remaining;
        string line;

        while(getline(inFile, line)){
            stringstream ss(line);

            string fname,ftype,fid,fcost,fday,fmember,froom;

            getline(ss,fname,',');
            getline(ss,ftype,',');
            getline(ss,fid,',');
            getline(ss,fcost,',');
            getline(ss,fday,',');
            getline(ss,fmember,',');
            getline(ss,froom,',');

            try{
                if(!fid.empty() && isdigit(fid[0]) &&
                   stoi(fid)==repairId && fmember==memberName)
                    continue;
            }
            catch(...){}

            remaining.push_back(line);
        }

        inFile.close();

        ofstream outFile("complaints.csv");
        for(string &l:remaining)
            outFile<<l<<endl;
    }
}


// ================= STOCK =================
class Stock{
public:

    // Check available quantity of item
    int checkAvailability(string item){
        ifstream file("stock.csv");

        string line,name,qty;

        while(getline(file,line)){
            stringstream ss(line);
            getline(ss,name,',');
            getline(ss,qty,',');

            try{
                if(name==item)
                    return stoi(qty);
            }
            catch(...){}
        }
        return 0;
    }

    // Reduce stock after usage
    void updateStock(string item,int take){

        ifstream file("stock.csv");
        vector<string> lines;
        string line;

        while(getline(file,line)){
            stringstream ss(line);
            string name,qty;
            getline(ss,name,',');
            getline(ss,qty,',');

            int q=0;
            try{ q=stoi(qty); } catch(...){}

            if(name==item){
                q-=take;
                if(q<0) q=0;
            }

            lines.push_back(name+","+to_string(q));
        }

        file.close();

        ofstream out("stock.csv");
        for(string x:lines)
            out<<x<<endl;
    }
};


// ================= REPAIRS =================
class Repairs {
protected:
    int repairId;
    double cost;
    static int nextId;

public:
    Repairs(){ repairId=nextId++; }

    virtual void diagnose(string room)=0;
    virtual string getTypeName()=0;

    int getRepairId(){ return repairId; }
};

int Repairs::nextId=100;


// ================= TYPES =================
class Plumbing:public Repairs{
public:
    void diagnose(string room){ cout<<"Plumbing issue in "<<room<<endl; }
    string getTypeName(){ return "Plumbing"; }
};

class Electric:public Repairs{
public:
    void diagnose(string room){ cout<<"Electric issue in "<<room<<endl; }
    string getTypeName(){ return "Electric"; }
};

class Carpentry:public Repairs{
public:
    void diagnose(string room){ cout<<"Carpentry issue in "<<room<<endl; }
    string getTypeName(){ return "Carpentry"; }
};


// ================= PERSON =================
class Person{
protected:
    int id;
    string name;
public:
    virtual void inputDetails()=0;
};


// ================= MEMBER =================

class Member : public Person {

public:

    // ================= ADMIN LOGIN =================
    bool login() {

        int uid;
        string pass;

        cout << "\n===== ADMIN LOGIN =====\n";

        cout << "Enter ID: ";
        cin >> uid;

        cout << "Enter Password: ";
        cin >> pass;


        // Open admin file
        ifstream file("admins.csv");

        if (!file) {

            cout << "admins.csv file not found!\n";
            return false;
        }


        string line;

        // Skip header
        getline(file, line);


        while (getline(file, line)) {

            stringstream ss(line);

            string fileID;
            string fileName;
            string filePassword;

            getline(ss, fileID, ',');
            getline(ss, fileName, ',');
            getline(ss, filePassword, ',');


            try {

                if (!fileID.empty() &&
                    stoi(fileID) == uid &&
                    filePassword == pass) {

                    id = uid;
                    name = fileName;

                    cout << "\nLogin Successful!\n";
                    cout << "Welcome " << name << "!\n";

                    return true;
                }

            }
            catch (...) {}
        }


        cout << "Invalid Login!\n";

        return false;
    }


    // ================= INPUT DETAILS =================

    void inputDetails() {

        cout << "Admin Name: " << name << endl;
    }


    // ================= TOTAL REPAIRS =================

    void showTotalRepairs() {

        ifstream file("complaints.csv");

        if (!file) {

            cout << "\nTotal Pending Repairs: 0\n";
            return;
        }

        string line;

        int count = 0;

        while (getline(file, line)) {

            if (!line.empty())
                count++;
        }

        cout << "\n===== REPAIR SUMMARY =====\n";

        cout << "Total Pending Repairs: "
             << count << endl;
    }


    // ================= ADD STOCK =================

    void updatestock() {

        ofstream file("stock.csv", ios::app);

        if (!file) {

            cout << "Error opening stock file!\n";
            return;
        }


        string item;
        int qty;

        cout << "\n===== UPDATE STOCK =====\n";

        cout << "Enter Item: ";
        cin >> item;

        cout << "Enter Quantity: ";
        cin >> qty;


        file << item << "," << qty << endl;

        cout << "Stock added successfully!\n";
    }


    // ================= VIEW COMPLAINTS =================

    void viewMyComplaints() {

        ifstream file("complaints.csv");

        if (!file) {

            cout << "No complaints file found!\n";
            return;
        }


        string line;

        cout << "\n===== ALL COMPLAINTS =====\n";


        while (getline(file, line)) {

            stringstream ss(line);

            string n, t, i, c, d, m, r;

            getline(ss, n, ',');
            getline(ss, t, ',');
            getline(ss, i, ',');
            getline(ss, c, ',');
            getline(ss, d, ',');
            getline(ss, m, ',');
            getline(ss, r, ',');


            cout << "\nName: " << n << endl;

            cout << "Type: " << t << endl;

            cout << "Repair ID: " << i << endl;

            cout << "Day: " << d << endl;

            cout << "Assigned Member: " << m << endl;

            cout << "Room: " << r << endl;

            cout << "------------------------\n";
        }
    }


    // ================= COMPLETE REPAIR =================

    void markcomplete() {

        ifstream file("complaints.csv");

        if (!file) {

            cout << "No complaints file found!\n";
            return;
        }


        vector<string> rec;

        string line;

        int id;


        cout << "\nEnter Repair ID: ";
        cin >> id;


        bool found = false;

        while (getline(file, line)) {

            stringstream ss(line);

            string n, t, i, c, d, m, r;

            getline(ss, n, ',');
            getline(ss, t, ',');
            getline(ss, i, ',');
            getline(ss, c, ',');
            getline(ss, d, ',');
            getline(ss, m, ',');
            getline(ss, r, ',');


            bool isCompleted = false;


            try {

                if (!i.empty() && isdigit(i[0])) {

                    int rid = stoi(i);


                    if (rid == id) {

                        found = true;


                        // ====================================
                        // ENTER ITEMS ACTUALLY USED
                        // ====================================

                        Stock stock;

                        char more = 'y';


                        while (more == 'y' || more == 'Y') {

                            string item;

                            int qty;


                            cout << "\nEnter Item Used: ";
                            cin >> item;


                            cout << "Enter Quantity Used: ";
                            cin >> qty;


                            // Directly decrease stock
                            stock.updateStock(item, qty);


                            cout << "Stock updated successfully!\n";


                            cout << "\nDo you want to enter another item? (y/n): ";
                            cin >> more;
                        }


                        // ====================================
                        // SAVE COMPLETED REPAIR
                        // ====================================

                        double cost = 0;

                        try {
                            cost = stod(c);
                        }
                        catch (...) {
                            cost = 0;
                        }


                        FileOperation::saveRecord(
                            n,
                            t,
                            rid,
                            cost,
                            d,
                            m,
                            r
                        );


                        cout << "\nRepair Completed Successfully!\n";


                        isCompleted = true;
                    }
                }

            }
            catch (...) {}


            // Keep only pending complaints
            if (!isCompleted) {

                rec.push_back(line);
            }
        }


        file.close();


        // Rewrite complaints file
        ofstream out("complaints.csv");


        for (string x : rec)
            out << x << endl;


        if (!found) {

            cout << "\nRepair ID not found!\n";
        }
    }


    // ================= PENDING COMPLAINTS =================

    void pendeningcomplaint() {

        ifstream file("complaints.csv");

        if (!file) {

            cout << "No complaints file found!\n";
            return;
        }


        string line;

        cout << "\n---- Pending Complaints ----\n";


        while (getline(file, line)) {

            cout << line << endl;
        }
    }
};

// ================= SRD =================
class SRD : public Person {
public:

    void inputDetails() {
        cout << "Enter SRD Name: ";
        cin >> name;
    }


    // Check item in stock and request it
    void requestItem() {

        string date;
        string day;
        string item;

        cout << "Enter Date: ";
        cin >> date;

        cout << "Enter Day: ";
        cin >> day;

        cout << "Enter Item: ";
        cin >> ws;
        getline(cin, item);


        // Open stock file
        ifstream stockFile("stock.csv");

        if (!stockFile) {
            cout << "Stock file not found!\n";
            return;
        }


        string line;
        bool found = false;


        // Check item in stock
        while (getline(stockFile, line)) {

            stringstream ss(line);

            string stockItem;
            string qty;

            getline(ss, stockItem, ',');
            getline(ss, qty, ',');


            if (stockItem == item) {

                found = true;

                cout << "\nItem: " << item << endl;
                cout << "Available in Stock\n";
                cout << "Quantity in Stock: " << qty << endl;

                break;
            }
        }

        stockFile.close();


        // If item is not available
        if (!found) {

            cout << "\nItem is not available in stock.\n";
            cout << "Please contact any Maintenance member.\n";

            return;
        }


        // Save request
        ofstream file("srd_records.csv", ios::app);

        if (!file) {

            cout << "Error opening srd_records.csv!\n";
            return;
        }


        file << name << ","
             << date << ","
             << day << ","
             << item << endl;


        cout << "Item request saved successfully!\n";
    }


    // View previous requests
    void viewPreviousItems() {

        ifstream file("srd_records.csv");

        if (!file) {

            cout << "No previous records found!\n";
            return;
        }


        string line;

        cout << "\n===== PREVIOUS ITEM REQUESTS =====\n";

        while (getline(file, line)) {

            cout << line << endl;
        }
    }
};

// ================= NON MEMBER =================
class NonMember : public Person {
public:

    void inputDetails() {

        int regNo;
        string password;

        cout << "\n===== STUDENT LOGIN =====\n";

        cout << "Enter Registration No: ";
        cin >> regNo;

        cout << "Enter Password: ";
        cin >> password;


        // Default password
        string defaultPassword = "student123";


        // Check password first
        if (password != defaultPassword) {

            cout << "Invalid Password!\n";
            return;
        }


        // Open student file
        ifstream file("students.csv");

        if (!file) {

            cout << "students.csv file not found!\n";
            return;
        }


        string line;

        // Skip header
        getline(file, line);


        bool found = false;


        while (getline(file, line)) {

            stringstream ss(line);

            string fileRegNo;
            string fileName;


            getline(ss, fileRegNo, ',');
            getline(ss, fileName, ',');


            try {

                if (!fileRegNo.empty() &&
                    stoi(fileRegNo) == regNo) {

                    id = regNo;
                    name = fileName;

                    found = true;

                    break;
                }

            }
            catch (...) {}
        }


        file.close();


        if (!found) {

            cout << "Registration Number not found!\n";
            return;
        }


        cout << "\nLogin Successful!\n";
        cout << "Welcome " << name << "!\n";
    }


    // Raise complaint
    void raiseComplaint(string day) {

        string dutyMember =
            FileOperation::getDutyMember(day);


        cout << "\nDuty Member: "
             << dutyMember << endl;


        int choice;


        cout << "\n1 Plumbing";
        cout << "\n2 Electric";
        cout << "\n3 Carpentry\n";


        cout << "Enter Choice: ";
        cin >> choice;


        Repairs* repair = nullptr;


        if (choice == 1)
            repair = new Plumbing();

        else if (choice == 2)
            repair = new Electric();

        else
            repair = new Carpentry();


        string room;


        // Plumbing
        if (choice == 1) {

            int side;
            int problem;

            string sideName;
            string problemName;
            string number;


            cout << "\n===== PLUMBING =====\n";

            cout << "1 A14 Side\n";
            cout << "2 A05 Side\n";
            cout << "3 B12 Side\n";
            cout << "4 B1 Side\n";

            cout << "Enter Side: ";
            cin >> side;


            if (side == 1)
                sideName = "A14 Side";

            else if (side == 2)
                sideName = "A05 Side";

            else if (side == 3)
                sideName = "B12 Side";

            else
                sideName = "B1 Side";


            cout << "\n1 Tap\n";
            cout << "2 Bathroom\n";
            cout << "3 Washroom\n";

            cout << "Enter Problem: ";
            cin >> problem;


            if (problem == 1) {

                problemName = "Tap";

                cout << "Enter Tap No: ";
                cin >> number;
            }

            else if (problem == 2) {

                problemName = "Bathroom";

                cout << "Enter Bathroom No: ";
                cin >> number;
            }

            else {

                problemName = "Washroom";

                cout << "Enter Washroom No: ";
                cin >> number;
            }


            room = sideName + " - "
                 + problemName + " - "
                 + number;
        }


        // Electric / Carpentry
        else {

            cout << "Enter Room: ";
            cin >> room;
        }


        FileOperation::saveComplaint(
            name,
            repair->getTypeName(),
            repair->getRepairId(),
            0,
            day,
            dutyMember,
            room
        );


        cout << "\nRepair ID: "
             << repair->getRepairId()
             << endl;


        cout << "Assigned Member: "
             << dutyMember
             << endl;


        delete repair;
    }
};
// ================= MAIN =================
int main(){

    int type;
    cout<<"1 Student\n2 SRD\n3 Member\n";
    cin>>type;

    if(type==1){
        NonMember n;
        n.inputDetails();

        string date, day;
        cout<<"Enter Date (DD-MM-YYYY): ";
        cin>>date;
        cout<<"Enter Day: ";
        cin>>day;
        n.raiseComplaint(date, day);

        while(true){
            cout<<"\\n1 Check Repair Update\\n2 Exit\\n";
            cout<<"Enter choice: ";
            int ch;
            cin>>ch;
            if(ch==1) n.checkRepairUpdates();
            else break;
        }
    }

    else if(type==2){
    SRD s;
    s.inputDetails();

    while(true){
        cout<<"\n1 Request Item\n2 View Previous\n3 Exit\n";
        cout<<"Enter choice: ";

        int ch;
        cin>>ch;

        if(ch==1){
            string day;
            cout<<"Enter Day: ";
            cin>>day;

            s.requestItem(day);   
        }
        else if(ch==2){
            s.viewPreviousItems();  
        }
        else if(ch==3){
            cout<<"Exiting SRD...\n";
            break;   
        }
        else{
            cout<<"Invalid choice!\n";
        }
    }
}
    

    else if(type==3){
        Member m;

        if(!m.login()) return 0;

        m.inputDetails();

        while(1){
            cout<<"1 Update Stock\n2 View Complaints\n3 Pending\n4 Complete\n5 Exit\n";
            int ch; cin>>ch;

           if(ch==1) m.updatestock();
else if(ch==2) m.viewMyComplaints();
else if(ch==3) m.pendeningcomplaint();  
else if(ch==4) m.markcomplete();
else break;
        }
    }

    return 0;
}
