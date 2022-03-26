# include<iostream>
# include<string>

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


/*
char: 0 - 255  1byte8bit 一个字节
int : 四个字节   
*/


void write(FILE *sfp, FILE *dfp)
{
    int number;
    number = getnumber(sfp);
    fwrite(&number,sizeof(int),1,dfp);
}


int main()
{
    FILE *sfp, *dfp;
    int num;
    sfp = fopen("Data Structure 2021 fall\\lab 3\\distance_info.txt","r");
    dfp = fopen("Data Structure 2021 fall\\lab 3\\zip","wb");
    while(!feof(sfp))
        write(sfp,dfp);
    printf("Success\n");
    fclose(sfp);
    fclose(dfp);
}