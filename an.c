#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include<stdbool.h>
#include<pthread.h>
#include<unistd.h>
#include<fcntl.h>
#define MAX_QUEUE_SIZE 1000
#define MAX_ELEVATOR_CAP 20
#define MAX_NEW_REQUESTS 30
#define MAX_FLOORS 500
#define MAX_ELEVATORS 100
#define soft_limit 5
typedef struct Preq{
    int reqId;
    int from;
    int to;
}Preq;
typedef struct solresp{
    long mtype;
    int correct;
}solresp;
typedef struct mainmem{
    char authstr[100][21];
    char instr[100];
    Preq newPreq[30];
    int floors[100];
    int droppedPsg[1000];
    int pickedPsg[1000][2];
} mainmem;
typedef struct helperReq{
    long mtype;
    int droppedcount;
    int picke;
} helperReq;
typedef struct solmsg{
    long mtype;
    int elevnum;
    char authguess[21];
} solmsg;
typedef struct helperResp{
    long mtype;
    int turnNumber;
    int newPreqcount;
    int errorOccured;
    int finished;
}helperResp;
typedef struct {
    Preq requests[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} waitingQ;
void initQ(waitingQ* q) {
    q->front =0;
    q->rear = -1;
    q->size =0;
}
int absolute(int a){
    if(a>0){
        return a;
    }
    else{
        return a*-1;
    }
}
void enq(waitingQ* q, Preq request) {
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->requests[q->rear] = request;
    q->size+=1;
}
int deq(waitingQ* q, Preq* request) {
    if (q->size == 0) {
        return -1;
    }
    *request = q->requests[q->front];
    q->front = (q->front +1) % 1000;
    q->size-=1;
    return 0;
}
void update(int n, int curr_state[][soft_limit][4],int best,int p_id,int start,int end, int cap[]){
    curr_state[best][cap[best]][0]=p_id;
    curr_state[best][cap[best]][1]=end;
    curr_state[best][cap[best]][2]=start;
    cap[best]++;
}
void movup(int n,mainmem *shmptr, int i, int moved[]){
    shmptr->instr[i]='u';
    moved[i]=1;
}
void movdown(int n,mainmem *shmptr, int i, int moved[]){
    shmptr->instr[i]='d';
    moved[i]=-1;
}
int assigned(int p_id, int n, int curr_state[][soft_limit][4]) {
    for(long i = 0; i < n; i++) {
        for(long j = 0; j < soft_limit; j++) {
            if(p_id==curr_state[i][j][0]) {
                return 1;
            }
        }
    }
    return 0;
}
void updateDist(int n,int cap[],mainmem*shmptr, int mindist, int best, int start){
    for(long j=0;j<n;j++)
                {
                    if(cap[j]<soft_limit&&abs(shmptr->floors[j]-start)<=mindist)
                    {
                        mindist=abs(shmptr->floors[j]-start);
                        best=j;
                    }
                }
}
int mincheck(int mindist, mainmem*shmptr, int start, int best, int j){
    mindist=abs(shmptr->floors[j]-start);
    return j;
}
void cmdu(int n, int commands[][2], int i){
    commands[i][0]++;
}
void cmdp(int n, int commands[][2], int i){
    commands[i][1]++;
}
int power(int base, int exp) {
    int result = 1;
    for (long i = 0; i < absolute(exp); i++) {
        result *= base;
    }
    return result;
}


void validchange(mainmem *shmptr,int n, int curr_state[][soft_limit][4], int picked_up, int i, int j, int picktot, int present[]){
    shmptr->pickedPsg[picked_up][0]=curr_state[i][j][0];
    shmptr->pickedPsg[picked_up++][1]=i;
    picktot++;
    curr_state[i][j][3]=1;
    present[i]++;
}
void decvals(int n, int i, int tosub[],int cap[]){
    tosub[i]--;
    cap[i]--;
}
int guesser(int n, int id, int elevatorNumber, char* ans)
{
    char* letters = (char*)malloc(7 * sizeof(char));
    strncpy(letters, "abcdef", 7);

    char current_guess[n + 1];
    int* arr = (int*)malloc(n * sizeof(int));
    solmsg req;
    req.mtype = 2;
    req.elevnum= elevatorNumber;
    if (msgsnd(id, &req, sizeof(solmsg) - sizeof(long), 0) < 0) {
        perror("Message send failed");
    }

    req.mtype = 3;
    solresp rep;
    for (int i = 0; i < n; i++) {
        arr[i] = 0;
    }

    int attempts = 0;
    int max_attempts = power(6, n);
    int index = n - 1; 
    for (; attempts < max_attempts; attempts++)
    {
        for (int i = index; i >= 0; i--)
        {
            if (arr[i] >= 6)
            {
                arr[i] = 0;
                if (i == 0) {
                    free(arr);
                    free(letters);
                    return 0;
                }
                arr[i - 1] += 1;
            }
        }
        for (int i = 0; i < n; i++)
        {
            current_guess[i] = (char)(letters[arr[i]]);
        }
        current_guess[n] = '\0';
        strncpy(req.authguess, current_guess, strlen(current_guess) + 1);
        if (msgsnd(id, &req, sizeof(solmsg) - sizeof(long), 0) < 0) {
            perror("Message send error during guessing phase");
            continue;
        }

        // Error-prone message receiving
        if (msgrcv(id, &rep, sizeof(solresp) - sizeof(long), 4, 0) < 0) {
            perror("Message receive failed");
            continue;
        }

        int right = rep.correct;
        if (right != 0)
        {
            strncpy(ans, current_guess, strlen(current_guess) + 1); 
            free(arr);
            free(letters);
            return 1;
        }
        arr[index] = arr[index] + 1;
    }
    free(arr);
    free(letters);

    return 0;
}

void update4(int n, int curr_state[][soft_limit][4],int p_id, int end, int start, int best, int cap[]){
    curr_state[best][cap[best]][0]=p_id;
    curr_state[best][cap[best]][1]=end;
    curr_state[best][cap[best]][2]=start;
    cap[best]++;
}
void todel( int target_j,int curr_state[][soft_limit][4],int i, int max_pass)
{   
    int z=0;
    z=target_j;
    for(int s= z;s<max_pass-1;s++)
    {
        curr_state[i][s][0]=curr_state[i][s+1][0];
        curr_state[i][s][1]=curr_state[i][s+1][1];
        curr_state[i][s][2]=curr_state[i][s+1][2];
        curr_state[i][s][3]=curr_state[i][s+1][3];
    }
    curr_state[i][max_pass-1][0]=-1;
    curr_state[i][max_pass-1][1]=-1;
    curr_state[i][max_pass-1][2]=-1;
    curr_state[i][max_pass-1][3]=-1;
}
void cmddec(int n, int commands[][2], int i){
    commands[i][0]--;
}
void cmddec2(int n, int commands[][2], int i){
    commands[i][1]--;
}
void reads(const char* filename, int* n, int* k, int* m, int* t, key_t* shmkey, key_t* mmkey, key_t* skeys, int** solverId) {
    FILE* file = fopen(filename, "r");
    if (fscanf(file, "%d %d %d %d", n, k, m, t) != 4) {
        perror("Failed to read n, k, m, t");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    if (fscanf(file, "%d %d", shmkey, mmkey) != 2) {
        perror("Failed to read shmkey, mmkey");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < *m; i++) {
        if (fscanf(file, "%d", &skeys[i]) != 1) {
            perror("Failed to read skeys");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }
    *solverId = (int*)malloc((*m) * sizeof(int));
    if (!(*solverId)) {
        perror("Failed to allocate memory for solverId");
        fclose(file);
        exit(EXIT_FAILURE);
    }
    for (long i = 0; i < *m; i++) {
        (*solverId)[i] = msgget(skeys[i], 0);
    }

    fclose(file);
}
int main(int argc, char* argv[]){
    char input[12] = "input.txt";
    int n, k, m, t;
    key_t shmkey, mmkey;
    key_t skeys[100];
    int* solverIddash;
    reads(input, &n, &k, &m, &t, &shmkey, &mmkey, skeys, &solverIddash);
    printf("%d",n);
    int solverId[m];
    for(int i=0;i<m;i++){
        solverId[i]=solverIddash[m];
    }
    mainmem* shmptr;
    int shmid = shmget(shmkey, sizeof(mainmem), 0);
    if (shmid < 0) {
        perror("Failed to get shared memory");
        free(solverIddash);
        exit(EXIT_FAILURE);
    }
    shmptr = shmat(shmid, NULL, 0);
    if (shmptr == (void*)-1) {
        perror("Failed to attach shared memory");
        free(solverIddash);
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<m;i++)
    {
        solverId[i]=msgget(skeys[i], 0);
    }
    int a;
    helperResp turns;
    int mainID = msgget(mmkey,0);
    long picktot=0, droptot=0; 
    int cap[n];
    int curr_state[n][soft_limit][4];
    memset(curr_state,-1,sizeof(curr_state));
    int commands[n][2];
    int present[n];
    int moved[n];
    int tosub[n];
    for(long i=0;i<n;i++){
        tosub[i]=0;
    }
    for(long i=0;i<n;i++){
        present[i]=0;
    }
    for(long i=0;i<n;i++){
        cap[i]=0;
    }
    for(long i=0;i<n;i++){
        moved[i]=0;
    }
for (long i = 0; i < n; i++) {
    for (long j = 0; j < 2; j++) {
        commands[i][j] = 0;
    }
}
    int zones = (n/m)+1;
    int zone[m];
    for(int i=0;i<m;i++){
        zone[i]=0;
    }
    waitingQ q;
    initQ(&q);
    while(1)
    {
msgrcv(mainID, &turns, sizeof(helperResp)-sizeof(long),2,0);
        int newp=turns.newPreqcount;
        long dropped=0;
        for(int i=0;i<n;i++){
            tosub[i]=0;
        }
        for(long i=0;i<n;i++)
        {
            for(long j=0;j<soft_limit;j++)
            {
                if(curr_state[i][j][1]==shmptr->floors[i] && curr_state[i][j][3]==1&&1&&curr_state[i][j][0] != -1)
                {
                    shmptr->droppedPsg[dropped++]=curr_state[i][j][0];
                    droptot++;
                    if(moved[i]>0)
                    {
                        cmddec(n, commands,i);
                    }
                    
                    else if (moved[i]<0)
                    {
                        cmddec2(n,commands,i);
                    }
                    todel(j,curr_state, i , soft_limit);
                    decvals(n,i,tosub,cap);
                    j--;
                }
            }
        }
        long picked_up=0;
        for(long i=0;i<q.size;i++)
        {
            Preq nextReq;
            if(deq(&q, &nextReq)==0)
            {
            int start=nextReq.from;
            int p_id=nextReq.reqId;
            int end=nextReq.to;
            int mindist=k;
            int best=-1;
            if(assigned(p_id, n, curr_state)) {
        continue;
    }
            if(start==end){
            continue;}
            for(long j=0;j<n;j++)
            {
                int going=start-shmptr->floors[j];
                if(absolute(shmptr->floors[j]-start)<=mindist && cap[j]<soft_limit && going>0 &&moved[j]>0)
                {
                    mincheck(mindist,shmptr,start,best,j);
                }
                else if(cap[j]<soft_limit && going<0 &&moved[j]<0&&absolute(shmptr->floors[j]-start)<=mindist)
                {
                    mincheck(mindist,shmptr,start,best,j);
                }
            }
            if(best==-1)
            {
                updateDist(n,cap,shmptr,mindist,best,start);
            }

            if(best==-1)
            {
                enq(&q, nextReq);
                continue;
            }
            update4(n,curr_state,p_id, end, start,best,cap);
            }
        }
        for(long i=0;i<newp;i++)
        {
            int start=shmptr->newPreq[i].from;
            int p_id=shmptr->newPreq[i].reqId;
            int end=shmptr->newPreq[i].to;
            int mindist=k;
            int best=-1;
            if(start==end)
            continue;
            if(assigned(p_id, n, curr_state)) {
        continue;
    }
            for(long j=0;j<n;j++)
            {
                int going=start-shmptr->floors[j];
                if(cap[j]<soft_limit && going>0 && absolute(shmptr->floors[j]-start)<=mindist && moved[j]>0)
                {
                    mincheck(mindist,shmptr,start,best,j);
                }
                else if(absolute(shmptr->floors[j]-start)<=mindist && cap[j]<soft_limit && going<0 && moved[j]<0)
                {
                    mincheck(mindist,shmptr,start,best,j);
                }
            }
            if(best==-1)
            {
                for(long j=0;j<n;j++)
                {
                    if(cap[j]<soft_limit&&absolute(shmptr->floors[j]-start)<=mindist)
                    {
                        mincheck(mindist,shmptr,start,best,j);
                    }
                }
            }
            if(best==-1)
            {
                Preq oldRequest=shmptr->newPreq[i];
                enq(&q,oldRequest);
                continue;
            }
            update(n,curr_state,best,p_id,start,end, cap);
        }
        memset(commands,0,sizeof(commands));
        for(long i=0;i<n;i++)
        {
            for(long j=0;j<soft_limit;j++)
            {
                if(curr_state[i][j][3]==1)
                {
                    if(curr_state[i][j][0]!=-1 && curr_state[i][j][1]>shmptr->floors[i])
                    {
                        cmdu(n,commands,i);
                    }
                    else if(curr_state[i][j][0]!=-1 && curr_state[i][j][1]<shmptr->floors[i])
                    {
                        cmdp(n,commands,i);
                    }
                }
                else
                {
                    if(curr_state[i][j][0]!=-1 && curr_state[i][j][2]>shmptr->floors[i])
                    {
                        cmdu(n,commands,i);
                    }
                    else if(curr_state[i][j][0]!=-1 && curr_state[i][j][2]<shmptr->floors[i])
                    {
                        cmdp(n,commands,i);
                    }
                }
                
            }
        }
        int old[n];
        for(long i=0;i<n;i++)
        {
            old[i]=moved[i];
        }
        for(int i=0;i<n;i++){
            moved[i]=0;
        }
        for(long i=0;i<n;i++)
        {
            shmptr->instr[i]='s';
            moved[i]=0;
            if(cap[i]!=0 && (commands[i][0]>commands[i][1]) && shmptr->floors[i]!=k-1)
            {
                movup(n,shmptr,i,moved);
            }
            else if(cap[i]!=0 && commands[i][0]<commands[i][1] && shmptr->floors[i]!=0)
            {
                movdown(n,shmptr,i,moved);
            }
            else if(cap[i]!=0 && commands[i][0]==commands[i][1])
            {
                if(old[i]==-1 && shmptr->floors[i]!=0)
                {
                    movdown(n,shmptr,i,moved);
                }
                else if(old[i]==1 && shmptr->floors[i]!=k-1)
                {
                    movup(n,shmptr,i,moved);
                }
            }
        }
        int solvers[m];
        for(int i=0;i<n;i++){
            solvers[i]=0;
        }
        for(long i=0;i<n;i++)
        {
            if(moved[i]!=0 && present[i]!=0)
            {
                char* ans= (char*)malloc(21*sizeof(char));

                for(int j=0;j<m;j++)
                {
                    if(solvers[j]==0)
                    {
                        solvers[j]=1;
                        guesser(present[i],solverId[j],i,ans);
                        strcpy(shmptr->authstr[i],ans);
                        solvers[j]=0;
                        break;
                    }
                }
                free(ans);
            }
        }
        for(long i=0;i<n;i++)
        {
            pupdate(n, i, present, tosub);
        }
        for(long i=0;i<n;i++)
        {
            for(long j=0;j<soft_limit;j++)
            {
                if(curr_state[i][j][0] != -1 && curr_state[i][j][2]==shmptr->floors[i] && curr_state[i][j][3] == -1)
                {
                    if(present[i] >= soft_limit)
                break;
                    validchange(shmptr,n,curr_state,picked_up, i, j, picktot, present);
                }
            }
        }
        helperReq request;
        request.mtype=1;
        request.droppedcount=dropped;
        request.picke=picked_up;
        msgsnd(mainID,&request,sizeof(helperReq)-sizeof(long),0);
        if(turns.finished==1){
            break;
        }
    }
    return 0;
}