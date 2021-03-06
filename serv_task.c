#include "serv_task.h"

void *task(void *arg)
{
    pthread_detach(pthread_self());
    Pack tmp = *(Pack *)arg;
    Pack *pack = &tmp;

    if(pack->choice <= GET_NEW_PW)
        Login(pack);
    if((pack->choice >= VIEW_perinfo) && (pack->choice <= EXIT3))
        Per_set(pack);
    if((pack->choice == CHECK))
        Check(pack);
    if(pack->choice == ADD_friend)
        Add_friend(pack);
    if(pack->choice == VIEW_friendrq)
        View_friendrq(pack);
    if(pack->choice == VIEW_friendlist)
        View_friendlist(pack);
    if(pack->choice == PROCESS_frq)
        Process_friendrq(pack);      
    if(pack->choice == FRIEND_chat)
        Chat_sb(pack);          
    if(pack->choice == GET_friendmsg)
        Friend_msg(pack);
    if(pack->choice == DELETE_friend)
        Delete_friend(pack);
    if(pack->choice == IS_friend)
        Is_friend(pack);
    if(pack->choice == SET_group)
        Set_group(pack);
    if(pack->choice == ADD_group)
        Add_group(pack);
    if(pack->choice == VIEW_grouprq)
        view_grouprq(pack);
    if(pack->choice == VIEW_grouplist)
        View_grouplist(pack);
    if(pack->choice == VIEW_groupinfo) 
        View_groupinfo(pack);
    if(pack->choice == EXIT_group) 
        Exit_group(pack);
    if(pack->choice == REMOVE_member) 
        Remove_member(pack);
    if(pack->choice == SET_admini)
        Set_admini(pack);
    if(pack->choice == TRANSFER_group)
        Transfer_group(pack);
    if(pack->choice == DISSOLVE_group)
        Dissolve_group(pack);
    if(pack->choice == GET_status)
        Get_status(pack);
    if(pack->choice == PROCESS_grq)
        Process_grouprq(pack);
    if(pack->choice == VIEW_friendinfo)
        View_friendinfo(pack);
    if(pack->choice == GROUP_chat)
        Group_chat(pack);
    if(pack->choice == GET_groupmsg)
        Group_msg(pack);
    if(pack->choice == VIEW_filelist)
        View_filelist(pack);
    if(pack->choice == FILE_info)
        File_info(pack);
    if(pack->choice == SHLELD_friend)
        Shield_friend(pack);
    if(pack->choice == IS_shield)
        Is_shield(pack);
    if(pack->choice == CANCEL_shield)
        Cancel_shield(pack);
    if(pack->choice == FILE_recv)
        Send_file(pack);
    if(pack->choice == FILE_send)
        Recv_file(pack);
    if(pack->choice == CANCEL_admini)
        Cancel_admini(pack);
    if(pack->choice == SHIELD)
        Shield(pack);
    if(pack->choice == IS_group)
        Is_group(pack);
    
}

void my_error(char *str, int line)
{
    fprintf(stderr, "line : %d \n", line);
    perror(str);
    exit(1);
}


MYSQL Mysql_con()
{
    MYSQL mysql;
    if(NULL == mysql_init(&mysql))
        my_error("mysql_init", __LINE__);

	if(mysql_library_init(0, NULL, NULL) != 0)
        my_error("mysql_library_init", __LINE__);

	if(NULL == mysql_real_connect(&mysql, "127.0.0.1", "root", "1551097460", "bxchat", 0, NULL, 0))
        my_error("mysql_real_connect", __LINE__);

	if(mysql_set_character_set(&mysql, "utf8") < 0)
        my_error("mysql_set_character_set failed", __LINE__);

    return mysql;
}

void Login(Pack *pack)
{
    char buf[200];
    per_list_t newNode;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    switch(pack->choice)
    {
        case LOGIN:
            printf("id:%d  pass:%s\n", pack->info.id, pack->info.password);
            memset(buf, sizeof(buf), 0);
            sprintf(buf, "select id from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);

            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);       
            if(!row)
	        {
                pack->status = -1;
                printf("id error\n");
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                break;
            }

            printf("***\n");
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select password from login_info where password = %s AND id = %d", pack->info.password, pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(!row)
                pack->status = -2;
            else
            {
                memset(buf, sizeof(buf), 0);    
                sprintf(buf, "update login_info set status = 1 where id = %d", pack->info.id);
                if(mysql_query(&mysql,buf) < 0)
                    my_error("mysql_query", __LINE__);

                memset(buf, sizeof(buf), 0);
                sprintf(buf, "select name from login_info where id = %d", pack->info.id);
                if(mysql_query(&mysql,buf))
                    my_error("mysql_query", __LINE__);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);
                strcpy(pack->info.name, row[0]);

                pack->status = 2;

                newNode = (per_list_t)malloc(sizeof(per_node_t));
                newNode->id = pack->info.id;
                newNode->serfd = pack->data.serfd;
                pthread_mutex_lock(&mutex);
                List_AddTail(plist, newNode);
                pthread_mutex_unlock(&mutex);
            }

            send(pack->data.serfd, pack, sizeof(Pack), 0);
            printf("id : %d login\n", pack->info.id);
            break;

        case REGISTER:
        //    printf("***dad\n");
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "insert into login_info values(NULL, \"%s\", \"%s\", \"%s\", \"%s\", 0)", pack->info.name, pack->info.password, pack->info.question, pack->info.answer);
            if(mysql_query(&mysql, buf))
                my_error("mysql_query failed", __LINE__);

            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select id from login_info where password = %s", pack->info.password);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            pack->info.id = atoi(row[0]);

            pack->status = 0;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case FIND_PASSWORD:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select id from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(!row)
            {
                pack->status = -1;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
            }
            else
            {
                memset(buf, sizeof(buf), 0);    
                sprintf(buf, "select question from login_info where id = %d", pack->info.id);
                if(mysql_query(&mysql,buf) < 0)
                    my_error("mysql_query", __LINE__);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);
                strcpy(pack->info.question, row[0]);
                pack->status = 0;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
            }
            break;

        case FIND_ANSWER:
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "select answer from login_info where answer = \"%s\" AND id = %d", pack->info.answer, pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            if(!row)
                pack->status = -5;
            else
                pack->status = 1;
            send(pack->data.serfd, pack, sizeof(Pack), 0);

        case GET_NEW_PW:
         //   printf("%s\n", pack->info.password);
            memset(buf, sizeof(buf), 0);    
            sprintf(buf, "update login_info set password = \"%s\" where id = %d", pack->info.password, pack->info.id);
          //  printf("%s\n", buf);            
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            pack->status = 7;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;
    }
}

