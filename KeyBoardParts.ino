#include <Keyboard.h>

#define KEYZ_CHAR   'z'  // z 입력값 지정

#define BAUDRATE    250000  //  시리얼 통신 속도

#define KP_CTRL        A0      // CTRL 키 핀번호.
#define KP_SHIFT        A1      // SHIFT 키 핀번호.
#define KP_STOP       A2      // STOP 키 핀번호.

#define LP_CTRL        9       // CTRL led 핀번호.
#define LP_SHIFT       10      // SHIFT led 핀번호.
#define LP_STOP       11      // STOP led 핀번호.

#define PROC_SPD    100       // 루프 반복 속도 (us) 
#define LD          255     // led (0-255) 밝기
#define LD_TM       1     // led(0-255) 밝기를 줄여주기 위한 변수

unsigned short V_CTRL = 0, V_SHIFT = 0, V_STOP = 0;           // 디지털 값을 저장하기 위한 변수
unsigned short _ON_STOP = 0;    
unsigned short TM_CTRL = LD, TM_SHIFT = LD, TM_STOP = LD;   // LED값
unsigned short ON_OFF = 0;


/* 
 * Init
*/
void setup() {
  Serial.begin(BAUDRATE); //
  // 출력 핀으로 설정
  pinMode(KP_CTRL,INPUT_PULLUP);     // 핀모드를 정의한다.
  pinMode(KP_SHIFT,INPUT_PULLUP);    // 아두이노 내부에 있는 풀업 저항을 이용하는 함수
  pinMode(KP_STOP,INPUT_PULLUP);   
  pinMode(LP_CTRL,OUTPUT);            //디지털 9번 핀을 출력모드로 설정
  pinMode(LP_SHIFT,OUTPUT);         
  pinMode(LP_STOP,OUTPUT);
  digitalWrite(LP_STOP,LD);           //연결되었다는 표시를 나타내기 위한 밝기 설정
  delay(1000); 
}



/* 
 * Debug
*/
void debug() {    //디버그를 할 때 사용하는 함수, 각 디지털,아날로그 신호를 확인할 수 있다.
  Serial.print("KEY: ");  
  Serial.print(digitalRead(KP_CTRL));
  Serial.print(" ");
  Serial.print(digitalRead(KP_SHIFT));
  Serial.print(" ");
  Serial.print(digitalRead(KP_STOP));
  Serial.print("   ");
  Serial.print("LED: ");
  Serial.print(digitalRead(LP_STOP));
  Serial.print("\n");
}

/*
 * Read pin connection value
*/
void READPIN() {  //loop를 통해 디지털값을 계속 읽어들이는 함수
  V_STOP = digitalRead(KP_STOP);
  V_CTRL = digitalRead(KP_CTRL);
  V_SHIFT = digitalRead(KP_SHIFT);
}

void Light_Shift() {  //SHIFT 키를 눌렀을 때  LED를 서서히 꺼지도록 하는 함수
       analogWrite(LP_SHIFT,LD);
      TM_SHIFT = LD;
      
      while(TM_SHIFT > 0){
        delay(1);
        TM_SHIFT = TM_SHIFT - LD_TM;
      analogWrite(LP_SHIFT, TM_SHIFT);
        }
}

void Light_CTRL() { //CTRL 키를 눌렀을 때 LED를 서서히 꺼지도록 하는 함수
       analogWrite(LP_CTRL,LD);
      TM_CTRL = LD;
      
      while(TM_CTRL > 0){
        delay(1);
        TM_CTRL = TM_CTRL - LD_TM;
      analogWrite(LP_CTRL, TM_CTRL);
        }
}



