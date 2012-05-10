/** @file main.cpp
 *  @author Radek Jirovsky
 *  Save storage for credentials (login and password)
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

#include "databasemanager.h"
#include <string>

using namespace std;

void helpMessage();
Item* fillItem(string name);

/**
 * @brief 
 * 
 */
int main(int argc, char **argv)
{
    DatabaseManager dbm;
    
    if(argc == 3){
        //cout << "#3" << endl;
        if(string(argv[1]) == "-f"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.printAllItems();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
            }
        } else if (string(argv[1]) == "-n"){
            try {
                dbm.createDatabase(argv[2],"klic");
            } catch (exception& ex){
                cerr << "Error creating database: " << ex.what() << endl;
            }
        } 
    } else if (argc == 4){
        //cout << "#4" << endl;
        if(string(argv[3]) == "-L"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.printAllItemsWithSecrets();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
            }
        } else {
            helpMessage();
        }
    } else if (argc == 5){
        //cout << "#5" << endl;
        if(string(argv[3]) == "-sn"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.printItemsByName(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
            }
        } else if(string(argv[3]) == "-sg"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.printItemsByGroup(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
            }
        } else if(string(argv[3]) == "-l"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.printItemByName(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error loading database: " << ex.what() << endl;
            }
            
        } else if(string(argv[3]) == "-E"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.exportDatabase(string(argv[4]));
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Export error: " << ex.what() << endl;
            }
        } else if(string(argv[3]) == "-I"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.importCSV(string(argv[4]));
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Import error: " << ex.what() << endl;
            }
        } else if(string(argv[3]) == "-rm"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                dbm.removeItem(argv[4]);
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error removing entry: " << ex.what() << endl;
            }
        } else if(string(argv[3]) == "-a"){
            try {
                dbm.loadDatabase(argv[2],"klic");
                Item * item = fillItem(argv[4]);
                dbm.addItem(item);
                dbm.saveDatabase();
                dbm.closeDatabase();
            } catch (exception& ex){
                cerr << "Error inserting entry: " << ex.what() << endl;
            }
        } else {
            helpMessage();
        }
    } else {
        helpMessage();
    }


//     dbm.addItem("", "nazev2", "login2", "heslo2");
//     cout << "All:" << endl;
//      dbm.printAllItems();
//     cout << "All with pass.:" << endl;
//     dbm.printAllItemsWithSecrets();
//      cout << "Search by name:" << endl;
//      dbm.printItemsByName("na");
//     cout << "Search by group:" << endl;
//     dbm.printItemsByGroup("default");
//      cout << "Removing" << endl;
//       dbm.removeItem("nazev1");

//      
//       dbm.editItem("banka","nazev5","","1234");
       
     
//      dbm.saveDatabase();
      
     

    return 0;
}

Item* fillItem(string name){
    
}

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
    cout << "      -l NAME - show complete entry with NAME (with secrets)" << endl;
    cout << "      -sn KEYWORD - search for entries containing KEYWORD in name" << endl;
    cout << "      -sg KEYWORD - search for entries containing KEYWORD in group" << endl;
    cout << "      -E FILE_NAME - export database to CSV file" << endl;
    cout << "      -I FILE_NAME - import entries from CSV with header: group;name;login;password;" << endl;
}
