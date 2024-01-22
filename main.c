#include "header.h"

User user = {
    "rzy","123456","000001" //把用户信息定义为全局变量，方便存储，避免大批量传参
};

int main() {
    printf("欢迎使用在线投票系统！\n");
    MYSQL* conn = connectDB();
    bool is_login = false;
    char flag, vote_id[6];
    Votes* v;
    while (!is_login) {
        printf("请先登录或注册！\n输入1以登录，输入2以注册，其他值则退出：");
        scanf("%c", &flag), getchar(); //消耗缓冲区的\n
        if (flag == '1') is_login = login(conn);
        else if (flag == '2') my_register(conn);
        else {
            printf("期待您的下一次使用，按任意键以退出..."), getch();
            return 0;
        }
    }

    while (1) {
        showMainMenu();
        scanf("%c", &flag), getchar();
        switch (flag) {
        case '0':
            printf("欢迎下次使用~~\n点击任意键退出..."), getch();
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
                printf("啊呀，创建失败了呢，再试试吧~\n按任意键返回主菜单...");
                getch(), system("cls");
                continue;
            }
            publicVote(v, conn);
            break;
        case '3':
            showVoteList(conn);
            printf("输入你希望参与的投票的id：");
            fgets(vote_id, sizeof(vote_id), stdin);
            runawayN(vote_id) == true ? 1 : scanf("%*[^\n]%*c");
            v = getDataFromServer(vote_id, conn);
            joinVote(v);
            updateVote(v, conn, vote_id);
            system("pause");
            break;
        case '4':
            showVoteList(conn);
            printf("输入你希望查看的投票的id：");
            fgets(vote_id, sizeof(vote_id), stdin);
            runawayN(vote_id) == true ? 1 : scanf("%*[^\n]%*c");
            v = getDataFromServer(vote_id, conn);
            showVote(v);
            system("pause");
            break;
        default:
            printf("输入不合法！");
            break;
        }
        freeVotes(v);
        system("cls");
    }
    return 0;
}

static void showMainMenu() {
    printf("感谢使用！您已进入在线投票系统！\n \
            ----------------------------------------------------------------\n\
            |          1、查看已经发布的投票                                 |\n\
            |          2、创建属于我的投票                                   |\n\
            |          3、参与已经发布的投票                                 |\n\
            |          4、查看已发布的投票状态                               | \n\
            |          0、退出在线投票系统                                   |\n\
            ----------------------------------------------------------------\n\
请输入您的选择：");    
}