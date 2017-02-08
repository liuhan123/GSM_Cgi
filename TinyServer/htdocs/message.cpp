
/*
 * CGI程序
 * 获取js页面的POST请求传递的参数
 * 解析参数
 *
 *
 *
 * author:          wlterzaho
 * createDate:      2016-12-5
 * version:         v1.3
 * lastUpdate:	    2016-12-20
 * other:
 *
 */




#include <iostream>
#include<vector>
#include<typeinfo>
#include <iterator>
#include <stdlib.h>
#include<stdio.h>
#include<fstream>
#include <unistd.h>
#include <string>

using namespace std;


int outVector(string fileName,vector<string> tel)
{
    ofstream out(fileName.c_str());
    if(!out.is_open())
    {
        return -1;
    }

    vector<string>::iterator iter = tel.begin();
    for(;tel.end()!=iter;iter++)
    {
        out<<*iter<<endl;
    }
    return 0;
}

int outVectordial(string fileName,vector<string> tel)
{
    ofstream out(fileName.c_str());
    if(!out.is_open())
    {
        return -1;
    }

    vector<string>::iterator iter = tel.begin();
    for(;tel.end()!=iter;iter++)
    {
        out<<*iter<<string(";")<<endl;
    }
    return 0;
}

int outString(string fileName,string message)
{
    ofstream out(fileName.c_str());
    if(!out.is_open())
    {
        return -1;
    }

    out<<message;
}


int main()
{

    char * lenstr = getenv("CONTENT_LENGTH");
    //char * lenstr = "123";
    if(NULL == lenstr)
    {
        exit(0);
    }
    else
    {
        int len = atoi(lenstr);
        char poststr[400] = "";
        fgets(poststr,len+1,stdin);
        string str(poststr);

        vector<string> num;//座机
        vector<string > phones;//手机

        int index = str.find("&");
        string message = str.substr(index+9);

        for(int i=0;i<message.size();i++)
        {
            if('`' == message[i])
            {
                message[i] = '\n';
            }
        }

        string numAndPhones = str.substr(7,index-7);

        int index_at = numAndPhones.find("@");
        string phonesString = numAndPhones.substr(0,index_at);
        string numString = numAndPhones.substr(index_at+1,index);

        int index_space = phonesString.find(" ");
        while(-1 != index_space)
        {
            phones.push_back(phonesString.substr(0,index_space));
            phonesString = phonesString.substr(index_space+1);
            index_space = phonesString.find(" ");
        }

        index_space = numString.find(" ");
        while(-1 != index_space)
        {
            num.push_back(numString.substr(0,index_space));
            numString = numString.substr(index_space+1);
            index_space = numString.find(" ");
        }


        if(-1 == outVectordial("telphonenum.txt",num))
        {
            cout<<"error1"<<endl;
        }

        if(-1 == outVector("telphone.txt",num))
        {
            cout<<"error1"<<endl;
        }

        if(-1 == outVector("mobile.txt",phones))
        {
            cout<<"error3"<<endl;
        }

        if(-1 ==  outString("message.txt",message))
        {
            cout<<"error2"<<endl;
        }

    }

    return 0;
}


