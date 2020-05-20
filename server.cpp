#define _CRT_SECURE_NO_WARNINGS
#include <bits/stdc++.h>
#include <Winsock2.h>
#include <thread>
#include <stdio.h>
#include "udp.h"
#pragma comment(lib,"ws2_32.lib")

#define ip_server "127.0.0.1"
#define Key_file "keydata.txt"
#define User_file "userdata.txt"
#define format_pulse "tim:"
#define format_login "Log:"

//0:正常登录		1:不存在		2:重连且需验证	3:无序列号	4:密码错误	5:服务器满员		6:重连且不需验证
#define format_Logcheck "Lck:"

//0:正常登录		1：不存在	2：序列号满人
#define format_Keycheck "Cck:"

#define format_sendkey "key:"
#define format_quit "Qut:"


const int Max_Res_Time = 30;//最长等待反馈时间 单位为s

using namespace std;

void flush();

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
	Key() {};
	Key(string v, int o, int m) : val(v), online(o), maxline(m) {};
};

struct user
{
	string username;
	string password;
	string userkey;
	bool is_check;
	bool is_online;
	
	time_t last_time;

	user() {};

	user(string name, string pass, string key, bool check, bool online)
		: username(name), password(pass), userkey(key), is_check(check), is_online(online) {};
};



int maxuser, nowuser;
int key_sum, uesr_sum;			//记录一共有多少许可证与用户
map<string, int> keymp;			//第一位是1
map<string, int>usermp;			//第一位是1
vector<Key> Key_list;
vector<user> user_list;

int sum[10] = { 10, 20, 30, 40, 50 };

void init()
{
	maxuser = 3;
	nowuser = 0;
	key_sum = 0;
	uesr_sum = 0;
	Key_list.clear();
	user_list.clear();
	Key_list.push_back(Key());
	user_list.push_back(user());
	ifstream in(Key_file);
	Key ktmp;
	while (in >> ktmp.val >> ktmp.online >> ktmp.maxline)
	{
		Key_list.push_back(ktmp);
		keymp[ktmp.val] = ++key_sum;
	}

	in = ifstream(User_file);
	user utmp;
	while (in >> utmp.username >> utmp.password >> utmp.is_check >> utmp.is_online >> utmp.userkey >> utmp.last_time)
	{
		user_list.push_back(utmp);
		usermp[utmp.username] = ++uesr_sum;
		if (utmp.is_online == 1 && utmp.is_check) nowuser++;
	}
	cout << "当前线上用户有：" << nowuser << endl;
}

void flush()
{
	ofstream out = ofstream(User_file);
	for (int i = 1; i < user_list.size(); i++)
		out << user_list[i].username << " " << user_list[i].password << " " << user_list[i].is_check
			<< " " << user_list[i].is_online << " " << user_list[i].userkey << " " << user_list[i].last_time << endl;

	out = ofstream(Key_file);
	for (int i = 1; i < Key_list.size(); i++)
		out << Key_list[i].val << " " << Key_list[i].online << " " << Key_list[i].maxline << endl;
	cout << "当前线上用户有：" << nowuser << endl;
}

//发送格式化信息
void sendmsg(string format, int k)
{
	string ret = format + (char)('0' + k);
	cout << k <<" " <<ret << endl;
	Sendto(ret);
}

//添加用户
void add_user(user utmp)
{
	user_list.push_back(utmp);
	usermp[utmp.username] = ++uesr_sum;
}

//添加序列号
void add_key(Key ktmp)
{
	Key_list.push_back(ktmp);
	usermp[ktmp.val] = ++key_sum;
}

//检查序列号
int Key_check(string k)
{
	if (k.size() != 10) return -1;
	return keymp[k];
}

//生成序列号
Key Key_From(int kind)
{
	srand(time(0));
	Key ret;
	ret.val = "0";
	while (Key_check(ret.val) != 0)
	{
		ret.val.clear();
		for (int i = 0; i < 10; i++)
			ret.val.push_back(rand() % 10 + '0');
	}

	ret.online = 1;
	ret.maxline = sum[kind-1];


	add_key(ret);

	return ret;
}


