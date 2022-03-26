# include<iostream>
# include<cstring>
# include<vector>
# include <utility>

# define MAX 23947347
//txt1 23947347
//txt2 264346
using namespace std;

//返回string的数，如“123456”
int getnumber(FILE *fp)
{    
    string str;
    char ch='2';
    int dec;
    while((ch!=' ')&&(ch != 10))
    {
        fread(&ch,1,1,fp);
        str += ch;
        if(feof(fp))
            break;
    }
    str.erase(str.length()-1);
    dec = atoi(str.c_str());
    return dec; 
}

int main()
{
    FILE *sfp,*dfp;
    int v1,v2,w;
    int i,length;
    int input;

    vector<vector<int>> vec(MAX+1, vector<int>());
    sfp = fopen("Data Structure 2021 fall\\lab 3\\distance_info.txt","r");
    dfp = fopen("Data Structure 2021 fall\\lab 3\\zippro","wb");
    while (!feof(sfp))
    {
        v1 = getnumber(sfp);
        v2 = getnumber(sfp);
        w = getnumber(sfp);
        vec[v1].push_back(v2);
        vec[v1].push_back(w);
    }
    fclose(sfp);
    input = MAX;
    fwrite(&input,sizeof(int),1,dfp);//写入总节点数
    for(i=1;i<=MAX;i++)
    {
        length = vec[i].size()/2;
        fwrite(&length,sizeof(int),1,dfp);//写入该节点边数
        for(auto j=vec[i].begin();j<vec[i].end();j++)
        {
            input = *j;
            fwrite(&input,sizeof(int),1,dfp);
        }
        if(i%100000==0)
            printf("%d is OK",i);
    }
    fclose(dfp);
    printf("ALL is OK");
    return 0;
}