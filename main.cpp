/**************************************************8
 * authour:
 * sid: 3150102193
 * time
 * purpose
 */
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "unistd.h"
#include "pwd.h"
#include <time.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "shell.h"


// Syntax Highlighting
#define GREEN         "\033[0;32;32m"
#define BLUE          "\033[0;32;34m"
#define WHITE         "\033[1;37m"
#define RED           "\033[1;31;40m"

#define MAXLINE 80 /* The maximum length command */

typedef struct node *Vnode;
typedef struct setnode *Snode;
struct node
{
    char variable[50];
    char value[50];
    Vnode next;
};

struct setnode
{
    char strvalue[100];
    Snode next;
};

char PATH[80];      // current path
char buf[100];      // buffer
char copybuf[100];
char destination[100];
char source[100];
char *symbol;       // one part of command
char *leftcommand;
Vnode vlist = NULL;
int vcount=0;
Snode slist = NULL;


void setpath(char* defaultpath) // set the path: defaultpath
{
    chdir(defaultpath);
}
/***********************************
 * hell 的环境变量应该包含shell=<pathname>/myshell，其中<pathname>/myshell 是可执行程序shell 的完整路径
 */
void Add_Environment()
{
    char * path = getenv("PATH");
    char tmp[80];
    getcwd(tmp,sizeof(tmp));    // get current
  //  printf("%s\n",path);
  //  printf("%s\n",tmp);
    strcat(path,":");
    strcat(path,tmp);
 //   printf("%s\n",path);
    setenv("PATH",path,1);
    char * t = getenv("PATH");
 //   printf("%s\n",t);
}

int redcommand()				/*读取用户输入*/
{
    fgets(buf,90,stdin);
    leftcommand = strtok(buf,"\n");
    strcpy(copybuf,buf);
    symbol = strtok(leftcommand," "); //处理字符串，删除最后的换行符
}


/********************************
 *
 */
void init(){
    Add_Environment(); // shell 的环境变量应该包含shell=<pathname>/myshell，其中<pathname>/myshell 是可执行程序shell 的完整路径
/*设置命令提示符*/
    fflush(stdout);
/*设置默认的搜索路径*/
    setpath("/home/gesq");        // set initial path
    strcpy(PATH,"/home/gesq");    // store path information in the $PATH

}


