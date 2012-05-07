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
#include <string>
#include <iostream>

#ifndef ITEM_H
#define ITEM_H

using namespace std;

class Item 
{

public:
    Item(string group, string name, string login, string password):m_group(group),m_name(name),m_login(login),m_password(password){}
    virtual ~Item();

    string getName() const {return m_name;}
    string getGroup() const {return m_group;}
    string getPassword() const;
    string getLogin() const {return m_login;}
    
    void setName(string name) {m_name = name;}
    void setGroup(string group){m_group = group;}
    void setPassword(string password){m_password = password;}
    void setLogin(string login) {m_login = login;}
    
    friend ostream& operator <<(ostream& out, const Item& item);

private:
    string m_name;  //unique identifer
    string m_group;
    string m_password;  //encrypted password
    string m_login;
};

#endif // ITEM_H
