/*  MWget - A Multi download for all POSIX systems.
 *  Homepage: http://mwget.sf.net
 *  Copyright (C) 2005- rgwan,xiaosuo
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <iostream>
#include <stdio.h>
#include "i18n.h"
#include <stdlib.h> //getenv要用。
#include <string.h> //strcat
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> //stat
#include <limits.h>
#define FAIL_RECORD_SAVE_FILE "/.mwget-history" //文件名，在~下，要加/
#define LINE 1024
#define echo(String) printf("%s\n",String)
#define MAX_ITEM 25
char fail_record_url[MAX_ITEM+3][LINE];//URL
//char **fail_record_file;//文件位置
char fail_record_file[MAX_ITEM+3][LINE];
//最大25组。防备溢出
char isdata=0;
unsigned char izdata=0;//行数
char *rfn;
//Y长度最大1024个，X长度最大50组。
//遵循FIFO
 char *CR="\r";
 char *LF="\n";
char fail_record_ignore=0;//跳过
char fail_record_allno=0;//取消

//Sorry啊，水平……只会C.和PHP.就不用C++写法了……
//这里不是51单片机，就放开了写吧，反正内存很大。
//把生成临时文件的代码抄过来
//想法是开始将URL和本地的路径保存到文件里
//然后下完了注销
//至于当时下载的环境嘛……就不去复原了。
//函数: str_replace
//功能: 将字符串s中所有f的内容替换成r的内容
//返回: 新字符串的地址(使用free释放), 失败返回0。
//Usage:后字符串=str_replace(原字符串, 需要替换的内容, 替换的内容);
//现在我发现程序写的越来越乱了
static char* substr(const char*str, unsigned start, unsigned end)
{
  unsigned n = end - start;
  static char stbuf[256];
  strncpy(stbuf, str + start, n);
  stbuf[n] = 0;
  return stbuf;
}


char   *replace(char   *source,   char   *sub,   char   *rep)  
{  
  char   *result;  
  /*pc1   是复制到结果result的扫描指针*/
  /*pc2   是扫描 source 的辅助指针*/
  /*pc3   寻找子串时,为检查变化中的source是否与子串相等,是指向sub的扫描指针 */
  /*找到匹配后,为了复制到结果串,是指向rep的扫描指针*/
  char   *pc1,   *pc2,   *pc3;         
  int   isource,   isub,   irep;  
  isub   =  strlen(sub);     /*对比字符串的长度*/
  irep   =  strlen(rep);     /*替换字符串的长度*/
  isource=  strlen(source);  /*源字符串的长度*/ 
  if(0 == *sub)    
   return strdup(source);  
  
  /*申请结果串需要的空间*/
  result   = (char *)malloc(( (irep > isub) ? (float)strlen(source) / isub* irep+ 1:isource ) * sizeof(char));
  pc1   =   result; /*为pc1依次复制结果串的每个字节作准备*/
  while(*source   !=   0)  
  {  
   /*为检查source与sub是否相等作准备,为pc2,pc3 赋初值*/
      pc2   =   source;  
      pc3   =   sub;
   /* 出循环的（任一）条件是：  
            *   *pc2  不等于 *pc3  （与子串不相等）  
            *   pc2   到源串结尾  
            *   pc3   到源串结尾  （此时,检查了全部子串,source处与sub相等）  
            *****************************************************/
      while(*pc2   ==   *pc3   &&   *pc3   !=   0   &&   *pc2   !=   0)  
          pc2++,   pc3++;  
   /* 如果找到了子串,进行以下处理工作*/
      if(0   ==   *pc3)  
      {  
          pc3   =   rep; 
    /*将替代串追加到结果串*/
          while(*pc3   !=   0)  
              *pc1++   =   *pc3++;  
          pc2--;  
          source   =   pc2;  
    /*     检查 source与sub相等的循环结束后，  
                * pc2 对应的位置是在 sub 中串结束符处。该是源串中下一个位置。  
                * 将  source 指向其前面一个字符。  
                ***************************************************/
      }  
      else /*如果没找到子串,下面复制source所指的字节到结果串*/ 
          *pc1++ = *source;  
  
      source++; /* 将source向后移一个字符*/ 
  }         
  *pc1   =   0;        
  return   result;

}
int ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr)
{
        int StringLen;
        char caNewString[64];
        char* FindPos;
        FindPos =(char *)strstr(sSrc, sMatchStr);
        if( (!FindPos) || (!sMatchStr) )
                return -1;

        while( FindPos )
        {
                memset(caNewString, 0, sizeof(caNewString));
                StringLen = FindPos - sSrc;
                strncpy(caNewString, sSrc, StringLen);
                strcat(caNewString, sReplaceStr);
                strcat(caNewString, FindPos + strlen(sMatchStr));
                strcpy(sSrc, caNewString);

                FindPos =(char *)strstr(sSrc, sMatchStr);
        }
        free(FindPos);
        return 0;
}
void get_History_File_Name()
{
rfn = NULL;//先清0再说。
rfn = strcat(getenv("HOME"),FAIL_RECORD_SAVE_FILE);
}
char *ReadData(FILE *fp, char *buf)
{
//char *temp;
char *i;
 i = fgets(buf, LINE, fp);//读取一行到buf
//buf=str_replace(buf,"\r","");//替换掉回车换行。
//buf=str_replace(buf,"\n","");

return i;
}

