
#include "stm32f10x.h"
			
#define MOTOR_2_Out_3 GPIO_Pin_8
#define MOTOR_2_Out_4 GPIO_Pin_9
#define Motor_2_PWM GPIO_Pin_6

#define MOTOR_1_Out_1 GPIO_Pin_5
#define MOTOR_1_Out_2 GPIO_Pin_6
#define Motor_1_PWM GPIO_Pin_7

#define ECHO GPIO_Pin_7
#define TRIGGER GPIO_Pin_10

int Max_PWM = 200-1;

char tmpBuffer[4] = {0};
uint16_t bufferIterator = 0;
int joystickX = 0;
int joystickY = 0;

uint16_t begin = 0;
uint16_t end = 0;

int enemyInSight = 0;

void EXTI9_5_IRQHandler(){
	if(EXTI_GetITStatus(EXTI_Line7)){
		EXTI_ClearITPendingBit(EXTI_Line7);

		if(GPIO_ReadInputDataBit(GPIOC, ECHO) == 1){
			begin = TIM_GetCounter(TIM2);

		}
		else if(begin!=0){
			end = TIM_GetCounter(TIM2);
			if(0.017*(end-begin) < 100){
				USART_SendData(USART3, '!');
				if(enemyInSight == 1){

				GPIO_SetBits(GPIOC, MOTOR_2_Out_3);/*oba silniki do przodu*/
				GPIO_ResetBits(GPIOC, MOTOR_2_Out_4);

				GPIO_SetBits(GPIOC, MOTOR_1_Out_1);
				GPIO_ResetBits(GPIOC, MOTOR_1_Out_2);
				}
			}
			else{
				USART_SendData(USART3, '?');

				if(enemyInSight == 1){

				GPIO_SetBits(GPIOC, MOTOR_2_Out_3);/*oba silniki do przodu*/
				GPIO_ResetBits(GPIOC, MOTOR_2_Out_4);

				GPIO_SetBits(GPIOC, MOTOR_1_Out_2);
				GPIO_ResetBits(GPIOC, MOTOR_1_Out_1);
				}
			}
		}


	}
}

void TIM2_IRQHandler(){
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		GPIO_SetBits(GPIOC, TRIGGER);
		begin = 0;
		end = 0;
	}

	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) == SET){
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);

		GPIO_ResetBits(GPIOC, TRIGGER);
	}
}


