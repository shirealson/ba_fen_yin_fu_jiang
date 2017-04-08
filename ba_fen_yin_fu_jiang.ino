#include "U8glib.h"
#define abs(x) ((x)>0?(x):-(x))
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_SS u8g.setFont(u8g_font_fub25n)

#define mic_PIN A0
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);//声明u8g对象
int period;//游戏进行阶段

const int fps=41;//1000除以24,24帧

const float g=0.5;//重力加速度
const int block_length=32;

//声明变量区--------------------------------
int sound;
int PlayerX;// 小人位置
int PlayerY;
int BlockX[5];
int BlockY[5];//一共五个方块
int BlockIndex1;//代表目前所处的方块下标
int BlockIndex2;//代表目前所处的方块的下一个方块下标
float v;//下坠速度
int score;//分数
bool fall=0;//是否处于下坠状态

//----------------------------------------

static unsigned char game_LOGO[] U8G_PROGMEM = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x00,0x00,0x01,0x02,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x08,0x07,0x00,0xE0,0x00,0x80,0x07,0x1E,0x00,0x1C,0x07,0x00,
0x00,0x00,0x0F,0x00,0x3C,0x07,0x00,0xE0,0x00,0x80,0x07,0x0E,0xE0,0x9C,0xFF,0x03,
0x00,0x1C,0x0F,0x00,0x1C,0x0F,0xE0,0xFF,0xFF,0xC0,0xFF,0xFF,0xE7,0xDD,0xFF,0x03,
0x00,0x1C,0x0E,0x00,0x1E,0x0E,0xE0,0xFF,0xFF,0xC0,0xFF,0xFF,0xC7,0xFF,0xFF,0x01,
0x00,0x1C,0x0E,0x00,0x0E,0x1E,0xE0,0xFF,0xFF,0xE0,0x9D,0x77,0x80,0xFF,0xF1,0x01,
0x00,0x1C,0x0E,0x00,0x0F,0x3C,0x00,0x0E,0x1C,0xF0,0xBC,0xF3,0x80,0xFF,0xFF,0x00,
0x00,0x1C,0x0E,0x80,0x07,0x7C,0x00,0x1E,0x1E,0xF0,0xFA,0xE3,0xF0,0x1F,0x7F,0x00,
0x00,0x1C,0x0E,0xC0,0x07,0xF8,0x00,0x1C,0x0E,0x60,0x9E,0xF1,0xF0,0xFD,0x1F,0x00,
0x00,0x0E,0x0E,0xC0,0x03,0xF0,0xF3,0xFF,0xFF,0x03,0x0F,0x70,0x60,0xDC,0x07,0x00,
0x00,0x0E,0x1C,0xE0,0xFF,0xFF,0xF7,0xFF,0xFF,0x83,0x07,0x70,0xF8,0xFF,0xFF,0x07,
0x00,0x0E,0x1C,0xF0,0xFF,0xFF,0xF3,0xFF,0xFF,0x83,0xFF,0xFF,0xFB,0xFF,0xFF,0x07,
0x00,0x0E,0x1C,0x60,0xFF,0x3F,0x81,0xFF,0x7F,0xC0,0xFB,0xFF,0xC3,0xFF,0xFF,0x00,
0x00,0x0F,0x1C,0x00,0x38,0x38,0x80,0xFF,0x7F,0xE0,0x03,0x70,0xC0,0xF3,0xE3,0x00,
0x00,0x07,0x38,0x00,0x38,0x38,0x80,0xFF,0x7F,0xF0,0xC3,0x70,0xC0,0xF3,0xE7,0x00,
0x80,0x07,0x38,0x00,0x38,0x38,0x80,0x03,0x70,0xF8,0xE3,0x71,0xC0,0x3F,0xFF,0x00,
0x80,0x07,0x78,0x00,0x3C,0x38,0x80,0xFF,0x7F,0xF0,0xE3,0x71,0xC0,0x3F,0xFF,0x00,
0xC0,0x03,0xF0,0x00,0x3C,0x38,0x80,0xFF,0x7F,0x80,0xC3,0x73,0xC0,0x1F,0xFE,0x00,
0xE0,0x03,0xF0,0x00,0x1E,0x3C,0x80,0xFF,0x7F,0x80,0x83,0x73,0xC0,0xFF,0xFF,0x00,
0xF0,0x01,0xE0,0x01,0x0F,0x1C,0x80,0x03,0x70,0x80,0x03,0x70,0xC0,0xFF,0xFF,0x00,
0xF8,0x00,0xE0,0xC3,0x87,0x1F,0x80,0xFF,0x7F,0x80,0x03,0x78,0xC0,0x07,0xF0,0x00,
0x70,0x00,0xC0,0xE3,0x83,0x1F,0x80,0xFF,0x7F,0x80,0x03,0x7E,0xC0,0xFF,0xFF,0x00,
0x60,0x00,0x80,0xE1,0x01,0x0F,0x80,0x03,0x70,0x80,0x03,0x3E,0xC0,0xFF,0xFF,0x00,
0x00,0x00,0x00,0xC0,0x00,0x00,0x80,0x03,0x70,0x80,0x03,0x0C,0xC0,0x01,0xE0,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x80,0xC0,0x01,0x70,0x00,0x00,0x01,0x00,0x04,0x87,0x81,0x8C,0x03,0x60,0x00,0x00,
0x80,0xC3,0x0D,0x70,0x40,0xFE,0xC3,0xFF,0x0F,0xC7,0x81,0x9F,0x03,0xE0,0x03,0x00,
0x80,0xFF,0x1F,0x30,0xC0,0xFF,0x83,0xFF,0x0F,0xC3,0x80,0x9B,0x09,0x68,0x07,0x00,
0x00,0xCB,0x06,0x30,0xC8,0x66,0x03,0x8C,0x01,0xCB,0x06,0xFF,0xDF,0x7F,0x06,0x00,
0x00,0xF8,0xCF,0xFF,0xDF,0x76,0x03,0x8C,0xC1,0x7F,0xCE,0xFE,0xCC,0xEC,0x1C,0x00,
0xE0,0xC3,0xDC,0xFF,0xDF,0x36,0x03,0x8C,0x89,0xFF,0xDF,0xCB,0xDF,0xFC,0x1F,0x00,
0xC0,0xFF,0x1F,0x70,0xC0,0xF6,0xC3,0xFF,0x9F,0xFD,0xDF,0x7B,0xCF,0xFD,0x0C,0x00,
0x00,0xF3,0x0F,0x70,0xC0,0x7E,0xC3,0xFF,0x9F,0x2D,0x0C,0x7F,0x87,0xC7,0x0E,0x00,
0x00,0xF3,0x07,0xF8,0xC0,0x3F,0x03,0x8C,0x81,0xED,0x0F,0x6D,0x1F,0xC7,0x07,0x00,
0x00,0xFB,0x07,0xD8,0xC0,0x3F,0x03,0x8C,0xC1,0x6F,0xCC,0xED,0x1F,0xCF,0x03,0x00,
0x00,0x3F,0x06,0xD8,0xC1,0x36,0x03,0x8E,0x81,0x67,0xCC,0x2D,0x02,0x8F,0x1B,0x00,
0x00,0xF7,0x07,0x9C,0xC3,0x30,0x0B,0x86,0x01,0x7F,0x8C,0x2F,0x82,0xDF,0x1B,0x00,
0x00,0x37,0x06,0x0E,0x0F,0x30,0x1B,0x87,0x01,0x7F,0x8C,0x37,0xC2,0xF9,0x1F,0x00,
0x00,0xB3,0x87,0x07,0x1E,0x3C,0x9F,0x83,0x81,0xF9,0x8F,0xBF,0xC3,0x38,0x1F,0x00,
0x00,0x30,0xC7,0x01,0x1C,0x38,0xDE,0x81,0xC1,0x60,0x0C,0x9B,0x63,0x1C,0x1E,0x00,
0x00,0x10,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x10,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,//开始画面
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