/*
 * Loop
*/
void loop() { //본격적인 메인함수.
  delayMicroseconds(PROC_SPD); 
  READPIN(); 
  debug(); //디버그 함수
  
  if(V_STOP == 0) {        //STOP 값을 검사하는 조건 검사문
    if(_ON_STOP == 0) {    //디폴트 값을 검사한다
      _ON_STOP = 1;        
      analogWrite(LP_STOP,LD); //아날로그 신호를 출력하기 위한 함수, (신호를 출력할 핀, 출력할 신호)
      TM_STOP = LD;            //TM_STOP 변수에 LD 값을 저장한다.(LED가 밝기 값만큼 켜짐)
      Keyboard.release(KEY_LEFT_CTRL);  //반복 입력되는 키의 입력을 멈추는 함수.
      Keyboard.release(KEY_RIGHT_ARROW);
      Keyboard.release(KEY_LEFT_SHIFT);
      Keyboard.release(KEY_LEFT_ARROW); 
      Keyboard.release(KEY_INSERT); 
      Keyboard.release(KEYZ_CHAR);    
    }
    
  }else if(V_STOP == 1) {          //눌렀던 키를 땟을 때, 스위치 값은 다시 HIGH 값으로 돌아가고,
    if(_ON_STOP == 1) {            //1 이였던 변수값도 다시 0으로 저장한다.
      _ON_STOP = 0; 
    }
    
    if(TM_STOP > 0) {                // 한번에 LED가 꺼지지 않고, 서서히 꺼지도록 검사하는 조건문
      TM_STOP = TM_STOP - LD_TM;     //255 였던 값이 1씩 빠르게 줄어든다.
      analogWrite(LP_STOP, TM_STOP); //led의 밝기를 조절할 수 있기 때문에 아날로그 신호를 쓴다.
    }
  }

  
  if(V_CTRL== 0) {    //메크로 설정은 기본적인 좌우로 움직이면서 공격하기 이다.
    
        ON_OFF = 1;  // 키를 눌렀을 때, LED 출력을 위해 변수를 바꾼다.
        
        while(ON_OFF == 1){ 
         Light_CTRL();   // LED 함수 호출
         Keyboard.press(KEY_LEFT_CTRL);    // press 함수는 키를 계속 입력하는 기능을 가지고 있다.
         Keyboard.press(KEY_LEFT_ARROW);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}
         delay(2000);  //2초동안 지속
              
         Keyboard.release(KEY_LEFT_CTRL); //예상치 못한 오류가 날 지 모르기 때문에 입력을 멈추는
         Keyboard.release(KEY_LEFT_ARROW); // release 함수를 사용했다.
         READPIN();                         //동작을 수행할 때마다 값을 읽어들여서 동작을 멈출것인지 확인한다.
         if(V_STOP == 0){ON_OFF=0; break;}    
         delay(500); 

         Keyboard.press(KEY_LEFT_CTRL);    //마찬가지 동작이다, 다른건 왼쪽으로 움직이냐, 오른쪽으로
         Keyboard.press(KEY_RIGHT_ARROW);  //움직이냐의 차이이다.
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}
         delay(2000);

         Keyboard.release(KEY_LEFT_CTRL);
         Keyboard.release(KEY_RIGHT_ARROW);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}         
         delay(500);
         
    }
  }


  if(V_SHIFT == 0) { //이 매크로는 좌우로 움직이면서, 떨어진 물건을 줍고, 스킬을 사용하며, 
                     //매크로가 끝나면, 물약을 먹는 동작의 매크로이다.
        ON_OFF = 1;   
        while(ON_OFF == 1){    //조건문과 반복문 자체는 똑같다. 
         Light_Shift(); 
         Keyboard.press(KEY_LEFT_SHIFT);    //입력되는 키가 늘어났다.
         Keyboard.press(KEY_LEFT_ARROW);
         Keyboard.press(KEYZ_CHAR);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}
         delay(2000);
              
         Keyboard.release(KEY_LEFT_SHIFT);
         Keyboard.release(KEY_LEFT_ARROW);
         Keyboard.release(KEYZ_CHAR);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}    
  
         Keyboard.press(KEY_LEFT_SHIFT);   
         Keyboard.press(KEY_RIGHT_ARROW);
         Keyboard.press(KEYZ_CHAR);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}
         delay(2000);

         Keyboard.release(KEY_LEFT_SHIFT);
         Keyboard.release(KEY_RIGHT_ARROW);
         Keyboard.release(KEYZ_CHAR);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}  

         Keyboard.press(KEY_INSERT);   //물약먹는 키를 입력하는 함수.
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}
         delay(500);

         Keyboard.release(KEY_INSERT);
         READPIN(); 
         if(V_STOP == 0){ON_OFF=0; break;}                  
    }
  }
}
