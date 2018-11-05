#include <EEPROM.h>

/* 코드 : LED 대응표
   convertCode(int num) -> 결과
   [num : 결과]
   0 : NULL LED 전부끔
   1-2-3 : LED1 RGB / POS : 1
   4-5-6 : LED2 RGB / POS : 2
   7-8-9 : LED3 RGB / POS : 3
   10-11-12 : LED4 RGB / POS : 4
   13-14-15 : LED5 RGB / POS : 5
   16-17 : PATTERN LED (ON/OFF)
   18-19 : GAME LED (ON/OFF)
   20-21 : INPUT LED (ON/OFF)
*/
void convertCode(int code) {
  switch (code) {
    case -1:
      for (int i = 0; i < 5; i++)
        setLED(i, 0, 0, 0);
      break;
    case 1:
      setLED(0, 255, 0, 0);
      break;
    case 2:
      setLED(0, 0, 255, 0);
      break;
    case 3:
      setLED(0, 0, 0, 255);
      break;
    case 4:
      setLED(1, 255, 0, 0);
      break;
    case 5:
      setLED(1, 0, 255, 0);
      break;
    case 6:
      setLED(1, 0, 0, 255);
      break;
    case 7:
      setLED(2, 255, 0, 0);
      break;
    case 8:
      setLED(2, 0, 255, 0);
      break;
    case 9:
      setLED(2, 0, 0, 255);
      break;
    case 10:
      setLED(3, 255, 0, 0);
      break;
    case 11:
      setLED(3, 0, 255, 0);
      break;
    case 12:
      setLED(3, 0, 0, 255);
      break;
    case 13:
      setLED(4, 255, 0, 0);
      break;
    case 14:
      setLED(4, 0, 255, 0);
      break;
    case 15:
      setLED(4, 0, 0, 255);
      break;
    case 16: // 입력 불가 상태 LED ON
      setLED(5, 255, 0, 0);
      break;
    case 17: // 입력 불가 상태 LED OFF
      setLED(5, 0, 0, 0);
      break;
    case 18: // 게임 플레이 상태 LED ON
      setLED(6, 255, 0, 0);
      break;
    case 19: // 게임 플레이 상태 LED OFF
      setLED(6, 0, 0, 0);
      break;
    case 20: // 패턴 입력 상태 LED ON
      setLED(7, 255, 0, 0);
      break;
    case 21: // 패턴 입력 상태 LED OFF
      setLED(7, 0, 0, 0);
      break;
    case 91: // 9_ 0번 LED OFF
      setLED(0, 0, 0, 0);
      break;
    case 92: // 9_ 1번 LED OFF
      setLED(1, 0, 0, 0);
      break;
    case 93: // 9_ 2번 LED OFF
      setLED(2, 0, 0, 0);
      break;
    case 94: // 9_ 3번 LED OFF
      setLED(3, 0, 0, 0);
      break;
    case 95: // 9_ 4번 LED OFF
      setLED(4, 0, 0, 0);
      break;
    default:
      break;
  }
}

// 버튼 입력 활성화 상태
boolean isBtnActivated = false;

// 버튼 상태
int btnSelect[6] = {0, 0, 0, 0, 0, 0};

// LED 출력 핀번호
int LED1[3] = {22, 23, 24};
int LED2[3] = {26, 25, 27};
int LED3[3] = {28, 29, 30};
int LED4[3] = {32, 31, 33};
int LED5[3] = {34, 35, 36};
int LED6[3] = {38, 37, 39};

// 버튼 입력 핀번호
int btn1 = 41;
int btn2 = 40;
int btn3 = 43;
int btn4 = 42;
int btn5 = 45;
int btn6 = 44;

// 게임 모드
// 1 이면 게임 플레이 모드
// 2 면 패턴 입력 모드
int gameMode = 1;

// 현재 플레이중인 스테이지 (0 ~ 4)
int playingStage = -1;

// 스테이지에 패턴이 들어있는지 확인
int stage[5] = {0, 0, 0, 0, 0};

// 게임 결과값
int gameResult;