//登录检查
void login(string k)
{
	cout << "login!\n";

	if (nowuser >= maxuser)
	{
		sendmsg(format_Logcheck, 5);
		return;
	}

	int index = 4;
	string username, password;
	while (k[index] != ':') username.push_back(k[index++]);
	index++;
	while (index < k.size()) password.push_back(k[index++]);
	cout << username << " " << password << endl;


	index = 0;
	if (index = usermp[username])
	{
		if (password == user_list[index].password)
		{
			if (user_list[index].is_online == 1 && user_list[index].is_check == 0)
				sendmsg(format_Logcheck, 2);
				
			else if (user_list[index].is_online == 1 && user_list[index].is_check == 1)
			{
				time(&user_list[index].last_time);
				nowuser++;
				sendmsg(format_Logcheck, 6);
			}
				
			else if (user_list[index].is_check == 1)
			{
				nowuser++;
				sendmsg(format_Logcheck, 0);
				time(&user_list[index].last_time);
				user_list[index].is_online = 1;
			}
			else
			{
				sendmsg(format_Logcheck, 3);
				user_list[index].is_online = 1;
			}
		}
		else sendmsg(format_Logcheck, 4);
	}
	else
	{
		user tmp(username, password, "0", 0, 0);
		add_user(tmp);
		sendmsg(format_Logcheck, 1);
	}
}

//序列号检测
void keycheck(string k)
{
	cout << "check!\n";
	int index = 4;
	string username, check_key;
	cout << k << endl;
	while (k[index] != ':') username.push_back(k[index++]);
	index++;

	while (index < k.size()) check_key.push_back(k[index++]);
	cout << username << " " << check_key << endl;

	//赋予新的序列号
	if (check_key[0] == '-')
	{
		cout << "give!" << endl;
		Key new_key = Key_From(check_key[1] - '0');
		cout << "finduser!\n";
		int index = usermp[username];
		user_list[index].userkey = new_key.val;
		user_list[index].is_check = 1;
		user_list[index].is_online = 1;
		time(&user_list[index].last_time);
		nowuser++;

		Sendto(format_Keycheck + (string)"0:"+ new_key.val);
		return;

	}


	cout << "match!\n";
	
	int kindex = Key_check(check_key);
	if (kindex > 0)
	{
		if (Key_list[kindex].maxline > Key_list[kindex].online)
		{
			Key_list[kindex].online++;
			int uindex = usermp[username];
			user_list[uindex].is_online = 1;
			user_list[uindex].is_check = 1;
			user_list[uindex].userkey = check_key;
			time(&user_list[uindex].last_time);
			nowuser++;
			sendmsg(format_Keycheck, 0);
		}
		else sendmsg(format_Keycheck, 2);
	}
	else sendmsg(format_Keycheck, 1);
}


void keyreturn(string k)
{
	cout << "return!\n";
	int index = 4;
	string username;
	cout << k << endl;

	while (index < k.size()) username.push_back(k[index++]);
	cout << username << endl;

	index = usermp[username];
	user_list[index].is_online = 0;
	nowuser--;
}

void check_clock()
{
	time_t now;
	time(&now);
	while (1)
	{
		time(&now);
		for (int i = 1; i < user_list.size(); i++)
		{
			if (difftime(now, user_list[i].last_time) >= Max_Res_Time && user_list[i].is_online == 1 && user_list[i].is_check == 1)
			{
				nowuser--;
				user_list[i].is_online = 0;
				cout << "offline:" << user_list[i].username << endl;
				flush();
			}
		}
	}
}

void time_refresh(string k)
{
	cout << "refresh!\n";
	int index = 4;
	string username;
	cout << k << endl;

	while (index < k.size()) username.push_back(k[index++]);
	cout << username << endl;

	index = usermp[username];
	time(&user_list[index].last_time);
}


void start()
{
	thread ct(check_clock);
	string rcv;
	while (1)
	{
		rcv.clear();

		while (rcv.size() < 3) rcv = Recvfrom();
		//cout << rcv << endl;
		if (strcmp(rcv, format_login, 4)) login(rcv);
		else if (strcmp(rcv, format_sendkey, 4)) keycheck(rcv);
		else if (strcmp(rcv, format_quit, 4)) keyreturn(rcv);
		else if (strcmp(rcv, format_pulse, 4)) time_refresh(rcv);
		flush();
	}
}




int main()
{
	init();

	Bind();

	start();

	Close();
}