void Check(Pack *pack)
{

    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;
    friend_list_t list, curPos, newNode;
    friend_t data;
    int id = pack->info.id;

    List_Init(list, friend_node_t);
    
    pack->num = 0;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id1 from friends where type = 0 and id2 = %d", pack->info.id);
    if(mysql_query(&mysql,buf) < 0)
        my_error("mysql_query", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        pack->status = ADD_friend;
        pack->num = 1;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update friends set type = 1 where id2 = %d", pack->info.id);
        if(mysql_query(&mysql,buf) < 0)
            my_error("mysql_query", __LINE__);
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    printf("%d %d\n", pack->info.id, pack->info.choice);

    pack->num = 0;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id1 from friend_msg where status = 0 and id2 = %d", pack->info.id);
    if(mysql_query(&mysql,buf) < 0)
        my_error("mysql_query", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        pack->num++;
    }
    //sleep(3);
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    printf("%d %d\n", pack->info.id, pack->info.choice);

    if(pack->num)
        List_ForEach(list, curPos)
            {
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "select name from login_info where id = %d", curPos->data.id);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);
                strcpy(curPos->data.name,row[0]);
                pack->fnode = curPos->data;
                int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
                printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->num);
                //printf("%d\n", n);
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update friend_msg set status = 1 where status = 0 and id2 = %d", pack->info.id);
                if(mysql_query(&mysql,buf) < 0)
                    my_error("mysql_query", __LINE__);
            }

   //printf("**\n");
    pack->num = 0;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select * from group_management where type = 0 and aid = %d", pack->info.id);
    if(mysql_query(&mysql,buf )< 0)
        my_error("mysql_query", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        pack->status = GROUP;
        pack->num = 1;
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update group_management set type = 1 where aid = %d and type = 0", pack->info.id);
        if(mysql_query(&mysql,buf) < 0)
            my_error("mysql_query", __LINE__);
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    printf("%d %d\n", pack->info.id, pack->info.choice);

    pack->num = 0;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select distinct gid from group_msg where status = 0 and rid = %d", pack->info.id);
    if(mysql_query(&mysql,buf) < 0)
        my_error("mysql_query", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        pack->num++;
    }
    //sleep(3);
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    printf("%d %d\n", pack->info.id, pack->info.choice);

    //printf("***\n");
    if(pack->num)
        List_ForEach(list, curPos)
            {
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "select gname from group_info where gid = %d", curPos->data.id);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);
                strcpy(curPos->data.name,row[0]);
                pack->fnode = curPos->data;
                int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
            //    printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->num);
                //printf("%d\n", n);
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_msg set status = 1 where status = 0 and rid = %d and gid = %d", pack->info.id, curPos->data.id);
                if(mysql_query(&mysql,buf) < 0)
                    my_error("mysql_query", __LINE__);
            }
}

void Per_set(Pack *pack)
{

    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;

    switch (pack->choice)
    {
        case VIEW_perinfo:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(pack->info.name, row[0]);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select question from login_info where id = %d", pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(pack->info.question, row[0]);
            send(pack->data.serfd, pack, sizeof(Pack), 0);
         //   printf("***\n");
            break;
        case CHANGE_name:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update login_info set name = \"%s\" where id = %d", pack->info.name, pack->info.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            pack->status = 0;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case CHANGE_question:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update login_info set question = \"%s\" where id = %d", pack->info.question, pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            pack->status = 1;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;

        case CHANGE_answer:
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update login_info set answer = \"%s\" where id = %d", pack->info.answer, pack->info.id);
            if(mysql_query(&mysql,buf) < 0)
                my_error("mysql_query", __LINE__);
            pack->status = 2;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
            break;
    }
}

