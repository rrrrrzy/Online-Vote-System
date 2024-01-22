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
        fprintf(stderr, "��ȡͶƱ�б�ʧ�ܣ�%s\n", mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    //��ȡͶƱ�б���Ӧid
    res = mysql_store_result(conn);
    int num = mysql_num_rows(res);
    //���
    printf("��ǰ����ͶƱ����Ŀ�У� %d ��\n", num);
    while ((row = mysql_fetch_row(res)) != NULL) {
        long long int i;
        printf("ID: %s \t ����: %s \t �Ѳ�������: %s\n", row[0], row[1], row[2]);
    }
    return num;
}

MYSQL* connectDB() {
    //�����ڶ�������ռ䣬��Ϊ���ܴ�����ʱ�����ĵ�ַ��
    MYSQL* conn = (MYSQL*)malloc(1 * sizeof(MYSQL*));
    conn = mysql_init(NULL);

    if (mysql_real_connect(conn, SITE, USER, PASSWD, DATABASE, 3306, NULL, 0)) {
        printf("����������ӳɹ���\n");
    }
    else {
        fprintf(stderr, "�����������ʧ��: %s ������������ͷ���ǽ����\n", mysql_error(conn));
    }
    return conn;
}

void closeDB(MYSQL* conn) {
    mysql_close(conn); //�Ͽ�����
    free(conn);        //�ͷŶѿռ�
    conn = NULL;       //����ָ������
}

void publicVote(Votes* v, MYSQL* conn) {
    MYSQL_STMT* stmt;  //��ʼ��SQL�������
    MYSQL_BIND bind[5];//��ʼ��SQL������
    const char* insert_query = "INSERT INTO votes (ques_num, join_num, questions, vote_title, creator)\
                                VALUES (?, ?, ?, ?, ?)"; //�������
    const cJSON* json = struct2json(v); //�ѽṹ��ת����json
    const char* json_data = (char*)cJSON_Print(json); //��json�ַ�����

    // ׼��SQL���
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "���ݿ�ٿ�ָ���ʼ��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_close(conn);
        return;
    }

    // ׼���󶨲���
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG; //ques_num�ĸ�����
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

    // SQL�������stmt
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��׼��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // �󶨲���
    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��󶨲���ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // ִ��SQL���
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��ִ��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // �ر����
    mysql_stmt_close(stmt);
}

void updateVote(Votes* v, MYSQL* conn, char* id) {
    MYSQL_STMT* stmt;  //��ʼ��SQL�������
    MYSQL_BIND bind[5];//��ʼ��SQL������
    const char* insert_query = "UPDATE votes \
                                SET ques_num = ?, join_num = ?, questions = ?, vote_title = ? \
                                WHERE id = ?"; //�������
    const cJSON* json = struct2json(v); //�ѽṹ��ת����json
    const char* json_data = (char*)cJSON_Print(json); //��json�ַ�����
    char* my_id = id;
    // ׼��SQL���
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "���ݿ�ٿ�ָ���ʼ��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_close(conn);
        return;
    }

    // ׼���󶨲���
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_LONG; //ques_num�ĸ�����
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

    // SQL�������stmt
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��׼��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // �󶨲���
    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��󶨲���ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // ִ��SQL���
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��ִ��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // �ر����
    mysql_stmt_close(stmt);
    printf("���³ɹ���\n");
}

Votes* getDataFromServer(const char* id, MYSQL* conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[103];
    sprintf(query, "SELECT ques_num, join_num, questions, vote_title, creator \
                    FROM votes WHERE id=%s", id);
    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "��ȡͶƱ��Ϣʧ�ܣ�%s\n", mysql_error(conn));
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
    //--------������Ҫ�ı���-------------------
    char temp_u_id[8], temp_passwd[18];
    //--------�����û����ı�Ҫ���------------
    printf("������������λ��ID��");
    fgets(temp_u_id, sizeof(temp_u_id), stdin);
    if (runawayN(temp_u_id) != true || strlen(temp_u_id) != 6 || strspn(temp_u_id, "0123456789") != strlen(temp_u_id)) {
        printf("���벻�Ϸ����������������ҳ..."), scanf("%*[^\n]%*c");
        getch(), system("cls");
        return false;
    }
    printf("�������������룺");
    fgets(temp_passwd, sizeof(temp_passwd), stdin);
    if (runawayN(temp_passwd) != true || strlen(temp_passwd) < 6) {
        printf("�����ֵ���Ϸ����������������ҳ..."), scanf("%*[^\n]%*c");
        getch(), system("cls");
        return false;
    }
    //--------�������ݿ���-------------------
    MYSQL_RES *res;
    MYSQL_ROW row;

    char query[43] = "SELECT passwd FROM user WHERE id=";

    strncat(query, temp_u_id, strlen(temp_u_id));
    //����id��ȡ�û�����
    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "��ȡ�û��б�������⣺%s\n", mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    res = mysql_store_result(conn);

    if ((row = mysql_fetch_row(res)) != NULL) {
        //�ж����������ͷ������洢�������Ƿ���ͬ
        if (strcmp(temp_passwd, row[0]) == 0) {
            printf("��½�ɹ��������������������...\n");
            strcpy(user.u_id, temp_u_id);
            strcpy(user.u_passwd, temp_passwd);
            getch(), system("cls");
            return true;
        }
    }
    fprintf(stderr, "��¼ʧ�ܣ��˺Ż��������\n");
    return false;
}