int main(void)
{
		GPIO_InitTypeDef gpio;

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

		GPIO_StructInit(&gpio);

		gpio.GPIO_Pin = MOTOR_2_Out_3;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOC, &gpio);

		gpio.GPIO_Pin = MOTOR_2_Out_4;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOC, &gpio);

		gpio.GPIO_Pin = MOTOR_1_Out_1;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOC, &gpio);

		gpio.GPIO_Pin = MOTOR_1_Out_2;
		gpio.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOC, &gpio);

		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);


		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = Motor_2_PWM;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &gpio);

		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = Motor_1_PWM;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		gpio.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &gpio);

		TIM_TimeBaseInitTypeDef tim;

		TIM_TimeBaseStructInit(&tim);
		tim.TIM_CounterMode = TIM_CounterMode_Up;
		tim.TIM_Prescaler = 64000 - 1;
		tim.TIM_Period = Max_PWM;
		TIM_TimeBaseInit(TIM4, &tim);

		TIM_OCInitTypeDef  channel;

		TIM_OCStructInit(&channel);
		channel.TIM_OCMode = TIM_OCMode_PWM1;
		channel.TIM_OutputState = TIM_OutputState_Enable;
		channel.TIM_Pulse = 100;/*jak 0 to wylaczone jak == okres to sta³y stan wysoki*/
		TIM_OC1Init(TIM4, &channel);
		TIM_OC2Init(TIM4, &channel);

	    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

		GPIO_StructInit(&gpio);
		gpio.GPIO_Pin = GPIO_Pin_10;/*tx*/
		gpio.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &gpio);

		gpio.GPIO_Pin = GPIO_Pin_11;/*rx*/
		gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
		GPIO_Init(GPIOB, &gpio);

		USART_InitTypeDef uart;

		USART_StructInit(&uart);
		uart.USART_BaudRate = 9600;
		uart.USART_WordLength = USART_WordLength_8b;
		uart.USART_Parity = USART_Parity_No;
		uart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_Init(USART3, &uart);

		USART_Cmd(USART3, ENABLE);

		TIM_Cmd(TIM4, ENABLE);

		GPIO_SetBits(GPIOC, MOTOR_2_Out_3);
		GPIO_ResetBits(GPIOC, MOTOR_2_Out_4);

		GPIO_SetBits(GPIOC, MOTOR_1_Out_1);
		GPIO_ResetBits(GPIOC, MOTOR_1_Out_2);

		TIM_SetCompare1(TIM4, 0);
		TIM_SetCompare2(TIM4, 0);

		/*tutaj siê zacznie czujnik odleg³oœci*/
		gpio.GPIO_Pin = TRIGGER;
		gpio.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOC, &gpio);

		/*Distance measuring echo*/
		gpio.GPIO_Pin = ECHO;
		gpio.GPIO_Mode = GPIO_Mode_IPD;
		GPIO_Init(GPIOC, &gpio);


		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
		EXTI_InitTypeDef exti;

		EXTI_StructInit(&exti);
		exti.EXTI_Line = EXTI_Line7;
		exti.EXTI_Mode = EXTI_Mode_Interrupt;
		exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
		exti.EXTI_LineCmd = ENABLE;
		EXTI_Init(&exti);

		GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource7);

		NVIC_InitTypeDef nvic;

		nvic.NVIC_IRQChannel = EXTI9_5_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority = 0x00;
		nvic.NVIC_IRQChannelSubPriority = 0x00;
		nvic.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvic);

		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

		tim.TIM_CounterMode = TIM_CounterMode_Up;
		tim.TIM_Prescaler = 64 - 1;
		tim.TIM_Period = 65535 - 1;
		TIM_TimeBaseInit(TIM2, &tim);

		nvic.NVIC_IRQChannel = TIM2_IRQn;
		nvic.NVIC_IRQChannelPreemptionPriority = 0;
		nvic.NVIC_IRQChannelSubPriority = 0;
		nvic.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&nvic);

		channel.TIM_OCMode = TIM_OCMode_Timing;
		channel.TIM_Pulse = 10;
		TIM_OC1Init(TIM2, &channel);

		TIM_ITConfig(TIM2, TIM_IT_Update|TIM_IT_CC1, ENABLE);

		GPIO_SetBits(GPIOC, TRIGGER);
		TIM_Cmd(TIM2, ENABLE);

		/*koniec czujnika*/

	while(1){
		if (USART_GetFlagStatus(USART3, USART_FLAG_RXNE)) {
			char c = USART_ReceiveData(USART3);
			if(c == '|'){
				enemyInSight = 0;

				joystickX = getFromBuffer();
			}
			else if(c == ';'){
				enemyInSight = 0;

				joystickY = getFromBuffer();
				setEngines();
			}
			else if(c == '*'){
				TIM_SetCompare1(TIM4, Max_PWM);
				TIM_SetCompare2(TIM4, Max_PWM);

				enemyInSight = 1;

			}
			else if((c >= '0' && c <= '9') || (c >= 0 && c <= 9) || c == '-'){
				enemyInSight = 0;

				tmpBuffer[bufferIterator++] = c;
			}

		}
	}
}

void attack(void){
			GPIO_SetBits(GPIOC, MOTOR_2_Out_3);/*oba silniki do przodu*/
			GPIO_ResetBits(GPIOC, MOTOR_2_Out_4);

			GPIO_SetBits(GPIOC, MOTOR_1_Out_1);
			GPIO_ResetBits(GPIOC, MOTOR_1_Out_2);
}

void rotate(void){
	GPIO_SetBits(GPIOC, MOTOR_2_Out_3);
	GPIO_ResetBits(GPIOC, MOTOR_2_Out_4);

	GPIO_SetBits(GPIOC, MOTOR_1_Out_2);
	GPIO_ResetBits(GPIOC, MOTOR_1_Out_1);
}