int is_internal_command() {

    int result;
    if (strcmp(symbol, "quit") == 0 || strcmp(symbol, "exit") == 0) {
        exit(0);
        return 0;
    }

    if (strcmp(symbol, "pwd") == 0) {
        puts(PATH);
        return 0;
    }


    if (strcmp(symbol, "time") == 0) {
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        printf("%s", asctime(timeinfo));
        return 0;
    }

    //echo <comment>  ——在屏幕上显示<comment>并换行（多个空格和制表符可能被缩减为一个空格）。

    if (strcmp(symbol, "p") == 0) {
        Vnode p = vlist;
        while (p != NULL) {
            printf("%s = %s\n", p->variable, p->value);
            p = p->next;
        }
        return 0;
    }

    if (strcmp(symbol, "echo") == 0) {
        char *t = strtok(NULL, " ");
        if (t == NULL)
            return 0;

        if(*t == '$' && (*(t+1) == '1' || *(t+1) == '2'||*(t+1) == '3'||*(t+1) == '3'||*(t+1) == '4' || *(t+1) == '5' || *(t+1) == '6' || *(t+1) == '7' || *(t+1) == '8' || *(t+1) == '9'))
        {
            t++;
            int num=0;
            while(*t != '\0')
            {
                num = 10 * num + *t - '0';
                t++;
            }
            int count=0;
            Snode p = slist;
            while(p != NULL)
            {
                count++;
                if(count == num)
                {
                    puts(p->strvalue);
                    return 0;
                }
                p = p->next;
            }
            if(num > count)
            {
                putchar('\n');
                return 0;
            }


            return 0;
        }
        if (*t == '$' && *(t + 1) != ' ') {
            char *variable = t + 1;
            Vnode p = vlist;
            int flag = 0;
            while (p) {
                if (strcmp(p->variable, t + 1) == 0) {
                    puts(p->value);
                    flag = 1;
                    break;
                }
                p = p->next;
            }
            if (flag == 0)
                putchar('\n');
            return 0;
        }


        while (t) {
            printf("%s ", t);
            //puts("1");
            t = strtok(NULL, " ");
        }
        putchar('\n');
    }
    if (strcmp(symbol, "ls") == 0) {
        //puts("hello");

        int s1=0;
        int s2=0;
        char para = 'm';
        int flag = 0;


        //puts(copybuf);


        for(int i=0;i<100;i++)
        {

            if(s1 == 0 && i>=1 && copybuf[i-1] == 'l' && copybuf[i] == 's')
            {
                s1 = 1;
                flag = 1;
            }
            if(s1 == 1&& copybuf[i] == '-' && copybuf[i+1] != '\0')
            {
                para=copybuf[i+1];
                flag = 2;
                s2 = 1;
            }
            if(s2 == 1 && ( para != 'l' && para != 'a' && para != 'i' && para != 'd' && para != 't' &&  para != 'F' &&para != 'R' ))
            {
                puts("Wrong ls parameter");
                return 1;
            }
            if(copybuf[i] == '\0')
                break;
        }

        //putchar(para);

        result = fork();
        if (result < 0) {
            printf("fail to ls\n");
            exit(1);
        }
        else if (result == 0 && flag == 1) {
            execlp("ls", "ls", NULL);
            exit(0);
        }
        else if (result == 0 && para == 'l') {
            execlp("ls", "ls", "-l", NULL);
            exit(0);
        }
        else if (result == 0 && para == 'a') {
            execlp("ls", "ls", "-a", NULL);
            exit(0);
        }
        else if (result == 0 && para == 'i') {
            execlp("ls", "ls", "-i", NULL);
            exit(0);
        }
        else if (result == 0 && para == 'd')  {
            execlp("ls", "ls", "-d", NULL);
            exit(0);
        }
        else if (result == 0 && para == 't') {
            execlp("ls", "ls", "-t", NULL);
            exit(0);
        }
        else if (result == 0 && para == 'F') {
            execlp("ls", "ls", "-F", NULL);
            exit(0);
        }
        else if (result == 0 && para == 'R') {
            execlp("ls", "ls", "-R", NULL);
            exit(0);
        }
        else {

            int status;
            waitpid(result, &status, 0);
           // puts("world");
            return 0;
        }

    }


    // extra function: show list
    /*if (strcmp(symbol, "ls") == 0) {
        //puts("fsda");
        symbol = strtok(NULL, " ");
        result = fork();
        if (result < 0) {
            printf("fail to ls\n");
            exit(1);
        }
        else if (result == 0 && symbol == NULL) {
            execlp("ls", "ls", NULL);
            _exit(0);
        } else if (result == 0 && strcmp(symbol, "-l") == 0) {
            execlp("ls", "ls", "-l", NULL);
        } else if (result == 0 && strcmp(symbol, "-a") == 0) {
            execlp("ls", "ls", "-a", NULL);
        } else if (result == 0 && strcmp(symbol, "-i") == 0) {
            execlp("ls", "ls", "-i", NULL);
        } else if (result == 0 && strcmp(symbol, "-d") == 0) {
            execlp("ls", "ls", "-d", NULL);
        } else if (result == 0 && strcmp(symbol, "-t") == 0) {
            execlp("ls", "ls", "-t", NULL);
        } else if (result == 0 && strcmp(symbol, "-F") == 0) {
            execlp("ls", "ls", "-F", NULL);
        } else if (result == 0 && strcmp(symbol, "-R") == 0) {
            execlp("ls", "ls", "-R", NULL);
        } else {
            int status;
            waitpid(result, &status, 0);
            return 0;
        }

    }*/
    // Keyfunction: cd . Parameter: address; .. return last dir;
    if (strcmp(symbol, "cd") == 0) {
        char *t;
        char tpath[80];
        int flag;
        t = strtok(NULL, " ");
        //printf("%d\n",t[0]);

        if(strcmp("cd",copybuf) == 0)
        {
            chdir("/usr/bin");
            strcpy(PATH,"/usr/bin");
            return 0;
        }

        if (t[0] == '/' && strlen(t) > 1) {
            strcpy(tpath, t);
            flag = chdir(tpath);
            //puts(tpath);
            if (flag == 0) {
                strcpy(PATH, tpath);
            } else {
                printf("Fail to enter %s\n", tpath);
            }
        } else if (strcmp("~", t) == 0)      // return default dir address
        {
            setpath("/usr/bin");
            strcpy(PATH, "/usr/bin");
            return 0;
        } else if (strcmp(t, "..") == 0) {
            char *lastpath;
            char *p = PATH;
            char *last;
            int cnum = 0;
            int tnum = 0;
            char tmp[80];
            while (*p != '\0') {
                if (*p == '/') {
                    last = p;
                    cnum += tnum;
                    tnum = 0;
                }
                tnum++;
                p++;
            }
            strncpy(tmp, PATH, cnum);
            int flag;
            flag = chdir(tmp);
            if (flag == 0) {
                strcpy(PATH, tmp);
            } else {
                puts("fail to exec cd ..");
            }
            return 0;
        } else if (strcmp("/", t) == 0) // return root dir
        {
            flag = chdir("/");
            if (flag == 0) {
                strcpy(PATH, "/");
            } else {
                printf("fail\n");
            }
        } else {   // cd set address
            strcpy(tpath, PATH);
            if (strcmp(PATH, "/") != 0)
                strcat(tpath, "/");
            strcat(tpath, t);

            flag = chdir(tpath);
            if (flag == 0) {
                strcpy(PATH, tpath);
            } else {
                printf("Fail to enter %s\n", tpath);
            }
            return 0;
        }
        return  0;
    }


    if (strcmp(symbol, "unset") == 0) {
        char *t;
        char tmp[80];
        Vnode p = vlist;
        Vnode last = p;
        t = strtok(NULL, " ");
        if (t[0] == '$') {
            strcpy(tmp, t + 1);
            while (p) {
                if (strcmp(tmp, p->variable) == 0) {
                    if (p == vlist && p->next == NULL) {
                        free(p);
                        vlist = NULL;
                    } else if (p == vlist && p->next != NULL) {
                        vlist = vlist->next;
                        free(p);
                    } else {
                        last->next = p->next;
                        free(p);
                    }
                }
                last = p;
                p = p->next;
            }
        }
        return 0;
    }

    // show files in a dictionary. you can input address as a parameter
    if (strcmp(symbol, "dir") == 0) {
        DIR *dir;
        struct dirent *ptr;
        char *t;
        char tpath[80];
        int result;
        int status;


        t = strtok(NULL, " ");


        if (t == NULL) {
            dir = opendir(PATH);
        } else {
            dir = opendir(t);
        }


        int count = 0;
        while ((ptr = readdir(dir)) != NULL) {
            printf("%s   ", ptr->d_name);
            count++;
            if (count % 8 == 0)
                putchar('\n');
        }
        putchar('\n');
        closedir(dir);
        return 0;
    }
    // show environment variable

    if (strcmp("environ", symbol) == 0) {
        char *pathvar = getenv("PATH");
        printf("%s\n", pathvar);
        return 0;
    }


    char *next = strstr(leftcommand, "=");
    if (next != NULL) {
        char varibale[50];
        int i;
        for (i = 0; i < 50; i++) {
            if (leftcommand[i] == '=') {
                break;
            }
            varibale[i] = leftcommand[i];
        }
        Vnode p = vlist;
        int flag = 0;
        while (p) {
            if (strcmp(varibale, p->variable) == 0) {
                strcpy(p->value, next + 1);
                flag = 1;
                break;
            }
            p = p->next;
        }
        if (flag == 1)
            return 0;
        p = vlist;
        if (vlist == NULL) {
            vlist = (Vnode) malloc(sizeof(struct node));
            if (vlist == NULL)
                puts("Fail to allocate memory");
            vlist->next = NULL;
            strcpy(vlist->variable, varibale);
            strcpy(vlist->value, next + 1);
            vcount++;
        } else {
            vcount++;
            while (p->next != NULL)
                p = p->next;
            Vnode newnode = (Vnode) malloc(sizeof(struct node));
            if (newnode == NULL)
                puts("Fail to allocate memory");
            p->next = newnode;
            newnode->next = NULL;
            strcpy(newnode->variable, varibale);
            strcpy(newnode->value, next + 1);
        }


        return 0;
    }


/**********************************************8
 *  function: set $(cmd) in shell
 *  redirect first to a temporary file and excute cmd
 *  read the output from the file to construct a list and delete the file
 */
    if(strcmp("set",symbol) == 0)
    {
        CleanList();

        char incmd[100];
        char tmp[100];
        char *start;
        int cstart = 0;
        int cend = 0;
        char *p=copybuf;
        int step0 = 0;
        int step_1 = 0;
        int step2 = 0;
        int flag = 0;
        int lbracket = 0, rbracket = 0;
       // puts(copybuf);
        for(int i=0;i<100;i++)
        {
            if(copybuf[i] == '\0')
                break;
            if(step2 == 1)
            {
                if(copybuf[i] == '(')
                {
                    lbracket++;
                    start = &copybuf[i+1];
                    cstart = i+1;
                }
                else if( copybuf[i] == ')')
                {
                    rbracket++;
                    cend = i-1;
                }
            }
            else if(step_1 == 1)
            {
                if(copybuf[i] == '$') {
                    step2 = 1;
                    //puts("AAAAAAAA");
                }
                else if(copybuf[i] == ' ')
                    ;
                else{
                    puts("Error syntex in set");
                    return 1;
                }
            }
            else if(i >= 2 && copybuf[i-2] == 's' && copybuf[i-1] == 'e' && copybuf[i] == 't')
            {
                //puts("BBBBBb");
                step_1 = 1;
            }

        }

        if(lbracket != rbracket)
        {
            printf("Not match (),( %d ; ) %d\n",lbracket,rbracket);
            return 1;
        }

        int length = cend - cstart + 1;

        char tcopy[100];
        memset(tcopy,0,sizeof(tcopy));
        strncpy(tcopy,start,length);

        strcpy(source,tcopy);
        char tpath[100] ;
        strcpy(tpath,PATH);
        if(strcmp(PATH,"/") != 0)
            strcat(tpath,"/");
        strcat(tpath,"settmp");

        strcpy(copybuf,tcopy);
        //puts(tcopy);
        //puts(tpath);

        int result;
        result = fork();
        if(result < 0)
        {
            puts("fail to fork in set");
        }
        else if(result == 0)
        {
            symbol=strtok(tcopy," ");


            int fd = open(tpath,O_WRONLY | O_CREAT |O_TRUNC ,0644);
            close(1);
            dup(fd);
            close(fd);

            is_internal_command();

            exit(0);
        }
        else
        {
            int *status;
            waitpid(result,status,0);
        }

        //puts("hello");

        ReadforList(tpath);

        return 0;
    }

    if(strcmp("shift",symbol) == 0 )
    {
        Snode  t;
        if(slist == NULL)
            return 0;
        else if(slist->next == NULL)
        {
            t = slist;
            slist = NULL;
            free(t);
        }
        else
        {
            t = slist;
            slist = slist->next;
            free
            (t);
        }
        return  0;
    }

    // help + commandname : get instructions of this command
    if(strcmp("help",symbol) == 0 )
    {
        char *t;
        t = strtok(NULL," ");
        if(strcmp("cd",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("bg",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("continue",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("echo",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("exec",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("exit",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("fg",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("jobs",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("pwd",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("set",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("shift",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("test",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("time",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("umask",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("unset",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("time",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("dir",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("environ",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("help",t) == 0)
        {
            printf("help");
        }
        else if(strcmp("quit",t) == 0)
        {
            printf("help");
        }
        else
        {
            printf("No this interior commands\n");
        }
    }

    puts("Not support this command");
    return 0;
}

/**************************
 *  Another set command. Before executing, clean the list for this command
 */
void CleanList()
{
    if(slist == NULL)
    {
        return;
    }
    else
    {
        Snode t = slist;
        Snode last;
        while(t!=NULL)
        {
            last = t;
            t = t->next;
            free(last);
        }
        slist = NULL;
        return;
    }
}

void ReadforList(char *tpath)
{
    char *pBuf;
    FILE *pFile=fopen(tpath,"r"); //获取文件的指针
    fseek(pFile,0,SEEK_END); //把指针移动到文件的结尾 ，获取文件长度
    int len=ftell(pFile); //获取文件长度

    //pBuf=new char[len+1]; //定义数组长度

    pBuf = (char*)malloc((len+1)*sizeof(char));

    rewind(pFile); //把指针移动到文件开头 因为我们一开始把指针移动到结尾，如果不移动回来 会出错
    fread(pBuf,1,len,pFile); //读文件
    pBuf[len]=0; //把读到的文件最后一位 写为0 要不然系统会一直寻找到0后才结束
    fclose(pFile); // 关闭文件

    /*读取临时文件的内容，开辟一个链表来存储文件的内容*/

    //puts(pbu);

    char *word = strtok(pBuf," ");
    int i = 0;
    Snode listend;
    while( word != NULL)
    {
        if(slist == NULL)
        {
            slist = (Snode)malloc((sizeof(struct setnode)));
            slist->next = NULL;
            strcpy(slist->strvalue,word);
            listend = slist;
            //puts(slist->strvalue);
        }
        else
        {
            Snode tmp = (Snode)malloc((sizeof(struct setnode)));
            tmp->next = NULL;
            strcpy(tmp->strvalue,word);
            listend->next = tmp;
            listend=tmp;
            //puts(tmp->strvalue);
        }
        word = strtok(NULL," ");
        i++;
    }
    Snode t = slist;

    /*while(t!=NULL) {
        puts(t->strvalue);
        t = t->next;
    }*/

    free(pBuf); // free memory

}



int check_intercommand(char * cmd)
{
    if( strcmp(cmd,"bg") == 0 || strcmp(cmd,"cd") == 0 || strcmp(cmd,"continue") == 0 ||  strcmp(cmd,"echo") == 0 || strcmp(cmd,"exec") == 0 || strcmp(cmd,"cd") == 0 || strcmp(cmd,"exit") == 0 || strcmp(cmd,"fg") == 0|| strcmp(cmd,"jobs") == 0 || strcmp(cmd,"pwd") == 0 || strcmp(cmd,"set") == 0 || strcmp(cmd,"shift") == 0 || strcmp(cmd,"test") == 0 || strcmp(cmd,"time") == 0 || strcmp(cmd,"umask") == 0 || strcmp(cmd,"unset") == 0 || strcmp(cmd,"clr") || strcmp(cmd,"environ") == 0 || strcmp(cmd,"dir") == 0 || strcmp(cmd,"help") == 0 || strcmp(cmd,"quit") == 0)
        return 0;
    else
        return 1;
}

int check_redirect(){
    char *t;
    char tchar[100];
    char *flag;
    //printf("%s\n",copybuf);
    if( strstr(copybuf,">") != NULL || strstr(copybuf,">>") != NULL)
    {

        if(strstr(copybuf,">>") != NULL)
        {
            flag = t = strstr(copybuf,">>");
            strcpy(tchar,t);
            t = strtok(tchar, " ");
            t = strtok(NULL," ");
        }
        if(strstr(copybuf,">") != NULL)
        {
            flag = t = strstr(copybuf,">");
            strcpy(tchar,t);
            t = strtok(tchar, " ");
            t = strtok(NULL," ");
        }
        char *p = copybuf;
        int count=0;
        while(p++ != flag)
            count++;
        strncpy(source,copybuf,count);
        strcpy(destination,t);
        symbol = strtok(source," ");
        //puts(source);
        //puts(destination);
        return 0;
    }
    else if( strstr(copybuf,"<") != NULL || strstr(copybuf,"<<") != NULL )
    {
        if(strstr(copybuf,"<<") != NULL)
        {
            flag = t = strstr(copybuf,"<<");
            strcpy(tchar,t);
            t = strtok(tchar, " ");
            t = strtok(NULL," ");
        }
        else if(strstr(copybuf,"<") != NULL )
        {
            flag = t = strstr(copybuf,"<");
            strcpy(tchar,t);
            t = strtok(tchar, " ");
            t = strtok(NULL," ");
        }
        char *p = copybuf;
        int count=0;
        while(p++ != flag)
            count++;
        strncpy(destination,copybuf,count);
        strcpy(source,t);
        //puts(source);
        //puts(destination);
        return 0;
    }
    else
        return 1;
}


int exec_redirect(){
    //printf(BLUE"hello\n");
    int result;
    int *status;


    if(strstr(copybuf,">>") != NULL)
    {
        result = fork();
        if(result < 0)
        {
            puts("Fail to fork in redirect!");
        }
        else if( result == 0)
        {
            char nowpath[100];
            strcpy(nowpath,PATH);
            strcat(nowpath,"/");
            strcat(nowpath,destination);


            int fd = open(nowpath,O_WRONLY | O_CREAT |O_APPEND ,0644);
            close(1);
            dup(fd);
            close(fd);

            is_internal_command();

            //puts(nowpath);
            exit(0);
        }
        else{
            waitpid(result,status,0);
            //puts("B");
        }
    }
    else if(strstr(copybuf,">") != NULL)
    {
        result = fork();
        if(result < 0)
        {
            puts("Fail to fork in redirect!");
        }
        else if( result == 0)
        {
            char nowpath[100];
            strcpy(nowpath,PATH);
            strcat(nowpath,"/");
            strcat(nowpath,destination);

            //puts(symbol);

            int fd = open(nowpath,O_WRONLY | O_CREAT |O_TRUNC ,0644);
            close(1);
            dup(fd);
            close(fd);

            is_internal_command();

            //puts(nowpath);
            exit(0);
        }
    }
    else if(strstr(copybuf,"<<") != NULL || strstr(copybuf,"<") != NULL)
        puts("要求说只要实现stdin或者stdout一种就行\n");

    //is_internal_command();
    return 0;
}

int main(void) {
    char *args[MAXLINE / 2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    init();             // initial parameter
    int i = 0;
    int result;
    while (should_run)   // infinite loop
    {
        printf(GREEN"myshell>"BLUE"%s"WHITE"$ ", PATH); //命令行提示符
        redcommand();/*读取用户输入*/
        if (strcmp(buf, "\n") == 0) {
            //printf("myshell>");
            continue;
        }

        if (check_redirect() == 0) {
            exec_redirect();
            continue;
        }



        if (check_intercommand(symbol) == 0) {
            is_internal_command();
            continue;
        }


    }


}

