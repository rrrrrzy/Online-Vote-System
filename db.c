#include "header.h"

#define SITE "edulearn.cn"
#define USER "onlineVote"
#define PASSWD "dazuoye_oV"
#define DATABASE "onlineVote"

int showVoteList(MYSQL* conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;

    const char* query = "SELECT id, vote_title, join_num FROM votes";

    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "拉取投票列表失败：%s\n", mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    //获取投票列表及对应id
    res = mysql_store_result(conn);
    int num = mysql_num_rows(res);
    //输出
    printf("当前可以投票的项目有： %d 项\n", num);
    while ((row = mysql_fetch_row(res)) != NULL) {
        long long int i;
        printf("ID: %s \t 标题: %s \t 已参与人数: %s\n", row[0], row[1], row[2]);
    }
    return num;
}

MYSQL* connectDB() {
    //这里在堆上申请空间，因为不能传递临时变量的地址！
    MYSQL* conn = (MYSQL*)malloc(1 * sizeof(MYSQL*));
    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, SITE, USER, PASSWD, DATABASE, 3306, NULL, 0)) {
        printf("与服务器连接成功！\n");
    }
    else {
        fprintf(stderr, "与服务器连接失败: %s 请检查您的网络和防火墙设置\n", mysql_error(conn));
    }
    return conn;
}

void closeDB(MYSQL* conn) {
    mysql_close(conn); //断开连接
    free(conn);        //释放堆空间
    conn = NULL;       //避免指针悬挂
}

void publicVote(Votes* v, MYSQL* conn) {
    MYSQL_STMT* stmt;  //初始化SQL语句载体
    MYSQL_BIND bind[5];//初始化SQL语句参数
    const char* insert_query = "INSERT INTO votes (ques_num, join_num, questions, vote_title, creator)\
                                VALUES (?, ?, ?, ?, ?)"; //插入语句
    const cJSON* json = struct2json(v); //把结构体转化成json
    const char* json_data = (char*)cJSON_Print(json); //把json字符串化

    // 准备SQL语句
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "数据库操控指令初始化失败：%s\n", mysql_stmt_error(stmt));
        mysql_close(conn);
        return;
    }

    // 准备绑定参数
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG; //ques_num的各参数
    bind[0].buffer = &v->ques_num;
    bind[0].buffer_length = 4;
    bind[0].is_null = 0;

    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &v->join_num;
    bind[1].buffer_length = 4;
    bind[1].is_null = 0;

    bind[2].buffer_type = MYSQL_TYPE_JSON;
    bind[2].buffer = (char*)json_data;
    bind[2].buffer_length = strlen(json_data);
    bind[2].is_null = 0;

    bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[3].buffer = v->vote_title;
    bind[3].buffer_length = strlen(v->vote_title);
    bind[3].is_null = 0;
    

    bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[4].buffer = v->creator;
    bind[4].buffer_length = strlen(v->creator);
    bind[4].is_null = 0;

    // SQL语句置入stmt
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
        fprintf(stderr, "数据库操控指令准备失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 绑定参数
    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "数据库操控指令绑定参数失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 执行SQL语句
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "数据库操控指令执行失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 关闭语句
    mysql_stmt_close(stmt);
}

void updateVote(Votes* v, MYSQL* conn, char* id) {
    MYSQL_STMT* stmt;  //初始化SQL语句载体
    MYSQL_BIND bind[5];//初始化SQL语句参数
    const char* insert_query = "UPDATE votes \
                                SET ques_num = ?, join_num = ?, questions = ?, vote_title = ? \
                                WHERE id = ?"; //插入语句
    const cJSON* json = struct2json(v); //把结构体转化成json
    const char* json_data = (char*)cJSON_Print(json); //把json字符串化
    char* my_id = id;
    // 准备SQL语句
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "数据库操控指令初始化失败：%s\n", mysql_stmt_error(stmt));
        mysql_close(conn);
        return;
    }

    // 准备绑定参数
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG; //ques_num的各参数
    bind[0].buffer = &v->ques_num;
    bind[0].buffer_length = 4;
    bind[0].is_null = 0;

    bind[1].buffer_type = MYSQL_TYPE_LONG;
    bind[1].buffer = &v->join_num;
    bind[1].buffer_length = 4;
    bind[1].is_null = 0;

    bind[2].buffer_type = MYSQL_TYPE_JSON;
    bind[2].buffer = (char*)json_data;
    bind[2].buffer_length = strlen(json_data);
    bind[2].is_null = 0;

    bind[3].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[3].buffer = v->vote_title;
    bind[3].buffer_length = strlen(v->vote_title);
    bind[3].is_null = 0;
    
    bind[4].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[4].buffer = my_id;
    bind[4].buffer_length = 4;
    bind[4].is_null = 0;

    // SQL语句置入stmt
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
        fprintf(stderr, "数据库操控指令准备失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 绑定参数
    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "数据库操控指令绑定参数失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 执行SQL语句
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "数据库操控指令执行失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 关闭语句
    mysql_stmt_close(stmt);
    printf("更新成功！\n");
}

