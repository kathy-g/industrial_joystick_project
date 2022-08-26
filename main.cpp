#include <Arduino.h>
#include "SimpleJ1939.hpp"
#include <mcp_can.h>
#include <String>
#include <iostream>
#define CAN_1000KBPS 14

// VARIABLES
int dir;
byte nPriority;
int autoencoder;
int eventTime = 100;
byte nSrcAddr;
byte nDestAddr;
bool test = true;
bool goOut;
bool goOut_CCM;
byte nData[8];
int autocount;
int nDataLen;
bool RB_is_pressed ;
bool Key_is_pressed;
bool RB_RisingEdge = false;
long lPGN;
int count;
bool RB_Pressed;
static unsigned constexpr MCP_CS = 5; 
int ccmode;
int ccmcount;
int section;
MCP_CAN CAN0(MCP_CS);
SimpleJ1939 J1939(&CAN0);
byte lastmsg_nData[8]={0,0,0,0,0,0,0,0};
union Byte
{
  unsigned char byte;
  struct
  {
    bool bit7 : 1;
    bool bit6 : 1;
    bool bit5 : 1;
    bool bit4 : 1;
    bool bit3 : 1;
    bool bit2 : 1;
    bool bit1 : 1;
    bool bit0 : 1;    
  };
};
union ByteRotary
{
  unsigned char byterotary;
  struct
  {
    bool bit0 : 1;
    bool bit1 : 1;
    bool bit2 : 1;
    bool bit3 : 1;
    bool bit4 : 1;
    bool bit5 : 1;
    bool bit6 : 1;
    bool bit7 : 1;    
  };
};
union ColorByte
{
  unsigned char colorbyte;
  struct
  {
    int bit0 : 1;
    int bit1 : 1;
    int bit2 : 1;
    
  };
};


// SEGMENTS AND KEYS
long segment1_2 = 0xFF78;
long segment3_4 = 0xFF79;
long key1_2 = 0xFF75;
long key3_4 = 0xFF76;
long key5_6 = 0xFF77;
byte priority = 0x06;
byte srcAddr = 0x87;
byte destAddr = 0xFF;
byte length = 8;

// COLORS
byte Yellow[3] = {255, 255, 0};
byte YellowOrange[3] = {255, 173, 66};
byte Orange[3] = {255, 165, 0};
byte OrangeRed[3] = {255, 69, 0};
byte Red[3] = {255, 0, 0};
byte RedPurple[3] = {146, 43, 62};
byte Purple[3] = {47, 25, 52};
byte PurpleBlue[3] = {138, 43, 221};
byte Blue[3] = {0, 0, 255};
byte BlueGreen[3] = {13, 153, 186};
byte Green[3] = {0, 128, 0};
byte GreenYellow[3] = {173, 255, 47};
byte selectedColor[3] = {0,0,0};
byte* Colors[12] = {Yellow, YellowOrange,Orange, OrangeRed, Red, RedPurple, Purple, PurpleBlue, Blue, BlueGreen, Green, GreenYellow } ;


// ON/OFF SECTIONS
byte Section1on[8] = {255, 255, 0,0,0,0,255,255};
byte Section2on[8] = {0,0,0,255, 255, 0,255,255};
byte Section3on[8] = {255, 255, 0,0,0,0,255,255};
byte Section4on[8] = {0,0,0,255, 255, 0,255,255};
byte alloff[8] = {0,0,0,0,0,0,0,0};
byte allon_1_2[8] = {255, 255, 0,255, 255, 0,255,255};
byte allon_3_4[8] = {255, 255, 0,255, 255, 0,255,255};



//************** VOID SETUP*************
void setup()
{
  Serial.begin(115200);
  if(CAN0.begin(MCP_ANY, CAN_250KBPS, MCP_8MHZ) == CAN_OK)
    {
     CAN0.setMode(MCP_NORMAL); 
    }
  else
  {
    Serial.print("CAN Init Failed. Check your SPI connections or CAN.begin intialization .\n\r");
    while(1);
  }
}

