#include <termios.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <locale.h>	//本地化设置
#include "libfahw.h"

/******************GSM***************************/

char dest[1024];	//存放中文转unicode
#define MAXLEN 1300
#define EXTRA 5		//4个字节留给字段"data", 1个字节留给"="
#define MAXINPUT MAXLEN+EXTRA+2	//1个字节留给换行符，还有一个留给后面的NULL
#define MAX_MESSAGE_LEN 1300
int res = 0;

struct message_info{
	char cnnu[16];
	char phnu[16];
	char message[MAX_MESSAGE_LEN];
};


struct message_info message;

struct pdu_info {
	char cnswap[32];
	char phswap[32];
};


//串口初始化
void serial_init(int fd)
{
	struct termios options;
	tcgetattr(fd, &options);
	options.c_cflag |= ( CLOCAL | CREAD );
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~CRTSCTS;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CSTOPB;
	options.c_iflag |= IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);
	tcsetattr(fd,TCSANOW,&options);
}

//sim卡初始化
int card_init(int fd)
{
	int nread,nwrite;
	char numberf[MAX_MESSAGE_LEN];
	char reply[MAX_MESSAGE_LEN];
	memset(numberf,0,sizeof(numberf));				//初始化
	strcpy(numberf,"at\r");							//拼接at命令，检测 Module 与串口是否连通，能否接收 AT 命令；OK (与串口通信正常)，无返回（与串口通信未连通）
	nwrite = write(fd,numberf,strlen(numberf));		//发送命令。
	printf("nwrite=%d,%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));					//初始化。
	sleep(1);										//暂停，留有返回时间。		
	nread = read(fd,reply,sizeof(reply));			//将从设备返回的值写入reply中。
	if(strstr(reply,"OK") == NULL)					//strstr(str1,str2) 函数用于判断字符串str2是否是str1的子串。如果是，则该函数返回str2在str1中首次出现的地址；否则，返回NULL。
	{
		printf("ERROR:check serial!\n");
		return 0;
	}
	memset(numberf,0,sizeof(numberf));
	strcpy(numberf,"AT+CSQ\r");						//检查网络信号强度，返回：+CSQ: **,##。其中**应在 10 到 31 之间，数值越大表明信号质量越好，
                                                    //##为误码率,值在 0 到 99 之间。否则应检查天线或 SIM 卡是否正确安装
	nwrite = write(fd,numberf,strlen(numberf));
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("nread=%d,%s\n",nread,reply);	
	memset(reply,0,sizeof(reply));
	memset(numberf,0,sizeof(numberf));
	strcpy(numberf,"AT+CREG\r");					//网络注册及状态查询；返回：OK。(现在存在的问题：即使程序是否可以发短信，打电话。这里的返回依旧是ERROR)
	nwrite = write(fd,numberf,strlen(numberf));
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	if(strstr(reply,"OK") == NULL)
	{
		printf("ERROR: registration network!\n");
//		return 0;
	}
	memset(reply,0,sizeof(reply));
	return 1;
}


//格式转换
void swap(char number[],char swap[])
{
	char ch1[] = "86";
	char tmp[16];
	int i;
	memset(swap,0,32);
	memset(tmp,0,16);
	strcpy(swap,number);
	strcat(swap,"f");
	strcat(ch1,swap);
	strcpy(swap,ch1);
	for(i = 0;i <= strlen(swap) - 1;i += 2){
		tmp[i + 1] = swap[i];
		tmp[i] = swap[i + 1];
	}
	strcpy(swap,tmp);
}


//获取短消息中心号码
int get_messageCenter(int fd,char* message)
{

	int nread,nwrite;
	char numberf[MAX_MESSAGE_LEN];
	char reply[MAX_MESSAGE_LEN];
	memset(numberf,0,sizeof(numberf));					//初始化
	strcpy(numberf,"AT+CSCA?");                			//构造AT命令，自动获取短消息中心号码，成功返回“\rOK\r+86***(11位中心号码）
	strcat(numberf,"\r\"");
	nwrite = write(fd,numberf,strlen(numberf));
	printf("nwrite=%d,%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
    printf("nread=%d,%s\n",nread,reply);
	char* target = "+86";								//将返回内容(reply)中的11位短消息中心号码截取转存到message中。
	char* index = strstr(reply,target);					//reply中是否有target子串，有则返回 位置，没有则返回 NULL；
	char out[12] = " ";
	if(NULL == index)									//没有匹配到+86
	{
		printf("can't get_messageCenter number!\n");
		write_state(0);
		return 0;
	}
	else       											//匹配到
	{
		index+=3;										//开始截取电话号码
		strncpy(out,index,11);
		out[11] = '\0';
		strncpy(message,out,12);						//复制12位到message中。
		write_state(1);
	}
	memset(reply,0,sizeof(reply));						//初始化
	return 1;
}


//从message.txt文件中读取短信内容
int read_message(char message[])
{
	FILE *fp1;
  	int len;
	if((fp1 = fopen("message.txt","r")) == NULL)   //只读打开message.txt
	{
	  	perror("fail to read");
	   	exit (1) ;
	}
	fgets(message,MAX_MESSAGE_LEN,fp1);
	len = strlen(message);
    res = chineseToUnicode(message);		//中文转换成unicode编码
    printf("%d\n",res);
    printf("message@:%s",message);
    memset(message,'\0',MAXLEN);
    strcpy(message,dest);
    printf("message#:%s",message);
    printf("转码：%s\n",message);
    len = (len-1)/3;				//walterzhao这里的len不是很确定
	return len;
}


//构造发送短信命令
int send(int fd,char *cmgf,char *cmgs,char *message)
{
	int nread,nwrite;
	char numberf[MAX_MESSAGE_LEN];
	char reply[MAX_MESSAGE_LEN];
	memset(numberf,0,sizeof(numberf));
	strcpy(numberf,"at\r");
	nwrite = write(fd,numberf,strlen(numberf));
	printf("nwrite=%d,%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	if(strstr(reply,"OK") == 0)
	{
		printf("ERROR:check serial\n");
	}
	else 
		printf("nread=%d,%s\n",nread,reply);
	memset(numberf,0,sizeof(numberf));
	strcpy(numberf,"AT+CMGF=");		//设置短信格式
	strcat(numberf,cmgf);
	strcat(numberf,"\r\"");
	nwrite = write(fd,numberf,strlen(numberf));
	printf("nwrite=%d,%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	if(strstr(reply,"OK") == NULL)
	{
		printf("ERROR:AT+CMGF=\n");
		return 0;
	}
	else 
		printf("nread=%d,%s\n",nread,reply);
	memset(numberf,0,sizeof(numberf));
	strcpy(numberf,"AT+CMGS=");		//命令解释：发送短信
									//命令格式：AT+CMGS=xxx<CR>    注: xxx 代表接收短信的电话号码.
									//命令返回：>                     注:  此时等待短信内容输入.  输入完短信
									//内容后,需要按 Ctrl+Z发送短信.
       								//+CMGS: xxx           注: xxx 代表通道端口代码,它是随机的.
          							//OK       (此返回值表示短信发送成功)
          							//ERROR   (此返回值表示短信发送不成功)
	strcat(numberf,cmgs);
	strcat(numberf,"\r");
	nwrite = write(fd,numberf,strlen(numberf));
	printf("nwrite=%d,%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("nread=%d,%s\n",nread,reply);
	memset(numberf,0,sizeof(numberf));
	strcpy(numberf,message);
	nwrite = write(fd,numberf,strlen(numberf));
	printf("nwrite=%d,%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));
	sleep(1);
	nread = read(fd,reply,sizeof(reply));
	printf("nread=%d,%s\n",nread,reply);
	return 1;
}

//发送中文短信
int send_zh_message(int fd,struct message_info info)
{
	char cmgf[] = "0";		//构造编码信息
	char cmgs[4] = {'\0'};
	char ch2[] = "0891";
	char ch3[] = "1100";
	char ch4[] = "000800";
	char ch5[] = "0d91";
	char ch6[] = "";
	char final[MAX_MESSAGE_LEN];
	struct pdu_info pdu;
	int len = 0;
	memset(final,0,80); 	//对某个数组填充 0为填充内容 80 是填充长度

	len = strlen(info.message)/2;
	printf("len=%d\n",len);
	sprintf(ch6,"%02X",len);
	swap(info.phnu,pdu.phswap);		//编造PDU编码
	swap(info.cnnu,pdu.cnswap);
	strcpy(final,ch2);
	strcat(final,pdu.cnswap);
	strcat(final,ch3);
	strcat(final,ch5);
	strcat(final,pdu.phswap);
	strcat(final,ch4);
	strcat(final,ch6);
	strcat(final,info.message);
	strcat(final,"\x1a");

	printf("len(ch6)=%d\n",strlen(ch6));
	printf("str_ch6:%s\n",ch6);
	len = strlen(ch6) + strlen(ch3)+ strlen(ch4)+ strlen(ch5)+strlen(pdu.phswap)+ strlen(info.message);
	puts(final);
	sprintf(cmgs,"%d",len/2);
	send(fd,cmgf,cmgs,final);
	return 1;
}


//walterzhao
void toUnicode(wchar_t *pWstring, char *pResult)
{
    unsigned int i = 0;
    for(i = 0; i < wcslen(pWstring);++i)
    {
        sprintf(pResult,"%04X",pWstring[i]);
        pResult += 4;
    }

}

//中文转换到unicode
int chineseToUnicode(char* content)
{
    setlocale(LC_ALL,"zh_CN.UTF-8");
    memset(dest,0x00,1024);			//内存空间的初始化
    strcpy(dest,content);
    wchar_t wbuf[256];
    memset(wbuf,L'\0',256);			//将某段空间设置成一个字符（\0）
    size_t rc = mbstowcs(wbuf,dest,strlen(dest));
    if(rc >= 0)
    {
        wbuf[rc] = L'\0';
        memset(dest,0x00,1024);
        toUnicode(wbuf,dest);
        printf("~~~~dest:%s",dest);
    }
    return 1;
}


//将mobile.txt文件中读取到的手机号码转化为构造命令所需的正确格式，无结束符，\r \n
 int get_number(char* number)
 {
	char* target = "1";
	char*index = strstr(number,target);
	char out[12] = " ";
	if(NULL == index)
	{
		return 0;
	}
	else
	{
		strncpy(out,index,11);
		out[11] = '\0';
		strncpy(number,out,12);
	}
}

//写状态文件
void write_state(int state){
    FILE *fp;
    fp = fopen("state.txt","w+");
    fwrite(&state,sizeof(state),5,fp);
	fclose(fp);
    }


//拨打电话
int  dial(int fd,char *dialnum)		
{

   	int nread,nwrite;
	char numberf[MAX_MESSAGE_LEN];
	char reply[MAX_MESSAGE_LEN];

	memset(numberf,0,sizeof(numberf));
	memset(reply,0,sizeof(reply));
	strcpy(numberf,"ATD");						//构造打电话命令，命令格式：ATD（11位号码); 成功返回OK；失败返回ERR
	strcat(numberf,dialnum);
	strcat(numberf,"\r");					
	nwrite = write(fd,numberf,strlen(numberf));
	printf("nwrite=%d,numberf:%s\n",nwrite,numberf);
	memset(reply,0,sizeof(reply));
	sleep(20);
	nread = read(fd,reply,sizeof(reply));
	if(strstr(reply,"SOUNDER")&&strstr(reply,"1") != NULL)				//对方开始振铃
	{	
		printf("connect!\n");
		sleep(10);								//在此默认等待10秒
		if(system("aplay start.wav") < 0)		//系统调用aplay播放音频，失败返回值<0。
		{
			printf("ERROR:error in aplay!\n");
		}	
		sleep(5);
		if(system("aplay /tmp/message.wav") < 0)
		{
			printf("ERROR:error in aplay!\n");
		}
		sleep(10);
		memset(numberf,0,sizeof(numberf));
		strcpy(numberf,"ATH");					//播放录音完成，构造挂机命令，挂断电话。	
		strcat(numberf,"\r");
		nwrite = write(fd,numberf,strlen(numberf));
		printf("nwrite=%d,numberf:%s\n",nwrite,numberf);
		memset(reply,0,sizeof(reply));
		sleep(1);
		nread = read(fd,reply,sizeof(reply));
		if(strstr(reply,"OK") == NULL)
		{
			printf("ERROR:can't hang up the phone!\n");
			return 0;
		}
		memset(reply,0,sizeof(reply));
	}		
	else									//未接通。
	{ 
		printf("can't connect!\n");
		memset(reply,0,sizeof(reply));
		return 0;
	}	
}



/******************END*****************************************/

int main()
{
	printf("%d",sizeof(message)); 
	int fd;														//Serial
	FILE *fp,*fp1;												//fp:mobile.txt、fp1:telphonenum.txt;
	char *p,*q;					
	char dialnum[100] = " ";
	/********************InitGSM*******************************/

	fd = open("/dev/ttyS1",O_RDWR|O_NOCTTY|O_NDELAY);			//打开串口
	if(-1 == fd)
	{
		perror("Can't open oprt!\n");
		return 0;
	}


	serial_init(fd);											//初始化串口。


    /******************InitBoard*****************************/  //Gpio
	int pin ;                                      				//Use GPIO7
	int board;                                                  //Gpio

	pin = GPIO_PIN(7); 											//Use GPIO7。
	int ret = -1;
	int value = -1;


	/******************InitBoard*****************************/
	/* 主板初始化 */
	if ((board = boardInit()) < 0) 
	{
	    printf("Fail to init board\n");
		return -1;
	}
	/*开发板型号*/
	if (board == BOARD_NANOPI_T2)				
		pin = GPIO_PIN(15);
	/* 通知系统需要导出控制的 GPIO 引脚编号，成功返回 0，失败返回负数 */
	if ((ret = exportGPIOPin(pin)) == -1) 			
	{
		printf("exportGPIOPin(%d) failed\n", pin);
		return -1;
	}
	/* 设置Pin7为输出模式 */
	if ((ret = setGPIODirection(pin, GPIO_IN)) == -1) 	//设置Pin7为输出模式
	{
		printf("setGPIODirection(%d) failed\n", pin);
		return -1;
	}
	/* sim卡初始化 */
	if(card_init(fd) == 0)		
	{
		printf("ERROR:card_init error\n!");
		return -1;
	}


	while(1) 
	{
	    value = getGPIOValue(pin);
	        if(value == GPIO_LOW)				//当引脚为低电平
	        {
	        	sleep(1);
	        	if(value == GPIO_LOW)
	        	{
					if((fp = fopen("mobile.txt","r")) == NULL)	//只读打开mobile.txt
	  				{
	  	 				perror("fail to read");
	     				exit (1) ;
	  				}		        		    	
	                if(!feof(fp))	//没有读到文件的末尾EOF；
	            	{           		
	                   	p = fgets(message.phnu,MAXLEN,fp);	//从fp中读出MAXLEN长度的内容放到message.phnu中；
	        	      	if(p == NULL)
	        	     	{	        	      					
	        	      		perror("ERROR:fgets mobile.txt\n");
	        	      		break;
	        	      	}	                   	
	              	   	while(p)	//p不为空则循环；
	              		{
	              			if(get_number(message.phnu) == 0)
	              			{
	              				printf("read mobile number over!\n");
	              				break;
	              			}
	        		    	if(strlen(message.phnu) != 11)//不是11，那就是NULL
	        		     	{
	        		    		printf("Read number error!\n");
	        	        		return 0;
	        		     	}
	        		    	printf("Number:%s\n",message.phnu);
	        		  		if(get_messageCenter(fd,message.cnnu) == 0)	//获取消息中心
	        		  		{
	        		  			printf("ERROR:get_messageCenter!\n");
								break;
	        		  		}
	        		    	printf("messageCenterNumber:%s\n",message.cnnu);
	        	            if(read_message(message.message) == 0)  //读取短信内容
	        	            {
	        		            printf("Read message error!\n");
	        		            return 0;
	        	            }
	        	      		printf("Message:%s\n",message.message);
	        				printf("Send Message start!\n");
							if(send_zh_message(fd,message) == 0)
							{
								printf("ERROR:send message error!");
								return 0;
							}
	        	      		printf("Send OK!\n");
	        	      		sleep(2);
	        	      		p = fgets(message.phnu,MAXLEN,fp);
	        	   			}
	        	    		sleep(5);	        		
	        			}
	        			printf("send message over!\n");
	        			fclose(fp);
						sleep(5);

	        			printf("dial start !\n");
						if((fp1 = fopen("telphonenum.txt","r")) == NULL) 	//只读打开telphonenum.txt
						{
	  						perror("fail to read telphonenum.txt!");
	   						exit (1) ;
						}
	        			if(system("ekho -f message.txt -o /tmp/message.wav") < 0)	//系统调用ekho语音合成软件，失败返回值<0。
	        			{
	        				printf("ERROR:error in ekho!\n");
	        			}
	        	  		if(!feof(fp1))
	            		{	            		
	                    	q = fgets(dialnum,100,fp1);
	              			while(q)
	              			{
	              	  			if(dial(fd,dialnum) == 0)	              	  
	              	  				return 0;	              	  
	        	      			printf("dial OK!\n");
	        	      			sleep(2);
	        	      			q = fgets(dialnum,100,fp1);
	        	  			}
	        	    		sleep(2);	        				       
	        			}  
	        			fclose(fp1);	        		
        		}				        
   	 		}
		setGPIOValue(pin,GPIO_HIGH);		//完成一次短信发送，拨打电话。设置引脚为高电平。
	}
}