Votes* getDataFromServer(const char* id, MYSQL* conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[103];
    sprintf(query, "SELECT ques_num, join_num, questions, vote_title, creator \
                    FROM votes WHERE id=%s", id);
    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "获取投票信息失败：%s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }
    res = mysql_store_result(conn);
    row = mysql_fetch_row(res);
    Votes* vote = (Votes*)malloc(sizeof(Votes));
    vote->ques_num = atoi(row[0]);
    vote->join_num = atoi(row[1]);
    strcpy(vote->vote_title, row[3]);
    strcpy(vote->creator, row[4]);
    char* my_json = row[2];
    cJSON* json = cJSON_Parse(my_json);
    json2struct(json, vote);
    
    return vote;
}

bool login(MYSQL* conn) {
    //--------定义需要的变量-------------------
    char temp_u_id[8], temp_passwd[18];
    //--------帮助用户理解的必要输出------------
    printf("请输入您的六位数ID：");
    fgets(temp_u_id, sizeof(temp_u_id), stdin);
    if (runawayN(temp_u_id) != true || strlen(temp_u_id) != 6 || strspn(temp_u_id, "0123456789") != strlen(temp_u_id)) {
        printf("输入不合法！按任意键返回主页..."), scanf("%*[^\n]%*c");
        getch(), system("cls");
        return false;
    }
    printf("请输入您的密码：");
    fgets(temp_passwd, sizeof(temp_passwd), stdin);
    if (runawayN(temp_passwd) != true || strlen(temp_passwd) < 6) {
        printf("输入的值不合法！按任意键返回主页..."), scanf("%*[^\n]%*c");
        getch(), system("cls");
        return false;
    }
    //--------调用数据库检查-------------------
    MYSQL_RES *res;
    MYSQL_ROW row;

    char query[43] = "SELECT passwd FROM user WHERE id=";

    strncat(query, temp_u_id, strlen(temp_u_id));
    //检索id获取用户密码
    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "拉取用户列表出现问题：%s\n", mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    res = mysql_store_result(conn);

    if ((row = mysql_fetch_row(res)) != NULL) {
        //判断输入的密码和服务器存储的密码是否相同
        if (strcmp(temp_passwd, row[0]) == 0) {
            printf("登陆成功！按任意键进入主界面...\n");
            strcpy(user.u_id, temp_u_id);
            strcpy(user.u_passwd, temp_passwd);
            getch(), system("cls");
            return true;
        }
    }
    fprintf(stderr, "登录失败，账号或密码错误\n");
    return false;
}

