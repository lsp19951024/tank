#include "kongzhi.h"

void kongzhi()
{
    u8 key;
    while(1)
    {
	//��׽��������
	key=KEY_Scan(0);
	    
	//�ж��Ƿ�Ϊ��ʼ������
	if(key == KEY1_PRES)
	{
	    //��ʼ�����£� ״̬֮����л���GAME_STATEΪ״ֵ̬
	    switch (GAME_STATE)
	    {
		    case 1:  GAME_STATE = 2; break;
		    case 2:  GAME_STATE = 4; break;
		    case 4:  GAME_STATE = 8; break;
		    case 8:  GAME_STATE = 16; break;
		    case 16: GAME_STATE = 1; break;
	    }	   
	}
	else if(key == KEY0_PRES)
	{
	    switch (GAME_STATE)
	    {
		    //GAME_MODEΪ��˫��ģʽ
		    case 1:
		    {
			if(GAME_MODE == 1)
				GAME_MODE == 2;
			else if(GAME_MODE == 2)
				GAME_MODE == 1;
			else
				GAME_MODE == 1;
		    }
		    case 2: break;
		    case 4: break;
		    case 8: break;
		    

		   
	    }
	}
    }
}