void add_line(char *buf)//存入数组函数
{//ReplaceStr(char* sSrc, char* sMatchStr, char* sReplaceStr)
char *tempbuff;
tempbuff=buf;
//echo("5SB");
//tempbuff=replace(tempbuff,CR,NULL);
//tempbuff=replace(tempbuff,LF,NULL);
ReplaceStr(tempbuff,CR,"");
//echo("7SB");
ReplaceStr(tempbuff,LF,"");
//echo("6SB");
if (isdata==0)
{
isdata=1;
//strcpy(fail_record_url[izdata],buf);
sprintf(fail_record_url[izdata],"%s",tempbuff);
//printf("DEBUG\n");
printf("fail_record_url[%i]=%s\n",izdata,fail_record_url[izdata]);//fail_record_url[izdata]);

}
else
{
isdata=0;
//strcpy(fail_record_file[izdata],buf);//=buf;//这里的操作你自己定义
sprintf(fail_record_file[izdata],"%s",tempbuff);
//printf("DEBUG1\n");
printf("fail_record_file[%i]=%s\n",izdata,fail_record_file[izdata]);//fail_record_file[izdata]);
izdata++;
}



}
char fail_record_touch(char *filename)//创建空文件
{
FILE *fd;
if ((fd=fopen(filename, "w"))==NULL)
{
//echo("fclose(fp)");
return 0;//没用
}
{
echo("fclose(fp)");
fclose(fd);
return 1;//成功
}
}
char *get_abs_path(char *path)
{
    char currentDir[PATH_MAX];
    char *temp;
    char *temp2;
    char abs;

        #ifdef WIN32 //还要兼容M$
		temp=substr(path,1,2);
		if (strcmp(temp,":")==0)
			{abs=1;}
		else
			{abs=0;}
	#endif
	#ifndef WIN32 //兼容Linux的代码,绝对路径的第一个字符是'/'
		temp=substr(path,0,1);
		if (strcmp(temp,"/")==0)
			{abs=1;}
		else
			{abs=0;}
	#endif
	if(abs==1)
		{//是绝对路径	
			return path;
		}else
		{//不是绝对路径
		    if( getcwd( currentDir, sizeof(currentDir)-1 ) != NULL )
		    {//substr(const char*str, unsigned start, unsigned end)
			if (strcmp(path,".")==0)
				{temp2="";}
				else
				{temp2=path;}
		#ifdef WIN32
			temp= strcat(currentDir,"\\");
		#endif
		#ifndef WIN32
			temp= strcat(currentDir,"/");
		#endif
			temp = strcat(temp,temp2);
			temp = replace(temp,"//","/");
		#ifdef WIN32
			temp = replace(temp,"\\\\","\\");
		#endif
			return temp;
    		    }
    			else
    			{
    			return 0;//返回错误
    			}
		}
}

void fail_record_init()
{
unsigned char i=0;
while (i!=(izdata+1))
{
	sprintf(fail_record_url[i],"%s\n","");
	sprintf(fail_record_file[i],"%s\n","");
}
izdata=0;
}

