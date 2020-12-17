#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/wait.h>
#include <ctype.h> 
#include <termios.h>

#define INPUT_SIZE 512
#define ENDING_WORD "quit"
#define DELIMETERS " \t\r\n\a"
#define ARGUMENT_SIZE 64
#define COMMAND_SIZE 17
int commandCount;
bool quit_control = false;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

struct Tasks {
    char*todo[ARGUMENT_SIZE];
};

void prompt()
{
    static int first_time = 1;
    if(first_time)
    { 
        printf( "%s", "\033[2J\033[1;1H" );
        first_time = 0;
        return;
    }
    char cwd[100];
    getcwd(cwd,100);
    char promptHeader[100] = "prompt:~";
    strcat(promptHeader,cwd);
    strcat(promptHeader,"#-> ");
    printf("%s",promptHeader);
}

char* readInput()
{    
    char *input = malloc((INPUT_SIZE+2) * sizeof(char*));
    if (!input) {
        fprintf(stderr, "Bellek tahsis hatasi\n");
        exit(EXIT_FAILURE);
    }
    if(fgets(input,INPUT_SIZE+2,stdin) == NULL)
    {
        free(input);
        input = NULL;
        exit(EXIT_SUCCESS);
    }  
    if(strchr(input,'\n')  == NULL)
    {
        fprintf(stderr, "Cok uzun bir komut satiri (bu proje icin '\\n' dahil 512 karakterden fazla).\n");
        int c;
        while((c = getc(stdin)) != '\n' && c != EOF);
        free(input);
        input = NULL;
        return NULL;
    }
    if(strlen(input) > 0 && input[strlen(input) - 1] == '\n')
        input[strlen(input) - 1] = '\0';
    if(strlen(input) == 0)
    {
        free(input);
        input = NULL;
        return NULL;
    }   
    if(strchr(input,';') == NULL)
        return input;
    char tmp = ';';
    for(int i = 0;i < strlen(input);i++)
    {
        if(input[i] == '\n' || input[i] == ' ' || input[i] == '\t' || input[i] == '\r')
            continue;
        if(input[i] == ';')
        {
            if(tmp == ';')
            {
                fprintf(stderr, "Syntax hatasi\n");
                free(input);
                input = NULL;
                return NULL;
            }
        }
        tmp = input[i];
    }
    return input;
}

char*** parseInput(char*input)
{
    char **tokens = malloc(COMMAND_SIZE * sizeof(char*));   
    if (!tokens) {
        fprintf(stderr, "Bellek tahsis hatasi\n");
        free(input);
        input = NULL;
        exit(EXIT_FAILURE);
    }

    char *tmp;
    tmp = strtok(input,";");
    int i = 0;
    while (tmp != NULL)
    {
        if(i != 0)
            commandCount++;
        tokens[i] = tmp;
        i++;
        tmp = strtok (NULL,";");
        if (i >= COMMAND_SIZE) 
        {
            fprintf(stderr, "Bir seferde en fazla 16 tane ard arda komut girilebilir\n");
            free(input);
            free(tokens);
            input = NULL;
            tokens = NULL;
            return NULL;
        }
    }
    tokens[i] = NULL;

    bool last_word_control = true;
    for(int h = 0;h < strlen(tokens[i-1]);h++)
    {
        if(!isblank(tokens[i-1][h]))
        {
            last_word_control = false;
            break;
        }
    }
    if(last_word_control == true)
    {
        tokens[i-1] = NULL;
        commandCount--;
        i--;
    }

    char ***cmd = malloc(commandCount * sizeof(char**));
    if (!cmd) 
    {   
        fprintf(stderr, "Bellek tahsis hatasi\n");
        free(input);
        free(tokens);
        free(cmd);
        input = NULL;
        tokens = NULL;
        cmd = NULL;
        exit(EXIT_FAILURE);
    }
    for(int x=0;x<commandCount; x++)
    {
        cmd[x] = malloc(ARGUMENT_SIZE * sizeof(char*));
        if (!cmd[x]) 
        {   
            fprintf(stderr, "Bellek tahsis hatasi\n");
            free(input);
            free(tokens);
            for(int h = 0;h < x;h++)
            {
                free(cmd[h]);
            }
            free(cmd);
            cmd = NULL;
            exit(EXIT_FAILURE);
        }   
    }  

    for(int y = 0;y < commandCount;y++)
    {
        cmd[y][0] = tokens[y];
        tmp = strtok(tokens[y],DELIMETERS);
        int b = 0;
        while(tmp != NULL)
        {   
            cmd[y][b] = tmp;
            tmp = strtok (NULL,DELIMETERS);
            b++;
            if(b >= ARGUMENT_SIZE)
            {
                fprintf(stderr, "Cok sayida arguman girildi\nBir komut icin bir seferde en fazla 63 arguman girilebilir\n");
                free(input);
                free(tokens);
                for(int k = 0; k<commandCount;k++)
                {
                    free(cmd[k]);
                }
                free(cmd);
                cmd = NULL;
                return NULL;
            }
        }
        if(b != 0)
            cmd[y][b] = NULL;
        else
            cmd[y][b+1] = NULL;
    }
    return cmd;
}

char* readFile(char input[])  
{    
    if(strchr(input,';') == NULL)
        return input;
    char tmp = ';';
    for(int i = 0;i < strlen(input);i++)
    {
        if(input[i] == '\n' || input[i] == ' ' || input[i] == '\t' || input[i] == '\r')
            continue;
        if(input[i] == ';')
        {
            if(tmp == ';')
            {
                fprintf(stderr, "Syntax hatasi\n");
                return NULL;
            }
        }
        tmp = input[i];
    }
    return input;
}