void Add_friend(Pack *pack)
{
    char buf[200];
    per_list_t curPos;
    MYSQL_RES *result;
    MYSQL_ROW row;

   // printf("***\n");
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -1;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id2 from friends where id2 = %d and id1 = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(row)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select status from friends where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        if(atoi(row[0]) == 1)
            pack->status = -3;
        else
            pack->status = -2;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into friends values(%d, %d, 0, 0)", pack->info.id, pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    pack->status = 0;
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(atoi(row[0]) == 1)
    {
        pack->status = fri;
        pthread_mutex_lock(&mutex);
        List_ForEach(plist, curPos)
        {
            if(curPos->id == pack->data.cid)
            {
                printf("%d*0\n", curPos->serfd);
                send(curPos->serfd,pack, sizeof(Pack), 0);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update friends set type = 1 where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
            if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    }
}

void View_friendrq(Pack *pack)
{
    char buf[200];
    int i, rq_num = 0;
    friend_list_t list, curPos, newNode;
    friend_t data;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friend_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id1 from friends where id2 = %d and status = 0", pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);

    row = mysql_fetch_row(result);
    if(row == NULL)
    {
        pack->status = 0;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    else
    {
        pack->status = 1;
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        rq_num++;
    }
    

    while(row = mysql_fetch_row(result))
    {
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        rq_num++;
    }
    pack->num = rq_num;
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    List_ForEach(list, curPos)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select name from login_info where id = %d", curPos->data.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        strcpy(curPos->data.name,row[0]);
        pack->fnode = curPos->data;
        int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
        //printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->rqf_num);
        //printf("%d\n", n);
    }
}

void View_friendlist(Pack *pack)
{
    char buf[200];
    int i, fri_num = 0;
    friend_list_t newNode;
    friend_list_t list, curPos;
    friend_t data;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friend_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id2 from friends where id1 = %d and not status = 0", pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);

    pack->num = 0;
    row = mysql_fetch_row(result);
    if(row == NULL)
    {
        pack->status = 0;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    else
    {
        pack->status = 1;
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        fri_num++;
    }

    while(row = mysql_fetch_row(result))
    {
        newNode = (friend_list_t)malloc(sizeof(friend_node_t));
        data.id = atoi(row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        fri_num++;
    }
    pack->num = fri_num;
 //   printf("%d*\n", pack->num);
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    if(pack->num)
    List_ForEach(list, curPos)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select name, status from login_info where id = %d", curPos->data.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);
        strcpy(curPos->data.name,row[0]);
        curPos->data.status = atoi(row[1]);
        pack->fnode = curPos->data;
        //sleep(3);
        int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
        printf("\t\t%-3d  %-20s  \n", pack->fnode.id, pack->fnode.name);
    }
}

void Process_friendrq(Pack *pack)
{
    char buf[200];

    if(pack->status == 1)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update friends set status = 1 where id1 = %d and id2 = %d", pack->data.cid, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "insert into friends values(%d, %d, 1, 1)", pack->info.id, pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
    }
    else
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "delete from friends where id1 = %d and id2 = %d;", pack->data.cid, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
    }
}

void Chat_sb(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;
    
    if(pack->status == -99)
    {
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into friend_msg values(%d, %d, \'%s\', -1, %d, %d, %d, %d, %d, %d, 0, NULL, NULL)", pack->info.id, pack->data.cid, pack->data.sendbuf, pack->fmnode.date.year,pack->fmnode.date.month, 
                                                                                            pack->fmnode.date.day, pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into friend_msg values(%d, %d, \'%s\', 0, %d, %d, %d, %d, %d, %d, 0, NULL, NULL)", pack->info.id, pack->data.cid, pack->data.sendbuf, pack->fmnode.date.year,pack->fmnode.date.month, 
                                                                                            pack->fmnode.date.day, pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(atoi(row[0]) == 1)
    {
        pack->status = sb;
        pthread_mutex_lock(&mutex);
        List_ForEach(plist, curPos)
        {
            if(curPos->id == pack->data.cid)
            {
                send(curPos->serfd,pack, sizeof(Pack), 0);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update friend_msg set status = 1 where id1 = %d and id2 = %d where status = 0", pack->info.id, pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)    
            my_error("mysql_query failed", __LINE__);
                    //printf("%s\n", buf);
    }
}

void Friend_msg(Pack *pack)
{
    char name[20];
    char buf[200];
    int i, frimsg_num = 0;
    friendmsg_list_t newNode;
    friendmsg_list_t list, curPos;
    friendmsg_t data;
    struct epoll_event tem;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friendmsg_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select name from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    strcpy(name, row[0]);
    strcpy(pack->fnode.name, row[0]);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select * from friend_msg where (id1 = %d and id2 = %d) or (id1 = %d and id2 = %d)", pack->info.id, pack->data.cid, pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        if((atoi(row[3]) == -1) && (atoi(row[0]) == pack->data.cid) && (atoi(row[1]) == pack->info.id))
            continue;
        newNode = (friendmsg_list_t)malloc(sizeof(friendmsg_node_t));
        if(atoi(row[0]) == pack->info.id)
        {
            strcpy(data.name1, pack->info.name);
            strcpy(data.name2, name);
        }
        else
        {
            strcpy(data.name2, pack->info.name);
            strcpy(data.name1, name);
        }
        strcpy(data.msgbuf, row[2]);
        data.date.year = atoi(row[4]);
        data.date.month = atoi(row[5]);
        data.date.day = atoi(row[6]);
        data.time.hour = atoi(row[7]);
        data.time.minute = atoi(row[8]);
        data.time.second = atoi(row[9]);
        newNode->data = data;
        List_AddTail(list, newNode);
        frimsg_num++;
    }
    pack->num = frimsg_num;
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    if(pack->num)
    {/*
        if(epoll_ctl(pack->data.efd, EPOLL_CTL_DEL, pack->data.serfd, NULL) < 0)
            my_error("epoll_ctl failed", __LINE__);
        pack->status = 999;*/
        List_ForEach(list, curPos)
        {
            pack->fmnode = curPos->data;
            int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
            //printf("%d\n",n);
        }
        /*tem.events = EPOLLIN;
        tem.data.fd = pack->data.serfd;
        if(epoll_ctl(pack->data.efd, EPOLL_CTL_ADD, pack->data.serfd, &tem) < 0)
            my_error("epoll_ctl failed", __LINE__);*/
    }

}

void Delete_friend(Pack *pack)
{
    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from friends where id1 = %d and id2 = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    //    printf("%s\n", buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from friends where id2 = %d and id1 = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    //    printf("%s\n", buf);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from friend_msg where id1 = %d and id2 = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from friend_msg where id2 = %d and id1 = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
}

void Is_friend(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -1;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id2 from friends where id1 = %d and id2 = %d and not status = 0", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -2;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    pack->status = 1;
    printf("****%d\n", pack->status);
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}

void Set_group(Pack *pack)
{
    int num = 0;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char buf[200];

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into group_info values(NULL, \"%s\", %d, \"%s\", 1, %d, %d, %d)", pack->fnode.name, pack->info.id, 
                                                pack->info.name, pack->fmnode.date.year, pack->fmnode.date.month, pack->fmnode.date.day);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    //printf("%s\n", buf);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gid from group_info where gname = \"%s\"", pack->fnode.name);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
        num++;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gid from group_info where gname = \"%s\"", pack->fnode.name);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    for(int i = 0; i < num; i++)
    {
        row = mysql_fetch_row(result);
        if(i == num-1)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "insert into group_management values(%d, %d, %d, 9, 1, 1)", atoi(row[0]), pack->info.id, pack->info.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
        }
    }
}

void Add_group(Pack *pack)
{
    char buf[200];
    per_list_t curPos;
    MYSQL_RES *result;
    MYSQL_ROW row;

   // printf("***\n");
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gid from group_info where gid = %d", pack->data.cid);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -1;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->info.id);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 0:
                pack->status = -2;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                return ;

            case 1:
            case 2:
            case 9:
            case 11:
                pack->status = -3;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                return ;

            case -1:
            case 3:
            case 12:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "delete from group_management where gid = %d and mid = %d", pack->data.cid, pack->info.id);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                break;

            default:
                pack->status = -10;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                return ;
        }      
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select mid from group_management where status = 2 or status = 9 and gid = %d", pack->data.cid);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "insert into group_management values(%d, %d, %d, 0, 0, 2)", pack->data.cid, pack->info.id, atoi(row[0]));
      //  printf("%s\n", buf);
            if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
        
        pthread_mutex_lock(&mutex);
        List_ForEach(plist, curPos)
        {
            if(curPos->id == atoi(row[0]))
            {
                pack->status = gro;
                send(curPos->serfd, pack, sizeof(Pack), 0);
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and aid = %d", pack->data.cid, pack->info.id, atoi(row[0]));
                    if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    pack->status = 0;
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}


void view_grouprq(Pack *pack)
{
    char buf[200];
    int i;
    friendmsg_list_t list, curPos, newNode;
    friendmsg_t data;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friendmsg_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select * from group_management where aid = %d", pack->info.id);
    //printf("%s***\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    pack->num = 0;
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[3]))
        {
            case -1:
            case 2:
            case 9:
            case 0:
            case 1:
            case 3:
            case 12:
            case 4:
            case 11:
            case 13:
                newNode = (friendmsg_list_t)malloc(sizeof(friendmsg_node_t));
                data.id = atoi(row[0]);
                data.time.hour = atoi(row[1]);
                data.time.minute = atoi(row[3]);
                data.time.second = atoi(row[5]);
                newNode->data = data;
			//printf("%-3d  %-20s  %-3d \n  ", data.id, data.name, pack->data.cid);
                List_AddTail(list, newNode);
                pack->num++;
                break;
        }
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    if(pack->num)
        List_ForEach(list, curPos)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select gname from group_info where gid = %d", curPos->data.id);
            //printf("%s*\n", buf);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(curPos->data.name1,row[0]);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d", curPos->data.time.hour);
            //printf("%s**\n", buf);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
               //     printf("%s\n", buf);
            strcpy(curPos->data.name2,row[0]);
            pack->fmnode = curPos->data;
		//	printf("%-3d  %-20s  %-3d  %-20s  \n", pack->fnode.id, pack->fnode.name, pack->data.cid, pack->data.sendbuf);
            int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
            //printf("%d %s\n", pack->fnode.id, pack->fnode.name);
            //printf("%d\n", n);
        }
}

