#include <reg52.h>
#include <math.h>
#include <INTRINS.H>
#define  uchar unsigned char
#define  uint unsigned int
#define  xchar unsigned char code


#define 	DELAYMS  80  // 延时最大值   
#define 	P1 		 P1  // 矩阵键盘引脚分配

#define 	ACCOUNTLENGTH 5 //ID & Password 长度
#define   ACCOUNTNUM 2//账户个数
/* lcd12864 引脚分配 */
#define  	DataPort P0	
sbit RS_Port	=	P2^7;
sbit RW_Port	=	P2^6;
sbit E_Port	=   P2^5;

/* LED 引脚分配 */
sbit LED = P2^4;

/* 蜂鸣器引脚分配*/
sbit BUZZER = P2^3;

void delay(uchar uc_dly);
void resetMainPassword(void);
void checkAccount(void); 
void addNewAccount(void);


/* 重复使用 变量全局定义 */
uchar inputMainPassword[5];
uchar i = 0;// 循环变量1
uchar j = 0;// 循环变量2
uchar temp = 0; // 缓存
uchar boolean = 1;// 密码正误标志 1:密码正确 0:密码错误 
uchar count = 4;// 主密码最大可被输错次数
uchar swch = 0;// switch 选择变量1
uchar ctn = 1;// continue 循环继续与否 ？
uchar cfm = 0;// confirm 确认跳出循环
uchar nxt = 0;// next 下一个
uchar x = 0;// switch 选择变量2
uchar loca = 0;// 当前账户位置


struct
{
    uchar id[ACCOUNTLENGTH];
    uchar key[ACCOUNTLENGTH];
}account[ACCOUNTNUM];// 3 accounts



/* 键盘扫描函数 */
uchar ScanKey(void)	    
{
  uchar Tmp,Tmp2,KeyNum;   
  P1=0xf0;					     
  if(P1!=0xf0) 			  
  {							
   delay(250);				   
   if(P1!=0xf0)		      
   {
     Tmp=P1;				      
		 P1 =0x0F;					    
		 _nop_();
		 _nop_();
		 _nop_();
		 _nop_();  
     Tmp2=P1;					    
     Tmp=Tmp+Tmp2;			      
     while(P1!=0x0F) ;		   
		 switch(Tmp&0x0F)			    
     {
			 case 0x0E:KeyNum=0;break;
			 case 0x0D:KeyNum=4;break;
			 case 0x0B:KeyNum=8;break;
			 case 0x07:KeyNum=12;break;
     }
		 switch(Tmp&0xf0)			  
     {
			 case 0xE0:KeyNum+=1;break;
			 case 0xD0:KeyNum+=2;break;
			 case 0xB0:KeyNum+=3;break;
			 case 0x70:KeyNum+=4;break;

		 } 

		 return KeyNum;			   
    }
  }
  return 0; 	
} 



/* 字符串打印区域 */
xchar Welcome[]=
{
	'P','A','S','S','W','O','R','D',' ','S','T','O','R','A','G','E',
	'|',' ',' ','W','E','L','C','O','M','E',' ','!',' ',' ',' ','|',
	'|','-','-','-','-','-','-','-','-','-','-','-','-','-','-','|',
	'|','-','-','-','-','-','-','-','-','-','-','-','-','-','-','|'
};


xchar EnterMainKeys[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'E','N','T','E','R',' ','M','A','I','N',' ','K','E','Y','S',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};