int checkingFlag = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600); // Debug Option Only

  // 각 버튼 핀모드 INPUT 변경
  pinMode(btn1, INPUT);
  pinMode(btn2, INPUT);
  pinMode(btn3, INPUT);
  pinMode(btn4, INPUT);
  pinMode(btn5, INPUT);
  pinMode(btn6, INPUT);

  // 각 LED 핀모드 OUTPUT 변경
  for (int i = 0; i < 3; i++) {
    pinMode(LED1[i], OUTPUT);
    pinMode(LED2[i], OUTPUT);
    pinMode(LED3[i], OUTPUT);
    pinMode(LED4[i], OUTPUT);
    pinMode(LED5[i], OUTPUT);
    pinMode(LED6[i], OUTPUT);
  }

  // 디버깅용 : 메모리 포맷 (0으로 씀)
  // formatMemory();

}

void loop() {
BACK_TO_START:
  convertCode(-1);
  clearBtnBuffer();
  isBtnActivated = false;
  convertCode(20); // 버튼 입력 불가 상태등 알림
  convertCode(17); // PATTERN LED OFF
  convertCode(19); // GAME LED OFF

  // 각 스테이지 첫부분에 패턴이 입력되었는지 확인함.
  // EEPROM ADDRESS : 0, 800, 1600, 2400, 3200
  for (int i = 0; i < 3201; i += 800) {
    stageCheck(i);
  }

  if (gameMode == 1) {
    convertCode(18); // GAME LED ON

    // 모든 스테이지가 비어있는 지 확인함.
    checkingFlag = 0;
    for (int i = 0; i < 5; i++) {
      checkingFlag += stage[i];
    }
    // 스테이지가 전부 비었으면 패턴입력 모드로 보내버림.
    if (checkingFlag == 0) {
      gameMode = 2;
      goto BACK_TO_START;
    }
    // ----------------- 확인 단계 끝 --------------------

    // 게임 플레이 모드
    if (playingStage < 0) // -1 이면 첫 실행이므로, 처음 스테이지부터 시작함.
      playingStage = getNextStage(-1);

    // 현재 스테이지의 패턴을 사용자에게 보여줍니다.
    playfromMemory(playingStage);

    // 패턴 재생이 끝나면, 패턴을 입력받습니다.
    // 지금부터 사용자의 입력을 받습니다.
    isBtnActivated = true;
    convertCode(21); // 버튼 입력 불가 상태 LED 꺼짐
    
    // 게임 플레이하고 그 결과를 gameResult 변수에 저장함
    gameResult = checkfromMemory(playingStage);
    Serial.println(gameResult);
    
    convertCode(-1);

    if (gameResult == 1)
      playingStage = getNextStage(playingStage);
    
    if (gameResult == 1 && playingStage == 5) {
      convertCode(3);
      convertCode(6);
      convertCode(9);
      convertCode(12);
      convertCode(15);
      playingStage = -1;
    } else if (gameResult == 1) {
      convertCode(2);
      convertCode(5);
      convertCode(8);
      convertCode(11);
      convertCode(14);
    } else if (gameResult == 777) {
      gameMode = 2;
      goto BACK_TO_START;
    } else {
      convertCode(1);
      convertCode(4);
      convertCode(7);
      convertCode(10);
      convertCode(13);
    }

    // 버튼 입력을 기다리고, 처음으로 되돌아간다.
    if (waitBtnEvent() > 0) {
      Serial.println("처음으로 궈궈~");
    }

  } else {
    // 패턴 입력 모드로 진입함
    convertCode(16); // PATTERN LED ON
    while (true) {
      for (int i = 0; i < 5; i++) {
        if (stage[i] == 0) { // 패턴이 없으면, 빨간 LED로 점등
          switch (i) {
            case 0:
              convertCode(1);
              break;
            case 1:
              convertCode(4);
              break;
            case 2:
              convertCode(7);
              break;
            case 3:
              convertCode(10);
              break;
            case 4:
              convertCode(13);
              break;
            default:
              break;
          } // END OF SWITCH
        } else { // 패턴이 저장되어 있으면, 초록 LED로 점등
          switch (i) {
            case 0:
              convertCode(2);
              break;
            case 1:
              convertCode(5);
              break;
            case 2:
              convertCode(8);
              break;
            case 3:
              convertCode(11);
              break;
            case 4:
              convertCode(14);
              break;
            default:
              break;
          } // END OF SWITCH
        }
      } // END OF FOR-LOOP

      // 지금부터 사용자의 입력을 받습니다.
      isBtnActivated = true;
      convertCode(21); // 버튼 입력 불가 상태 LED 꺼짐

      // 패턴 입력 한다
      gameResult = addPattern();

      if (gameResult == 777) {
        Serial.println("패턴 입력 완전!! 종료 gameMode = 1로 변경");
        gameMode = 1;
        playingStage = -1;
        break; // WHILE BREAK
      }

      // 각 스테이지 첫부분에 패턴이 입력되었는지 확인함.
      // EEPROM ADDRESS : 0, 800, 1600, 2400, 3200
      for (int i = 0; i < 3201; i += 800) {
        stageCheck(i);
      }

    } // END OF WHILE
  }
}

