#include "U8glib.h"
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)

#define setFont_SS u8g.setFont(u8g_font_fub25n)
int period;//游戏进行阶段
//声明变量区--------------------------------
int PlayerX;// 小人位置
int PlayerY;
int Block_X[5];
int Block_Y[5];//一共五个方块
int score;//分数


//----------------------------------------


void start_page()//游戏开始画面
{
  
  
  
  
  }
void setup() {
 Serial.begin(9600);
 period=1;
}

void loop() {
 switch(period)
 case 1：
 start_page();
 break;
 case 2:
 game_on;
 break;
 case 3:
 game_over()
 break;

}
