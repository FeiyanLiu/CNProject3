#include <bits/stdc++.h>
#include <Winsock2.h>
#include <stdio.h>
#include <thread>
#include "udp_client.h"
#pragma comment(lib,"ws2_32.lib")

#define format_login "Log:"
#define format_sendkey "key:"
#define format_quit "Qut:"
#define format_pulse "tim:"
#define format_Online "Onck"
#define SerErr "ERR"

//0:正常登录		1:不存在		2:重连且需验证	3:无序列号	4:密码错误	5:服务器满员		6:重连且不需验证
#define format_Logcheck "Lck:"

//0:正常登录		1：不存在	2：序列号满人
#define format_Keycheck "Cck:"

#define format_correct "Cor"

#define Key_file "keydata.txt"



using namespace std;


const int cycle_res_time = 10*60;//每次回复周期 单位为s
const int wait_time = 5;//最长等待回复 单位为s


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

	user() {};

	user(string name, string pass, string key, bool check, bool online)
		: username(name), password(pass), userkey(key), is_check(check), is_online(online) {};
}myself;

//发送格式化信息
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



void init()
{
	myself.username.clear();
	myself.password.clear();
	myself.userkey.clear();
	myself.is_check = 0;
	myself.is_online = 0;
}


void login()//登录
{
	string rcv;
	int logmsg = -1;


	cout << "输入用户名：";
	cin >> myself.username;
	cout << "输入密码：";
	cin >> myself.password;

	//发送登录请求
	Sendto(format_login + myself.username + ":" + myself.password);

	rcv = recvmsg(format_Logcheck);
	logmsg = rcv[4] - '0';

	switch (logmsg)
	{
	case 1:
		myself.is_online = 1;
		cout << "已创建新用户，账号：" << myself.username << "密码：" << myself.password << endl;
		break;
	case 2:
		myself.is_online = 1;
		cout << "用户已重连" << endl;
		break;
	case 3:
		myself.is_online = 1;
		cout << "已登录，该用户暂缺序列号" << endl;
		break;
	case 4:
		cout << "密码错误，请重试" << endl;
		break;
	case 5:
		cout << "服务器已满人，请稍后再试" << endl;
		break;
	case 6:
		myself.is_check = 1;
		myself.is_online = 1;
		cout << "用户已重连" << endl;
		break;
	case 0:
		myself.is_online = 1;
		myself.is_check = 1;
		cout << "登录成功！" << endl;
		break;
	default:
		cout << "信息异常，服务器已崩溃" << endl;
		break;
	}
}


void check_key()
{
	string inkey;
	inkey.clear();
	while (!((inkey.size() == 2 && inkey[0] == '-' && inkey[1] >= '1' && inkey[1] <= '5') || (inkey.size() == 10)))
	{
		if (inkey.size()) cout << "输入格式错误！\n";
		cout << "输入得到的(10位)序列码（输入 -[数字] 表示为申请 数字*10的人数类型的序列码，最多50人）：";
		cin >> inkey;
	}


	/* ↓ 发送序列码给服务端检验 ↓ */
	Sendto(format_sendkey + myself.username + ":" + inkey);

	/* ↑ 发送序列码给服务端检验 ↑ */

	string rcv;
	int keymsg = -1;
	rcv = recvmsg(format_Keycheck);
	keymsg = rcv[4] - '0';

	switch (keymsg)
	{
	case 1:
		cout << "该序列号不存在" << endl;
		break;
	case 2:
		cout << "该序列号使用人数已满" << endl;
		break;
	case 0:
		myself.is_check = 1;
		if (rcv.size() > 5)
		{
			for (int i = 6; i < 16; i++)
				myself.userkey.push_back(rcv[i]);
			cout << "申请序列号成功：" << myself.userkey << endl;
		}
		else
		{
			myself.userkey = inkey;
			cout << "绑定序列号成功" << endl;
		}
		break;
	default:
		break;
	}
}



void return_key()//返回票据--正常退出
{
	Sendto(format_quit + myself.username);
	myself.is_online = 0;
}

void comfirm_online()//计时
{
	while (1)
	{
		cout << "发送在线信息!" << endl;
		Sendto(format_pulse + myself.username);
		recvmsg(format_Online);
		time_t start_time;
		time(&start_time);
		time_t end_time;
		time(&end_time);
		while (difftime(end_time ,start_time) != cycle_res_time)
		{
			time(&end_time);
		}
	}
}



void main_screen()//主界面
{
	
	thread time_count(comfirm_online);
	cout << "***************************************" << endl;
	cout << "欢迎使用本软件，输入QUIT退出程序！" << endl;
	string cmd;
	cin >> cmd;
	while (cmd.compare("QUIT") != 0 && cmd.compare("quit") != 0)
		cin >> cmd;

	time_count.detach();
	TerminateThread(time_count.native_handle(),0);
	

	return_key();//归还票据
	cout << "程序正常退出，向服务器归还票据" << endl;
	return;
}


int main()
{

	Bind();

	init();

	while (!myself.is_online) login();

	while (!myself.is_check) check_key();

	main_screen();


	Close();
}