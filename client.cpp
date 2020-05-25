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

//0:������¼		1:������		2:����������֤	3:�����к�	4:�������	5:��������Ա		6:�����Ҳ�����֤
#define format_Logcheck "Lck:"

//0:������¼		1��������	2�����к�����
#define format_Keycheck "Cck:"

#define format_correct "Cor"

#define Key_file "keydata.txt"



using namespace std;


const int cycle_res_time = 10*60;//ÿ�λظ����� ��λΪs
const int wait_time = 5;//��ȴ��ظ� ��λΪs


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

//���͸�ʽ����Ϣ
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
		cout << "����������,�˳�����\n";
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


void login()//��¼
{
	string rcv;
	int logmsg = -1;


	cout << "�����û�����";
	cin >> myself.username;
	cout << "�������룺";
	cin >> myself.password;

	//���͵�¼����
	Sendto(format_login + myself.username + ":" + myself.password);

	rcv = recvmsg(format_Logcheck);
	logmsg = rcv[4] - '0';

	switch (logmsg)
	{
	case 1:
		myself.is_online = 1;
		cout << "�Ѵ������û����˺ţ�" << myself.username << "���룺" << myself.password << endl;
		break;
	case 2:
		myself.is_online = 1;
		cout << "�û�������" << endl;
		break;
	case 3:
		myself.is_online = 1;
		cout << "�ѵ�¼�����û���ȱ���к�" << endl;
		break;
	case 4:
		cout << "�������������" << endl;
		break;
	case 5:
		cout << "�����������ˣ����Ժ�����" << endl;
		break;
	case 6:
		myself.is_check = 1;
		myself.is_online = 1;
		cout << "�û�������" << endl;
		break;
	case 0:
		myself.is_online = 1;
		myself.is_check = 1;
		cout << "��¼�ɹ���" << endl;
		break;
	default:
		cout << "��Ϣ�쳣���������ѱ���" << endl;
		break;
	}
}


void check_key()
{
	string inkey;
	inkey.clear();
	while (!((inkey.size() == 2 && inkey[0] == '-' && inkey[1] >= '1' && inkey[1] <= '5') || (inkey.size() == 10)))
	{
		if (inkey.size()) cout << "�����ʽ����\n";
		cout << "����õ���(10λ)�����루���� -[����] ��ʾΪ���� ����*10���������͵������룬���50�ˣ���";
		cin >> inkey;
	}


	/* �� ���������������˼��� �� */
	Sendto(format_sendkey + myself.username + ":" + inkey);

	/* �� ���������������˼��� �� */

	string rcv;
	int keymsg = -1;
	rcv = recvmsg(format_Keycheck);
	keymsg = rcv[4] - '0';

	switch (keymsg)
	{
	case 1:
		cout << "�����кŲ�����" << endl;
		break;
	case 2:
		cout << "�����к�ʹ����������" << endl;
		break;
	case 0:
		myself.is_check = 1;
		if (rcv.size() > 5)
		{
			for (int i = 6; i < 16; i++)
				myself.userkey.push_back(rcv[i]);
			cout << "�������кųɹ���" << myself.userkey << endl;
		}
		else
		{
			myself.userkey = inkey;
			cout << "�����кųɹ�" << endl;
		}
		break;
	default:
		break;
	}
}



void return_key()//����Ʊ��--�����˳�
{
	Sendto(format_quit + myself.username);
	myself.is_online = 0;
}

void comfirm_online()//��ʱ
{
	while (1)
	{
		cout << "����������Ϣ!" << endl;
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



void main_screen()//������
{
	
	thread time_count(comfirm_online);
	cout << "***************************************" << endl;
	cout << "��ӭʹ�ñ����������QUIT�˳�����" << endl;
	string cmd;
	cin >> cmd;
	while (cmd.compare("QUIT") != 0 && cmd.compare("quit") != 0)
		cin >> cmd;

	time_count.detach();
	TerminateThread(time_count.native_handle(),0);
	

	return_key();//�黹Ʊ��
	cout << "���������˳�����������黹Ʊ��" << endl;
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