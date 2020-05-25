#include <bits/stdc++.h>
#include <Winsock2.h>
#include "udp_php.h"
#pragma comment(lib,"ws2_32.lib")
using namespace std;

#define format_login "Log:"
#define format_sendkey "key:"
#define format_quit "Qut:"
#define format_pulse "tim:"
#define SerErr "ERR"

//0:正常登录		1:不存在		2:重连且需验证	3:无序列号	4:密码错误	5:服务器满员		6:重连且不需验证
#define format_Logcheck "Lck:"

//0:正常登录		1：不存在	2：序列号满人
#define format_Keycheck "Cck:"

#define format_correct "Cor"

#define Key_file "keydata.txt"

void sendmsg(string format, int k)
{
	string ret = format + (char)('0' + k);
	Sendto(ret);
}

bool strcmp(string a, string b, int k)
{
	if (a.size() < k || b.size() < k) return 0;
	for (int i = 0; i < k; i++)
		if (a[i] != b[i])
			return 0;
	return 1;
}

string recvmsg(string format)
{
	string rcv;
	rcv.clear();

	while (!strcmp(rcv, format, 4) && !strcmp(rcv, SerErr, 3))
		rcv = Recvfrom();
	
	if (strcmp(rcv, SerErr, 3))
	{
		cout << "服务器崩溃,退出程序！\n";
		exit(1);
	}
	return rcv;
}


int main(int argc, char** argv)
{
	int status = 0;//0为注册，1为购买
	if (argc == 4) { status = 1; }
	else if (argc == 3) { status = 0; }
	else { cout << "检查输入!用户名或密码不能为空！" << endl; return 0; }
	if (strlen(argv[1]) == 0)
	{
		cout << "用户名不能为空" << endl;
		return 0;
	}
	if (strlen(argv[2]) == 0)
	{
		cout << "密码不能为空" << endl;
		return 0;
	}


	string username = argv[1];
	string password = argv[2];
	int c=0;
	if(status==1)c= atol(argv[3]);
	Bind();
	
	//发送登录请求
	Sendto(format_login + username + ":" + password);
	//cout << format_login + username + ":" + password << endl;
	string rcv;
	int logmsg = -1;

	string inkey;
	inkey.clear();
	if (c == 10)inkey = "-1";
	if (c == 20)inkey = "-2";
	if (c == 30)inkey = "-3";
	if (c == 40)inkey = "-4";
	if (c == 50)inkey = "-5";


	rcv = recvmsg(format_Logcheck);
	//cout << rcv << endl;
	logmsg = rcv[4] - '0';
	//cout << "logmsg:"<<logmsg << endl;
	string userkey;
	int keymsg = -1;
	switch (logmsg)
	{
	case 1:
		
		cout << "已创建新用户，账号：" << username <<" "<< "密码：" << password << endl;
		if (status == 0)break;
		
	case 2:
		if (status == 0)
		{
			cout << "账户已存在"<< endl;
			break;
		}
	case 3:

		if (status == 1)
		{
			Sendto(format_sendkey + username + ":" + inkey);
			rcv = recvmsg(format_Keycheck);
			keymsg = rcv[4] - '0';

			userkey.clear();
			if (keymsg == 0)
			{
				if (rcv.size() > 5)
				{
					for (int i = 6; i < 16; i++)
						userkey.push_back(rcv[i]);
					cout << "申请序列号成功：" << userkey << endl;
				}

			}
			break;
		}
		else 
		if (status == 0)
		{
			cout << "账户已存在" << endl;
			break;
		}

		



	case 4:
		cout << "密码错误，请重试" << endl;
		return 0;
	case 5:
		cout << "服务器已满人，请稍后再试" << endl;
		return 0;
	case 6:
		if (status == 0)
		{
			cout << "账户已存在" << endl;
			break;
		}
		cout << "已有序列码！同一用户不可再申请序列码" << endl;
		break;
	case 0:
		if (status == 0)
		{
			cout << "账户已存在" << endl;
			break;
		}
		cout << "已有序列码！同一用户不可再申请序列码" << endl;
		break;
	default:
		cout << "信息异常，服务器已崩溃" << endl;
		break;
	}
	Sendto(format_quit + username);
}
