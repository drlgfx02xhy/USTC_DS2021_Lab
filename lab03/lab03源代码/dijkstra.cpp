#include<iostream>
#include<cstring>
#include<algorithm>
#include<queue>

using namespace std;

//#define MAXEDGE 58333344 //text1
#define MAXEDGE 733846	// text2
//#define MAXVEX 23947300 //text1
#define MAXVEX 264346 // text2
#define INF 1000000


struct edge
{
	int v,w,next;
}edge[MAXEDGE];

int head[MAXVEX+1];

struct VNode {
	int dist = 0, last_node = 0;//到起点的最短距离距离和最短路径前驱节点的编号
	bool vis = false;//是否被标记，Dijkstra算法要求
}node[MAXVEX+1];

struct Vitality_node {
	int pos;
	int dis;
	struct Vitality_node *next;
};//定义临时结构体变量



void constructds()
{
	FILE *fp;
	int u,v,w;
	int edgeno = 0;
    fp = fopen("Data Structure 2021 fall\\lab 3\\zip2","rb");
    while(!feof(fp))
    {
        fread(&u,sizeof(int),1,fp);
		fread(&v,sizeof(int),1,fp);
		fread(&w,sizeof(int),1,fp);
        edge[edgeno].v = v;
		edge[edgeno].w = w;
		edge[edgeno].next = 0;
		if(head[u] == 0)
		{
			head[u] = edgeno;
		}
		else
		{
			int temp = head[u];
			edge[edgeno].next = temp;
			head[u] = edgeno;
		}//把边插入到对应的head[u]后面
		edgeno ++;
    }
	fclose(fp);
}

Vitality_node* QSort1(Vitality_node *Q) //朴素dijistra
{
	Vitality_node *min,*p;
	min = Q->next; //最短点
	p = Q->next;
	while(p->next!=nullptr);
	{
		if(p->dis < min->dis) min = p;
	}
	return min;
}


bool Dijkstra(int S, int T)
{
	int length = 0;
	//cout << vex_num << "*" << endl;;
	for (int i = 1; i <= MAXVEX; i++) {
		node[i].dist = INF; node[i].vis = false;
		//cout << head[i] << " ";
	}//初始化点信息
	node[S].dist = 0;
	Vitality_node p = { S,0 };
	struct Vitality_node *Q;
	Q = (Vitality_node*)malloc(sizeof(Vitality_node));
	Q->next = nullptr;

	p.next = Q->next;
	Q->next = &p;
	length ++;

	while (Q->next) {
		Vitality_node *min = QSort1(Q);
		Vitality_node t = *min;
		Q->next = Q->next->next; //Pop(Q)
		int u = t.pos;
		if (node[u].vis) continue;
		if (node[u].dist == INF || u == T) break; //优化常数项，评分细则（4）
		node[u].vis = true;//u是目前到达的最短处，从这个点向外松弛
		for (int i = head[u]; i!=0; i = edge[i].next) {//更新最短路径
			int now = edge[i].v;
			//cout << node[now].dist << " ";
			if ((node[now].dist > node[u].dist + edge[i].w) && !node[now].vis) {
				node[now].dist = node[u].dist + edge[i].w;
				node[now].last_node = u;//记录前驱节点
				p = { now, node[now].dist };
				p.next = Q->next;
				Q->next = &p;
				length ++;
			}
		}
	}

	if (node[T].dist == INF) return false;

	printf_s("The shortest path's length from Src to Dst is: %d\n", node[T].dist);

	int t = T;
	printf_s("The shortest path from Dst to Src is:\n");
	while (t != S) {
		printf_s("%d<-", t);
		t = node[t].last_node;
	}
	printf_s("%d\n", t);
	return true;
}

int main()
{
	printf("Input the two number:\n");
	int u,v;
	scanf("%d %d",&u,&v);
	constructds();
	if(Dijkstra(u,v) == false)
		printf_s("No way!\n");
	
}