void* doJobs(void* args)
{
    struct Tasks *tasks = (struct Tasks*) args;    
    if(strcmp(tasks->todo[0],ENDING_WORD) == 0)
        quit_control = true;        
    else if(strcmp(tasks->todo[0],"cd") == 0)
    {   
        if(tasks->todo[1] == NULL)
            chdir("/root");
        else
            if(chdir(tasks->todo[1]) == -1)
                fprintf(stderr, "Directory bulunamadi\n");
    }
    else
    {
        pthread_mutex_lock(&mutex);
        int status;
        pid_t pid,wpid;
        pid = fork();
        if(pid < 0)
            fprintf(stderr, "%s : Islem icin process olusturulamadi lutfen tekrar deneyin\n",tasks->todo[0]);
        else if(pid == 0)
        {   
            if(execvp(tasks->todo[0],tasks->todo) == -1)
                fprintf(stderr, "%s : Komut yok veya komut yurutulemez\n",tasks->todo[0]);
        }
        else
        {
            do {
            wpid = waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
        pthread_mutex_unlock(&mutex);
        pthread_exit(NULL);
    }
}

int main(int argc, char* argv[])
{
    char *input;
    
    if(argc > 2)
    {
        fprintf(stderr, "Cok fazla baslangic argumani mevut\nOrnek kullanim :\n1- ./Shell ornek.txt\n2- ./Shell\n");
        exit(EXIT_FAILURE);
    }
    else if (argc == 2)
    {
        argv[2] = NULL;
        FILE *handler = fopen(argv[1], "r");
        if(!handler)
        {
            fprintf(stderr, "Batch dosyasi mevcut degil veya acilamiyor\n");
            exit(EXIT_FAILURE);
        }      
        prompt();   
        char temp[INPUT_SIZE+1];
        char *commands[512];
        int z = 0;
        while(fgets(temp,INPUT_SIZE+2,handler))
        {
            if(z == 512)
            {
                fprintf(stderr, "Batch dosyasinda max 512 satirlik komut bulunabilir\n");
                for(int k = 0;k < z;k++)
                {
                    free(commands[k]);
                    commands[k] = NULL;
                }
                exit(EXIT_FAILURE);
            }
            if(strlen(temp) > 512)
            {
                fprintf(stderr, "Cok uzun bir komut satiri (bu proje icin '\\n' dahil 512 karakterden fazla).\n");
                int c;
                while((c = getc(handler)) != '\n' && c != EOF);
                continue;
            }
            if(temp[strlen(temp)-1] == '\n')
                temp[strlen(temp)-1] = '\0';

            commands[z] = malloc((INPUT_SIZE+1) * sizeof(char));
            if (!commands[z]) 
            {   
                fprintf(stderr, "Bellek tahsis hatasi\n");
                for(int k = 0;k < z;k++)
                {
                    free(commands[k]);
                    commands[k] = NULL;
                }
                exit(EXIT_FAILURE);
            }
            strcpy(commands[z],temp);
            z++;
        }
        fclose(handler);
        commands[z] = NULL;
        for(int i = 0; i < z ;i++)
        {
            commandCount = 1;
            prompt();
            printf("%s\n",commands[i]);

            char*input = readFile(commands[i]);
            if(!input)
            {
                free(commands[i]);
                commands[i] = NULL;
                continue;
            }
            char***args = parseInput(input);
            if(!args)
                continue;

            pthread_t tids[commandCount];
            struct Tasks tasks[commandCount];

            for(int t =0;t<commandCount;t++)
            {
                for(int n = 0;n < ARGUMENT_SIZE;n++)
                {
                    tasks[t].todo[n] = args[t][n];
                } 
                pthread_attr_t attr;
                pthread_attr_init(&attr);
                pthread_create(&tids[t],&attr,doJobs,&tasks[t]);
            }

            for(int y =0;y<commandCount;y++)
            {
                pthread_join(tids[y],NULL);
                free(args[y]);
            }
            free(input);
            free(args);
            args = NULL;
            input = NULL;
            if(quit_control)
                break;
        }
    pthread_mutex_destroy(&mutex);
    exit(EXIT_SUCCESS);
    }
    else
    {
        prompt();  
        struct termios old_termios, new_termios;
        tcgetattr(0,&old_termios);
        new_termios             = old_termios;
        new_termios.c_cc[VEOF]  = 3; // ^C
        new_termios.c_cc[VINTR] = 4; // ^D
        tcsetattr(0,TCSANOW,&new_termios);

        while(1)
        {
            commandCount = 1;
            prompt();
            input = readInput();
            if(!input)
                continue;
            char ***args = parseInput(input);
            if(!args)
                continue;
            pthread_t tids[commandCount];
            struct Tasks tasks[commandCount];

            for(int x =0;x<commandCount;x++)
            {
                for(int a = 0;a < ARGUMENT_SIZE;a++)
                {
                    tasks[x].todo[a] = args[x][a];
                }
                pthread_attr_t attr;
                pthread_attr_init(&attr);
                pthread_create(&tids[x],&attr,doJobs,&tasks[x]);
            }
            for(int y =0;y<commandCount;y++)
            {
                pthread_join(tids[y],NULL);
                free(args[y]);
            }       
            free(args);
            free(input);
            args = NULL;
            input = NULL;   
            if(quit_control)
                break;
        }
        tcsetattr(0,TCSANOW,&old_termios);
    }
    pthread_mutex_destroy(&mutex);       
    exit(EXIT_SUCCESS);
}