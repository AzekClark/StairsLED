/**
  ******************************************************************************
  * @file    TIM1/TIM1_ComplementarySignals/main.c
  * @author  MCD Application Team
  * @version V1.5.2
  * @date    30-September-2014
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm8l15x.h"

/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup TIM1_ComplementarySignals
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define TIM1_PERIOD                  65535
#define TIM1_PRESCALER                   0
#define TIM1_REPTETION_COUNTER           0

/* Channel 1 duty cycle = 100 * CCR1_VAL / TIM1_PERIOD + 1
                        = 32767 * 100 / (65535 + 1) = 50 %*/
#define CCR1_VAL                     32767

/* Channel 2 duty cycle = 100 * CCR2_VAL / TIM1_PERIOD + 1
                        = 16383 * 100 / (65535 + 1) = 25 %*/
#define CCR2_VAL                     16383

/* Channel 3 duty cycle = 100 * CCR3_VAL / TIM1_PERIOD + 1
                        = 8191 * 100 / (65535 + 1) = 12.5 %*/
#define CCR3_VAL                      8191

#define DEADTIME                       117

#define TIM4_PERIOD 124
#define TIM4_US_PERIOD 4


#define US_MEDIUM_DEVIDER 10
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint16_t US_Value;
volatile uint8_t US_PendingResultFlag;
volatile uint16_t Delay_count;
extern volatile uint16_t TIM2_Counter;

uint8_t value, counter;
uint16_t medium_value;
uint16_t value_summ;
volatile const uint8_t US_MediumValueThreshold = 50;
volatile uint16_t Light_Counter_s;
volatile uint16_t Light_Counter_ms;
volatile const uint16_t Light_Delay = 3;
volatile uint8_t  Light_Status;
volatile uint8_t Light_UpdateStatusFlag;

#pragma location=0x1000 //stort eeprom address;
__no_init uint8_t PWM_Red;
#pragma location=0x1002
__no_init uint8_t PWM_Blue;
#pragma location=0x1004
__no_init uint8_t PWM_Green;
#pragma location=0x1006
__no_init uint8_t PWM_Brightness;


/* Private function prototypes -----------------------------------------------*/
static void CLK_Config(void);
static void GPIO_Config(void);
static void TIM1_Config(void);
static void TIM2_Config(void);
static void TIM3_Config(void);
static void TIM4_Config(void);
static void EXTI_Config(void);

static void US_Ping(void);

static void delay_us(uint16_t us);
static void delay_ms(uint16_t ms);