// LED 키는 함수 (0 ~ 5번까지)
// 실행 : LED켜야 할 때 다른 함수가 호출함 + 단독으로 실행될 수 없음
// 반환값 : 없음
void setLED(int LED, int red, int green, int blue) {
  switch (LED) {
    case 0: // 1번 LED
      digitalWrite(LED1[0], red);
      digitalWrite(LED1[1], green);
      digitalWrite(LED1[2], blue);
      break;
    case 1: // 2번 LED
      digitalWrite(LED2[0], red);
      digitalWrite(LED2[1], green);
      digitalWrite(LED2[2], blue);
      break;
    case 2: // 3번 LED
      digitalWrite(LED3[0], red);
      digitalWrite(LED3[1], green);
      digitalWrite(LED3[2], blue);
      break;
    case 3: // 4번 LED
      digitalWrite(LED4[0], red);
      digitalWrite(LED4[1], green);
      digitalWrite(LED4[2], blue);
      break;
    case 4: // 5번 LED
      digitalWrite(LED5[0], red);
      digitalWrite(LED5[1], green);
      digitalWrite(LED5[2], blue);
      break;
    case 5: // 패턴 LED 상태 LED (단색LED이므로 인자값 하나만 씀)
      digitalWrite(LED6[0], red);
      break;
    case 6: // 게임 플레이 상태 LED (단색LED이므로 인자값 하나만 씀)
      digitalWrite(LED6[1], red);
      break;
    case 7: // 입력 불가 상태 LED (단색LED이므로 인자값 하나만 씀)
      digitalWrite(LED6[2], red);
      break;
    default:
      break;
  }
}

// 버튼 입력 기다리는 함수
// 입력을 허가해주는 isBtnActivated가 활성화되어야만 작동.
// 실행 : 버튼 눌러야될때 waitBtnEvent -> btnCommand가 실행되는 구조임
// 반환값 : 푸쉬 버튼의 위치 (배열 index와 일치시키기 위해서는 -1을 해줘야함)
int waitBtnEvent() {
  while (1) {
    if (isBtnActivated && digitalRead(btn1) == 1) {
      Serial.println("btn1 OK");
      btnCommand(1);
      return 1;
    } else if (isBtnActivated && digitalRead(btn2) == 1) {
      Serial.println("btn2 OK");
      btnCommand(2);
      return 2;
    } else if (isBtnActivated && digitalRead(btn3) == 1) {
      Serial.println("btn3 OK");
      btnCommand(3);
      return 3;
    } else if (isBtnActivated && digitalRead(btn4) == 1) {
      Serial.println("btn4 OK");
      btnCommand(4);
      return 4;
    } else if (isBtnActivated && digitalRead(btn5) == 1) {
      Serial.println("btn5 OK");
      btnCommand(5);
      return 5;
    } else if (isBtnActivated && digitalRead(btn6) == 1) {
      Serial.println("btn6 OK");
      delay(300);
      return 777;
    } else {
      // Do nothing.
    }
  } // END OF WHILE;
}

// 사용자 버튼 상호작용 함수
// 실행 : waitBtnEvent()를 통해 실행되어야 함. 단독으로 실행될 수 없음.
// 반환값 : 없음
void btnCommand(int n) {
  delay(300);
  switch (n) {
    case 1:
      if (btnSelect[0] == 0 || btnSelect[0] == 3)
        btnSelect[0] = 1;
      else
        ++btnSelect[0];
      break;
    case 2:
      if (btnSelect[1] == 0 || btnSelect[1] == 6)
        btnSelect[1] = 4;
      else
        ++btnSelect[1];
      break;
    case 3:
      if (btnSelect[2] == 0 || btnSelect[2] == 9)
        btnSelect[2] = 7;
      else
        ++btnSelect[2];
      break;
    case 4:
      if (btnSelect[3] == 0 || btnSelect[3] == 12)
        btnSelect[3] = 10;
      else
        ++btnSelect[3];
      break;
    case 5:
      if (btnSelect[4] == 0 || btnSelect[4] == 15)
        btnSelect[4] = 13;
      else
        ++btnSelect[4];
      break;
    default:
      break;
  }//End of Switch
} //End of btnCommand()