void View_grouplist(Pack *pack)
{
    char buf[200];
    int i;
    friend_list_t newNode;
    friend_list_t list, curPos;
    friend_t data;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friend_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gid, status from group_management where mid = %d", pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);

    pack->num = 0;
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[1]))
        {
            case 1:
            case 2:
            case 9:
                newNode = (friend_list_t)malloc(sizeof(friend_node_t));
                data.id = atoi(row[0]);
                data.status = atoi(row[1]);
                newNode->data = data;
                List_AddTail(list, newNode);
                pack->num++;
                break;
        }
    }
    //printf("%d*\n", pack->num);
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    if(pack->num)
        List_ForEach(list, curPos)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select gname from group_info where gid = %d", curPos->data.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(curPos->data.name,row[0]);
            //sleep(1);
            pack->fnode = curPos->data;
            int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
            printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->num);
            //printf("%d\n", n);
        }
}

void Group_msg(Pack *pack)
{
    char buf[200];
    friendmsg_list_t newNode;
    friendmsg_list_t list, curPos;
    friendmsg_t data;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, friendmsg_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gname from group_info where gid = %d", pack->data.cid);
    //printf("***%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    strcpy(pack->fnode.name, row[0]);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select * from group_msg where gid = %d and rid = %d", pack->data.cid, pack->info.id);
    //printf("***%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    pack->num = 0;
    while(row = mysql_fetch_row(result))
    {
        newNode = (friendmsg_list_t)malloc(sizeof(friendmsg_node_t));
        data.id = atoi(row[1]);
        strcpy(data.msgbuf, row[3]);
        data.date.year = atoi(row[5]);
        data.date.month = atoi(row[6]);
        data.date.day = atoi(row[7]);
        data.time.hour = atoi(row[8]);
        data.time.minute = atoi(row[9]);
        data.time.second = atoi(row[10]);
        newNode->data = data;
        List_AddTail(list, newNode);
        pack->num++;
    }

    //pack->status = 0;
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    if(pack->num)
        List_ForEach(list, curPos)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d", curPos->data.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(curPos->data.name1, row[0]);
            //sleep(1);
            pack->fmnode = curPos->data;
          //  printf("[%s %d-%d %d:%d:%d]\n", pack->fmnode.name1, pack->fmnode.date.month, pack->fmnode.date.day,
			//														pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
		//	printf("%s\n", pack->fmnode.msgbuf);
            int n = send(pack->data.serfd, pack, sizeof(Pack), 0);

           // printf("%d\n",n);
        }
}

