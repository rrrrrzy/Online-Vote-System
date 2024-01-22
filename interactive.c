#include "header.h"


Votes* createVote() {
    Votes* vote = (Votes*)malloc(sizeof(Votes));
    printf("请输入投票名称：");
    fgets(vote->vote_title, 61, stdin);
    runawayN(vote->vote_title);
    printf("请输入问题个数（最多5个）：");
    scanf("%d", &vote->ques_num);
    getchar(); // 消耗掉输入缓冲区中的换行符
    
    if (vote->ques_num > 5) {
        printf("您的输入不符合规格\n");
        free(vote);
        return NULL;
    }
    int i,j;
    for (i = 0; i < vote->ques_num; i++) {
        printf("请输入问题%d的标题：", i + 1);
        fgets(vote->my_ques[i].title, 31, stdin);
        runawayN(vote->my_ques[i].title);
        printf("请输入问题%d的选项数（最多5个）：", i + 1);
        scanf("%d", &vote->my_ques[i].choice_num);
        getchar(); // 消耗掉输入缓冲区中的换行符
        
        if (vote->my_ques[i].choice_num > 5) {
            printf("您的输入不符合规格\n");
            free(vote);
            return NULL;
        }
        
        for (j = 0; j < vote->my_ques[i].choice_num; j++) {
            printf("请输入问题%d的选项%d：", i + 1, j + 1);
            char* choice = (char*)malloc(sizeof(char) * 31);
            fgets(choice, 31, stdin);
            runawayN(choice);
            vote->my_ques[i].choices[j] = choice;
            vote->my_ques[i].choose_num[j] = 0;
        }
    }

    strcpy(vote->creator, user.u_id);

    
    printf("投票创建成功！\n\n");
    checkVote(vote);
    
    return vote;
}

void checkVote(Votes* v) { //检查自己即将发布的vote
    printf("投票名称：%s\n", v->vote_title);
    printf("问题个数：%d\n", v->ques_num);

    for (int i = 0; i < v->ques_num; i++) {
        printf("问题%d的标题：%s\n", i + 1, v->my_ques[i].title);
        printf("问题%d的选项数：%d\n", i + 1, v->my_ques[i].choice_num);

        for (int j = 0; j < v->my_ques[i].choice_num; j++) {
            printf("问题%d的选项%d：%s\n", i + 1, j + 1, v->my_ques[i].choices[j]);
        }
    }

    printf("创建者：%s\n", v->creator);
    printf("您认为还需要修改吗？提交以后就不能修改了呦。需要输入“y”，不需要输入“n”");
	char a; 
	scanf("%c",&a);
    getchar();

    while (a != 'y' && a != 'n') {
        printf("您的输入不正确，请重新输入（y/n）：");
        scanf(" %c", &a);
        getchar();
    }

    if (a == 'y') {
        createVote();
    }
    else {
        return;
    }
}

void joinVote(Votes* v) {
    printf("欢迎参与投票：%s\n", v->vote_title);
    int i, j, k;
    for (i = 0; i < v->ques_num; i++) {
        printf("问题%d的标题：%s\n", i + 1, v->my_ques[i].title);
        printf("问题%d的选项数：%d\n", i + 1, v->my_ques[i].choice_num);

        for (j = 0; j < v->my_ques[i].choice_num; j++) {
            printf("%d. %s\n", j + 1, v->my_ques[i].choices[j]);
        }

        printf("请输入问题%d的选项编号进行投票：", i + 1);
        scanf("%d", &k);
        getchar(); // 消耗掉输入缓冲区中的换行符

        if (k < 1 || k > v->my_ques[i].choice_num) {
            printf("选项编号超出范围，请重新选择。\n");
            i--; // 重新投票同一个问题
            continue;
        }

        v->my_ques[i].choose_num[k - 1]++;
        
    }
	v->join_num++;
    printf("投票成功！感谢您的参与。\n");
    printf("当前投票参与人数：%d\n", v->join_num);
}

void showVote(Votes* v) {
	if(strcmp(v->creator,user.u_id)==0)
	{
		printf("验证通过，您具有观看权限"); 
	    printf("投票名称：%s\n", v->vote_title);
	    printf("问题个数：%d\n", v->ques_num);
		int i,j;
	    for (i = 0; i < v->ques_num; i++) {
	        printf("\n问题%d的标题：%s\n", i + 1, v->my_ques[i].title);
	        printf("问题%d的选项数：%d\n", i + 1, v->my_ques[i].choice_num);
	
	        for (j = 0; j < v->my_ques[i].choice_num; j++) {
	            printf("问题%d的选项%d：%s\n", i + 1, j + 1, v->my_ques[i].choices[j]);
	            printf("问题%d的选项%d的投票人数：%d (占比为%.2f%%)\n", i + 1, j + 1, v->my_ques[i].choose_num[j], (float)v->my_ques[i].choose_num[j]/v->join_num*100);
	        }
	    }
	
	    printf("参与投票的总人数：%d\n", v->join_num);
	}
	else
	{
		printf("不好意思，您暂时没有权限观看结果！\n");
	}
}