static unsigned char bafen[] U8G_PROGMEM={
0xC0,0x03,0xE0,0x03,0xF8,0x03,0xB8,0x03,0x88,0x07,0x80,0x0F,0xF0,0x1F,0xF8,0x1F,
0xD8,0x37,0xB8,0x3B,0xD8,0x37,0xF8,0x3F,0xF8,0x3F,0xF8,0x7F,0xF8,0x6F,0xF0,0x3F,//C:\Users\hasee\Desktop\八分音符酱.bmp0
};

static unsigned char restart_note[] U8G_PROGMEM={
0x02,0x01,0x10,0x00,0x20,0x00,0x00,0x40,0x00,0xE0,0x43,0x80,0xF9,0x3F,0x04,0x01,
0xE4,0x0F,0x10,0x00,0x2E,0x00,0x00,0xFF,0x9F,0x1F,0x80,0x70,0x20,0x08,0x04,0x01,
0x04,0x01,0x10,0xF0,0x28,0x00,0x00,0x40,0x00,0x10,0xF0,0x17,0x20,0x08,0x84,0x00,
0xE0,0x0F,0x10,0x90,0x28,0x00,0x00,0xFE,0xCF,0xFF,0x27,0x12,0x20,0x08,0x9F,0x04,
0x00,0xC1,0xFF,0x97,0x24,0x00,0x00,0x00,0x00,0x10,0x40,0x11,0x20,0x08,0x52,0x08,
0xF7,0x1F,0x10,0x90,0x24,0x00,0x00,0xFE,0x0F,0xFF,0xF1,0xF7,0x21,0x08,0xF2,0x1F,
0x04,0x00,0x28,0x90,0x2C,0xFC,0x7F,0x42,0x08,0x11,0x01,0x90,0xFC,0x7F,0x12,0x10,
0xE4,0x0F,0x28,0x90,0x24,0x00,0x00,0x42,0x08,0xFF,0x81,0x90,0x20,0x08,0x11,0x00,
0x24,0x08,0x28,0x90,0x26,0x00,0x00,0xFE,0x0F,0x11,0xF1,0x97,0x20,0x08,0xD2,0x0F,
0xE4,0x0F,0x44,0xF0,0x25,0x00,0x00,0x02,0x08,0xFF,0x81,0x90,0x20,0x08,0x4C,0x08,
0x34,0x08,0x44,0x90,0x24,0x00,0x00,0x02,0x00,0x10,0xA0,0x92,0x10,0x08,0x48,0x08,
0xEC,0x0F,0x82,0x00,0x24,0x01,0x00,0x02,0x80,0xFF,0x93,0x94,0x10,0x08,0x54,0x08,
0x24,0x08,0x01,0x01,0x24,0x01,0x00,0x02,0x00,0x10,0x80,0x88,0x08,0x08,0xD2,0x0F,
0x20,0xCE,0x00,0x06,0xC7,0x01,0x00,0x01,0xC0,0xFF,0xE7,0x84,0x04,0x08,0x41,0x08,//请大吼一声重新开始
};



