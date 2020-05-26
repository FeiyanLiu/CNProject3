#include <bits/stdc++.h>
#include <Winsock2.h>
#include <stdio.h>
#include <thread>
#include <graphics.h>              // 引用图形库头文件
#include <conio.h>
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
int r[4] = {130,140,170,180};//按钮的二维数组
int t[4] = { 0,0,300,120 };//文本的二维数组

using namespace std;


const int cycle_res_time = 10*60;//每次回复周期 单位为s
const int wait_time = 5;//最长等待回复 单位为s
int button_judge(int x, int y)
{
	if (x > r[0] && x<r[2] && y>r[1] && y < r[3])return 1;
	return 0;
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


string login()//登录
{
	string rcv;
	int logmsg = -1;

	TCHAR s[10]; TCHAR c[10];
	InputBox(s, 10, _T("请输入用户名"));
	myself.username = TCHARToChar(s);
	InputBox(c, 10, _T("请输入密码"));
	myself.password = TCHARToChar(c);
	//cout << "输入用户名：";
	//cin >> myself.username;
	//cout << "输入密码：";
	//cin >> myself.password;
	//发送登录请求
	Sendto(format_login + myself.username + ":" + myself.password);
	string feedback="";
	rcv = recvmsg(format_Logcheck);
	logmsg = rcv[4] - '0';

	switch (logmsg)
	{
	case 1:
		myself.is_online = 1;
		feedback = "已创建新用户，账号：" + myself.username + "密码：" + myself.password;
		//cout << "已创建新用户，账号：" << myself.username << "密码：" << myself.password << endl;
		break;
	case 2:
		myself.is_online = 1;
		feedback = "用户已重连";
		//cout << "用户已重连" << endl;
		break;
	case 3:
		myself.is_online = 1;
		feedback = "已登录，该用户暂缺序列号";
		//cout << "已登录，该用户暂缺序列号" << endl;
		break;
	case 4:
		feedback = "密码错误，请重试";
		//cout << "密码错误，请重试" << endl;
		break;
	case 5:
		feedback = "服务器已满人，请稍后再试";
		//cout << "服务器已满人，请稍后再试" << endl;
		break;
	case 6:
		myself.is_check = 1;
		myself.is_online = 1;
		feedback = "用户已重连";
		//cout << "用户已重连" << endl;
		break;
	case 0:
		myself.is_online = 1;
		myself.is_check = 1;
		feedback = "登录成功！";
		//cout << "登录成功！" << endl;
		break;
	default:
		feedback = "信息异常，服务器已崩溃";
		//cout << "信息异常，服务器已崩溃" << endl;
		break;
	}
	return feedback;
}


string check_key()
{
	string inkey;
	inkey.clear();
	while (!((inkey.size() == 2 && inkey[0] == '-' && inkey[1] >= '1' && inkey[1] <= '5') || (inkey.size() == 10)))
	{
		TCHAR s[20];
		if (inkey.size()) InputBox(s, 20, _T("输入格式错误！"));//cout << "输入格式错误！\n";
		else InputBox(s, 20, _T("输入得到的(10位)序列码（输入 -[数字] 表示为申请 数字*10的人数类型的序列码，最多50人）："));
		inkey = TCHARToChar(s);
		memset(s, sizeof(s), 0);
		//cout << "输入得到的(10位)序列码（输入 -[数字] 表示为申请 数字*10的人数类型的序列码，最多50人）：";
		//cin >> inkey;
	}

	string check_result = "";
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
		check_result = "该序列号不存在";
		//cout << "该序列号不存在" << endl;
		break;
	case 2:
		check_result = "该序列号使用人数已满";
		//cout << "该序列号使用人数已满" << endl;
		break;
	case 0:
		myself.is_check = 1;
		if (rcv.size() > 5)
		{
			for (int i = 6; i < 16; i++)
				myself.userkey.push_back(rcv[i]);
			check_result = "申请序列号成功：" + myself.userkey;
			//cout << "申请序列号成功：" << myself.userkey << endl;
		}
		else
		{
			myself.userkey = inkey;
			check_result = "绑定序列号成功";
			//cout << "绑定序列号成功" << endl;
		}
		break;
	default:
		break;
	}
	return check_result;
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
		//cout << "发送在线信息!" << endl;
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
	short win_width, win_height;//定义窗口的宽度和高度
	win_width = 300; win_height = 200;
	string t1, t2 = "";
	Bind();
	init();
	RECT R = { r[0],r[1],r[2],r[3] };//按钮矩形指针
	RECT T = { t[0],t[1],t[2],t[3] };//文本矩形指针
	
	while (!myself.is_online) t1=login();
	initgraph(win_width, win_height);//初始化窗口（黑屏）
	for (int i = 0; i < 256; i += 5)
	{
		setbkcolor(RGB(i, i, i));//设置背景色，原来默认黑色
		cleardevice();//清屏（取决于背景色）
		//Sleep(15);//延时15ms
	}
	LOGFONT f;
	gettextstyle(&f);					//获取字体样式
	_tcscpy_s(f.lfFaceName, _T("宋体"));	//设置字体为宋体
	f.lfQuality = ANTIALIASED_QUALITY;    // 设置输出效果为抗锯齿  
	settextstyle(&f);                     // 设置字体样式
	settextcolor(BLACK);				//BLACK在graphic.h头文件里面被定义为黑色的颜色常量
	drawtext(t1.c_str(), &T, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形区域R1内输入文字，水平居中，垂直居中，单行显示
	Sleep(1000);

	while (!myself.is_check) t2=check_key();
	cleardevice();//清屏（取决于背景色）
	drawtext("确定", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形区域R1内输入文字，水平居中，垂直居中，单行显示
	drawtext(t2.c_str(), &T, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形区域R1内输入文字，水平居中，垂直居中，单行显示
	setlinecolor(BLACK);
	rectangle(r[0], r[1], r[2], r[3]);
	MOUSEMSG m;//鼠标指针
	int state = 0;
	while (true)
	{
		cout << state;
		m = GetMouseMsg();//获取一条鼠标消息
		int event = 0; 
		switch (m.uMsg)
		{
		case WM_MOUSEMOVE:
			setrop2(R2_XORPEN);
			setlinecolor(LIGHTCYAN);//线条颜色为亮青色
			setlinestyle(PS_SOLID, 3);//设置画线样式为实现，10磅
			setfillcolor(WHITE);//填充颜色为白色
			if (button_judge(m.x, m.y) != 0&&state!=2)
			{
				if (event != button_judge(m.x, m.y))
				{
					event = button_judge(m.x, m.y);//记录这一次触发的按钮
					fillrectangle(r[0], r[1], r[2], r[3]);//有框填充矩形（X1,Y1,X2,Y2）
				}
			}
			else
			{
				if (event != 0)//上次触发的按钮未被修正为原来的颜色
				{
					fillrectangle(r[0], r[1], r[2], r[3]);//两次同或为原来颜色
					event = 0;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			setrop2(R2_NOTXORPEN);//二元光栅——NOT(屏幕颜色 XOR 当前颜色)
			for (int i = 0; i <= 10; i++)
			{
				setlinecolor(RGB(25 * i, 25 * i, 25 * i));//设置圆颜色
				circle(m.x, m.y, 2 * i);
				Sleep(30);//停顿30ms
				circle(m.x, m.y, 2 * i);//抹去刚刚画的圆
			}
			if (button_judge(m.x, m.y)&&state==0) {
				cleardevice();
				thread time_count(comfirm_online);
				//cout << "***************************************" << endl;
				//cout << "欢迎使用本软件，输入QUIT退出程序！" << endl;
				drawtext("欢迎使用本软件，按退出按钮退出程序！", &T, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形区域R1内输入文字，水平居中，垂直居中，单行显示
				drawtext("退出", &R, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形区域R1内输入文字，水平居中，垂直居中，单行显示																	   //string cmd;
				state = 1;//cin >> cmd;
				//while (cmd.compare("QUIT") != 0 && cmd.compare("quit") != 0)
					//cin >> cmd;
				time_count.detach();
				TerminateThread(time_count.native_handle(), 0);
				//cout << "程序正常退出，向服务器归还票据" << endl;
				FlushMouseMsgBuffer();//单击事件后清空鼠标消息
			}
			else if (button_judge(m.x, m.y) && state == 1) {
				state = 2;
				cleardevice();
				return_key();//归还票据
				drawtext("程序正常退出，向服务器归还票据", &T, DT_CENTER | DT_VCENTER | DT_SINGLELINE);//在矩形区域R1内输入文字，水平居中，垂直居中，单行显示
				Sleep(1500);
				Close();
				FlushMouseMsgBuffer();
			}
			else {
				FlushMouseMsgBuffer();//单击事件后清空鼠标消息
			}
			break;
			//FlushMouseMsgBuff();//清空鼠标消息缓存区
		}
	}
}
