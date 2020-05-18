#include <bits/stdc++.h>
#include <Winsock2.h>
#include <stdio.h>
#include "udp.h"
#pragma comment(lib,"ws2_32.lib")

#define format_login "Log:"
#define format_check "Cck:"
#define format_sendkey "key:"


#define format_correct "Cor"

#define Key_file "keydata.txt"

using namespace std;

const int maxn = 1e5 + 5;

struct Key
{
	string val;
	int online, maxline;
}Key_list[maxn];

bool strcmp(string a, string b, int k)
{
	if (a.size() < k || b.size() < k) return 0;
	for (int i = 0; i < k; i++)
		if (a[i] != b[i])
			return 0;
	return 1;
}

void login()//登录
{
	void main_screen();
	string username;
	string password;
	string check_key;
	string rcv;
	bool is_checked = false;


	cout << "输入用户名：";
	cin >> username;
	cout << "输入密码：";
	cin >> password;

	//先检验是否已经检验过了
	Sendto(format_login + username + ":" + password);

	rcv.clear();
	rcv = Recvfrom();
	while (!strcmp(rcv, format_check, 4))
		rcv = Recvfrom();
	is_checked = rcv[4] - '0';

	if (!is_checked)//检验失败、未检验
	{
		cout << "输入得到的序列码（输入 -[数字] 表示为申请 数字*10的人数类型的序列码，最多50人）：";
		cin >> check_key;

		while (check_key.size() == 2 && check_key[0] == '-' && (check_key[1] <= '0' || check_key[1] > '5'))
		{
			cout << "输入得到的序列码（输入 -[数字] 表示为申请 数字*10的人数类型的序列码，最多50人）：";
			cin >> check_key;
		}
		/*while (check_key.size()!=2)
		{
			cout << "序列码错误，请重新输入序列码：";
			cin >> check_key;
		}*/


		/* ↓ 发送序列码给服务端检验 ↓ */
		Sendto(format_sendkey + username + ":" + password + ":" + check_key);



		/* ↑ 发送序列码给服务端检验 ↑ */

		while (!is_checked)
		{
			rcv.clear();
			while (!strcmp(rcv, format_check, 4))
				rcv = Recvfrom();
			is_checked = rcv[4] - '0';
			if (is_checked)
				break;
			

			cout << "校验失败，序列码错误，请重新输入序列码：";
			cin >> check_key;
			/* ↓ 发送序列码给服务端检验 ↓ */

			Sendto(format_sendkey + username + ":" + password + ":" + check_key);
	
			/* ↑ 发送序列码给服务端检验 ↑ */

		}
	}
	
	//检验成功，进入主界面



	cout << "校验成功" << endl;
	main_screen();

	
}



void ask_for_key()//申请票据
{



}

void return_key()//返回票据--正常退出
{ 



}

void main_screen()//主界面
{
	cout << "欢迎使用本软件，输入QUIT退出程序！" << endl;
	string cmd;
	cin >> cmd;
	while (cmd.compare("QUIT") != 0 && cmd.compare("quit") != 0)
	{
		cin >> cmd;
	}
	return_key();//归还票据
	cout << "程序正常退出，向服务器归还票据" << endl;
}


int main()
{
	

	Bind();
	
login();

	Close();
}