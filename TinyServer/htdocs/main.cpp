/*
 *
 * funtion:
 * 获取服务器本地文件,读取数据
 * 生成js页面
 * 调用cgi程序
 * 调用方式:POST
 *
 *
 * author:              walterzhao
 * createDate:          2016-12-4
 * version:             v1.3
 * lastUpdateDate:      2016-12-20
 * other:
 *
 *
 **/

#include<string>
#include<iostream>
#include<vector>
#include<fstream>
#include<stdlib.h>
#include<string>

using namespace std;

//注意一下两个文件放到cgi程序目录的上一级目录
const string telNumberFileName = "telphone.txt";
const string mobileFileName = "mobile.txt";
//const string mobileFileName = "/Users/walter/Desktop/mobile.txt";
//const string telNumberFileName = "/Users/walter/Desktop/telphone.txt";

//这里是html中的head标签
void htmlHead() {

    cout<<"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">\n";
    cout<<"<html xmlns=\"http://www.w3.org/1999/xhtml\">\n";
    cout<<"<title>发送短信</title>\n";
    cout<<"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/>\n";
    cout<<"<head>\n";
    cout<<"<style type=\"text/css\">\n";
    cout<<".but{\n";
    cout<<"width:100%;\n";
    cout<<"color:red\n";
    cout<<"}\n";
    cout<<"</style>\n";
    cout<<"<script type=\"text/javascript\">\n";
    cout<<"var count = 0;\n";
    cout<<"function addByScript(str) {\n";
    cout<<"var table = document.getElementById(\"tbl2\");\n";
    cout<<"var newRow = table.insertRow(table.rows.length);\n";
    cout<<"newRow.id = \"row\" + count;\n";
    cout<<"var contentCell = newRow.insertCell(-1);\n";
    cout<<"contentCell.innerHTML = '<input type=\"text\" id = \"'+ count +'\" value=\"' + str + '\" />'\n";
    cout<<"contentCell = newRow.insertCell(-1);\n";
    cout<<"var delBtn = document.createElement(\"input\");\n";
    cout<<"delBtn.type = \"button\";\n";
    cout<<"delBtn.className = \"button\";\n";
    cout<<"delBtn.id = \"btnDel\"+count;\n";
    cout<<"delBtn.value = \"删除\";\n";
    cout<<"delBtn.onclick = new Function(\"del(this)\");\n";
    cout<<"contentCell.appendChild(delBtn);\n";
    cout<<"count++;\n";
    cout<<"}\n";
    cout<<"function del(obj) {\n";
    cout<<"var row = obj.parentNode.parentNode;\n";
    cout<<"row.parentNode.removeChild(row);\n";
    cout<<"count--;\n";
    cout<<"}\n";
    cout<<"</script>\n";
    cout<<"<script type=\"text/javascript\">\n";
    cout<<"var mobileCount = 0;\n";
    cout<<"function addMobileByScript(str) {\n";
    cout<<"var table = document.getElementById(\"tbl1\");\n";
    cout<<"var newRow = table.insertRow(table.rows.length);\n";
    cout<<"newRow.id = \"row\" + mobileCount;\n";
    cout<<"var contentCell = newRow.insertCell(-1);\n";
    cout<<"contentCell.innerHTML = '<input type=\"text\" id = \"'+ mobileCount +'\" value=\"' + str + '\" />'\n";
    cout<<"contentCell = newRow.insertCell(-1);\n";
    cout<<"var delBtn = document.createElement(\"input\");\n";
    cout<<"delBtn.type = \"button\";\n";
    cout<<"delBtn.className = \"button\";\n";
    cout<<"delBtn.id = \"btnDel\"+count;\n";
    cout<<"delBtn.value = \"删除\";\n";
    cout<<"delBtn.onclick = new Function(\"del(this)\");\n";
    cout<<"contentCell.appendChild(delBtn);\n";
    cout<<"mobileCount++;\n";
    cout<<"}\n";
    cout<<"function del(obj) {\n";
    cout<<"var row = obj.parentNode.parentNode;\n";
    cout<<"row.parentNode.removeChild(row);\n";
    cout<<"mobileCount--;\n";
    cout<<"}\n";
    cout<<"</script>\n";
    cout<<"<script type=\"text/javascript\">\n";
    cout<<"var phones = '';\n";
    cout<<"function checkNum()\n";
    cout<<"{\n";
    cout<<"var moblieStr ='';\n";
    cout<<"var numStr = '';\n";
    cout<<"var temp = '';\n";
    cout<<"var numTemp ='';\n";
    cout<<"var index_space = 0;\n";
    cout<<"var index__ = 0;\n";
    cout<<"var tailNum = '';//固定电话号码\n";
    cout<<"var frontNum = '';//区号\n";
    cout<<"var moblielist\n";

    cout<<"if(document.getElementById(\"tbl1\").childNodes[1]==null){\n";
    cout<<"moblielist = document.getElementById(\"tbl1\").childNodes[0].childNodes;\n";
    cout<<"}\n";
    cout<<"else{\n";
    cout<<"if(document.getElementById(\"tbl1\").childNodes[0].childNodes.length>0){\n";
    cout<<"moblielist = document.getElementById(\"tbl1\").childNodes[0].childNodes;\n";
    cout<<"}else{\n";
    cout<<"moblielist = document.getElementById(\"tbl1\").childNodes[1].childNodes;\n";
    cout<<"}\n";
    cout<<"}\n";

    cout<<"for(var i =0;i<moblielist.length;i++)\n";
    cout<<"{\n";
    cout<<"temp=moblielist[i].childNodes[0].childNodes[0].value;\n";
    cout<<"moblieStr = moblieStr+temp+' ';\n";
    cout<<"}\n";
    cout<<"var numlist\n";
    cout<<"if(document.getElementById(\"tbl2\").childNodes[1]==null){\n";
    cout<<"numlist = document.getElementById(\"tbl2\").childNodes[0].childNodes;\n";
    cout<<"}else{\n";
    cout<<"if(document.getElementById(\"tbl2\").childNodes[0].childNodes.length>0){\n";
    cout<<"numlist = document.getElementById(\"tbl2\").childNodes[0].childNodes;\n";
    cout<<"}else{\n";
    cout<<"numlist = document.getElementById(\"tbl2\").childNodes[1].childNodes;\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"for(var i =0;i<numlist.length;i++)\n";
    cout<<"{\n";
    cout<<"temp=numlist[i].childNodes[0].childNodes[0].value;\n";
    cout<<"numStr = numStr+temp+' ';\n";
    cout<<"}\n";
    cout<<"//移动电话号码合法性检测\n";
    cout<<"if(moblielist == '')\n";
    cout<<"{\n";
    cout<<"alert('没有移动电话号码，请检查');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"var moblieTemp = moblieStr;\n";
    cout<<"var numTemp = numStr;\n";
    cout<<"var count = 0;\n";
    cout<<"while(moblieTemp.length != parseInt('0'))\n";
    cout<<"{\n";
    cout<<"count++;\n";
    cout<<"var temp = moblieTemp.substring(0,12);\n";
    cout<<"var tempLen = temp.length;\n";
    cout<<"if(12 != tempLen)\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个移动电话号码有误，请检查error0');\n";
    cout<<"return fasle;\n";
    cout<<"}\n";
    cout<<"for(var i=0;i<11;i++)\n";
    cout<<"{\n";
    cout<<"if(!(temp.substring(i,i+1)>='0' && temp.substring(i,i+1)<='9'))\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个移动电话号码有误，请检查error1');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"if(' ' != temp.substring(11,12))\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个移动电话号码有误，请检查error2');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"moblieTemp = moblieTemp.substring(12);\n";
    cout<<"if(moblieTemp.length<parseInt('12') && moblieTemp.length != parseInt('0'))\n";
    cout<<"{\n";
    cout<<"count++;\n";
    cout<<"alert('第'+count+'个移动电话号码有误，请检查error3');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"count = 0;\n";
    cout<<"//固定电话合法性检测\n";
    cout<<"if('' == numStr)\n";
    cout<<"{\n";
    cout<<"alert('没有固定电话号码，请检查error4');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"numTemp = numStr;\n";
    cout<<"while(numTemp.length != parseInt('0'))\n";
    cout<<"{\n";
    cout<<"count++;\n";
    cout<<"index_space = numTemp.indexOf(' ');\n";
    cout<<"temp = numTemp.substring(0,parseInt(index_space));\n";
    cout<<"//区号合法性检测\n";
    cout<<"index__ = temp.indexOf('-');\n";
    cout<<"if(parseInt(index__)>4)\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个固定电话区号有误，请检查error5');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"frontNum = temp.substring(0,parseInt(index__));\n";
    cout<<"for (var i = 0; i < frontNum.length; i++)\n";
    cout<<"{\n";
    cout<<"if(!(frontNum.substring(i,i+1)>='0' && frontNum.substring(i,i+1)<='9'))\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个固定电话区号有误，请检查error6');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"//固定电话号码合法性检查\n";
    cout<<"tailNum = temp.substring(parseInt(index__)+1);\n";
    cout<<"if(tailNum.length<parseInt('5') || tailNum.length>parseInt('8'))\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个固定电话号码有误，请检查error7');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"for(var i=0;i<tailNum.length;i++)\n";
    cout<<"{\n";
    cout<<"if(!(tailNum.substring(i,i+1)>='0' && tailNum.substring(i,i+1)<='9'))\n";
    cout<<"{\n";
    cout<<"alert('第'+count+'个固定电话号码有误，请检查error8');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"numTemp = numTemp.substring(parseInt(index_space)+1);\n";
    cout<<"}\n";
    cout<<"phones = moblieStr+'@'+numStr;\n";
    cout<<"return true;\n";
    cout<<"}\n";
    cout<<"</script>\n";
    cout<<"<script type=\"text/javascript\">\n";
    cout<<"var message = '';\n";
    cout<<"function checkMessage()\n";
    cout<<"{\n";
    cout<<"message = document.getElementById(\"mes\").value;\n";
    cout<<"if('' == message)\n";
    cout<<"{\n";
    cout<<"alert('没有输入短信，请检查error9');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"else if(message.length > parseInt('70'))\n";
    cout<<"{\n";
    cout<<"alert('短信内容超过70字符，请检查error10');\n";
    cout<<"return false;\n";
    cout<<"}\n";
    cout<<"//处理换行输出\n";
    cout<<"var index_n = 0;\n";
    cout<<"while(true)\n";
    cout<<"{\n";
    cout<<"var tail = '';\n";
    cout<<"var prve = '';\n";
    //cout<<"alert('in');\n";
    cout<<"index_n = message.indexOf('\\n');\n";
    cout<<"if(parseInt(-1) == index_n)\n";
    cout<<"{\n";
    cout<<"break;\n";
    cout<<"}\n";
    cout<<"else\n";
    cout<<"{\n";
    cout<<"prve = message.substring(0,index_n);//前面\n";
    cout<<"tail = message.substring(index_n+1);\n";
    cout<<"//自定义的一个转移字符，当出现换行输出的时候，就在换行符处添加一个这个自定义的转移字符，在cgi程序解析的时候再变成换行符\n";
    cout<<"message = prve+'`'+tail;\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"return true;\n";
    cout<<"}\n";
    cout<<"</script>\n";
    cout<<"<script type=\"text/javascript\">\n";
    cout<<"function mysend(state)\n";
    cout<<"{\n";
    cout<<"var flag_phy = false;\n";
    cout<<"if('1' == state)\n";
    cout<<"{\n";
    cout<<"flag_phy = true;\n";
    cout<<"}\n";
    cout<<"else\n";
    cout<<"{\n";
    cout<<"alert('短息发送模块错误，请检查error11')\n";
    cout<<"window.location.reload();";//如果发短信模块异常,则需要刷新页面
    cout<<"}\n";
    cout<<"if(true == checkNum() && true == checkMessage() && flag_phy)\n";
    cout<<"{\n";
    //cout<<"alert('#1');\n";
    cout<<"var xmlhttp;\n";
    cout<<"if (window.XMLHttpRequest)\n";
    cout<<"{// code for IE7+, Firefox, Chrome, Opera, Safari\n";
    cout<<"xmlhttp=new XMLHttpRequest();\n";
    //cout<<"alert('#2');\n";
    cout<<"}\n";
    cout<<"else\n";
    cout<<"{// code for IE6, IE5\n";
    cout<<"xmlhttp=new ActiveXObject(\"Microsoft.XMLHTTP\");\n";
    cout<<"}\n";
    cout<<"xmlhttp.onreadystatechange=function()\n";
    cout<<"{\n";
    //cout<<"alert('success');\n";
    cout<<"if (xmlhttp.readyState==4 && xmlhttp.status==200)\n";
    cout<<"{\n";
    cout<<"alert(\"保存成功\");\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"xmlhttp.open(\"POST\",\"message.cgi\",true);\n";
    cout<<"xmlhttp.setRequestHeader(\"Content-type\",\"application/x-www-form-urlencoded\");\n";
    cout<<"xmlhttp.send(\"phones=\"+phones+\"&message=\"+message);\n";
    cout<<"}\n";
    cout<<"else\n";
    cout<<"{\n";
    cout<<"alert('发送失败');\n";
    cout<<"}\n";
    cout<<"}\n";
    cout<<"</script>\n";
    cout<<"</head>\n";
}


void loadTelFile(vector<string> tel)
{
    string out = "";
    for(int i=0;i<tel.size();i++)
    {
        out = string("<script type=\"text/javascript\"> addByScript('")+tel[i]+string("')</script>\n");
        cout<<out;
    }
}

void loadPhonesFile(vector<string> phones)
{
    string out = "";
    for(int i=0;i<phones.size();i++)
    {
        out = string("<script type=\"text/javascript\"> addMobileByScript('")+phones[i]+string("')</script>\n");
        cout<<out;
    }
}


void htmlBody(vector<string> number,vector<string> phones,string state)
{

    cout<<"<body>\n";
    cout<<"<table>\n";
    cout<<"<tr>\n";
    cout<<"<td align=\"top\" valign=\"top\" id=\"nums\">\n";
    cout<<"<fieldset id = \"left\" style=\"width:400px;\">\n";
    cout<<"<legend>\n";
    cout<<"选择要发送的电话号码：\n";
    cout<<"</legend> <input type=\"button\" class=\"button\" value=\"新增\" onclick=\"addMobileByScript('请输入电话号码')\"/>\n";
    cout<<"<table id=\"tbl1\">\n";
    cout<<"</table>\n";

    //在这里添加载入文件的模块
    loadPhonesFile(phones);


    cout<<"</fieldset>\n";
    cout<<"</td> \n";

    cout<<"<td align=\"top\" valign=\"top\" id=\"nums\">\n";
    cout<<"<fieldset id = \"center\" style=\"width:400px;\">\n";
    cout<<"<legend>\n";
    cout<<"选择要拨打的固定电话号码:\n";
    cout<<"例如:0917-4955231\n";
    cout<<"</legend> <input type=\"button\" class=\"button\" value=\"新增\" onclick=\"addByScript('请输入固定电话号码')\"/>\n";
    cout<<"<table id=\"tbl2\">\n";
    cout<<"</table>\n";

    loadTelFile(number);
    cout<<"</fieldset>\n";
    cout<<"</td>\n";


    cout<<"<td align=\"left\" valign=\"top\">\n";
    cout<<"<fieldset id = \"right\" style=\"width:400px;\">\n";
    cout<<"<legend >\n";
    cout<<"输入要发送的短信：\n";
    cout<<"</legend>\n";
    cout<<"<textarea name=\"message\" id=\"mes\" rows=\"5\" cols=\"65\">\n";
    cout<<"</textarea>\n";
    string sendStr = string("<input type=\"button\" class=\"but\" value=\"保存\" onclick=\"mysend('") + state + string("')\"/>\n");
    cout<<sendStr;
    cout<<"</fieldset>\n";
    cout<<"</td>\n";
    cout<<"</tr>\n";
    cout<<"</table>\n";
    cout<<"</body>\n";
}

//从服务器本地获取电话号码的数据文件
vector<string> getTelNum(string fileName)
{
    ifstream in(fileName.c_str());
    vector<string> telNum;

    if(!in.is_open())
    {
        cout<<fileName +string(" open error")<<endl;
        exit(0);
    }

    string line = "";
    while(getline(in,line))
    {
        telNum.push_back(line);
    }

    return telNum;
}

//获取发送短信装置的状态
//1:状态良好可以发送 0:状态异常,无法发送
string getState() {
    //ifstream in("/Users/walter/Desktop/states.txt");
    ifstream in("states.txt");
    if (!in.is_open()) {
        cout << "states.txt open error" << endl;
        return "0";
    }

    string line = "";
    getline(in, line);


    return line;
}


int main()
{
    vector<string> tel = getTelNum(telNumberFileName);
    vector<string> phones = getTelNum(mobileFileName);

    string state = getState();
    if("" == state)
    {
        state = "0";
    }
    cout<<"Content-Type:text/html\r\n\r\n";
    htmlHead();
    //注意这里要添加新的调用函数,满足添加固定电话的逻辑,关于body的函数需要添加新的参数
    htmlBody(tel,phones,state);
    cout<<"</html>";
    return 0;
}


