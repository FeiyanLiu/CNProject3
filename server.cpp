#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>
#include <Winsock2.h>
#include <stdio.h>
#include "udp.h"
#pragma comment(lib,"ws2_32.lib")

#define ip_server "127.0.0.1"
#define Key_file "keydata.txt"
#define User_file "userdata.txt"

#define format_login "Log:"
#define format_check "Cck:"
#define format_sendkey "key:"


using namespace std;

bool strcmp(string a, string b, int k)
{
	if (a.size() < k || b.size() < k) return 0;
	for (int i = 0; i < k; i++)
		if (a[i] != b[i])
			return 0;
	return 1;
}

struct Key
{
	string val;
	int online, maxline;
};

struct user
{
	string username;
	string password;
	bool is_check;
	bool is_online;
	string userkey;
};

int key_sum, uesr_sum;			//记录一共有多少许可证与用户
map<string, int> keymp;			//第一位是1
vector<Key> Key_list;
vector<user> user_list;

int sum[10] = { 10, 20, 30, 40, 50 };

void init()
{
	Key_list.clear();
	user_list.clear();
	int tot = 0;
	ifstream in(Key_file);
	Key ktmp;
	while (in >> ktmp.val >> ktmp.online >> ktmp.maxline)
	{
		Key_list.push_back(ktmp);
		keymp[ktmp.val] = ++tot;
	}

	in = ifstream(User_file);
	user utmp;
	while (in >> utmp.username >> utmp.password >> utmp.is_check >> utmp.userkey)
	{
		user_list.push_back(utmp);
		cout << utmp.username << endl;
	}

	for (auto& e : user_list) cout << e.username << endl;
}


void add_user(user utmp)
{
	user_list.push_back(utmp);
	
	ofstream out(User_file);
	out << utmp.username << " " << utmp.password << " " << utmp.is_check << " " << utmp.userkey << endl;
	//fclose(stdout);
	//fclose(fp);
}

void add_key(Key ktmp)
{

}


int Key_check(Key k)
{
	if (k.val.size() != 10) return -1;
	if (keymp[k.val] > 0) return keymp[k.val];
	return 0;
}

Key Key_From(int kind)
{
	srand(time(0));
	Key ret;
	ret.val = "0";
	while (Key_check(ret) != 0)
	{
		ret.val.clear();
		for (int i = 0; i < 10; i++)
			ret.val.push_back(rand() % 10 + '0');
	}

	ret.online = 0;
	ret.maxline = sum[kind-1];

	add_key(ret);

	return ret;
}

void login(string k)
{
	cout << "login!\n";
	int index = 4;
	string username, password;
	while (k[index] != ':') username.push_back(k[index++]);
	index++;
	while (index < k.size()) password.push_back(k[index++]);
	cout << username << " " << password << endl;
	for (auto& e : user_list)
	{
		if (e.username == username && e.password == password)
		{
			if (e.is_check) 
			{ 
				Sendto(format_check + (string)"1"); 
				e.is_online = 1;

				//上线

			}
			else Sendto(format_check + (string)"0");
			cout << "find!\n";
			return;
		}
	}

	user tmp;
	tmp.username = username;	
	tmp.password = password;
	tmp.is_check = 0;
	tmp.is_online = 0;
	tmp.userkey = "0";
	add_user(tmp);
	Sendto(format_check + (string)"0");
}

void check(string k)
{
	cout << "check!\n";
	int index = 4;
	string username, password,check_key;
	cout << k << endl;
	while (k[index] != ':') username.push_back(k[index++]);
	index++;
	while (k[index] != ':') password.push_back(k[index++]);
	index++;
	while (index < k.size()) check_key.push_back(k[index++]);
	cout << username << " " << password << " " << check_key << endl;
	if (check_key[0] == '-')
	{
		cout << "give!" << endl;
		Key new_key = Key_From(check_key[1] - '0');
		cout << "finduser!\n";
		for (auto& u : user_list)
		{
			if (u.username == username && u.password == password)
			{
				u.is_online = 0; 
				u.is_check = 1;
				u.userkey = new_key.val;
				Sendto(format_check + (string)"1:"+ new_key.val);
				return;
			}
		}
		
	}


	cout << "match!\n";
	
	int find = 0;
	for (auto& e : Key_list)
	{
		if (e.val == check_key&& e.maxline > e.online)
		{
			find = 1;
			e.online++;

			for (auto& u : user_list)
			{
				if (u.username == username && u.password == password)
				{
					find = 1;
					u.is_online = 1; //默认上线
					u.is_check = 1;
					u.userkey = check_key;
					Sendto(format_check + (string)"1");
					break;
				}
			}
			break;
		}
	}

	if (!find)
	{
		Sendto(format_check + (string)"0");//检验失败
		return;
	}
	

}

void start()
{
	string rcv;
	while (1)
	{
		rcv.clear();

		while (rcv.size() < 3) rcv = Recvfrom();
		//cout << rcv << endl;
		if (strcmp(rcv, format_login, 4)) login(rcv);
		else if (strcmp(rcv, format_sendkey, 4)) check(rcv);
	}
}






int main()
{
	init();
	Bind();

	start();

	Close();
}