static void PWM_SetValue(uint8_t red, uint8_t blue, uint8_t green, uint8_t brightness);
static void PWM_Fade(uint8_t brightness, uint8_t set, int8_t add);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  if ( (PWM_Brightness == 0xFF) || (PWM_Brightness == 0x00) )
  {
    FLASH_Unlock(FLASH_MemType_Data); // unlock eeprom
  PWM_Brightness = 0xFF;
  PWM_Red = 255;
  PWM_Green = 255;
  PWM_Blue = 127;
  }
  
  
  Light_Status = 0; // off
   /* CLK configuration -------------------------------------------*/
  CLK_Config(); 

   /* GPIO configuration -------------------------------------------*/
  GPIO_Config(); 

   /* TIM1 configuration -------------------------------------------*/
  TIM1_Config();
  
   /* TIM2 configuration -------------------------------------------*/
  TIM2_Config();
   /* TIM2 configuration -------------------------------------------*/
  TIM3_Config();
   /* TIM4 configuration -------------------------------------------*/
  TIM4_Config();
   
   /* EXTIconfiguration -------------------------------------------*/
  EXTI_Config();

  //GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);
 
  enableInterrupts();
  
  //GPIO_SetBits(GPIOB, GPIO_Pin_2);
  value_summ = 0;
  counter = 0;
  Light_UpdateStatusFlag =0;
  PWM_SetValue(0,0,0,0);
  while (1)
  {
  //GPIO_ToggleBits(GPIOB, GPIO_Pin_2);
    //delay_ms(100);
    PWM_Fade(255,0,5);
    PWM_Fade(0,255,-5);
    /*
    US_Ping();
    while (US_Value == 0);
    value = (float)US_Value / 58;
    US_Value = 0;
    value_summ += value;
    counter++;
    if (counter == US_MEDIUM_DEVIDER) {
      medium_value = (value_summ / US_MEDIUM_DEVIDER);
      value_summ = 0;
      counter = 0;
      Light_UpdateStatusFlag = 1;
    }
    
    if (Light_UpdateStatusFlag) 
    {
      if ((medium_value <= US_MediumValueThreshold) && (Light_Status != 1)){
        Light_Status = 1;
        PWM_Fade(PWM_Brightness, 0, 1);
        // to fill
        TIM3_TimeBaseInit(TIM3_Prescaler_16, TIM3_CounterMode_Up, 1000);
        TIM3_ClearFlag(TIM3_FLAG_Update);
        TIM3_ITConfig(TIM3_IT_Update, ENABLE);    
        Light_Counter_ms =0;
        Light_Counter_s = 0;
        TIM3_Cmd(ENABLE);
      }
        
      
      if ((Light_Counter_s == Light_Delay) && (Light_Status != 0))
      {
        if (medium_value > US_MediumValueThreshold) {
          PWM_Fade(0, PWM_Brightness, -1);
          Light_Status = 0;
          TIM3_Cmd(DISABLE);
        } 
        else 
        {
          Light_Counter_s = 0;
        }
      }
      Light_UpdateStatusFlag = 0;
    } 
    
    /*

*/
  }
}
static void PWM_Fade(uint8_t brightness, uint8_t set, int8_t add)
{
  while (brightness != set)
  {
    PWM_SetValue(PWM_Red, PWM_Blue, PWM_Green, set);
    set += add;
    delay_ms(3);
  }
}
static void PWM_SetValue(uint8_t red, uint8_t blue, uint8_t green, uint8_t brightness) 
{
  TIM1_SetCompare1(blue * brightness);
  TIM1_SetCompare2(red * brightness);
  TIM1_SetCompare3(green * brightness);
}

/**
  * @brief  Configure peripherals Clock   
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
  /* Internal clock  16 MHZ */
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);  
}

/**
  * @brief  Configure TIM Channels GPIO 
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
  /* TIM1 Channels 1, 2, 3 and 1N configuration: PD2, PD4, PD5 */
  GPIO_Init(GPIOD, GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);

  /* TIM1 Channels 2N and 3N configuration: PE1 and PE2 */
  //GPIO_Init(GPIOE, GPIO_Pin_1 | GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);
  
  /* TIM1 Break input pin configuration: PD6 */
  /*GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT); */
  
  GPIO_Init(GPIOD, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);
}
/**
  * @brief  Configure EXTI peripheral 
  * @param  None
  * @retval None
  */
static void EXTI_Config(void)
{
  GPIO_Init(GPIOB, GPIO_Pin_0, GPIO_Mode_In_FL_IT);
  EXTI_SetPinSensitivity(EXTI_Pin_0, EXTI_Trigger_Rising_Falling);
}
/**
  * @brief  Configure TIM3 peripheral 
  * @param  None
  * @retval None
  */
static void TIM3_Config(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM3, ENABLE);
  
  TIM3_DeInit();
}
/**
  * @brief  Configure TIM4 peripheral 
  * @param  None
  * @retval None
  */
static void TIM4_Config(void)
{
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM4, ENABLE);
  
  TIM4_DeInit();
}
/**
  * @brief  Delay in microseconds using timer 4
  * @param  us: delay value in us
  * @retval None
  */
static void delay_us(uint16_t us)
{
        TIM4_Cmd(DISABLE);       // stop

        TIM4_TimeBaseInit(TIM4_Prescaler_4, TIM4_US_PERIOD);
        TIM4_ClearFlag(TIM4_FLAG_Update);
        TIM4_ITConfig(TIM4_IT_Update, ENABLE);
        
        Delay_count = us>>1;
        TIM4_Cmd(ENABLE);       // let's go

        while(Delay_count);
}
/**
  * @brief  Delay in miliseconds using timer 4
  * @param  ms: delay value in ms
  * @retval None
  */