void View_groupinfo(Pack *pack)
{    
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    friend_list_t newNode;
    friend_list_t list, curPos;
    friend_t data;

    List_Init(list, friend_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select * from group_info where gid = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    if(row = mysql_fetch_row(result))
    {
        strcpy(pack->fmnode.name1, row[1]);
        pack->fmnode.id = atoi(row[2]);
        strcpy(pack->fmnode.name2, row[3]);
        pack->num = atoi(row[4]);
        pack->fmnode.date.year = atoi(row[5]);
        pack->fmnode.date.month = atoi(row[6]);
        pack->fmnode.date.day = atoi(row[7]);
        pack->status = 0;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select mid, status from group_management where gid = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    pack->num = 0;
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[1]))
        {
            case 1:
            case 2:
            case 9:
                newNode = (friend_list_t)malloc(sizeof(friend_node_t));
                data.id = atoi(row[0]);
                data.status = atoi(row[1]);
                newNode->data = data;
                List_AddTail(list, newNode);
                pack->num++;
                break;
        }
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    if(pack->num)
        List_ForEach(list, curPos)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d", curPos->data.id);
        //    printf("*%s\n", buf);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            strcpy(curPos->data.name,row[0]);
            pack->fnode = curPos->data;
            int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
            //printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->rqf_num);
            //printf("%d\n", n);
        }
}

void Remove_member(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
   // printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "delete from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);

                pack->status = 0;
                send(pack->data.serfd, pack, sizeof(Pack), 0);

                memset(buf, 0, sizeof(buf));
                sprintf(buf, " select number from group_info where gid = %d", pack->data.cid);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                result = mysql_store_result(&mysql);
                row = mysql_fetch_row(result);

                memset(buf, 0, sizeof(buf));
                sprintf(buf, " update group_info set number = %d where gid = %d", atoi(row[0])-1, pack->data.cid);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);

/*??????*/
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "insert into group_management values(%d, %d, %d, 4, 0, 1)", pack->data.cid, pack->fnode.id, pack->info.id);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                    
                pthread_mutex_lock(&mutex);
                List_ForEach(plist, curPos)
                {
                    if(curPos->id == pack->fnode.id)
                    {
                        pack->status = gro;
                        send(curPos->serfd, pack, sizeof(Pack), 0);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and aid = %d and status = 4", pack->data.cid, pack->fnode.id, pack->info.id);
                            if(mysql_query(&mysql, buf) < 0)
                        my_error("mysql_query failed", __LINE__);
                    }
                }
                pthread_mutex_unlock(&mutex);
                return ;

            case 2:
            case 9:
                pack->status = -3;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                return ;
        }
    }
    pack->status = -2;
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}

void Exit_group(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " delete from group_management where gid = %d and mid = %d", pack->data.cid, pack->info.id);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " select number from group_info where gid = %d", pack->data.cid);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " update group_info set number = %d where gid = %d", atoi(row[0])-1, pack->data.cid);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

    //????????????????????????
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select mid from group_management where status = 2 or status = 9 and gid = %d", pack->data.cid);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "insert into group_management values(%d, %d, %d, 4, 0, 1)", pack->data.cid, pack->info.id, atoi(row[0]));
      //  printf("%s\n", buf);
            if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);

        pthread_mutex_lock(&mutex);
        List_ForEach(plist, curPos)
        {
            if(curPos->id == atoi(row[0]))
            {
                pack->status = gro;
                send(curPos->serfd, pack, sizeof(Pack), 0);
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and aid = %d", pack->data.cid, pack->info.id, atoi(row[0]));
                    if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
}

void Set_admini(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id from login_info where id = %d", pack->fnode.id);
    printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -4;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
        pack->status = -1;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set status = 2 , type = 0, aid = %d where gid = %d and mid = %d and status = 1", pack->fnode.id, pack->data.cid, pack->fnode.id);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);

                pthread_mutex_lock(&mutex);
                List_ForEach(plist, curPos)
                {
                    if(curPos->id == pack->fnode.id)
                    {
                        pack->status = gro;
                        send(curPos->serfd, pack, sizeof(Pack), 0);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and status = 2", pack->data.cid, pack->fnode.id);
                            if(mysql_query(&mysql, buf) < 0)
                        my_error("mysql_query failed", __LINE__);
                    }
                }
                pthread_mutex_unlock(&mutex);
                pack->status = 0;
                break;
            case 2:
                pack->status = -2;
                break;
            case 9:
                pack->status = -3;
                break;
            default:
                pack->status = -1;
                break;
        }
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}

