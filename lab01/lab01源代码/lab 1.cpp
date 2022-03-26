# include<iostream>
# include<stdlib.h>
# include<time.h>
# include<windows.h>

using namespace std;



/**---------------------------常量定义-----------------------------**/
int TotalMoney = 10000;   //初始时银行现存资金总额
int CloseTime = 600;    //银行歇业时间
int DurTime_min ;  //随机量上下界
int DurTime_max ;  //随机量上下界
int InterTime_min ;//随机量上下界
int InterTime_max ;//随机量上下界
int Money_min ;    //随机量上下界
int Money_max ;    //随机量上下界
# define OpenTime 0



/**----------------------------集合定义-----------------------------**/
typedef struct event
{
    /* 一个交易：一个事件 */
    int OccurTime;  //发生时间
    int DurTime;    //交易时间
    int InterTime;  //与下一位顾客到来的时间间隔
    int Money;      //存取数目
    int EType;        //-1表示取钱，+1表示存钱
    struct event *next;  //指向下一个事件
}event, *EventList;


typedef struct QNode
{
    /* 第一队节点定义，队列元素即event */
    event e;
    struct QNode *next;
}QNode;

typedef struct Queue
{
    /* 第一队 */
    QNode *head;
    QNode *rear;
}Queue;


/**----------------------------全局变量------------------------------**/
int TotalTime = 0;          //所有人的总等待时间 
int MaxPeople = 0;          //总共接待人数
int CurTime = OpenTime;     //当前时间
int mark_money;             //mark的金额，用于快速判断队列2中的业务能否办理
int TotalPeople = 0;        //当前银行内总人数
int numq1 = 0,numq2 = 0;    //当前队列1和队列2中的人数
double t_ave;               //每个人在银行逗留的总时长
QNode tag_node;             //遍历Q2时的标志符，用于判断队列2中的业务能否办理
Queue Q1,Q2;                //队列1、2
EventList EL;               //事件表



/**--------------------------------操作定义------------------------------**/
event randomlize(event e_pre, event e)
{
    Sleep(1000);
    srand(time(0));
    e.DurTime = rand()%(DurTime_max-DurTime_min)+DurTime_min;
    e.InterTime = rand()%(InterTime_max-InterTime_min)+InterTime_min;
    e.Money = rand()%(Money_max-Money_min)+Money_min;
    e.next = nullptr;
    e.EType = abs(e.Money)/e.Money;
    e.OccurTime = e_pre.OccurTime+e_pre.InterTime;
    return e;
};//生成一个除了发生时间（由上一个事件决定）外，其余随机的事件

int randomlize_e0(event &e)
{
    srand(time(0));
    e.DurTime = rand()%(DurTime_max-DurTime_min)+DurTime_min;
    e.InterTime = rand()%(InterTime_max-InterTime_min)+InterTime_min;
    e.Money = rand()%(Money_max-Money_min)+Money_min;
    e.next = nullptr;
    e.EType = abs(e.Money)/e.Money;
    e.OccurTime = 0;
    return 1;
};//生成第一个随机事件e0

QNode GetHeadQ(Queue Q)
{
    /*取队头，返回值为队头的队列元素*/
    QNode p;
    p = *Q.head->next;
    return p;
}

int QueueEmpty(Queue Q)
{
    /*判断是否队空，返回值为0（否）1（是）*/
    if(Q.head == Q.rear)
        return 1;
    else
        return 0;
}

int EnqueueQ(Queue &Q, event e)
{
    /*入队尾，返回值为1（成功）0（失败）*/
    QNode *p;
    p = (QNode* )malloc(sizeof(QNode));
    if(!p) return 0;
    p->e = e;
    p->next = nullptr;
    Q.rear->next = p;
    Q.rear = p;
    return 1;
}



int DeQueueQ(Queue &Q, QNode &e)
{
    /*队头出队，返回值为1（成功）0（失败），队头元素赋值给e，更新队列Q*/
    if(Q.head == Q.rear) return 0;
    QNode *p;
    p = Q.head->next;
    e = *p;
    Q.head->next = p->next;
    if(Q.rear == p) Q.rear = Q.head;
    free(p);
    return 1;
}


int InitQueue(Queue &Q)
{
    /*初始化队列，返回值为1（成功）0（失败）*/
    Q.head = Q.rear = (QNode*)malloc(sizeof(QNode));
    if(!Q.head) return 0;
    Q.head->next = nullptr;
    return 1;
}