static void delay_ms(uint16_t ms)
{
        TIM4_Cmd(DISABLE);       // stop

        TIM4_TimeBaseInit(TIM4_Prescaler_128, TIM4_PERIOD);
        TIM4_ClearFlag(TIM4_FLAG_Update);
        TIM4_ITConfig(TIM4_IT_Update, ENABLE);

        Delay_count = ms;
        TIM4_Cmd(ENABLE);       // let's go

        while(Delay_count);
}
/**
  * @brief  Configure TIM2 peripheral 
  * @param  None
  * @retval None
  */
static void TIM2_Config(void)
{
  /* Enable TIM1 clock */
  /* TIM1 configuration:
   - TIM1CLK is set to 2 MHz, the TIM2 Prescaler is equal to 0 so the TIM1 counter
   clock used is 2 MHz / (0+1) = 2 MHz
   - Channels output frequency = TIM1CLK / (TIM1_PERIOD + 1) * (TIM1_PRESCALER + 1)
                              = 2 000 000 / 65536 * 1 = 30.51 Hz
    
   Time Base configuration 
    set to 1 ms tick*/
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
}
/**
  * @brief  Configure TIM1 peripheral 
  * @param  None
  * @retval None
  */
static void TIM1_Config(void)
{
  /* Enable TIM1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);

  /* TIM1 configuration:
   - TIM1CLK is set to 2 MHz, the TIM2 Prescaler is equal to 0 so the TIM1 counter
   clock used is 2 MHz / (0+1) = 2 MHz
   - Channels output frequency = TIM1CLK / (TIM1_PERIOD + 1) * (TIM1_PRESCALER + 1)
                              = 2 000 000 / 65536 * 1 = 30.51 Hz
    
   Time Base configuration */
  TIM1_TimeBaseInit(TIM1_PRESCALER, TIM1_CounterMode_Up, TIM1_PERIOD, TIM1_REPTETION_COUNTER);

  /* Channels 1, 1N, 2, 2N, 3 and 3N Configuration in PWM2 mode */
  TIM1_OC1Init(TIM1_OCMode_PWM2, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, CCR1_VAL,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
  TIM1_OC2Init(TIM1_OCMode_PWM2, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, CCR2_VAL,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
  TIM1_OC3Init(TIM1_OCMode_PWM2, TIM1_OutputState_Enable, TIM1_OutputNState_Disable, CCR3_VAL,
               TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);

  /* Automatic Output Enable, Break, dead time and lock configuration */
  /* Enable the break feature with polarity low so applying a low lovel on PD6
     sets the non-inverted channels and inverted channels in high level 
    (TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set).
    Once the PD6 is connected to high level, the non-inverted channels and 
    inverted channels return to PWM state at the next update (TIM1_AutomaticOutput_Enable).
    Non-inverted Channels and inverted channels rising edge are delayed 
    by 58.5 us = DEADTIME / TIM1CLK = 117 / 2 MHz
    As the lock level is level 1, Channels Idle state, break enable and polarity 
    bits can no longer be written.
  */
  /* TIM1_BDTRConfig(TIM1_OSSIState_Enable, TIM1_LockLevel_1, DEADTIME,
                  TIM1_BreakState_Enable, TIM1_BreakPolarity_Low, TIM1_AutomaticOutput_Enable); */

  /* Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);

  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);
}
/**
  * @brief  Send 10us ping to Ultrasonic module
  * @param  None
  * @retval None
  */
static void US_Ping(void)
{
  US_Value = 0;
  TIM2_Counter = 0;
  
  GPIO_SetBits(GPIOD, GPIO_Pin_3);
  delay_us(10);
  GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  
  TIM2_TimeBaseInit(TIM2_Prescaler_16, TIM2_CounterMode_Up, 65535);
  TIM2_ClearFlag(TIM2_FLAG_Update);
  TIM2_ITConfig(TIM2_IT_Update, ENABLE);    
  
  US_PendingResultFlag = 1;
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/