//************** DETECTION OF ROTATION *************
void detectRotationOfJoystick()
{
  ByteRotary b;
  b.byterotary = nData[3];


  if (lPGN ==  0xFF64 && b.bit0 ==1)  // rotation?
  {
    if (lPGN ==  0xFF64 && b.bit5==0)  // if clockwise
    {
      count = count+1; 
      ccmcount = ccmcount+1;
      autoencoder = autoencoder+1; 
      dir = 1;  
      if (count==24 )
      {
        count=0;
        ccmcount =0;
        autoencoder =0;
      }
      Serial.println(count);
    }
    if (lPGN ==  0xFF64 && b.bit5 ==1)  // if counterclockwise
    {
      count = count-1;
      ccmcount = ccmcount-1; 
      autoencoder = autoencoder-1;
      dir = 2; 
      if (count==-1)
      {
        count=23;
        ccmcount =23;
        autoencoder = 23;
      }
    Serial.println(count);
    }
 }
}

//************** MANUAL MODE *************
void ManualModeRotation()
{
  detectRotationOfJoystick();
  if(dir==1) //clockwise
  {
   
    if (count == 0) // Initaial Position
    {
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length) ;
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);
    }
    if (count > 0 && count <= 5) //Segment 2
    {
      section = 2;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length) ;
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);    
    }     
    if (count >= 6 && count <= 11) //Segment 4
    {
      section = 4;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length) ;
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &Section3on[0], length) ;           
    }       
    if (count >= 12 && count <= 17) // Segment 3
    {
      section = 3;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length) ; 
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &allon_3_4[0], length) ;          
    } 
    if (count >= 18 && count <= 23) // Segment 1
    {
      section = 1;   
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &allon_3_4[0], length)    ;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &allon_1_2[0], length) ;
    }
   
  }

  if(dir==2) //counter clockwise
  {
  
    if (count == 0 )
    {
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length) ; 
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);  
    }

    if (count > 0 && count <= 5) //Segment 2
    {
      section = 2;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length);
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);
    }  

    if (count >= 6 && count <= 11) //Segment 4
    {
      section = 4;
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length);
    } 
      
    if (count >= 12 && count <= 17) //Segment 3
    {
      section = 3;
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &Section3on[0], length) ;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length) ;     
    } 

    if (count >= 18 && count <= 23)  // Segment 1
    {
      section = 1;
      J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length) ;
      J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &allon_3_4[0], length);
    }
    
  }
}

//************** COLOR SELECTION IN CCM *************
void setColorInCCMode()
{
  if (ccmcount == 0 || ccmcount == 1)
  {
    Serial.println("Yellow");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=Yellow[i];
    }  
  }  
  if (ccmcount == 2 || ccmcount == 3)
  {
    Serial.println("Yellow-Orange");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=YellowOrange[i];
    }
  }  
  if (ccmcount == 4 || ccmcount == 5)
  {
    Serial.println("Orange");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=Orange[i];
    }
  }  
  if (ccmcount == 6 || ccmcount == 7)
  {
    Serial.println("Orange-Red");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=OrangeRed[i];
    }
  }
  if (ccmcount == 8 || ccmcount == 9)
  {
    Serial.println("Red");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=Red[i];
    }
  } 
  if (ccmcount == 10 || ccmcount == 11)
  {
    Serial.println("Red-Purple");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=RedPurple[i];
    }
  } 
  if (ccmcount == 12 || ccmcount == 13)
  {
    Serial.println("Purple");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=Purple[i];
    }
  } 
  if (ccmcount == 14 || ccmcount == 15)
  {
    Serial.println("Purple-Blue");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=PurpleBlue[i];
    }
  } 
  if (ccmcount == 16 || ccmcount == 17)
  {
    Serial.println("Blue");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=Blue[i];
    }
  } 
  if (ccmcount == 18 || ccmcount == 19)
  {
    Serial.println("Blue-Green"); 
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=BlueGreen[i];
    }
  } 
  if (ccmcount == 20 || ccmcount == 21)
  {
    Serial.println("Green");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=Green[i];
    }
  } 
  if (ccmcount == 22 || ccmcount == 23)
  {
    Serial.println("Green-Yellow");
    for (int i = 0; i<3; i++)
    {
      selectedColor[i]=GreenYellow[i];
    }
  }   
}