int power(int x, int power){
	int result = 1;
	for(int i=0; i <power; i++){
		result = result*x;
	}
	return result;
}

int calcLower(int higher){
	int tmp = joystickX>0 ? joystickX : joystickX*-1;
	return (int)(higher * (100-tmp)/100.0);
}

int calcPwm(void){
	return (int)(Max_PWM*(joystickY*joystickY + joystickX*joystickX)/10000.0);
}

int getFromBuffer(void){
	int result = 0;
	if(tmpBuffer[0] == '-'){
		for(int i = 1; i < bufferIterator; i++){
			result+=((int)tmpBuffer[i] - '0') * power(10, bufferIterator - i - 1);
		}

		result*=-1;
	}
	else{
		for(int i = 0; i < bufferIterator; i++){
			result+=((int)tmpBuffer[i] - '0') * power(10, bufferIterator - i - 1);
		}
	}

	bufferIterator = 0;

	return result;
}

void setEngines(void){
	if(joystickY < 0){
		GPIO_SetBits(GPIOC, MOTOR_2_Out_3);/*oba silniki do przodu*/
		GPIO_ResetBits(GPIOC, MOTOR_2_Out_4);

		GPIO_SetBits(GPIOC, MOTOR_1_Out_1);
		GPIO_ResetBits(GPIOC, MOTOR_1_Out_2);

		int pwm = calcPwm();
		if(joystickX > 0){
			TIM_SetCompare1(TIM4, pwm);
			TIM_SetCompare2(TIM4, calcLower(pwm));
		}
		else if(joystickX < 0){
			TIM_SetCompare2(TIM4, pwm);
			TIM_SetCompare1(TIM4, calcLower(pwm));
		}
		else{
			TIM_SetCompare1(TIM4, pwm);
			TIM_SetCompare2(TIM4, pwm);
		}

	}
	else if(joystickY > 0){
		GPIO_SetBits(GPIOC, MOTOR_2_Out_4);/*oba silniki do ty³u*/
		GPIO_ResetBits(GPIOC, MOTOR_2_Out_3);

		GPIO_SetBits(GPIOC, MOTOR_1_Out_2);
		GPIO_ResetBits(GPIOC, MOTOR_1_Out_1);

		int pwm = calcPwm();
		if(joystickX > 0){
			TIM_SetCompare2(TIM4, pwm);
			TIM_SetCompare1(TIM4, calcLower(pwm));
		}
		else if(joystickX < 0){
			TIM_SetCompare1(TIM4, pwm);
			TIM_SetCompare2(TIM4, calcLower(pwm));
		}
		else{
			TIM_SetCompare1(TIM4, pwm);
			TIM_SetCompare2(TIM4, pwm);
		}

	}
	else{
		if(joystickX == 0){
			TIM_SetCompare1(TIM4, 0);
			TIM_SetCompare2(TIM4, 0);
		}
		else if(joystickX < 0){
			int pwm = calcPwm();

			GPIO_SetBits(GPIOC, MOTOR_2_Out_4);/*oba silniki do przodu*/
			GPIO_ResetBits(GPIOC, MOTOR_2_Out_3);

			GPIO_SetBits(GPIOC, MOTOR_1_Out_2);
			GPIO_ResetBits(GPIOC, MOTOR_1_Out_1);

			TIM_SetCompare1(TIM4, pwm);
			TIM_SetCompare2(TIM4, 0);

		}
		else{
			int pwm = calcPwm();
			GPIO_SetBits(GPIOC, MOTOR_2_Out_4);/*oba silniki do przodu*/
			GPIO_ResetBits(GPIOC, MOTOR_2_Out_3);

			GPIO_SetBits(GPIOC, MOTOR_1_Out_2);
			GPIO_ResetBits(GPIOC, MOTOR_1_Out_1);

			TIM_SetCompare2(TIM4, pwm);
			TIM_SetCompare1(TIM4, 0);
		}
	}
}