// 메모리에서 게임 패턴 불러와서 사용자 입력과 맞춰본다.
// 실행 : 게임을 시작. gameResult에 들어갈 변수를 반환해줌
// 반환값 : 클리어 여부 (1 : 클리어 / 0 : FAIL)
int checkfromMemory(int stage) {
  int next_val, val, pos, next_pos; // pos : 1,2,3,4,5
  int input;
  int start_idx, end_idx;

  // 해당 스테이지에 저장된 EEPROM 주소 범위를 지정.
  switch (stage) {
    case 0:
      // 0 ~ 799
      start_idx = 0;
      end_idx = 800;
      break;
    case 1:
      // 800 ~ 1599
      start_idx = 800;
      end_idx = 1600;
      break;
    case 2:
      // 1600 ~ 2399
      start_idx = 1600;
      end_idx = 2400;
      break;
    case 3:
      // 2400 ~ 3199
      start_idx = 2400;
      end_idx = 3200;
      break;
    case 4:
      // 3200 ~ 3999
      start_idx = 3200;
      end_idx = 4000;
      break;
    default:
      return 0;
  } // END OF SWITCH STATEMENT

  for (int i = start_idx; i < end_idx; i++) {
    boolean isFinal = false;
    val = EEPROM.read(i);
    Serial.print("val is ");
    Serial.println(val);
    next_val = EEPROM.read(i + 1);
    Serial.print("next_val is ");
    Serial.println(next_val);

    pos = getPosition(val);

    if ( (i + 1) != end_idx ) { // i가 마지막 자리가 아니면...
      next_pos = getPosition(next_val); // 다음 자리 확인하기
      if (next_val == 0)
        isFinal=true;
    } else {
      next_pos = pos;
      isFinal = true;
    }
    
    if (val != 0) {
      // 사용자 입력 받기
      while (1) {
        input = waitBtnEvent();
        ledRefresh();
        showBtnBuffer();
        
        if (input == 777)
          return input;
          
        if (pos != input) { // 다른 버튼을 눌렀을 때
          if (input != next_pos || btnSelect[pos - 1] != val) {
            Serial.println("잘못된 버튼을 눌렀군요!");
            return 0;
          } else {
            Serial.println("맞추셨어요!");
            btnSelect[pos - 1] = 0;
            convertCode(pos + 90);
            break;
          }
        } else {
          if ( isFinal && btnSelect[pos - 1] == val ) {
            return 1;
          }
          Serial.println("같은 버튼을 누르고 잇어요!");
        }
      } // END OF WHILE
      Serial.println("while을 빠져나왓어요");
    } else {
      break;
    }
  }
}

// 메모리에서 게임 패턴 불러와서 LED 상태 재생해주는 함수
// 실행 : 스테이지 첫 방문시
void playfromMemory(int stage) {
  // 1스테이지당 800개씩 INPUT 가능
  int val;
  int start_idx, end_idx;
  switch (stage) {
    case 0:
      // 0 ~ 799
      start_idx = 0;
      end_idx = 800;
      break;
    case 1:
      // 800 ~ 1599
      start_idx = 800;
      end_idx = 1600;
      break;
    case 2:
      // 1600 ~ 2399
      start_idx = 1600;
      end_idx = 2400;
      break;
    case 3:
      // 2400 ~ 3199
      start_idx = 2400;
      end_idx = 3200;
      break;
    case 4:
      // 3200 ~ 3999
      start_idx = 3200;
      end_idx = 4000;
      break;
    default:
      return;
  } // END OF SWITCH STATEMENT

  for (int i = start_idx; i < end_idx; i++) {
    val = EEPROM.read(i);
    if (val != 0) {
      convertCode(val);
      delay(800); // LED 패턴 재생 타임
      convertCode(90 + getPosition(val));
    } else {
      break;
    }
  } // END OF FOR LOOP
}

// 다음 스테이지를 불러오는 함수 (0 / 800 / 1600 / 2400 / 3000)
// 실행 : 첫 게임모드 시 & 클리어 시
int getNextStage(int prev) {
  if (prev == -1) { // 첫 실행시
    for (int i = 0; i < 5; i++) {
      if (stage[i] != 0) {
        Serial.print("다음 스테이지는 ");
        Serial.print(i);
        Serial.println(" 입니다.");
        return i;
      }
    }
  } else { // 이전 prev 기준으로 증가
    for (int i = prev+1; i < 5; i++) {
      if (stage[i] != 0) {
        Serial.print("다음 스테이지는 ");
        Serial.print(i);
        Serial.println(" 입니다.");
        return i;
      }
    }
    return 5;
  }
}