//************** CHANGE COLOR OF PAYLOAD ************* 
void PayloadColorChange()
{
if(section==1)
{
  setColorInCCMode();
  for (int i = 0; i < 3; i++)
  {
    Section1on[i] = selectedColor[i];
    allon_1_2[i] = selectedColor[i];
  }
  Serial.println("Color for Section 1");
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section1on[0], length) ;
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length) ;
}
if(section==2)
{ 
  setColorInCCMode();
  for (int i = 0; i < 3; i++)
  {
    Section2on[i]=0;
    Section2on[(3 + i)] = selectedColor[i];
    allon_1_2[(3 + i)] = selectedColor[i];
  }
  Serial.println("Color for Section 2");
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &Section2on[0], length) ;
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length) ;
}
if(section==3)
{  
      setColorInCCMode();
  for (int i = 0; i < 3; i++)
  {
    Section4on[i]=0;
    Section4on[(3 + i)] = selectedColor[i];
    allon_3_4[(3 + i)] = selectedColor[i];
  }
  Serial.println("Color for Section 3");
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &Section4on[0], length) ;
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length) ;
}
if(section==4)
{  
  setColorInCCMode();
  for (int i = 0; i < 3; i++)
  {
    
    Section3on[i] = selectedColor[i];
    allon_3_4[i] = selectedColor[i];
  }
  Serial.println("Color for Section 4");
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &Section3on[0], length) ;
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length) ;
}
 
}

//************** RESET COLOR OF PAYLOAD ************* 
void PayloadColorReset()
{
  for (int i = 0; i < 3; i++)
  {
    selectedColor[i]=Yellow[i];
  } 

  for (int i = 0; i < 3; i++)
  {
    Section1on[i] = selectedColor[i];
  }
  for (int i = 0; i < 3; i++)
  {
    Section2on[i] = 0;
    Section2on[(3 + i)] = selectedColor[i];
  }
  for (int i = 0; i < 3; i++)
  {
    Section3on[i] = selectedColor[i];
  }
  for (int i = 0; i < 3; i++)
  {
    Section4on[i] = 0;
    Section4on[(3 + i)] = selectedColor[i]; 
  }
  for (int i = 0; i < 3; i++)
  {
    allon_1_2[i] = selectedColor[i];
    allon_3_4[i] = selectedColor[i];
    allon_1_2[(3 + i)] = selectedColor[i];
    allon_3_4[(3 + i)] = selectedColor[i];
  }
}

//************** RESET BUTTON *************
void ResetMode()
{
  count = 0;
  ccmcount=0;
  autoencoder=0;
  autocount=0;
  Serial.println("Resetted");
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length) ;
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);
  J1939.Transmit(key1_2, priority, srcAddr, destAddr, &alloff[0], length);
  J1939.Transmit(key3_4, priority, srcAddr, destAddr, &alloff[0], length);
  J1939.Transmit(key5_6, priority, srcAddr, destAddr, &alloff[0], length);
  PayloadColorReset();
}

//************** CHECK RESET BUTTON *************
void checkResetButton()

{
    if(lPGN ==  0xFF64 && nData[1] == 0x40)
      {
        goOut=true;
        goOut_CCM=true;
        ResetMode();
        ManualModeRotation();
      }
}

//************** GO INTO COLOR CHANGE **************
void goToColorChange()
{
  ByteRotary d;
  d.byterotary = nData[3] ;
  do
  {
    if (J1939.Receive(&lPGN, &nPriority, &nSrcAddr, &nDestAddr, nData, &nDataLen) ==0)
    {
      d.byterotary = nData[3];
      detectRotationOfJoystick();
    }
    PayloadColorChange();
    Serial.println(ccmcount);
    if(lPGN ==  0xFF64 && nData[1] == 0x40)
    {
      checkResetButton(); 
    }
    if(lPGN ==  0xFF64 && d.bit6 ==1 && RB_is_pressed ==false)  // Key wasnt pressed and is pressed now
    {
     Serial.println(goOut_CCM);
      Serial.println("Out of Color Change Mode");
      goOut_CCM=true;
    }
    else if(lPGN ==  0xFF64 && d.bit6 ==0 )   // Key is not pressed anymore
    {
      RB_is_pressed=false; 
    }
  
  } while(goOut_CCM != true); 
  test = false;
  count=0;
  ccmcount=0;
  autoencoder=0;
} 
   
