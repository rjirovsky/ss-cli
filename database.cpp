/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Radek Jirovský <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "database.h"
#include <fstream>

#include <sstream>
#include <fcntl.h>

const string Database::HEADER = "SAFE_STORAGE";
const string Database::CAPTION = "group;name;login;password;";

Database::Database()
{

}

Item* Database::getItemByName(const string& name)
{
    Item* item = NULL;
    for (list<Item*>::iterator iterator = items.begin(), end = items.end(); iterator != end; ++iterator) {
        if ((*iterator)->name == name){
            item = *iterator;
            break;
        }
    }
    return item;
}

void Database::insertItem(Item* item)
{    
    items.push_back(item);
    modified = true;
}

void Database::deleteItem(Item* item)
{
    items.remove(item);
    modified = true;
}

string Database::encrypt(string str)
{
    return str;
}

string Database::decrypt(string str)
{
    return str;
}

Database::~Database()
{
    for (list<Item*>::iterator it = items.begin(); it != items.end(); ++it) {        
        delete *it;
    }
    items.clear();
}

ostream& operator <<(ostream& out, const Item& item) {
    
    out << item.name << "(" << item.group << ")";
    
    return out;
}