void my_register(MYSQL* conn) {
    system("cls"), printf("欢迎您注册在线投票系统~\n\
    请您仔细阅读注册须知：\n\n\
        -------------------------------------------------------------------------------------------------------\n\
            1、每个用户都独有一个六位整数的 id 作为登录账号的唯一凭证，由用户自己指定，设定后不能修改\n\
            2、在账号注册时，用户可以设置账号名称 (目前只支持英文，使用中文会产生乱码)，账号的名称可以重复，\n\
               作为您参与投票的显示信息\n\
            3、每位用户都应当设置 6~16 位的密码，支持ASCII码表中的全部字符，密码修改功能将于后期上线，请谨慎设置密码\n\
        --------------------------------------------------------------------------------------------------------\n\n\
    输入 \"y\" 代表您已阅读并认可上述须知，输入 \"n\" 或其他字符则退出注册程序");
    char flag, temp_u_id[8], temp_passwd[18], confirm_passwd[16], temp_name[10];
    scanf("%c", &flag), getchar();// scanf("%*[^\n]%*c");
    if (flag != 'y') return;
    system("cls");
SETSTART:
    printf("请为自己设置一个 6 位的 id，注意只能输入数字：");
    fgets(temp_u_id, sizeof(temp_u_id), stdin);
    runawayN(temp_u_id) == true ? 1 : scanf("%*[^\n]%*c");
    if (strlen(temp_u_id) != 6 || strspn(temp_u_id, "0123456789") != strlen(temp_u_id)) {
        printf("输入不合法！按任意键返回主页...");
        getch(), system("cls");
        return;
    }
    //检测数据库中是否已有对应 u_id
    if(!u_idExist(temp_u_id, conn)) {
        printf("您输入的 id 已经被别人用啦，再试一个吧~\n按任意键重试...");
        getch(), system("cls");
        goto SETSTART;
    }

    printf("请为自己设置一个 6~16 位的密码，多余的字符将被截断，支持ASCII中的字符：");
    fflush(stdout), fgets(temp_passwd, sizeof(temp_passwd), stdin);
    runawayN(temp_passwd) == true ? 1 : scanf("%*[^\n]%*c");
    if (strlen(temp_passwd) < 6 || strlen(temp_passwd) > 18) {
        printf("2输入不合法！%d按任意键返回主页...", strlen(temp_passwd));
        getch(), system("pause");
        return;
    }

    printf("再输一遍验证一下哦：");
    fgets(confirm_passwd, sizeof(confirm_passwd), stdin);
    runawayN(confirm_passwd) == true ? 1 : scanf("%*[^\n]%*c");
    if (strcmp(temp_passwd, confirm_passwd) != 0) {
        printf("两次密码不一样呢，再试试吧！"), system("pause"), system("cls");
        goto SETSTART;
    }

    printf("好耶！下面为自己设置一个 10 位以内的洋昵称吧，超出部分将被截断，输入中文会造成乱码：");
    fgets(temp_name, sizeof(temp_name), stdin);
    runawayN(temp_name) == true ? 1 : scanf("%*[^\n]%*c");

    //数据库部分
    MYSQL_STMT* stmt;  //初始化SQL语句载体
    MYSQL_BIND bind[3];//初始化SQL语句参数
    const char* insert_query = "INSERT INTO user (id, name, passwd)\
                                VALUES (?, ?, ?)"; //插入语句

    // 准备SQL语句
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "数据库操控指令初始化失败：%s\n", mysql_stmt_error(stmt));
        mysql_close(conn);
        return;
    }

    // 准备绑定参数
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_VAR_STRING; //ques_num的各参数
    bind[0].buffer = temp_u_id;
    bind[0].buffer_length = strlen(temp_u_id);
    bind[0].is_null = 0;

    bind[1].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[1].buffer = temp_name;
    bind[1].buffer_length = strlen(temp_name);
    bind[1].is_null = 0;

    bind[2].buffer_type = MYSQL_TYPE_VAR_STRING;
    bind[2].buffer = temp_passwd;
    bind[2].buffer_length = strlen(temp_passwd);
    bind[2].is_null = 0;

    // SQL语句置入stmt
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
        fprintf(stderr, "数据库操控指令准备失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 绑定参数
    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "数据库操控指令绑定参数失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 执行SQL语句
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "数据库操控指令执行失败：%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // 关闭语句
    mysql_stmt_close(stmt);
    printf("注册成功！按任意键返回登录..."), getch(), system("cls");
    return;
}

bool u_idExist(const char* id, MYSQL* conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;

    char* query = "SELECT id FROM user";
    //检索id获取用户密码
    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "拉取用户列表失败：%s\n", mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
            //判断输入的id和服务器存储的id有没有相同的
            fprintf(stderr, "%s", mysql_error(conn));
            if (strcmp(row[0], id) == 0) return false;
        
    }
    return true;
}