//************** CHANGE FREQUENCY **************
void ChangeFrequency()
{
  eventTime = 100 + count*30;
}

//************** AUTO MODE BUTTON *************
void autoFlicker()
{
  if (autocount == 0)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] =Yellow[i];
      allon_1_2[3+i]=Yellow[i];
      allon_3_4[i] =Yellow[i];
      allon_3_4[3+i]=Yellow[i];
    }
    Serial.println("Yellow Activated");      
  }
  if (autocount == 1)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] =YellowOrange[i];
      allon_1_2[3+i]=YellowOrange[i];
      allon_3_4[i] =YellowOrange[i];
      allon_3_4[3+i]=YellowOrange[i];
    } 
    Serial.println("Yellow-Orange Activated");
  } 
  if (autocount == 2)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] =Orange[i];
      allon_1_2[3+i]=Orange[i];
      allon_3_4[i] =Orange[i];
      allon_3_4[3+i]=Orange[i];
    }
    Serial.println("Orange Activated"); 
  } 
  if (autocount == 3)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] =OrangeRed[i];
      allon_1_2[3+i]=OrangeRed[i];
      allon_3_4[i] =OrangeRed[i];
      allon_3_4[3+i]=OrangeRed[i];
    }
    Serial.println("Orange-Red Activated");  
  }
  if (autocount == 4)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = Red[i];
      allon_1_2[3 + i] = Red[i];
      allon_3_4[i] = Red[i];
      allon_3_4[3 + i] = Red[i];
    }
    Serial.println("Red Activated");
  }
  if (autocount == 5)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = RedPurple[i];
      allon_1_2[3 + i] = RedPurple[i];
      allon_3_4[i] = RedPurple[i];
      allon_3_4[3 + i] = RedPurple[i];
    }
    Serial.println("Red-Purple Activated");
  }
  if (autocount == 6)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = Purple[i];
      allon_1_2[3 + i] = Purple[i];
      allon_3_4[i] = Purple[i];
      allon_3_4[3 + i] = Purple[i];
    }
    Serial.println("Purple Activated");
  }
  if (autocount == 7)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = PurpleBlue[i];
      allon_1_2[3 + i] = PurpleBlue[i];
      allon_3_4[i] = PurpleBlue[i];
      allon_3_4[3 + i] = PurpleBlue[i];
    }
    Serial.println("Purple-Blue Activated");
  }
  if (autocount == 8)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = Blue[i];
      allon_1_2[3 + i] = Blue[i];
      allon_3_4[i] = Blue[i];
      allon_3_4[3 + i] = Blue[i];
    }
    Serial.println("Blue Activated");
  }
  if (autocount == 9)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = BlueGreen[i];
      allon_1_2[3 + i] = BlueGreen[i];
      allon_3_4[i] = BlueGreen[i];
      allon_3_4[3 + i] = BlueGreen[i];
    }
    Serial.println("Blue-Green Activated");
  }
  if (autocount == 10)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = Green[i];
      allon_1_2[3 + i] = Green[i];
      allon_3_4[i] = Green[i];
      allon_3_4[3 + i] = Green[i];
    }
    Serial.println("Green Activated");
  }
  if (autocount == 11)
  {
    for (int i = 0; i < 3; i++)
    {
      allon_1_2[i] = GreenYellow[i];
      allon_1_2[3 + i] = GreenYellow[i];
      allon_3_4[i] = GreenYellow[i];
      allon_3_4[3 + i] = GreenYellow[i];
    }
    Serial.println("Green-Yellow Activated");
  }
}