char fail_record_readrecord()//这个是读文件的函数
{
//izdata=0;//初始化指针。
/*
char *realfilename;
realfilename=strcat(getenv("HOME"),FAIL_RECORD_SAVE_FILE);
fd = fopen(realfilename, "r"); //开文件，只读。
	if(fd == NULL){
		cerr<<_("Can not access the cache file: ")<<realfilename<<_(" ,Ignore.")<<endl;
		return -1;
	}
*/
FILE *fp;
char *buf, *p;
printf("$HOME=%s\n",getenv("HOME"));
printf("$FILENAME=%s\n",rfn);
echo("fclose(fp)A");
fp=fopen(rfn, "r");
echo("fclose(fp)1");
 if (fp==NULL) 
{
//echo("NULL");
  if (fail_record_touch(rfn)!=0)
  {//新建文件成功

//fclose(fp);
	return 1;
  }
}
else
{
//echo("SB");
 buf=(char*)malloc(LINE*sizeof(char));

 while(1) {
//echo ("2SB");
  p=ReadData(fp, buf);//每次调用文件指针fp会自动后移一行
//echo ("3SB");
  if(!p)//文件读取结束则跳出循环
   break;
//echo ("4SB");
  add_line(buf);
 }
echo("fclose(fp)");
fclose(fp);
return 1; 
}
/*
  //printf("%s\n",_("Can not open cache file to read."));
  if (fail_record_touch(realfilename)!=0)
  {//新建文件成功

//fclose(fp);
	return 1;
  }
*/
// else
  //{
//echo("fclose(fp)2");
//fclose(fp);
 // return 0;//返回失败码
 // }
 //}
/*
//printf("%s\n",replace("aabbccdd\n\n","",""));
 buf=(char*)malloc(LINE*sizeof(char));

 while(1) {
  p=ReadData(fp, buf);//每次调用文件指针fp会自动后移一行
  if(!p)//文件读取结束则跳出循环
   break;
  add_line(buf);
 }
echo("fclose(fp)");
fclose(fp);
return 1; 
*/
}
char fail_record_saverecord()//写文件函数
{
unsigned char i=0;
FILE *fv;
char *temp;
//char *rfn;
char j;
j=(izdata+1);
//rfn=getenv("HOME"); //这里我相当奇怪。
//如果写strcat(getenv("HOME"),FAIL_RECORD_SAVE_FILE)
//算鸟。折中下
//用函数给全局变量赋值
//要不然gcc修正后我就麻烦死了
//返回就是$HOME/.mwget-history/.mwget-history
//真的相当奇怪。
//memset(rfn,0,sizeof(rfn));
//memset(rfn,0,sizeof(rfn));
//echo(FAIL_RECORD_SAVE_FILE);
echo(rfn);
fv=fopen(rfn, "w+");
printf("izdata=%i\n",izdata);
//printf("%i",fail_record_touch(rfn));
//fwrite("dummy",strlen("dummy"),1,fv);
if (fv==NULL)
	{
	printf("No\n");
	//fclose(fp);
	return 0;//不成功
	}
	else
	{
	while(i!=j)//循环直到数组结束
		{
		printf("i=%i,j=%i",i,j);
		if (strcmp(fail_record_url[i],"")==0) //如果这个数的值为“”的话
		{//神马都不做
		echo("There");
		}
		else
		{
		echo("Here");
		temp=strcat(fail_record_url[i],LF);//先写URL，后写FILE。和读取顺序对准。后面加个LF来换行
		fwrite(temp,strlen(temp),1,fv);
		temp=strcat(fail_record_file[i],LF);
		fwrite(temp,strlen(temp),1,fv);
		}
	i++;
	}
fclose(fv);
return 1;
}
}
char fail_record_unset_by_url(const char *url)//这个是解注册函数
{
char i=0,j=0;
j=izdata+1;
echo(url);
		while (i!=j)
		{
			if (strcmp(fail_record_url[i],url)==0)
			{
			sprintf(fail_record_url[i],"%s","");
			sprintf(fail_record_file[i],"%s","");
			printf("BUG:%i",i);
			}
			i++;
		}
		if (fail_record_saverecord()==0)//写盘
		{//不成功
		return 0;
		}
		else
		{ 
		printf("BUG\n");
				//if (fail_record_readrecord()==0)
				//{
				//return 0;
				//}
				//else
				//{
				//return 1;
				//}//重载
				
		}
		

}
char fail_record_unset_by_id(unsigned char id)//这个是解注册函数
{
//这个相当简单，删掉ID值指向的数组，然后写盘。
char temp=0;
	if (fail_record_readrecord()==0)
	{
	return 0;
	}
	else
	{
		if (id>=(MAX_ITEM+1))
		{
		return 0;
		}
		else
		{
		sprintf(fail_record_url[id],"%s","");
		sprintf(fail_record_file[id],"%s","");
			if (fail_record_saverecord()==0)//写盘
			{//不成功
			return 0;
			}
			else
			{

				if (fail_record_readrecord()==0)
					{
					return 0;
					}
					else
					{
					return 1;
					}//重载
				
			}
		}
	}

}

void fail_record_registry(char *filename,const char *url)
{
get_History_File_Name();//取History名
char *filetemp;
unsigned char temp;
filetemp=get_abs_path(filename); //标记绝对文件名
printf("FILETEMP=%s,FILEURL=%s\n",filename,url);
printf("OK=%i",fail_record_readrecord()); //读入原来的数据
if (filetemp==0)
{//有问题了。
}
else
{
temp=izdata+1;
if (izdata>=(MAX_ITEM+1))
{//超限
fail_record_unset_by_id(0);//FIFO,把原来的烂帐删掉。
temp=izdata+1;//这下就可以插入了
sprintf(fail_record_file[temp],"%s",filetemp);
sprintf(fail_record_url[temp],"%s",url);
}
else
{//直接插
sprintf(fail_record_file[temp],"%s",filetemp);
sprintf(fail_record_url[temp],"%s",url);
}
}
izdata++;
printf("\nOK=%i\n",fail_record_saverecord());
}
/*
void fail_record_registry_url(char *url)
{
char *filetemp;
unsigned char temp;
filetemp=get_abs_path(filename); //标记绝对文件名
if (filetemp==0)
{//有问题了。
}
else
{
temp=izdata+1;
if (izdata>=(MAX_ITEM+1))
{//超限
fail_record_unset_by_id(0);//FIFO,把原来的烂帐删掉。
temp=izdata+1;//这下就可以插入了
sprintf(fail_record_url[temp],"%s",url);
}
else
{//直接插
sprintf(fail_record_url[temp],"%s",url);
}
}
}
*/

void fail_record_check()//检查是否有没有完成的下载。
{

}

