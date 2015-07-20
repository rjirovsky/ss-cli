/** @file main.cpp
 * 
 *  @author Radek Jirovsky
 *  @version 1.01
 * 
 *  Save storage for credentials (login and password)
 * 
 *  Copyright (C) 2012  Radek Jirovský rjirovsky@gmail.com
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <iostream>
#include <string>

#include <termios.h>    ///for obtaining key from stdin 
#include <unistd.h>     ///for obtaining key from stdin 


#include "databasemanager.h"

using namespace std;

void helpMessage();
Item* fillItem(string name);
string getKey();
string getNewKey();

/**
 * @brief User interface for Safe Storage database files.
 * 
 */
int main(int argc, char **argv)
{
    DatabaseManager dbm;
    
    ///start parsing args
    if(argc == 3){
        if(string(argv[1]) == "-f"){            ///set active database
            try {
                dbm.loadDatabase(argv[2],getKey());
                //dbm.sortDatabase();
                dbm.printAllItems();
                //dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else if (string(argv[1]) == "-n"){    ///new database file
            try {
                dbm.createDatabase(argv[2],getNewKey());
            } catch (exception& ex){
                cerr << "Error creating database: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } 
    } else if (argc == 4){
        if(string(argv[3]) == "-L"){            ///print all with secrets
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.printAllItemsWithSecrets();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else {
            helpMessage();
        }
    } else if (argc == 5){
        if(string(argv[3]) == "-sn"){           ///search by name
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.printItemsByName(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else if(string(argv[3]) == "-sg"){    ///search by group
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.printItemsByGroup(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else if(string(argv[3]) == "-l"){     ///print by name
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.printItemByName(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
                dbm.closeDatabase();
            }
            
        } else if(string(argv[3]) == "-E"){     ///export
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.exportDatabase(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Export error: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else if(string(argv[3]) == "-I"){     ///import
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.importCSV(string(argv[4]));
                dbm.sortDatabase();
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Import error: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else if(string(argv[3]) == "-rm"){    ///remove item
            try {
                dbm.loadDatabase(argv[2],getKey());
                dbm.removeItem(argv[4]);
                
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error removing entry: " << ex.what() << endl;
                dbm.closeDatabase();
            }
        } else if(string(argv[3]) == "-a"){     /// add item
            Item* item;
            try {
                dbm.loadDatabase(argv[2],getKey());
                item = fillItem(argv[4]);
                dbm.addItem(item);          
                dbm.sortDatabase();
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error inserting entry: " << ex.what() << endl;
                delete item;
                item = NULL;
                dbm.closeDatabase();
            }
        } else if(string(argv[3]) == "-e"){     /// edit item
            Item* item;
            try {
                dbm.loadDatabase(argv[2],getKey());
         
                item = fillItem(argv[4]);
                dbm.editItem(item);
                
                delete item;    // entries are copied, item addr is not used
                item = NULL;
                dbm.sortDatabase();
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error editing entry: " << ex.what() << endl;
                delete item;
                item = NULL;
                dbm.closeDatabase();
            }
        } else {
            helpMessage();
        }
    } else {
        helpMessage();
    }

    return 0;
}

/**
 * @brief   Get input from user.
 * 
 * @param   name    name of new entry
 * @return  filled Item
 */
Item* fillItem(string name){
    Item* item = new Item(); 
    
    item->name = name;
    
    cout << "Group [default]: ";
    getline(cin,item->group);

    cout << "Login [none]: ";
    getline(cin,item->login);

    item->password = getNewKey();
    
    return item;
}

/**
 * @brief   Get secret key from user. It isn't echoed on cout.
 * 
 * @return  key
 */
string getKey() {
    
/* Windows:

    #include <windows.h>
    

        HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE); 
        DWORD mode = 0;
        GetConsoleMode(hStdin, &mode);
        SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));
        
        string pw;
        cout << "Enter master password: ";
        
        getline(cin, pw);        
        cout<< endl;
  
        //cleanup:
        
        SetConsoleMode(hStdin, mode);
        
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt); */
    
    cout << "Enter master password: ";
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    string pw;
    getline(cin, pw);
    
    cout << endl;
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return pw;    
}

/**
 * @brief   Get new password from user.
 * 
 * @return  password
 */
string getNewKey() {
    
    // on Windows see getKey()
    
    string pw1, pw2;
    
    termios oldt;
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    
    while (true){    
        cout << "Enter new password: ";
        getline(cin, pw1);
        cout << endl << "Re-enter password: ";
        getline(cin, pw2);
        cout << endl;
        
        if(pw1 == pw2){
            break;
        } else {
            cerr << "Passwords are different! Try again." << endl;
        }
    }
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        
    return pw1;    
}

/**
 * @brief   Print help.
 */
void helpMessage(){
    
    cout << "Safe Storage (ss-cli) - password database" << endl;
    cout << "Usage: ss-cli [OPTION]" << endl;
    cout << "Options:" << endl;
    cout << "   no options - this help" << endl;
    cout << "   -n FILE_NAME - create new database file" << endl;
    cout << "   -f FILE_NAME [ARGS]- work with existing database" << endl;
    cout << "   Args:" << endl;
    cout << "      no args - list all entries (without secrets)" << endl;
    cout << "      -L - list all entries with login and password (be carefull!)" << endl;    
    cout << "      -a NAME - add new entry with unique NAME to database" << endl;
    cout << "      -rm NAME - remove entry with NAME from database" << endl;
    cout << "      -e NAME - edit existing entry" << endl;
    cout << "      -l NAME - show complete entry with NAME (with secrets)" << endl;
    cout << "      -sn KEYWORD - search for entries containing KEYWORD in name" << endl;
    cout << "      -sg KEYWORD - search for entries containing KEYWORD in group" << endl;
    cout << "      -E FILE_NAME - export database to CSV file" << endl;
    cout << "      -I FILE_NAME - import entries from CSV with header: group;name;login;password;" << endl;
    
}