//************** GO OUT OF AUTO MODE *************
void goOutOfAutoMode()
{
  count = 0;
  ccmcount=0;
  autoencoder=0;
  for (int i =0; i<3; i++)
  {
    allon_1_2[i] = Section1on[i];
    allon_1_2[3+i] = Section2on[3+i];
    allon_3_4[i] = Section3on[i];
    allon_3_4[3+i] = Section4on[3+i];
  }
  
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &alloff[0], length) ;
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &alloff[0], length);
  J1939.Transmit(key1_2, priority, srcAddr, destAddr, &alloff[0], length);
  J1939.Transmit(key3_4, priority, srcAddr, destAddr, &alloff[0], length);
  J1939.Transmit(key5_6, priority, srcAddr, destAddr, &alloff[0], length);
  test = false;
}

//************** SEND COLORS *************
void sendColorFLicker()
{
  J1939.Transmit(segment1_2, priority, srcAddr, destAddr, &allon_1_2[0], length);
  J1939.Transmit(segment3_4, priority, srcAddr, destAddr, &allon_3_4[0], length);
  J1939.Transmit(key1_2, priority, srcAddr, destAddr, &allon_1_2[0], length);
  J1939.Transmit(key3_4, priority, srcAddr, destAddr, &allon_1_2[0], length);
  J1939.Transmit(key5_6, priority, srcAddr, destAddr, &allon_1_2[0], length);
  Serial.println("Flickering message sent");
}
//************** AUTOMATIC MODE *************
void autoMode()                     
{
  autocount =0;
  Byte x;
  x.byte = nData[1];
  int previousTime = 0; // save last used time
  int currentTime = 0;

  do
  {
    if (J1939.Receive(&lPGN, &nPriority, &nSrcAddr, &nDestAddr, nData, &nDataLen) ==0)  
    {
      x.byte = nData[1];
      detectRotationOfJoystick();
    }
    currentTime =  millis();
    ChangeFrequency();
    Serial.println(eventTime);
    if (autocount == 12)
    {
      autocount =0;
    }
    if( currentTime - previousTime >= eventTime)
    {
      autoFlicker();
      sendColorFLicker();
      autocount = autocount +1; 
      previousTime = currentTime;     
    }
    
    if(lPGN ==  0xFF64 && x.byte == 0x40)// CHECK reset button
    {
      checkResetButton(); 
    }
    // Checking conditions for in and out
    if(lPGN ==  0xFF64 && x.byte==1 && Key_is_pressed ==false)  // Key wasnt pressed and is pressed now
    {
      Serial.println("Out of Automatic Mode");
      goOut=true;
    }
    else if(lPGN ==  0xFF64 &&x.byte==0 )   // Key is not pressed anymore
    {
      Key_is_pressed=false; 
    }
  } while (goOut != true);
  goOutOfAutoMode();  
}


//************** VOID LOOP *************
void loop()
{ 
  if (J1939.Receive(&lPGN, &nPriority, &nSrcAddr, &nDestAddr, nData, &nDataLen) == 0 ) //&& nData != lastmsg_nData 
  {
    ByteRotary d;
    d.byterotary = nData[3];
    ManualModeRotation();
    //*******************   CCM   ***************************
    if (lPGN == 0xFF64 && d.bit6 == 1 && goOut_CCM == true && test == false) // if we got out we dont go directly back in mode
    {
      RB_is_pressed = true;
      test = true;
    }
    if (lPGN == 0xFF64 && d.bit6 == 0 && test == true)
    {
      RB_is_pressed = false;
    }
    if (lPGN == 0xFF64 && d.bit6 == 1 && RB_is_pressed == false && test == true)
    {
      RB_is_pressed = true;
      goOut_CCM = false;
      goToColorChange();
    }
    else
    {
    }

    //*******************   AUTOMODE   ***************************
    if (lPGN == 0xFF64 && nData[1] == 1 && goOut == true && test == false) // if we got out we dont go directly back in mode
    {
      Key_is_pressed = true;
      test = true;
    }
    if (lPGN == 0xFF64 && nData[1] == 0 && test == true)
    {
      Key_is_pressed = false;
    }
    if (lPGN == 0xFF64 && nData[1] == 1 && Key_is_pressed == false && test == true)
    {
      Key_is_pressed = true;
      goOut = false;
      autoMode();
    }
    else
    {
    }

    //*******************   RESET MODE   ***************************
    if (lPGN == 0xFF64 && nData[1] == 0x40)
    {
      ResetMode();
    }
    else
    {
    }
  }
}
