#include "header.h"

cJSON* struct2json(Votes* v) {
    const char* j_title = "title";
    const char* j_choice_num = "choice_num";
    const char* j_description = "description";
    const char* j_choose_num = "choose_num";
    char j_choice[8] = "choice";
    char j_my_ques[9] = "my_ques";
    
    cJSON* my_ques = NULL;
    my_ques = cJSON_CreateObject();
    for (int i = 0; i < v->ques_num; i++) {
        cJSON* ques = NULL;
        ques = cJSON_CreateObject();

        cJSON_AddStringToObject(ques, j_title, v->my_ques[i].title);
        cJSON_AddNumberToObject(ques, j_choice_num, v->my_ques[i].choice_num);
        for (int j = 0; j < v->my_ques[i].choice_num; j++) {
            cJSON* choice = NULL;
            choice = cJSON_CreateObject();

            cJSON_AddStringToObject(choice, j_description, v->my_ques[i].choices[j]);
            //上面传的就是指针，因为字符串操作时一定要传指针的
            cJSON_AddNumberToObject(choice, j_choose_num, v->my_ques[i].choose_num[j]);
            //把choice传入my_ques
            char name = '0' + j;
            j_choice[6] = name;
            cJSON_AddItemToObject(ques, j_choice, choice);
        }
        char name = '0' + i;
        j_my_ques[7] = name;
        cJSON_AddItemToObject(my_ques, j_my_ques, ques);
    }

    return my_ques;
}

bool runawayN(char * c) {
    char * find = strchr(c, '\n');          //查找换行符
    if(find != NULL) {                              //如果find不为空指针
        *find = '\0';  
        return true;
    }
    else {
        return false;       //字符串里没有\n代表用户输入超过限度，需要清空缓冲区
    }
}

void json2struct(cJSON* json, Votes* vote) {
    if (!cJSON_IsObject(json)) {
        printf("Error: JSON is not an object.\n");
        return;
    }

    // 解析 "my_ques" 数组

    for (int i = 0; i < vote->ques_num; ++i) {
        char j_my_ques[9] = "my_ques";
        char name = '0' + i;
        j_my_ques[7] = name;

        cJSON* quesItem = cJSON_GetObjectItemCaseSensitive(json, j_my_ques);
        if (cJSON_IsObject(quesItem)) {
            // 解析问题的信息
            cJSON* titleItem = cJSON_GetObjectItemCaseSensitive(quesItem, "title");
            cJSON* choiceNumItem = cJSON_GetObjectItemCaseSensitive(quesItem, "choice_num");
            if (cJSON_IsString(titleItem) && cJSON_IsNumber(choiceNumItem)) {
                strncpy(vote->my_ques[i].title, titleItem->valuestring, sizeof(vote->my_ques[i].title));
                vote->my_ques[i].choice_num = choiceNumItem->valueint;            
            }
            else {
                // 解析失败，释放之前分配的内存
                fprintf(stderr, "错误：解析第%d个问题的名称或选项数目失败！\n", i + 1);
                return;
            }

            for (int j = 0; j < vote->my_ques[i].choice_num && j < 5; ++j) {
                char j_choice[8] = "choice";
                char _name = '0' + j;
                j_choice[6] = _name;
                cJSON* choiceItem = cJSON_GetObjectItemCaseSensitive(quesItem, j_choice);
                if (cJSON_IsObject(choiceItem)) {
                    cJSON* choose_num = cJSON_GetObjectItemCaseSensitive(choiceItem, "choose_num");
                    cJSON* description = cJSON_GetObjectItemCaseSensitive(choiceItem, "description");

                    if (cJSON_IsNumber(choose_num) && cJSON_IsString(description)) {
                        char* choice_description = (char*)malloc(sizeof(char)*strlen(description->valuestring));
                        //这里用堆区变量去接，因为是指针数组，后面choos_num会被释放，必须存到堆区
                        strncpy(choice_description, description->valuestring, strlen(description->valuestring)); //必须是值传递
                        vote->my_ques[i].choices[j] = choice_description;
                        vote->my_ques[i].choose_num[j] = choose_num->valueint;
                    }
                    else {
                        fprintf(stderr, "错误：解析第%d个问题的第%d个选项内容失败！\n", i + 1, j + 1);
                    }
                }
                else {
                    fprintf(stderr, "错误：解析第%d个问题的第%d个选项失败！\n", i + 1, j + 1);
                    return;
                }

            }
            
        } else {
            // 解析失败
            printf("Error: Failed to parse 'my_ques[%d]'.\n", i);
            return;
        }
    }
}

void freeVotes(Votes* votes) {
    for (int i = 0; i < votes->ques_num; i++) {
        for (int j = 0; j < votes->my_ques[i].choice_num; j++) {
            free(votes->my_ques[i].choices[j]); //遍历以释放通过malloc创建的堆空间
            votes->my_ques[i].choices[j] = NULL; //避免指针悬挂
        }
    }
    votes = NULL; //指针置空
}