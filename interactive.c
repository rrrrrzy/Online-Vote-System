#include "header.h"


Votes* createVote() {
    Votes* vote = (Votes*)malloc(sizeof(Votes));
    printf("������ͶƱ���ƣ�");
    fgets(vote->vote_title, 61, stdin);
    runawayN(vote->vote_title);
    printf("������������������5������");
    scanf("%d", &vote->ques_num);
    getchar(); // ���ĵ����뻺�����еĻ��з�
    
    if (vote->ques_num > 5) {
        printf("�������벻���Ϲ��\n");
        free(vote);
        return NULL;
    }
    int i,j;
    for (i = 0; i < vote->ques_num; i++) {
        printf("����������%d�ı��⣺", i + 1);
        fgets(vote->my_ques[i].title, 31, stdin);
        runawayN(vote->my_ques[i].title);
        printf("����������%d��ѡ���������5������", i + 1);
        scanf("%d", &vote->my_ques[i].choice_num);
        getchar(); // ���ĵ����뻺�����еĻ��з�
        
        if (vote->my_ques[i].choice_num > 5) {
            printf("�������벻���Ϲ��\n");
            free(vote);
            return NULL;
        }
        
        for (j = 0; j < vote->my_ques[i].choice_num; j++) {
            printf("����������%d��ѡ��%d��", i + 1, j + 1);
            char* choice = (char*)malloc(sizeof(char) * 31);
            fgets(choice, 31, stdin);
            runawayN(choice);
            vote->my_ques[i].choices[j] = choice;
            vote->my_ques[i].choose_num[j] = 0;
        }
    }

    strcpy(vote->creator, user.u_id);

    
    printf("ͶƱ�����ɹ���\n\n");
    checkVote(vote);
    
    return vote;
}

void checkVote(Votes* v) { //����Լ�����������vote
    printf("ͶƱ���ƣ�%s\n", v->vote_title);
    printf("���������%d\n", v->ques_num);

    for (int i = 0; i < v->ques_num; i++) {
        printf("����%d�ı��⣺%s\n", i + 1, v->my_ques[i].title);
        printf("����%d��ѡ������%d\n", i + 1, v->my_ques[i].choice_num);

        for (int j = 0; j < v->my_ques[i].choice_num; j++) {
            printf("����%d��ѡ��%d��%s\n", i + 1, j + 1, v->my_ques[i].choices[j]);
        }
    }

    printf("�����ߣ�%s\n", v->creator);
    printf("����Ϊ����Ҫ�޸����ύ�Ժ�Ͳ����޸����ϡ���Ҫ���롰y��������Ҫ���롰n��");
	char a; 
	scanf("%c",&a);
    getchar();

    while (a != 'y' && a != 'n') {
        printf("�������벻��ȷ�����������루y/n����");
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
    printf("��ӭ����ͶƱ��%s\n", v->vote_title);
    int i, j, k;
    for (i = 0; i < v->ques_num; i++) {
        printf("����%d�ı��⣺%s\n", i + 1, v->my_ques[i].title);
        printf("����%d��ѡ������%d\n", i + 1, v->my_ques[i].choice_num);

        for (j = 0; j < v->my_ques[i].choice_num; j++) {
            printf("%d. %s\n", j + 1, v->my_ques[i].choices[j]);
        }

        printf("����������%d��ѡ���Ž���ͶƱ��", i + 1);
        scanf("%d", &k);
        getchar(); // ���ĵ����뻺�����еĻ��з�

        if (k < 1 || k > v->my_ques[i].choice_num) {
            printf("ѡ���ų�����Χ��������ѡ��\n");
            i--; // ����ͶƱͬһ������
            continue;
        }

        v->my_ques[i].choose_num[k - 1]++;
        
    }
	v->join_num++;
    printf("ͶƱ�ɹ�����л���Ĳ��롣\n");
    printf("��ǰͶƱ����������%d\n", v->join_num);
}

void showVote(Votes* v) {
	if(strcmp(v->creator,user.u_id)==0)
	{
		printf("��֤ͨ���������йۿ�Ȩ��"); 
	    printf("ͶƱ���ƣ�%s\n", v->vote_title);
	    printf("���������%d\n", v->ques_num);
		int i,j;
	    for (i = 0; i < v->ques_num; i++) {
	        printf("\n����%d�ı��⣺%s\n", i + 1, v->my_ques[i].title);
	        printf("����%d��ѡ������%d\n", i + 1, v->my_ques[i].choice_num);
	
	        for (j = 0; j < v->my_ques[i].choice_num; j++) {
	            printf("����%d��ѡ��%d��%s\n", i + 1, j + 1, v->my_ques[i].choices[j]);
	            printf("����%d��ѡ��%d��ͶƱ������%d (ռ��Ϊ%.2f%%)\n", i + 1, j + 1, v->my_ques[i].choose_num[j], (float)v->my_ques[i].choose_num[j]/v->join_num*100);
	        }
	    }
	
	    printf("����ͶƱ����������%d\n", v->join_num);
	}
	else
	{
		printf("������˼������ʱû��Ȩ�޹ۿ������\n");
	}
}