int InitEventList(EventList &EL, event e0)
{
    /*初始化事件表,事件表中存放的是下一事件，即暂未发生、紧接着将要发生
    的顾客事件，返回值为0（失败）1（成功）*/
    EL = (event *)malloc(sizeof(event));
    if(!EL) return 0;
    e0.next = nullptr;
    EL->next = &e0;
    return 1;
}

int NextEvent(Queue &Q,EventList &EL,event &e_next)
{
    /*时间推移，下一事件变为当前处理事件，入队列1，并由下一事件生成下下事件，
    更新入事件表，成为下一事件。返回值为0（不在营业时间内）1（在营业时间内）*/
    printf("%s","下一位顾客到来，进入队列1\n");
    event *e_nextnext;
    e_nextnext = (event *)malloc(sizeof(event));
    e_next = *EL->next;           //原事件表中元素即为原下一事件
    EnqueueQ(Q,e_next);           //将原下一事件入Q
    *e_nextnext = randomlize(e_next,*e_nextnext); //根据原下一事件，随机生成原下下事件，即新下一事件
    EL->next = e_nextnext;
    CurTime = e_next.OccurTime;   //时间推移，下一事件变为当前事件，更新CurTime
    e_next = *EL->next;           //原下下事件变为新下一事件
    numq1 ++;                     //队列1人数增加
    MaxPeople ++;                 //总接待人数增加
    TotalPeople ++;               //当前银行内人数增加
    if(CurTime > CloseTime) return 0;
    return 1;
}

int ProcessQ(Queue &Q,EventList &EL, int kind)
{
    /*一队队头出一队，记为当前事件cur_1,process它。同时，在当前事件的交易时间段内看有没有新的事件发生，
    即OccurTime处于当前process事件的OccurTime~OccurTime+DurTime之间的事件，需要让它入队列1，并且随
    机置他的后来事件，入事件表，成为新下一事件*/
    QNode cur_q;                          //当前需要process的事件
    event e_next,e_nextnext;              //原下个事件和原下下个事件
    DeQueueQ(Q,cur_q);                    //当前事件出队，赋值给cur_q
    TotalMoney += cur_q.e.Money;          //处理交易后的totalmoney
    e_next = *EL->next;                   //原下个事件
    int t_end_cur,t_start_cur,t_left;
    int t_wait = 0; 
    t_start_cur = CurTime;                 // t_start_cur 是开始 处理当前事件的 时刻
    t_end_cur = CurTime + cur_q.e.DurTime; // t_end_cur 是 处理完当前process事件后 的 时刻
    t_left = t_end_cur - t_start_cur;      // t_left 是 距离当前事件处理完成 还需要的 时间
    while(e_next.OccurTime < t_end_cur)    // 若原下一事件 发生的时间 在 当前process事件处理完时刻 之前, 
                                           // 则将原下一事件入队列1排队，同时随机生成下下个事件加
                                           // 入事件表，成为新下一事件
    {
        t_left = t_end_cur - e_next.OccurTime; 
        t_wait = e_next.OccurTime - CurTime;       // t_wait 指 当前银行内所有的人都在等待 的时间段
        TotalTime += t_wait * TotalPeople;         // 计算等待时间
        if(NextEvent(Q,EL,e_next) == 0) return 0;  // 推动事件发生，置新下一事件（原下一事件入队列1）
                                                   // 若NextEvent返回值为0，代表银行已经关门，
                                                   // 于是停止处理当前处理的事件
    } //把处理业务期间所有新的事件全部入Q1排队，并update EL中的内容
    TotalTime += t_left * TotalPeople;   // 计算最后大家一起等到当前业务办理结束的时间
    TotalPeople --;                      // 业务办理完，银行内人数减1（事实上队列中人数也需要减1，在主函数中减）
    CurTime = t_end_cur ;                // 更新CurTime
    return 1;
}

int broadcast(int numq1,int numq2)
{
    /*播报当前情况*/
    printf("当前时间为%d;",CurTime);
    printf("当前队一有%d人;",numq1);
    printf("当前队二有%d人;",numq2);
    printf("当前银行内有%d人;",TotalPeople);
    printf("截至目前总接待人数为%d人\n",MaxPeople);
    return 1;
}





