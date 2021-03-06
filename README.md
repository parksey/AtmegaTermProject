# **SMART HOME Service**
<p>
Microprocessor2 term project
</p>
<br>

## **Base Program**
- Microchip Studio(AVR Studio)
  - 기존 Code Vision을 사용 했으나 프로그램 크기가 커지면서 무료 버전 사용할 수 없어 대신 사용
- ComPortMaster : UART 통신을 위해 사용
<br>

## 동작 시나리오
<img width="50%" src="https://user-images.githubusercontent.com/54196094/175772857-f0eaa4ca-982a-431b-bdcd-91ab31a0f31e.png"/>
<br>

## 예외 시나리오
<img width="50%" src="https://user-images.githubusercontent.com/54196094/175772861-5907c2ed-4b3c-4c69-9190-e0773ed6c041.png"/>
<br>


## **Main Function**

0. **시작화면 설정**
   - [x] LCD 임의의 위치에 글자 출력 함수 구현

1. **전등 기능**
   - [x] 조도 센서를 이용하여 빛의 세기 감지 
   - [x] LED 자동으로 ON/OFF 

2. **외출 기능**
   - [X] 블루투스 모듈사용하여 UART통신 확인 (스마트폰 & 컴퓨터)
   - [X] UART 통신용 패킷 설정
   - [X] 해당 패킷으로 전반적인 센서 제어

3. **알람 기능**
   - [X] 알람 시간 입력하여 타이머 작동하게 설정
   - [X] 알람 후 센서 제어

4. **응급 상황(화재, 지진) 문 여닫이 기능**
   - [x] 특정 패킷을 통한 모터(문) 제어

<br>

## **사용 부품**
|구성품 LIST|수량|
|------|---|
|ATMEGA128|1|
|Breadboard|1|
|온도 센서(LM35)|1|
|조도 센서(KY-018)|1|
|불꽃 감지 센서|1|
|서보 모터|1|
|블루투스 모듈(HC-06)|1|
|LED|1|
|저항(10K, 220)|각각 1개씩|
|FND|1|
|LCD|1|

<br>


## **개선 사항**
1. 현재 블루투스는 핸드폰과 컴퓨터 둘다 가능하지만, 패킷을 직접 작성하여 전달
 - => 앱이나 웹에서 후에 버튼으로 동작되면 좋을 것 같다.
2. 불꽃 감지 센서의 경우 ADMUX를 거치기 때문에 해당 ADMUX를 사용해야하는 센서가 여러개인 경우 동시 동작이 힘들다.
 - => 데이터 시트 보고 좀 수정 필요
3. 전체적인 외형 변경 필요