void Dissolve_group(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

   /* memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from group_management where gid = %d\n", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);*/

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "update group_info set number = -1 where gid = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    
   //?????????????????????
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select mid, status from group_management where gid = %d", pack->data.cid);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[1]))
        {
            case 1:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set status = -1, type = 0, form = 1, aid = %d where gid = %d and mid = %d and status = %d", atoi(row[0]), pack->data.cid, atoi(row[0]), atoi(row[1]));
            //  printf("%s\n", buf);
                    if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

                pthread_mutex_lock(&mutex);
                List_ForEach(plist, curPos)
                {
                    if(curPos->id == atoi(row[0]))
                    {
                        pack->status = gro;
                        send(curPos->serfd, pack, sizeof(Pack), 0);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and status = %d", pack->data.cid, pack->info.id, atoi(row[1]));
                            if(mysql_query(&mysql, buf) < 0)
                        my_error("mysql_query failed", __LINE__);
                    }
                }
                pthread_mutex_unlock(&mutex);
                break;
            case 2:
            case 9:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set status = -1, type = 0, form = 1 where gid = %d and mid = %d and status = %d", pack->data.cid, atoi(row[0]), atoi(row[1]));
            //  printf("%s\n", buf);
                    if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

                pthread_mutex_lock(&mutex);
                List_ForEach(plist, curPos)
                {
                    if(curPos->id == atoi(row[0]))
                    {
                        pack->status = gro;
                        send(curPos->serfd, pack, sizeof(Pack), 0);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and status = %d", pack->data.cid, pack->info.id, atoi(row[1]));
                            if(mysql_query(&mysql, buf) < 0)
                        my_error("mysql_query failed", __LINE__);
                    }
                }
                pthread_mutex_unlock(&mutex);
                break;
        }
    }
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from group_management where gid = %d and not status = -1\n", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "delete from group_msg where gid = %d\n", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
}

void Transfer_group(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -1;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -2;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        if(atoi(row[0]) == 9)
        {
            pack->status = -3;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
        }
        else if(atoi(row[0]) == 2)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update group_management set status = 9, type = 0, form = 1 where gid = %d and mid = %d and aid = %d", pack->data.cid, pack->fnode.id, pack->fnode.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update group_management set status =  1 where gid = %d and mid = %d and aid = %d and status = 9", pack->data.cid, pack->info.id, pack->info.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update group_info set oid = %d where gid = %d", pack->fnode.id, pack->data.cid);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d",pack->fnode.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            sprintf(buf, "update group_info set oname = \"%s\" where gid = %d", row[0], pack->data.cid);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

               //????????????????????? 
            pthread_mutex_lock(&mutex);
            List_ForEach(plist, curPos)
            {
                if(curPos->id == pack->fnode.id)
                {
                    pack->status = gro;
                    send(curPos->serfd, pack, sizeof(Pack), 0);
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and status = 9", pack->data.cid, pack->fnode.id);
                        if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                }
            }
            pthread_mutex_unlock(&mutex);

            pack->status = 0;
            send(pack->data.serfd, pack, sizeof(Pack), 0);
        }
        else if(atoi(row[0]) == 1)
        {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "delete from group_management where gid = %d and mid = %d and status = 1", pack->data.cid, pack->fnode.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "insert into group_management values(%d, %d, %d, 9, 0, 1)", pack->data.cid, pack->fnode.id, pack->fnode.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update group_management set status =  1 where gid = %d and mid = %d and aid = %d and status = 9", pack->data.cid, pack->info.id, pack->info.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update group_info set oid = %d where gid = %d", pack->fnode.id, pack->data.cid);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

            memset(buf, 0, sizeof(buf));
            sprintf(buf, "select name from login_info where id = %d",pack->fnode.id);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            result = mysql_store_result(&mysql);
            row = mysql_fetch_row(result);
            sprintf(buf, "update group_info set oname = \"%s\" where gid = %d", row[0], pack->data.cid);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);

               //????????????????????? 
            pthread_mutex_lock(&mutex);
            List_ForEach(plist, curPos)
            {
                if(curPos->id == atoi(row[0]))
                {
                    pack->status = gro;
                    send(curPos->serfd, pack, sizeof(Pack), 0);
                    memset(buf, 0, sizeof(buf));
                    sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and status = 9", pack->data.cid, pack->fnode.id);
                        if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                }
            }
            pthread_mutex_unlock(&mutex);
        }
    }
}

void Get_status(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->info.id);
    printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
            case 11:
                pack->status = 1;
                break;
            case 2:
                pack->status = 2;
                break;
            case 9:
                pack->status = 9;
                break;
        }      
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}

void Process_grouprq(Pack *pack)
{
    per_list_t curPos;
    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;

    if(pack->status == 1)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update group_management set status = 1 , type = 1, form = 1 where gid = %d and mid = %d and aid = %d and status = 0", pack->data.cid, pack->fnode.id, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update group_management set status = 11 , type = 1 ,form = 1 where gid = %d and  mid = %d and not aid = %d and status = 0", pack->data.cid, pack->fnode.id, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "select number from group_info where gid = %d", pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        result = mysql_store_result(&mysql);
        row = mysql_fetch_row(result);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, " update group_info set number = %d where gid = %d", atoi(row[0])+1, pack->data.cid);
        //printf("%s\n", buf);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);

        /*????????????*/
     /*   pthread_mutex_lock(&mutex);
        List_ForEach(plist, curPos)
        {
            if(curPos->id == atoi(row[0]))
            {
                pack->status = gro;
                send(curPos->serfd, pack, sizeof(Pack), 0);
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set type = 1 where gid = %d and mid = %d and aid = %d and status = 1", pack->data.cid, pack->fnode.id, pack->info.id);
                    if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
            }
        }
        pthread_mutex_unlock(&mutex);*/
    }
    else if(pack->status == 2)
    {
        
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update group_management set status = 3, type = 1, form = 1 where gid = %d and mid = %d and  aid = %d and status = 0", pack->data.cid, pack->fnode.id, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update group_management set status = 12, type = 1, form = 1 where gid = %d and  mid = %d and not aid = %d and status = 0", pack->data.cid, pack->fnode.id, pack->info.id);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);
    }  
}