// 패턴 추가하는 함수
// 반환값 : 정상 1 / 비정상 0
int addPattern() {
  int stage = waitBtnEvent() - 1;
  clearBtnBuffer();
  convertCode(-1);

  int start_idx, end_idx, pos; // pos : 1,2,3,4,5
  int input;
  switch (stage) {
    case 0:
      // 0 ~ 799
      start_idx = 0;
      end_idx = 800;
      break;
    case 1:
      // 800 ~ 1599
      start_idx = 800;
      end_idx = 1600;
      break;
    case 2:
      // 1600 ~ 2399
      start_idx = 1600;
      end_idx = 2400;
      break;
    case 3:
      // 2400 ~ 3199
      start_idx = 2400;
      end_idx = 3200;
      break;
    case 4:
      // 3200 ~ 3999
      start_idx = 3200;
      end_idx = 4000;
      break;
    default:
      return 777; // 패턴 입력 완전 종료 ( 777 )
  } // END OF SWITCH STATEMENT

  for (int i = start_idx; i < end_idx; i++) {
    if ( i == start_idx ) {
      pos = waitBtnEvent(); // 첫 패턴 순서 기억
      if (pos == 777) { // 6번 푸쉬버튼 누른 경우
        // 패턴 삭제
        convertCode(20); // 입력 불가 알림 LED
        for (int k = i; k < end_idx; k++)
          EEPROM.write(k, 0);
        Serial.println("패턴 삭제");
        return 1;
      }
    }
    while (1) {
      showBtnBuffer();
      ledRefresh();
      input = waitBtnEvent();
      if (pos != input) { // 다른 버튼을 눌렀을 때
        // EEPROM에 그 값을 쓴다.
        EEPROM.write(i, btnSelect[pos - 1]);
        Serial.print(i);
        Serial.print("번째에 다음 입력 : ");
        Serial.println(btnSelect[pos - 1]);
        btnSelect[pos - 1] = 0;
        convertCode(pos + 90);
        pos = input;
        if (input == 777) {
          convertCode(20); // 입력 불가 알림 LED
          for (int k = i + 1; k < end_idx; k++)
            EEPROM.write(k, 0); // 마지막 입력 이후 0으로 채움
          Serial.println("패턴 입력 종료");
          return 1;
        }
        break; // WHILE문을 빠져나온다
      } else { // 같은 버튼을 눌렀을 때
        // 아무것도 안함.
      }
    } // END OF WHILE
  } // END OF FOR-LOOP

  return 1;
}

// 패턴이 있는지 없는지 스테이지 상태 기록 함수
// 실행 : 첫 실행시 & 패턴 모드 빠져나올 때
void stageCheck(int i) {
  switch (i) {
    case 0:
      stage[0] = EEPROM.read(i);
      break;
    case 800:
      stage[1] = EEPROM.read(i);
      break;
    case 1600:
      stage[2] = EEPROM.read(i);
      break;
    case 2400:
      stage[3] = EEPROM.read(i);
      break;
    case 3200:
      stage[4] = EEPROM.read(i);
      break;
    default:
      break;
  }
}

// LED 키는 코드를 버튼 위치 번호로 바꿔주는 함수
// 반환값 : 버튼 위치
int getPosition(int code) {
  if (code < 4) {
    return 1;
  } else if (code < 7) {
    return 2;
  } else if (code < 10) {
    return 3;
  } else if (code < 13) {
    return 4;
  } else if (code < 16) {
    return 5;
  } else {
    return 6;
  }
}

// 버튼 입력값 비우기
void clearBtnBuffer() {
  for (int i = 0; i < 6; i++)
    btnSelect[i] = 0;
}

// 버튼 입력값으로 LED 점등
void ledRefresh() {
  for (int i = 0; i < 6; i++)
    convertCode(btnSelect[i]);
}

// Only for debugging
void showBtnBuffer() {
  Serial.print("버튼 버퍼값 : ");
  for (int i = 0; i < 6; i++) {
    Serial.print(btnSelect[i]);
    Serial.print(", ");
  }
  Serial.println();
}


// Only for debugging
void formatMemory() {
  for (int i = 0; i < 4096; i++) {
    EEPROM.write(i, 0);
  }
}
