#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "sram.h"   
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "piclib.h"	
#include "string.h"	
#include "math.h"
#include "exti.h"

u32 GAME_STATE;
u8  GAME_MODE;
u16 curindex;	//ͼƬ��ǰ����
u16 totpicnum; 	//ͼƬ�ļ�����
u8 *pname;	//��·�����ļ���
u8 pass;

//ALIENTEK ̽����STM32F407������ ʵ��41
//ͼƬ��ʾ ʵ��-�⺯���汾 
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com
//������������ӿƼ����޹�˾    
//���ߣ�����ԭ�� @ALIENTEK 

//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��		    
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{	  
	u8 res;
	u16 rval=0;
 	DIR tdir;	 					//��ʱĿ¼
	FILINFO tfileinfo;					//��ʱ�ļ���Ϣ	
	u8 *fn;	 			 			   			     
	res=f_opendir(&tdir,(const TCHAR*)path); 		//��Ŀ¼
  	tfileinfo.lfsize=_MAX_LFN*2+1;				//���ļ�����󳤶�
	tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
	if(res==FR_OK&&tfileinfo.lfname!=NULL)
	{
		while(1)//��ѯ�ܵ���Ч�ļ���
		{
	        res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
	        if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
     		fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				rval++;//��Ч�ļ�������1
			}	    
		}  
	} 
	return rval;
}


int main(void)
{
	
	u8 res;
	u8 temp;
 	DIR picdir;	 	//ͼƬĿ¼
	FILINFO picfileinfo;	//�ļ���Ϣ
	
	u8 *fn;   		//���ļ���
	u8 *pname;		//��·�����ļ���
	u16 *picindextbl;	//ͼƬ������ 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  	//��ʼ����ʱ����
	LED_Init();		//��ʼ��LED 
	uart_init(115200);
	KEY_Init();
 	LCD_Init();		//LCD��ʼ�� 
	
	LCD_Display_Dir(1);
	GAME_STATE = 1;
	
	EXTIX_Init();
		
	
	W25QXX_Init();		//��ʼ��W25Q128
	my_mem_init(SRAMIN);	//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);	//��ʼ��CCM�ڴ�� 
	exfuns_init();		//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 	//����SD�� 
 	f_mount(fs[1],"1:",1); 	//����FLASH.
	POINT_COLOR=RED;      
	while(font_init()) 	//����ֿ�
	{	    
		LCD_ShowString(30,50,200,16,16,"Font Error!");
		delay_ms(200);				  
		LCD_Fill(30,50,240,66,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	 					    	 
 	while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
 	{	    
		Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  
	totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
  	while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0		
 	{	    
		Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}
  	picfileinfo.lfsize=_MAX_LFN*2+1;			//���ļ�����󳤶�
	picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
 	pname=mymalloc(SRAMIN,picfileinfo.lfsize);		//Ϊ��·�����ļ��������ڴ�
 	picindextbl=mymalloc(SRAMIN,2*totpicnum);		//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
 	while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//�ڴ�������
 	{	    
		Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
		delay_ms(200);				  
		LCD_Fill(30,170,240,186,WHITE);//�����ʾ	     
		delay_ms(200);				  
	}  	
	//��¼����
	res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
	if(res==FR_OK)
	{
		curindex=0;//��ǰ����Ϊ0
		while(1)//ȫ����ѯһ��
		{
			temp=picdir.index;			//��¼��ǰindex
			res=f_readdir(&picdir,&picfileinfo);       	//��ȡĿ¼�µ�һ���ļ�
			if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�		  
			fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			res=f_typetell(fn);	
			if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�	
			{
				picindextbl[curindex]=temp;//��¼����
				curindex++;
			}	    
		} 
	}   
	Show_Str(30,170,240,16,"��ʼ��ʾ...",16,0); 
	delay_ms(1500);
	piclib_init();						//��ʼ����ͼ	   	   
	curindex=0;						//��0��ʼ��ʾ
   	res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
	
	pass = 0;
	GAME_MODE = 1;
	
	while(1)//�򿪳ɹ�
	{	
		if(pass == 1)
		{
			dir_sdi(&picdir,picindextbl[curindex]);		//�ı䵱ǰĿ¼����	   
			res=f_readdir(&picdir,&picfileinfo);       	//��ȡĿ¼�µ�һ���ļ�
			if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
			fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			strcpy((char*)pname,"0:/PICTURE/");		//����·��(Ŀ¼)
			strcat((char*)pname,(const char*)fn);  		//���ļ������ں���
		//	LCD_Clear(BLACK);
			ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ    
			
			LCD_ShowxNum(0, 100, GAME_STATE, 2, 16, 0);
			LCD_ShowxNum(25, 100, pass, 2, 16, 0);
			LCD_ShowxNum(25, 120, curindex, 2, 16, 0);
			LCD_ShowxNum(0, 120, GAME_MODE, 2, 16, 0);
			
			pass = 0;
		}
		if(GAME_MODE )
		{
			dir_sdi(&picdir,picindextbl[0]);		//�ı䵱ǰĿ¼����	   
			res=f_readdir(&picdir,&picfileinfo);       	//��ȡĿ¼�µ�һ���ļ�
			if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
			fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
			strcpy((char*)pname,"0:/PICTURE/");		//����·��(Ŀ¼)
			strcat((char*)pname,(const char*)fn);  		//���ļ������ں���
		//        LCD_Clear(BLACK);
			ai_load_picfile(pname,202,262,lcddev.width,lcddev.height,1);//��ʾͼƬ  
			
			LCD_ShowxNum(0, 120, GAME_MODE, 2, 16, 0);
			
			GAME_MODE = 0;
		}
								
		LCD_DrawLine(0, 270, 700, 270);
		LCD_DrawLine(0, 300, 700, 300);
		LCD_DrawLine(202, 0, 202, 480);
		LCD_DrawLine(232, 0, 232, 480);
		delay_ms(1000);			    
		
	} 											  
	myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�			    
	myfree(SRAMIN,pname);				//�ͷ��ڴ�			    
	myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�	
	
	
}



//int zhuan(u8 curindex)
//{
//	dir_sdi(&picdir,picindextbl[curindex]);		//�ı䵱ǰĿ¼����	   
//	res=f_readdir(&picdir,&picfileinfo);       	//��ȡĿ¼�µ�һ���ļ�
//	if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
//	fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);			 
//	strcpy((char*)pname,"0:/PICTURE/");		//����·��(Ŀ¼)
//	strcat((char*)pname,(const char*)fn);  		//���ļ������ں���
//	return pname;
//}