void View_friendinfo(Pack *pack)
{
    char buf[200];
    MYSQL_RES *result;
    MYSQL_ROW row;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select * from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    pack->fnode.id = atoi(row[0]);
    strcpy(pack->fnode.name, row[1]);
    pack->fnode.status = atoi(row[5]);

    send(pack->data.serfd, pack, sizeof(Pack), 0);
}

void Group_chat(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gname from group_info where gid = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    if(row = mysql_fetch_row(result))
        strcpy(pack->fmnode.name1, row[0]);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select mid, status from group_management where gid = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[1]))
        {
            case 1:
            case 2:
            case 9:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "insert into group_msg values(%d, %d, %d, \'%s\', 0, %d, %d, %d, %d, %d, %d)", pack->data.cid, pack->info.id, atoi(row[0]), pack->data.sendbuf, pack->fmnode.date.year, pack->fmnode.date.month, 
                                                                                                        pack->fmnode.date.day, pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second);
            //  printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);

                pthread_mutex_lock(&mutex);
                List_ForEach(plist, curPos)
                {
                    if(curPos->id == atoi(row[0]))
                    {
                        pack->status = mem;
                       // strcpy(pack->fmnode.name2, pack->info.name);
                        memset(buf, 0, sizeof(buf));
                        sprintf(buf, "update group_msg set status = 1 where gid = %d and sid = %d and rid = %d and not status = 1", pack->data.cid, pack->info.id, atoi(row[0]));
                        if(mysql_query(&mysql, buf) < 0)    
                            my_error("mysql_query failed", __LINE__);
                    //             printf("%s\n", buf);
                        send(curPos->serfd,pack, sizeof(Pack), 0);
                        break;
                    }
                }
                pthread_mutex_unlock(&mutex);
                break;
        }
    }
}

void Recv_file(Pack *pack)
{
    /*int fd = open(pack->finode.file_name, O_RDWR | O_CREAT | O_APPEND, 0666);
	int ret = write(fd, pack->finode.buf, strlen(pack->finode.buf));
    printf("%d\n", ret);
    close(fd);*/
    //Pack temp;
    //temp = *pack;
    int ret;
    char buf[1024];
    struct epoll_event tem;
	int fd = open(pack->finode.file_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
    
    printf("dada\n");
    if(epoll_ctl(pack->data.efd, EPOLL_CTL_DEL, pack->data.serfd, NULL) < 0)
        my_error("epoll_ctl failed", __LINE__);
    pack->status = 999;
    send(pack->data.serfd, pack, sizeof(Pack), 0);
    int sum = 0;
    while(1)
    {
        
        if( sum >= pack->finode.file_size)
		{
			printf("\n [recv-%s] receive file done!!!\n", pack->finode.file_name);
			break;
		}
        bzero(buf, 1024);
        ret = recv(pack->data.serfd, buf, sizeof(buf), 0);
        write(fd, buf, ret);
        sum+=ret;
                //printf("%d %d\n", sum, pack->finode.file_size);
    }
    tem.events = EPOLLIN;
    tem.data.fd = pack->data.serfd;
    if(epoll_ctl(pack->data.efd, EPOLL_CTL_ADD, pack->data.serfd, &tem) < 0)
        my_error("epoll_ctl failed", __LINE__);
    
    //*pack = temp;
    close(fd);
}

void Send_file(Pack *pack)
{
    char buffer[1024];
    char buf[500];
    MYSQL_RES *result;
    MYSQL_ROW row;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select file_size from friend_msg where id1 = %d and id2 = %d and file_name = \"%s\"", pack->data.cid, pack->info.id, pack->finode.file_name);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    if(row = mysql_fetch_row(result))
        pack->finode.file_size = atoi(row[0]);

    pack->status = FILE_recv;
    send(pack->data.serfd, pack, sizeof(Pack), 0);
	char file_path[130] = {0};//????????????

	//????????????
    sprintf(file_path, "./%s", pack->finode.file_name);
	int fd = open(file_path, O_RDWR);
	if (fd == -1)
	{
		printf("????????????%s??????\n", file_path);
		pack->status =  -1;
        return ;
	}	
    int send_len = 0;
	
	while (1)
	{	
		bzero(buffer, 1024);
		//????????????
		int ret = read(fd, buffer, sizeof(buffer));
		//printf("%d\n", ret);
		if (ret <= 0)
		{
			printf("??????????????????\n");
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "update friend_msg set type = 2 where id1 = %d and id2 = %d and file_name = \"%s\"", pack->data.cid, pack->info.id, pack->finode.file_name);
            if(mysql_query(&mysql, buf) < 0)
                my_error("mysql_query failed", __LINE__);
                    break;
		}
			
		//????????????
		send(pack->data.serfd, buffer, ret, 0);
		
		send_len += ret;//???????????????????????????
		
		//???????????????????????? 
		//printf("%d\n", send_len);
	}
	//printf("%d\n", send_len);
	// ???????????? 
	close(fd);
}

void Shield_friend(Pack *pack)
{
    char buf[200];

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "update friends set status = 2 where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
}

void Is_shield(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from friends where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    if(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
                pack->status = 0;
                break;
            case 2:
                pack->status = 1;
                break;
            default:
                pack->status = -1;
        }
        send(pack->data.serfd, pack, sizeof(Pack), 0);
    }   
}

