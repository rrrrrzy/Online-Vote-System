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
	char u_id[7]; //一个六位整数
}User;

typedef struct m_ques {
    int   choice_num;	//该问题有多少个选项
    char  title[31];	//问题的标题
	char* choices[5];	//选项内容
    int	  choose_num[5]; //每个选项都有多少个人选
}Question;

typedef struct m_Votes {
    int       ques_num;		//存放问题个数
    char      vote_title[61];	//投票的名称
    Question  my_ques[5];	//存放问题，包含：选项数目-标题-选项详情-各选项投票人数
    int       join_num;		//参与投票的总人数
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