int main()
{

    printf("请输入业务办理时长的下、上界:");
    scanf("%d %d",&DurTime_min,&DurTime_max);
    printf("请输入两件业务之间间隔时长的下、上界:");
    scanf("%d %d",&InterTime_min,&InterTime_max);
    printf("请输入办理业务金额的下、上限：（负代表取钱，正代表存钱）");
    scanf("%d %d",&Money_min,&Money_max);

    QNode q1head,q2head;              //队列1、2的第一个元素，即排在队头的客户事件
    event e0;                         //初始化的第一个顾客
    event e_temp;                     //接收NextEvent的返回元素的变量，无实际作用

    printf("%s","开业\n");
    InitQueue(Q1);
    printf("%s","初始化队列1...\n");
    InitQueue(Q2);
    printf("%s","初始化队列2...\n");
    randomlize_e0(e0);                 
    InitEventList(EL,e0);             //初始化事件表
    NextEvent(Q1,EL,e_temp);          //时间推移，当前事件入队列1，开始process

    x: while(CurTime < CloseTime)
    {
        mark_money = TotalMoney;      // 置S，方便后续快速判断是否结束遍历Q2
        if(QueueEmpty(Q1) == 1)
            NextEvent(Q1,EL,e_temp);  // 一队没人了，时间推移，下一事件发生，顾客入队列1
        else
        {
            q1head = *Q1.head->next; 
            if(q1head.e.EType < 0)    //取钱
            {
                if(TotalMoney + q1head.e.Money > 0)  //钱够，可以进行业务
                {   
                    broadcast(numq1,numq2);
                    printf("开始办理队一业务\n");
                    if(ProcessQ(Q1,EL,1) == 0) break;
                    printf("当前业务办理结束\n\n");
                    numq1 --;
                    goto x;
                }
                else //钱不够
                {
                    broadcast(numq1,numq2);
                    printf("开始办理队一业务\n");
                    printf("%s","银行金额不够办理，该顾客出队列1，进入队列2\n");
                    DeQueueQ(Q1,q1head);   //Q1队头元素出队列1
                    numq1 --;
                    EnqueueQ(Q2,q1head.e); //该元素入队列2
                    numq2 ++;
                    goto x;
                }
            }
            else //存钱
            {
                broadcast(numq1,numq2);
                printf("开始办理队一业务\n");
                if(ProcessQ(Q1,EL,1) == 0) break;
                printf("当前业务办理结束\n\n");
                numq1 --;
                y: if(CurTime > CloseTime) break;
                if(QueueEmpty(Q2) == 1)   //队二空，则继续进行队一的业务
                    goto x;
                else  //队二不空，从队头开始判断，能否满足
                {
                    broadcast(numq1,numq2);
                    printf("%s","尝试办理队列2中顾客的业务\n");
                    tag_node = GetHeadQ(Q2); //记录tag，用于判断循环是否已经遍历过队列2中所有元素了
                    q2head = tag_node; 
                    z: if(CurTime > CloseTime) break;   
                    if(q2head.e.Money + TotalMoney > 0) //钱够，可以办理业务
                    {
                        printf("开始办理队二业务\n");
                        if(ProcessQ(Q2,EL,2) == 0) break;
                        numq2 --;
                        printf("%s","当前业务办理结束\n");
                        printf("%s","尝试办理队列2中下一位顾客的业务\n");
                        if(TotalMoney < mark_money) //判断办理完剩下来的钱能否支持继续办理
                        {
                            printf("%s","银行金额不够办理，结束队列2业务办理尝试，转而办理队列1的业务\n");
                            goto x; //不能，转回队一业务
                        }
                        else //能，转去判断队二下一个顾客的需求能否被满足
                        {
                            printf("%s","尝试办理下一位顾客的业务\n");
                            goto y;
                        }

                    }
                    else //钱不够，无法满足，不办理业务
                    {
                        printf("%s","银行金额不够办理该顾客业务，该顾客排入队列2队尾\n");
                        DeQueueQ(Q2,q2head); 
                        EnqueueQ(Q2,q2head.e); //Q2原队头 加到 队尾 
                        if(tag_node.e.OccurTime != Q2.head->next->e.OccurTime) // 没遍历完整个Q2
                            goto z; // 继续判断队二下一个顾客的需求能否被满足
                        else //遍历完了Q2
                        {
                            printf("队列2中的业务暂时都无法办理\n");
                            goto x; //回去办理Q1
                        }
                    }
                }
            }
        }
    }
    printf("%s","银行关门\n");
    printf("总时间为%d\n",TotalTime);
    printf("总人数为%d\n",MaxPeople);
    t_ave = double(TotalTime)/MaxPeople;
    printf("平均时间为%lf",t_ave);
    return 0;
}