void start_page()//游戏开始画面
{
  sound=analogRead(mic_PIN);
  Serial.print("Sound:");
  Serial.println(sound);
  if(sound>600)
  {
     period=2;//开始游戏
    for(int i=0;i<5;i++)//初始化地图信息
    {
        BlockY[i]=55;
        BlockX[i]=block_length*i;
       
      }
        PlayerY=BlockY[0]-32;//角色位置
        PlayerX=15;
        fall=0;
        v=0;
        BlockIndex1=0;
        BlockIndex2=1;
        score=0;
    
    }
 
  u8g.firstPage();
   do
   {
        u8g.drawXBMP(0,0,128,64,game_LOGO);
   } while (u8g.nextPage());   
  //delay(fps);
  }

  



void dixing()//生成地形函数
{
  
 for(int i=0;i<=4;++i)
  if(BlockX[i]<=0-block_length-1)
  {
    BlockX[i]=127;
  BlockY[i]=63-4*random(1,min(BlockY[(i+4)%5]+2,5));//高度范围1~5，相比上一个矩阵最多高2

  }
}
  void game_on_draw()//该函数用来绘图
  {
    for(int i=0;i<5;i++)
    {
      u8g.drawBox(BlockX[i],BlockY[i],block_length,5);//长32，高5的方块
      
    }
    
    u8g.drawXBMP(PlayerX,PlayerY,16,16,bafen);
   setFont_S;//设置字体
   u8g.setPrintPos(60,10);
   u8g.print("Score:");
   u8g.print(score); 
    
    }

    void game_on_move()//该函数用来控制方块的移动及判定
    {
      sound=analogRead(mic_PIN);//读麦克风声音
      int PlayerFootY=PlayerY+16;//假设人物高16像素,这是脚的位置
      for(int i=0;i<5;i++)
      BlockX[i]-=2;//每帧左移2像素
      dixing();//生成地形
      
        if((PlayerX<BlockX[BlockIndex1]+block_length&&abs(BlockY[BlockIndex1]-PlayerFootY)<3)||(PlayerX+16>BlockX[BlockIndex2] && abs(BlockY[BlockIndex2]-PlayerFootY)<3))
        //判定核心语句 当人物的最左边在方块范围内且处于方块上时不会下坠
         {
           fall=0;
            v=0;//重置速度
            
          }
        else
        {
        
           fall=1;
          
        }

    if(sound>500&& fall==0)
        {
          v=-map(sound,500,1000,0,8);//吼叫产生向上速度
          fall=1;//设状态为下坠
          
          }

                  
       if(fall==1)
       {
         PlayerY=PlayerY+v;//下坠
         v=v+g;
        }

        if(PlayerY>64)
        {
          period=3;//死了
          
          }
             Serial.print("BlockIndex1:");Serial.println(BlockIndex1);
             Serial.print("BlockIndex2:");Serial.println(BlockIndex2);
        if(PlayerX>=BlockX[BlockIndex2])//控制脚下方块的下标增加
        {
           if(BlockIndex2==4)
          {
            BlockIndex2=0;
            BlockIndex1=4;
            
            }
          
          else
          {
             BlockIndex1=BlockIndex2;
             BlockIndex2++;
      
            
            }
          }
         
         
       
          score++;//加一分
         
       
        
        
      }

  void game_on()
{
  
  
  game_on_move();
  u8g.firstPage();
   do
   {
       game_on_draw();
   } while (u8g.nextPage());   
  
  
  }

void game_over()
{
   sound=analogRead(mic_PIN);//读麦克风
  
  if(sound>600)
  {
     period=1;//回到主页
    delay(500);
  }
  u8g.firstPage();
   do
   {
        u8g.drawXBMP(1,32,128,14,restart_note);//显示失败提示
        setFont_L;
        u8g.setPrintPos(20,20);
        u8g.print("Your Score:");
        u8g.print(score);
   } while (u8g.nextPage());   


}

void setup() {
 Serial.begin(9600);
 period=1;
}


void loop() {
 switch (period)
 {
 case 1:
 start_page();
 break;
 case 2:
 game_on();
 break;
 case 3:
 game_over();
 break;
 }
}
