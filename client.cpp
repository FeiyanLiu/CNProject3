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

void login()//��¼
{
	void main_screen();
	string username;
	string password;
	string check_key;
	string rcv;
	bool is_checked = false;


	cout << "�����û�����";
	cin >> username;
	cout << "�������룺";
	cin >> password;

	//�ȼ����Ƿ��Ѿ��������
	Sendto(format_login + username + ":" + password);

	rcv.clear();
	rcv = Recvfrom();
	while (!strcmp(rcv, format_check, 4))
		rcv = Recvfrom();
	is_checked = rcv[4] - '0';

	if (!is_checked)//����ʧ�ܡ�δ����
	{
		cout << "����õ��������루���� -[����] ��ʾΪ���� ����*10���������͵������룬���50�ˣ���";
		cin >> check_key;

		while (check_key.size() == 2 && check_key[0] == '-' && (check_key[1] <= '0' || check_key[1] > '5'))
		{
			cout << "����õ��������루���� -[����] ��ʾΪ���� ����*10���������͵������룬���50�ˣ���";
			cin >> check_key;
		}
		/*while (check_key.size()!=2)
		{
			cout << "������������������������룺";
			cin >> check_key;
		}*/


		/* �� ���������������˼��� �� */
		Sendto(format_sendkey + username + ":" + password + ":" + check_key);



		/* �� ���������������˼��� �� */

		while (!is_checked)
		{
			rcv.clear();
			while (!strcmp(rcv, format_check, 4))
				rcv = Recvfrom();
			is_checked = rcv[4] - '0';
			if (is_checked)
				break;
			

			cout << "У��ʧ�ܣ�������������������������룺";
			cin >> check_key;
			/* �� ���������������˼��� �� */

			Sendto(format_sendkey + username + ":" + password + ":" + check_key);
	
			/* �� ���������������˼��� �� */

		}
	}
	
	//����ɹ�������������



	cout << "У��ɹ�" << endl;
	main_screen();

	
}



void ask_for_key()//����Ʊ��
{



}

void return_key()//����Ʊ��--�����˳�
{ 



}

void main_screen()//������
{
	cout << "��ӭʹ�ñ����������QUIT�˳�����" << endl;
	string cmd;
	cin >> cmd;
	while (cmd.compare("QUIT") != 0 && cmd.compare("quit") != 0)
	{
		cin >> cmd;
	}
	return_key();//�黹Ʊ��
	cout << "���������˳�����������黹Ʊ��" << endl;
}


int main()
{
	

	Bind();
	
login();

	Close();
}