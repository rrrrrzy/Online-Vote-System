#include<stdio.h>
#include "cJSON.h"
#include "mysql/mysql.h"
#include<winsock.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<conio.h>
#pragma comment (lib, "libmysql.lib")

typedef struct m_user {
	char u_name[10];
    char u_passwd[16];
	char u_id[7]; //һ����λ����
}User;

typedef struct m_ques {
    int   choice_num;	//�������ж��ٸ�ѡ��
    char  title[31];	//����ı���
	char* choices[5];	//ѡ������
    int	  choose_num[5]; //ÿ��ѡ��ж��ٸ���ѡ
}Question;

typedef struct m_Votes {
    int       ques_num;		//����������
    char      vote_title[61];	//ͶƱ������
    Question  my_ques[5];	//������⣬������ѡ����Ŀ-����-ѡ������-��ѡ��ͶƱ����
    int       join_num;		//����ͶƱ��������
    char      creator[7];
}Votes;

extern User user;

MYSQL* connectDB();

int showVoteList(MYSQL* conn);

void closeDB(MYSQL* conn);

void publicVote(Votes* v, MYSQL* conn);

Votes* createVote();

void checkVote(Votes* v);

void updateVote(Votes* v, MYSQL* conn, char* id);

void joinVote(Votes* v);

bool login(MYSQL* conn);

Votes* getDataFromServer(const char* id, MYSQL* conn);

void showVote(Votes* v);

bool runawayN(char * c);

cJSON* struct2json(Votes* v);

void json2struct(cJSON* json, Votes* vote);

void my_register(MYSQL* conn);

static void showMainMenu();

bool u_idExist(const char* id, MYSQL* conn);

void freeVotes(Votes* v);