void my_register(MYSQL* conn) {
    system("cls"), printf("��ӭ��ע������ͶƱϵͳ~\n\
    ������ϸ�Ķ�ע����֪��\n\n\
        -------------------------------------------------------------------------------------------------------\n\
            1��ÿ���û�������һ����λ������ id ��Ϊ��¼�˺ŵ�Ψһƾ֤�����û��Լ�ָ�����趨�����޸�\n\
            2�����˺�ע��ʱ���û����������˺����� (Ŀǰֻ֧��Ӣ�ģ�ʹ�����Ļ��������)���˺ŵ����ƿ����ظ���\n\
               ��Ϊ������ͶƱ����ʾ��Ϣ\n\
            3��ÿλ�û���Ӧ������ 6~16 λ�����룬֧��ASCII����е�ȫ���ַ��������޸Ĺ��ܽ��ں������ߣ��������������\n\
        --------------------------------------------------------------------------------------------------------\n\n\
    ���� \"y\" ���������Ķ����Ͽ�������֪������ \"n\" �������ַ����˳�ע�����");
    char flag, temp_u_id[8], temp_passwd[18], confirm_passwd[16], temp_name[10];
    scanf("%c", &flag), getchar();// scanf("%*[^\n]%*c");
    if (flag != 'y') return;
    system("cls");
SETSTART:
    printf("��Ϊ�Լ�����һ�� 6 λ�� id��ע��ֻ���������֣�");
    fgets(temp_u_id, sizeof(temp_u_id), stdin);
    runawayN(temp_u_id) == true ? 1 : scanf("%*[^\n]%*c");
    if (strlen(temp_u_id) != 6 || strspn(temp_u_id, "0123456789") != strlen(temp_u_id)) {
        printf("���벻�Ϸ����������������ҳ...");
        getch(), system("cls");
        return;
    }
    //������ݿ����Ƿ����ж�Ӧ u_id
    if(!u_idExist(temp_u_id, conn)) {
        printf("������� id �Ѿ�����������������һ����~\n�����������...");
        getch(), system("cls");
        goto SETSTART;
    }

    printf("��Ϊ�Լ�����һ�� 6~16 λ�����룬������ַ������ضϣ�֧��ASCII�е��ַ���");
    fflush(stdout), fgets(temp_passwd, sizeof(temp_passwd), stdin);
    runawayN(temp_passwd) == true ? 1 : scanf("%*[^\n]%*c");
    if (strlen(temp_passwd) < 6 || strlen(temp_passwd) > 18) {
        printf("2���벻�Ϸ���%d�������������ҳ...", strlen(temp_passwd));
        getch(), system("pause");
        return;
    }

    printf("����һ����֤һ��Ŷ��");
    fgets(confirm_passwd, sizeof(confirm_passwd), stdin);
    runawayN(confirm_passwd) == true ? 1 : scanf("%*[^\n]%*c");
    if (strcmp(temp_passwd, confirm_passwd) != 0) {
        printf("�������벻һ���أ������԰ɣ�"), system("pause"), system("cls");
        goto SETSTART;
    }

    printf("��Ү������Ϊ�Լ�����һ�� 10 λ���ڵ����ǳưɣ��������ֽ����ضϣ��������Ļ�������룺");
    fgets(temp_name, sizeof(temp_name), stdin);
    runawayN(temp_name) == true ? 1 : scanf("%*[^\n]%*c");

    //���ݿⲿ��
    MYSQL_STMT* stmt;  //��ʼ��SQL�������
    MYSQL_BIND bind[3];//��ʼ��SQL������
    const char* insert_query = "INSERT INTO user (id, name, passwd)\
                                VALUES (?, ?, ?)"; //�������

    // ׼��SQL���
    stmt = mysql_stmt_init(conn);
    if (!stmt) {
        fprintf(stderr, "���ݿ�ٿ�ָ���ʼ��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_close(conn);
        return;
    }

    // ׼���󶨲���
    memset(bind, 0, sizeof(bind));
    bind[0].buffer_type = MYSQL_TYPE_VAR_STRING; //ques_num�ĸ�����
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

    // SQL�������stmt
    if (mysql_stmt_prepare(stmt, insert_query, strlen(insert_query)) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��׼��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // �󶨲���
    if (mysql_stmt_bind_param(stmt, bind) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��󶨲���ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // ִ��SQL���
    if (mysql_stmt_execute(stmt) != 0) {
        fprintf(stderr, "���ݿ�ٿ�ָ��ִ��ʧ�ܣ�%s\n", mysql_stmt_error(stmt));
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return;
    }

    // �ر����
    mysql_stmt_close(stmt);
    printf("ע��ɹ�������������ص�¼..."), getch(), system("cls");
    return;
}

bool u_idExist(const char* id, MYSQL* conn) {
    MYSQL_RES *res;
    MYSQL_ROW row;

    char* query = "SELECT id FROM user";
    //����id��ȡ�û�����
    if (mysql_real_query(conn, query, strlen(query))) {
        fprintf(stderr, "��ȡ�û��б�ʧ�ܣ�%s\n", mysql_error(conn));
        mysql_close(conn);
        return false;
    }

    res = mysql_store_result(conn);
    while ((row = mysql_fetch_row(res)) != NULL) {
            //�ж������id�ͷ������洢��id��û����ͬ��
            fprintf(stderr, "%s", mysql_error(conn));
            if (strcmp(row[0], id) == 0) return false;
        
    }
    return true;
}