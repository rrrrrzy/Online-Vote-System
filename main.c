#include "header.h"

User user = {
    "rzy","123456","000001" //���û���Ϣ����Ϊȫ�ֱ���������洢���������������
};

int main() {
    printf("��ӭʹ������ͶƱϵͳ��\n");
    MYSQL* conn = connectDB();
    bool is_login = false;
    char flag, vote_id[6];
    Votes* v;
    while (!is_login) {
        printf("���ȵ�¼��ע�ᣡ\n����1�Ե�¼������2��ע�ᣬ����ֵ���˳���");
        scanf("%c", &flag), getchar(); //���Ļ�������\n
        if (flag == '1') is_login = login(conn);
        else if (flag == '2') my_register(conn);
        else {
            printf("�ڴ�������һ��ʹ�ã�����������˳�..."), getch();
            return 0;
        }
    }

    while (1) {
        showMainMenu();
        scanf("%c", &flag), getchar();
        switch (flag) {
        case '0':
            printf("��ӭ�´�ʹ��~~\n���������˳�..."), getch();
            return 0;
            break;
        case '1':
            showVoteList(conn);
            system("pause"), system("cls");
            continue;
            break;
        case '2':
            v = createVote();
            if (v == NULL) {
                printf("��ѽ������ʧ�����أ������԰�~\n��������������˵�...");
                getch(), system("cls");
                continue;
            }
            publicVote(v, conn);
            break;
        case '3':
            showVoteList(conn);
            printf("������ϣ�������ͶƱ��id��");
            fgets(vote_id, sizeof(vote_id), stdin);
            runawayN(vote_id) == true ? 1 : scanf("%*[^\n]%*c");
            v = getDataFromServer(vote_id, conn);
            joinVote(v);
            updateVote(v, conn, vote_id);
            system("pause");
            break;
        case '4':
            showVoteList(conn);
            printf("������ϣ���鿴��ͶƱ��id��");
            fgets(vote_id, sizeof(vote_id), stdin);
            runawayN(vote_id) == true ? 1 : scanf("%*[^\n]%*c");
            v = getDataFromServer(vote_id, conn);
            showVote(v);
            system("pause");
            break;
        default:
            printf("���벻�Ϸ���");
            break;
        }
        freeVotes(v);
        system("cls");
    }
    return 0;
}

static void showMainMenu() {
    printf("��лʹ�ã����ѽ�������ͶƱϵͳ��\n \
            ----------------------------------------------------------------\n\
            |          1���鿴�Ѿ�������ͶƱ                                 |\n\
            |          2�����������ҵ�ͶƱ                                   |\n\
            |          3�������Ѿ�������ͶƱ                                 |\n\
            |          4���鿴�ѷ�����ͶƱ״̬                               | \n\
            |          0���˳�����ͶƱϵͳ                                   |\n\
            ----------------------------------------------------------------\n\
����������ѡ��");    
}