void Cancel_shield(Pack *pack)
{
    char buf[200];

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "update friends set status = 1 where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
}

void Shield(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from friends where id2 = %d and id1 = %d", pack->info.id, pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    if(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
                pack->status = 0;
                break;
            case 2:
                pack->status = 1;
                break;
            default:
                pack->status = -1;
        }
        send(pack->data.serfd, pack, sizeof(Pack), 0);
    } 
}

void File_info(Pack *pack)
{
    char buf[500];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    if(pack->status == -99)
    {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "insert into friend_msg values(%d, %d, \"??????????????????????????????\", -1, %d, %d, %d, %d, %d, %d, 1, \"%s\", %d)", pack->info.id, pack->data.cid, pack->fmnode.date.year,pack->fmnode.date.month, 
                                                                    pack->fmnode.date.day, pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second, pack->finode.file_name, pack->finode.file_size);
        //printf("%s\n", buf);
        if(mysql_query(&mysql, buf) < 0)
            my_error("mysql_query failed", __LINE__);

        return ;
    }
    //int fd = open(pack->finode.file_name, O_RDWR | O_CREAT | O_TRUNC, 0666);
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into friend_msg values(%d, %d, \"??????????????????????????????\", 0, %d, %d, %d, %d, %d, %d, 1, \"%s\", %d)", pack->info.id, pack->data.cid, pack->fmnode.date.year,pack->fmnode.date.month, 
                                                                pack->fmnode.date.day, pack->fmnode.time.hour, pack->fmnode.time.minute, pack->fmnode.time.second, pack->finode.file_name, pack->finode.file_size);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    sprintf(pack->data.sendbuf, "??????????????????????????????");

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from login_info where id = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(atoi(row[0]) == 1)
    {
        pack->status = sb;
        pthread_mutex_lock(&mutex);
        List_ForEach(plist, curPos)
        {
            if(curPos->id == pack->data.cid)
            {
                send(curPos->serfd,pack, sizeof(Pack), 0);
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        memset(buf, 0, sizeof(buf));
        sprintf(buf, "update friend_msg set status = 1 where id1 = %d and id2 = %d", pack->info.id, pack->data.cid);
        if(mysql_query(&mysql, buf) < 0)    
            my_error("mysql_query failed", __LINE__);
        //            printf("%s\n", buf);
    }
    Recv_file(pack);
}

void View_filelist(Pack *pack)
{
    char buf[200];
    int i;
    file_list_t newNode;
    file_list_t list, curPos;
    file_t data;
    unsigned int num_fields;
    MYSQL_RES *result;
    MYSQL_ROW row;

    List_Init(list, file_node_t);

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select file_name, file_size , type, status, id1, id2 from friend_msg where id1 = %d and id2 = %d and not type = 0", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    //printf("%s\n", buf);
    pack->num = 0;
    while(row = mysql_fetch_row(result))
    {
        if((atoi(row[3]) == -1) && (atoi(row[4]) == pack->data.cid) && (atoi(row[5]) == pack->info.id))
            break;
        newNode = (file_list_t)malloc(sizeof(file_node_t));
        data.file_size = atoi(row[1]);
        data.num = atoi(row[2]);
        strcpy(data.file_name, row[0]);
        newNode->data = data;
        List_AddTail(list, newNode);
        pack->num++;
    }
    //printf("%d*\n", pack->num);
    send(pack->data.serfd, pack, sizeof(Pack), 0);

    if(pack->num)
        List_ForEach(list, curPos)
        {
            //sleep(1);
            pack->finode = curPos->data;
            int n = send(pack->data.serfd, pack, sizeof(Pack), 0);
            //printf("%d %s %d\n", pack->fnode.id, pack->fnode.name, pack->num);
            //printf("%d\n", n);
        }
}

void Cancel_admini(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select id from login_info where id = %d", pack->fnode.id);
    printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -4;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
        pack->status = -1;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, " select status from group_management where gid = %d and mid = %d", pack->data.cid, pack->fnode.id);
    //printf("%s\n", buf);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
                pack->status = -2;
                break;
            case 2:
                memset(buf, 0, sizeof(buf));
                sprintf(buf, "update group_management set status = 1 , aid = %d where gid = %d and mid = %d and status = 2", pack->fnode.id, pack->data.cid, pack->fnode.id);
                //printf("%s\n", buf);
                if(mysql_query(&mysql, buf) < 0)
                    my_error("mysql_query failed", __LINE__);
                pack->status = 0;
                break;
            case 9:
                pack->status = -2;
                break;
            default:
                pack->status = -5;
                break;
        }
    }
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}

void Is_group(Pack *pack)
{
    char buf[300];
    MYSQL_RES *result;
    MYSQL_ROW row;
    per_list_t curPos;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select gid from group_info where gid = %d", pack->data.cid);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    row = mysql_fetch_row(result);
    if(!row)
    {
        pack->status = -1;
        send(pack->data.serfd, pack, sizeof(Pack), 0);
        return ;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select status from friends where gid = %d and mid = %d", pack->data.cid, pack->info.id);
    if(mysql_query(&mysql, buf) < 0)
        my_error("mysql_query failed", __LINE__);
    result = mysql_store_result(&mysql);
    while(row = mysql_fetch_row(result))
    {
        switch (atoi(row[0]))
        {
            case 1:
            case 2:
            case 9:
                pack->status = 1;
                send(pack->data.serfd, pack, sizeof(Pack), 0);
                return ;
        }
    }
    pack->status = -2;
    send(pack->data.serfd, pack, sizeof(Pack), 0);
}