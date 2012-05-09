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

/**
 * @brief 
 * 
 */
int main(int argc, char **argv)
{
    DatabaseManager dbm;
    
    if (argc < 3){
        cout << "Safe Storage (ss-cli) - password database" << endl;
        cout << "Usage: ss-cli [OPTION]" << endl;
        cout << "Options:" << endl;
        cout << "   no options - this help" << endl;
        cout << "   -n FILE_NAME - create new database file" << endl;
        cout << "   -f FILE_NAME [ARGS]- work with existing database" << endl;
        cout << "   Args:" << endl;
        cout << "      no args - list all entries (without secrets)" << endl;
        
        return 1;
        
    } else if(argc = 3){
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
    }
      

//      dbm.addItem("", "is_mu", "xjirovsk", "heslo");
//     dbm.addItem("skupina1", "nazev1", "login1", "heslo1");
//     dbm.addItem("skupina2", "nazev2", "login2", "heslo2");
//     dbm.addItem("skupina2", "nazev5", "login2", "heslo2");
//     dbm.addItem("skupina3", "nazev3", "login3", "heslo3");
//     dbm.addItem("skupina4", "nazev4", "login4", "heslo4");

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
//      dbm.printAllItems();
//      
//       dbm.editItem("banka","nazev5","","1234");
       
     
//      dbm.saveDatabase();
      
     

    return 0;
}