xchar FinishedEnter[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'F','I','N','I','S','H','E','D',' ','E','N','T','E','R',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar KeysWrong[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'K','E','Y','S',' ','W','R','O','N','G',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar ERROR[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ','E','R','R','O','R',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar addID[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ','A','D','D',' ','N','O','.','*',' ','I','D',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar addKey[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ','A','D','D',' ','N','O','.','*',' ','K','E','Y',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar numAccount[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ','T','H','E',' ','N','O','.','*',' ','I','D','&','K','E','Y',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};


xchar LastChance[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'L','A','S','T',' ','C','H','A','N','C','E',' ',' ',' ',' ',' ',
	'W','A','R','N','I','N','G','!','!','!',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar Continue[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'C','O','N','T','I','N','U','E',' ','T','O',' ','A','D','D',' ',
	'P','R','E','S','S',' ','#',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'P','R','E','S','S',' ','*',' ','Q','U','I','T',' ',' ',' ',' '
};

xchar IDchange[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'I','D',' ','C','H','A','N','G','N','G','E',' ','T','O',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar KEYchange[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'K','E','Y',' ','C','H','A','N','G','E',' ','T','O',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar NewMainKey[]=
{
	'I','N','P','U','T',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'M','A','I','N',' ','P','A','S','S','W','O','R','D',':',' ',' ',
	'N','E','W',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};


xchar AccessSuccess[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'A','C','C','E','S','S',' ','S','U','C','C','E','S','S',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar Reenter[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ','R','E','-','E','N','T','E','R',' ',' ',' ',' ',' ',
	' ',' ',' ','P','A','S','S','W','O','R','D',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar StartArea[]=
{
	
	'C','H','O','O','S','E',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'2','C','H','E','C','K',' ','A','C','C','O','U','N','T','S',' ',
	'1','A','D','D',' ','N','E','W',' ','A','C','C','O','U','N','T',
	'3','R','E','S','E','T',' ','M','A','I','N',' ','K','E','Y',' '
};

xchar Direction[]=
{
	'C','H','O','O','S','E',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'2','N','E','X','T',' ',' ','A','C','C','O','U','N','T',' ',' ',
	'1','P','R','E',' ',' ',' ','A','C','C','O','U','N','T',' ',' ',
	'3','M','O','D','I','F','Y',' ','A','C','C','O','U','N','T',' '
};

xchar Area1[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'A','D','D',' ',' ','N','E','W',' ','A','C','C','O','U','N','T',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'S','T','A','R','T',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar Area2[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'C','H','E','C','K',' ','A','C','C','O','U','N','T','S',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'S','T','A','R','T',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar Area3[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'R','E','S','E','T',' ','M','A','I','N',' ','K','E','Y',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'S','T','A','R','T',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};

xchar DataClear[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	'D','A','T','A',' ','I','S',' ','C','L','E','A','R','E','D',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};


uchar Print[]=
{
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',
	' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '
};



/* 短延时函数 */
#pragma disable
void delay(uchar uc_dly)
{
	while (uc_dly--);
}



/* 长延时函数 */
//延时时间为 s 级别
#pragma disable
void delays()
{
	uchar uc_dly,uc_dly1,uc_dly2;

	uc_dly =DELAYMS;
	
	while (uc_dly --)
	{
		for (uc_dly1=0;uc_dly1<50;uc_dly1++)
			for (uc_dly2=0;uc_dly2<50;uc_dly2++);
	};
}



// LCD 写命令
#pragma disable
void wr_cmd(uchar cmd)
{
	E_Port = 0;
	_nop_();
	_nop_();

	RS_Port=0;
	_nop_();
	_nop_();

	RW_Port=0;
	_nop_();
	_nop_();

	E_Port=1;
	_nop_();
	_nop_();

	DataPort=cmd;
	_nop_();
	_nop_();

	E_Port=0;
	_nop_();
	_nop_();

	delay(5);
}

// LCD 写数据
#pragma disable
void wr_dat(uchar dat)
{	
	E_Port = 0;
	_nop_();
	_nop_();

	RS_Port=1;
	_nop_();
	_nop_();
	
	RW_Port=0;
	_nop_();
	_nop_();

	E_Port=1;
	_nop_();
	_nop_();
		
	DataPort=dat;
	_nop_();
	_nop_();

	E_Port=0;
	_nop_();
	_nop_();

	delay(5);
}


/* 初始化 lcd12864 */
#pragma disable
void Init(void)
{
	wr_cmd(0x30); 	//DL=1:8-BIT interface
	wr_cmd(0x30); 	//RE=0:basic instruction
	wr_cmd(0x06); 	//Entire display shift right by 1
	wr_cmd(0x08); 	//Display OFF,Cursor OFF,Cursor position blink OFF

	wr_cmd(0x34); 	//DL=1:8-BIT interface
	wr_cmd(0x34); 	//RE=0:Extend instruction
	wr_cmd(0x03);
}


/* 在LCD上显示 设定好的字符串 */
#pragma disable
void DisStr(uchar *CorpInf)
{
	uchar uc_GBCnt;

	wr_cmd(0x30); 	//DL=1:8-BIT interface
	wr_cmd(0x30); 	//RE=0:basic instruction
	wr_cmd(0x0C); 	//Display OFF,Cursor OFF,Cursor position blink OFF

	wr_cmd(0x80);
	for (uc_GBCnt=0;uc_GBCnt<16;uc_GBCnt++)
	{
		wr_dat(CorpInf[2 * uc_GBCnt]);
		wr_dat(CorpInf[2 * uc_GBCnt + 1]);
	};

	wr_cmd(0x90);
	for (uc_GBCnt=0;uc_GBCnt<16;uc_GBCnt++)
	{
		wr_dat(CorpInf[2 * uc_GBCnt + 32]);
		wr_dat(CorpInf[2 * uc_GBCnt + 33]);
	};

	delays();
}

#pragma disable
//在 LCD 指定0x98位置开始 显示特定值 
void DisLoca(uint cur,uchar CorpInf)
{

	wr_cmd(0x30); 	//DL=1:8-BIT interface
	wr_cmd(0x30); 	//RE=0:basic instruction
	wr_cmd(0x0C); 	//Display OFF,Cursor OFF,Cursor position blink OFF

	
	wr_cmd(0x98 + cur);
	wr_dat(CorpInf);


	delays();
}

// 在 LCD 右上角显示特定值
void DisCor(uchar CorpInf)
{

	wr_cmd(0x30); 	//DL=1:8-BIT interface
	wr_cmd(0x30); 	//RE=0:basic instruction
	wr_cmd(0x0C); 	//Display OFF,Cursor OFF,Cursor position blink OFF

	
	wr_cmd(0x87);	//右上角位置
	wr_dat(CorpInf);


	delays();
}

// 在 LCD 指定0x90位置 开始显示特定值 
void DisNum(uint cur,uchar CorpInf)
{

	wr_cmd(0x30); 	//DL=1:8-BIT interface
	wr_cmd(0x30); 	//RE=0:basic instruction
	wr_cmd(0x0C); 	//Display OFF,Cursor OFF,Cursor position blink OFF

	
	wr_cmd(0x90 + cur);
	wr_dat(CorpInf);


	delays();
}

#pragma disable
// 清除屏幕内容
void CRAM_OFF()
{
	wr_cmd(0x30); 	//DL=1:8-BIT interface
	wr_cmd(0x30); 	//RE=0:basic instruction
	wr_cmd(0x08);	//Display ON,Cursor OFF,Cursor position blink OFF
	wr_cmd(0x01); 	//CLEAR
	delay(250);
}



/* 数字输入模式 */
uchar numInputMode(uchar x)
{
    switch(x)
    {
    case 0x01:
        return '1';
    case 0x02:
        return '2';
    case 0x03:
        return '3';
    case 0x04:
        return 'A';
    case 0x05:
        return '4';
    case 0x06:
        return '5';
    case 0x07:
        return '6';
    case 0x08:
        return 'B';
    case 0x09:
        return '7';
    case 0x0A:
        return '8';
    case 0x0B:
        return '9';
    case 0x0C:
        return 'C';
    case 0x0D:
        return '*';
    case 0x0E:
        return '0';
    case 0x0F:
        return '#';
		case 0x10:
				return 'D';
    default:
        return 0;
    }
    
}



void Buzzer(void)
{
	BUZZER = 0;// Low is on
	delays();
	BUZZER = 1;
}

void LedBlink(void)
{
	 LED = 0;
   delays();
   LED = 1;
}

/*
 * 按下键盘上的按键
 * if 
 *      如果读取键值成功
 *      返回该键值
 * else
 *      没有读取到键值
 *      持续读取直到读取到键值，跳出while(1)
 */

uchar PressKeyNum()
{
    uchar temp = 0;// 键值缓存变量
    uchar x = ' ';// 返回值缓存变量

    while(1)
    {
        temp = numInputMode(ScanKey());
		// ScanKey为键盘扫描函数，返回16进制键值
        if(temp != 0)
        {
            x = temp;
			LedBlink();// 蓝色LED闪烁一次，表示按键按下成功
            break;
        }
    }

    return x;
}

/* 混合输入模式 
 * 针对每一个包含多值的按键
 * 用一个数组来保存它的所有值
 * 通过按下 A or B 键使数组自增或自减
 * 以此来实现 滚动选值输入
 * 这样4x4矩阵键盘:
	即可以输入数字 0-9，
	又可以输入26个字母，
	还可以输入*、#、.、_、@、:等6种特殊符号
 */
uchar mixInputMode(uchar x)
{
  temp = '0';// 按键数组缓存
	cfm = 0;
	nxt = 0;
    switch(x)
    {
    case 0x01:
        return '1';
    case 0x02:
    {
			xchar key_two[7]={'2','a','b','c','A','B','C'};
			uint i = 0;// key_two[] 中每个键值的位置
			temp = key_two[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
        		DisCor(key_two[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 6)
						i = 0;
					temp = key_two[i];
					DisCor(key_two[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 6;
					temp = key_two[i];
                    DisCor(key_two[i]);
				}
				// 如果 A or B键没有被按下，则默认选择数组首值
				else
				{
					temp = key_two[i];
                    DisCor(key_two[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	   }
    case 0x03:
    {
			xchar key_three[7]={'3','d','e','f','D','E','F'};
			uint i = 0;// key_three[] 中每个键值的位置
			temp = key_three[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_three[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 6)
						i = 0;
					temp = key_three[i];
                    DisCor(key_three[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 6;
		  			temp = key_three[i];
                    DisCor(key_three[i]);
				}
				// 如果 A or B键没有被按下，则默认选择数组首值
				else
				{
					temp = key_three[i];
                    DisCor(key_three[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	  }
    case 0x04:
        return 'A';
    case 0x05:
    {
			xchar key_four[7]={'4','g','h','i','G','H','I'};
			uint i = 0;// key_four[] 中每个键值的位置
			temp = key_four[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_four[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 6)
						i = 0;
					temp = key_four[i];
                    DisCor(key_four[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 6;
					temp = key_four[i];
                    DisCor(key_four[i]);
				}
				// 如果 A or B键没有被按下，则默认选择数组首值
				else
				{
					temp = key_four[i];
                    DisCor(key_four[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x06:
    {
			xchar key_five[7]={'5','j','k','l','J','K','L'};
			uint i = 0;// key_five[] 中每个键值的位置
			temp = key_five[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_five[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 6)
						i = 0;
					temp = key_five[i];
                    DisCor(key_five[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 6;
					temp = key_five[i];
                    DisCor(key_five[i]);
				}
				// 如果 A or B键没有被按下，则默认选择数组首值
				else
				{
					temp = key_five[i];
                    DisCor(key_five[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x07:
		{
			xchar key_six[7]={'6','m','n','o','M','N','O'};
			uint i = 0;//key_six[] 中每个键值的位置
			temp = key_six[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_six[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 6)
						i = 0;
					temp = key_six[i];
                    DisCor(key_six[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 6;
					temp = key_six[i];
                    DisCor(key_six[i]);
				}
				else
				{
					temp = key_six[i];
                    DisCor(key_six[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x08:
        return 'B';
    case 0x09:
    {
			xchar key_seven[9]={'7','p','q','r','s','P','Q','R','S'};
			uint i = 0;// key_seven[] 中每个键值的位置
			temp = key_seven[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
      		    DisCor(key_seven[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 8)
						i = 0;
					temp = key_seven[i];
                    DisCor(key_seven[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 8;
					temp = key_seven[i];
                    DisCor(key_seven[i]);
				}
				else
				{
					temp = key_seven[i];
                    DisCor(key_seven[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x0A:
    {
			xchar key_eight[7]={'8','t','u','v','T','U','V'};
			uint i = 0;// key_eight[] 中每个键值的位置
			temp = key_eight[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_eight[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 6)
						i = 0;
					temp = key_eight[i];
                    DisCor(key_eight[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 6;
					temp = key_eight[i];
                    DisCor(key_eight[i]);
				}
				else
				{
					temp = key_eight[i];
                    DisCor(key_eight[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x0B:
   {
			xchar key_nine[9]={'9','w','x','y','z','W','X','Y','Z'};
			uint i = 0;// key_nine[] 中每个键值的位置
			temp = key_nine[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_nine[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 8)
						i = 0;
					temp = key_nine[i];
                    DisCor(key_nine[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 8;
					temp = key_nine[i];
                    DisCor(key_nine[i]);
				}
				else
				{
					temp = key_nine[i];
                    DisCor(key_nine[i]);
				}
				
				if(nxt == 'C')//按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x0C:
        return 'C';
    case 0x0D:
    {
			xchar key_star[3]={'*','.','@'};
			uint i = 0;// key_star[] 中每个键值的位置
			temp = key_star[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_star[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 2)
						i = 0;
					temp = key_star[i];
                    DisCor(key_star[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 2;
					temp = key_star[i];
                    DisCor(key_star[i]);
				}
				else
				{
					temp = key_star[i];
                    DisCor(key_star[i]);
				}
				
				if(nxt == 'C')// 按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
    case 0x0E:
        return '0';
    case 0x0F:
   {
		 xchar key_well[3]={'#','_',':'};
			uint i = 0;// key_star[] 中每个键值的位置
			temp = key_well[i];// 数组缓存
			
			/* 
			 * 通过按下 A or B 键使数组自增或自减 
			 * 改变 temp 对应的数组值，直至满意 
			 */
			while(1)
			{
                DisCor(key_well[i]);// 变换的值显示在 LCD 右上角，以供选择
				nxt = PressKeyNum();
				if(nxt == 'B')
				{
					i++;
					if(i > 2)
						i = 0;
					temp = key_well[i];
                    DisCor(key_well[i]);
				}
				else if(nxt == 'A')
				{
					i--;
					if(i < 0)
						i = 2;
					temp = key_well[i];
                    DisCor(key_well[i]);
				}
				else
				{
					temp = key_well[i];
                    DisCor(key_well[i]);
				}
				
				if(nxt == 'C')// 按 C 键选中目标
						break;
			}
			return temp;
			break;
	 }
		case 0x10:
				return 'D';
    default:
        return 0;
    }
    
}

/*
 * 按下键盘上的按键
 * if 
 *      如果读取键值成功
 *      返回该键值
 * else
 *      没有读取到键值
 *      持续读取直到读取到键值，跳出while(1)
 */
uchar PressKeyMix(void)
{
    temp = 0;// 键值缓存
    x = ' ';// 返回值缓存

    while(1)
    {
        temp = mixInputMode(ScanKey());
        if(temp != 0)
        {
            x = temp;
						LedBlink();
            break;
        }
    }

    return x;
}

uchar setMainPassword[5] = {'1','2','3','4','5'};// 主密码预设为 12345

void main()
{
	while(1)
	{
		Init();// 初始化 Lcd12864

		// 账户初始值全部为［空格］
		for(i = 0;i < ACCOUNTLENGTH;i++)
		{
			account[0].id[i] = ' ';
			account[0].key[i] = ' ';
		}
		
		for(i = 0;i < ACCOUNTLENGTH;i++)
		{
			account[1].id[i] = ' ';
			account[1].key[i] = ' ';
		}
		
		/* 主密码判断模块 */
		while (1)
		{
			CRAM_OFF();
			temp = 0;//重置 temp 为 0
			
			DisStr(Welcome);
			delays();delays();delays();delays();delays();
			CRAM_OFF();
			DisStr(EnterMainKeys);
						
			/* 通过键盘输入 5 位主密码 */
			for(i = 0;i < 5;i++)
			{				
					inputMainPassword[i] = PressKeyNum();
					DisLoca(i,'*');// 以暗文的形式显示
					delays();
			}
			
			/* 
			 * 测试区域
			 * 同时在 LCD 上打印以下两项：
			 * 1. 用户输入的主密码
			 * 2. 预设的主密码
			 * 用于排查主密码判断模块的逻辑错误
			 */
			
			for(j = 0;j < 5;j++)
			{
				Print[16+j] = inputMainPassword[j];
			}
			for(j = 0;j < 5;j++)
			{
				Print[32+j] = setMainPassword[j];
			}		
			DisStr(FinishedEnter);
			DisStr(Print);
			delays();
			delays();
			/* ------测试区域结束------ */
			
			count = 4;//重置 count 为 4
			while(count != 0)
			{
				boolean = 1;//重置 boolean 为 1
				
				/* 将 输入的密码 与 预设的主密码 比较 */	
				for (i = 0;i < 5;i++)
				{
					if(inputMainPassword[i] != setMainPassword[i])
						boolean = 0;
				}
				
				if(boolean == 0)// 密码错误
				{
					DisStr(KeysWrong);
					delays();
					CRAM_OFF();
					DisStr(Reenter);
					for(i = 0;i < 5;i++)
					{		
						inputMainPassword[i] = PressKeyNum();
						DisLoca(i,'*');
						delays();
					}
				

					/* 
					 * 测试区域
					 * 同时在 LCD 上打印以下两项：
					 * 1. 用户输入的主密码
					 * 2. 预设的主密码
					 * 用于排查主密码判断模块的逻辑错误
					 */
			
					for(j = 0;j < 5;j++)
					{
						Print[16+j] = inputMainPassword[j];
					}
					for(j = 0;j < 5;j++)
					{
						Print[32+j] = setMainPassword[j];
					}		
					DisStr(FinishedEnter);
					DisStr(Print);
					delays();
					delays();
					/* ------测试区域结束------ */
							
					count --;
					
					/*
					 * 倒数第二次密码输入错误条件下
					 * 判断最后一次密码输入是否正确
					 */
					if(count == 0)
					{
						boolean = 1;
						
						for (i = 0;i < 5;i++)
						{
							if(inputMainPassword[i] != setMainPassword[i])
							{
								boolean = 0;
								break;
							}
						}
						break;
					}
					
					/* 警告仅有最后一次机会 界面 */
					if(count == 1)
					{
						LedBlink();// LED显示提醒
						Buzzer();// 蜂鸣器提醒
						DisStr(LastChance);
						delays();delays();
						CRAM_OFF();
					}
					
				}
				else// 密码正确
				{
					DisStr(AccessSuccess);
					break;
				}
			}
		
		if(boolean == 0)
		{
			while(1)
			{
				DisStr(DataClear);
				
				//密码输入错误次数达最大，清空全部数据
				j = 0;
				i = 0;
				for(i = 0;i < ACCOUNTNUM;i++ )
				{
					for(j = 0;j < ACCOUNTLENGTH;j++)
					{
						account[i].id[j] = ' ';
						account[i].key[j] = ' ';
					}
				}
			}
		}	
		break;
	}	



	CRAM_OFF();
	ctn = 1;//重置 ctn 为 1
	
	/* ------------进入用户界面------------ */
		while(ctn)
		{		
			DisStr(StartArea);
			delays();delays();
			swch = PressKeyNum();
		
			switch(swch)
			{
				case '1':// 添加新账户
				{
						DisStr(Area1);
						delays();
						CRAM_OFF();
						addNewAccount();
						ctn = 1;//reset ctn;
						delays();
						break;
				}
				case '2':// 浏览账户
				{
						DisStr(Area2);
						delays();
						CRAM_OFF();
						checkAccount();	
						delays();
						break;
				}
				case '3':// 重设主密码
				{	
						DisStr(Area3);
						delays();
						CRAM_OFF();
						resetMainPassword();
						delays();
						break;
				}
				default:
				{
					ctn = 0;
					break;
				}
			}
		}
	}
}

// 重设主密码功能
void resetMainPassword(void)
{
    	/* 核对主密码 */
		DisStr(EnterMainKeys);
		for(i = 0;i < 5;i++)
		{					
				inputMainPassword[i] = PressKeyNum();
				DisLoca(i,'*');// cleartext
				delays();
		}	
		
	/* 比较输入值与预设值 */
    while(1)
    {
				
			boolean = 1;//重置 boolean 1
			for (i = 0;i < 5;i++)
			{
				if(inputMainPassword[i] != setMainPassword[i])
					boolean = 0;
			}

			if(boolean == 0)// 密码错误
			{
				DisStr(KeysWrong);
				delays();
				CRAM_OFF();
				DisStr(Reenter);
				delays();
				
				for(i = 0;i < 5;i++)
				{		
					inputMainPassword[i] = PressKeyNum();
					DisLoca(i,'*');// 暗文显示
					delays();
				}
				
			}
			else// 密码正确
			{
                /* 重设主密码 */
				DisStr(NewMainKey);
		        for(i = 0;i < 5;i++)
		        {		
		          setMainPassword[i] = PressKeyNum();
				  DisLoca(i,setMainPassword[i]);// 明文显示
				  delays();
		        }
				break;
			}            
    }
}

// 浏览账户 + 修改账户功能

void checkAccount() 
{
    nxt = 1; // 是否继续 ？
    i = 0;// 计数器1
    j = 0;// 计数器2
    loca = 0;// 账户位置
    temp = 0;// 键值缓存
	x = 0;// switch 选择变量
	cfm = 0;
	
    while(nxt)
    {
        DisStr(numAccount);
		DisNum(4,0x30 + loca + 1);
		Print[15] = 0x30 + loca;
        for(i = 0;i < ACCOUNTLENGTH;i++)
            {
                Print[16 + i] = account[loca].id[i];
                Print[48 + i] = account[loca].key[i];
            }
        DisStr(Print);
        delays();delays();delays();
						
		//choose: 1.PRE ACCOUNT 2.NEXT ACCOUNT 3.MODIFY ACCOUNT
		DisStr(Direction);
		x = 0;//重置 x
		x = PressKeyNum();
        switch(x)
        {
            case '1':
                    if(loca == 0)
                        {
                            // 当前为第一个账户时，前一个账户不存在
							DisStr(ERROR);
                            break;
                        }
                    else
                        {
                            loca--;
                            break;
                        }
            case '2':
                    if(loca == 14)
                        {
                            // 当前为最后一个账户时，后一个账户不存在
							DisStr(ERROR);
                            break;
                        }
                    else
                        {
                            loca++;
                            break;
                        }
            case '3':
                    DisStr(IDchange);// 提示更改id
                    cfm = 0;
                    j = 0;
					while(j < ACCOUNTLENGTH)
					{
						temp = 0;//重置 temp
						temp = PressKeyNum();

						if(temp == 'D')
						{
							DisStr(FinishedEnter);
							break;//按 D 确认
						}
						else
						{
							account[loca].id[j] = temp;
							DisLoca(j,account[loca].id[j]);
							delays();delays();delays();
							j++;
						}
					}
										 
					DisStr(KEYchange);// 提示更改密码
					j = 0;
                    while(j < 5)
					{
						temp = 0;//重置 temp
						temp = PressKeyNum();

						if(temp == 'D')
						{
							DisStr(FinishedEnter);
							break;//按 D 确认
						}
						else
						{
							account[loca].key[j] = temp;
							DisLoca(j,account[loca].key[j]);
							delays();delays();delays();
							j++;
						}
					}

                    nxt = 0;
                    break;           
            default:
					{
						DisStr(ERROR);
              			nxt = 0;
					}
        }
    }	
}

void addNewAccount(void)
{
    j = 0;// 计数器
    ctn = 1;// continue 是否继续添加新账户 标志
    loca = 0;// 账户位置
    temp = 0;// 键值缓存
    x = 0;// 跳出循环与否 标志
    while(ctn)
    {
		//请输入ID
		DisStr(addID);
		DisNum(4,0x30 + loca + 1);
        while(j < ACCOUNTLENGTH)
       {
            temp = 0;//重置 temp
						temp = PressKeyMix();

            if(temp == 'D')
            {
              DisStr(FinishedEnter);
							break;//按 D 确认
            }
			else
			{
				account[loca].id[j] = temp;
				DisLoca(j,account[loca].id[j]);
				delays();delays();delays();
				j++;
			}
		}
        //请输入密码
        DisStr(addKey);
		DisNum(4,0x30 + loca + 1);                 
        j = 0;//重置 j
        while(j < ACCOUNTLENGTH)
        {
            temp = 0;//重置 temp
						temp = PressKeyMix();

            if(temp == 'D')
            {
              DisStr(FinishedEnter);
			  break;//按 D 确认
            }
			else
			{
				account[loca].key[j] = temp;
				DisLoca(j,account[loca].key[j]);
				delays();delays();delays();
				j++;
			}
         }
			//按 # 继续,按 * 退出
            DisStr(Continue);
            x = 1;
						x = PressKeyNum();
            if(x == '#')
            {
                ctn = 1;
                loca++;
            }
            else
            {
                ctn = 0;
                DisStr(FinishedEnter);//提示输入完成
                delays();delays();delays();
                break;
            }
        }    
}
