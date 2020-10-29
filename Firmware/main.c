/**
  ******************************************************************************
  * @file    GPIO/GPIO_Toggle/main.c
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
#include "stm8l15x_clk.h"
#include "stm8l15x_tim1.h"
#include "stm8l15x_tim2.h"
#include "stm8l15x_adc.h"
#include "stm8l15x_dma.h"
#include "stm8l15x_syscfg.h"
#include "stm8l15x_it.h"
/** @addtogroup STM8L15x_StdPeriph_Examples
  * @{
  */

/** @addtogroup GPIO_Toggle
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define USE_STM8L1526_EVAL

#define TIM1_PERIOD                    512
#define TIM1_PRESCALER                   0
#define TIM1_REPTETION_COUNTER           0

/* Channel 1 duty cycle = 100 * CCR1_VAL / TIM1_PERIOD + 1
                        = 32767 * 100(65535 + 1) = 50 %*/
#define CCR1_VAL                     255
/* Channel 2 duty cycle = 100 * CCR2_VAL / TIM1_PERIOD + 1
                        = 16383 * 100 / (65535 + 1) = 25 %*/
#define CCR2_VAL                     16383

/* Channel 3 duty cycle = 100 * CCR3_VAL / TIM1_PERIOD + 1
                        = 8191 * 100 / (65535 + 1) = 12.5 %*/
#define CCR3_VAL                      8191

#define DEADTIME                       7

#define ADC1_DR_ADDRESS        ((uint16_t)0x5344)
#define BUFFER_SIZE            ((uint8_t) 0x02)
#define BUFFER_ADDRESS         ((uint16_t)(&Buffer))


/* define the GPIO port and pins connected to Leds mounted on STM8L152X-EVAL board */
#ifdef USE_STM8L1526_EVAL
#define LED_GPIO_PORT  GPIOA
#define LED_GPIO_PINS  GPIO_Pin_4 | GPIO_Pin_6 
#elif defined USE_STM8L1528_EVAL
#define LED_GPIO_PORT  GPIOH
#define LED_GPIO_PINS  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3
#else
#error "Please select first the STM8L15X_EVAL board to be used "
#endif

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t PWM = 0xFFFF;

uint16_t Buffer[BUFFER_SIZE]  = {0, 0};
/* Private function prototypes -----------------------------------------------*/
void Delay (uint16_t nCount);
static void CLK_Config(void);
static void GPIO_Config(void);
static void TIM1_Config(void);
static void ADC_Config(void);
static void DMA_Config(void);
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void main(void)
{
  /* Initialize LEDs mounted on STM8L152X-EVAL board */
  //CLK_SYSCLKDiv_2
  CLK_SYSCLKSourceSwitchCmd(ENABLE);
  CLK_SYSCLKSourceConfig(CLK_SYSCLKSource_HSI);
  CLK_SYSCLKDivConfig(CLK_SYSCLKDiv_1);
    
  CLK_Config(); 
  GPIO_Config(); 
  GPIO_Init(LED_GPIO_PORT, LED_GPIO_PINS, GPIO_Mode_Out_PP_Low_Fast);
  GPIO_WriteBit(LED_GPIO_PORT, GPIO_Pin_4, SET);
  
  TIM1_Config();
  
  ADC_Config();
  DMA_Config();
  
  
  /* Enable ADC1 DMA requests*/
  ADC_DMACmd(ADC1, ENABLE);

  /* Start ADC1 Conversion using TIM1 TRGO*/
  ADC_ExternalTrigConfig(ADC1, ADC_ExtEventSelection_Trigger2,
                         ADC_ExtTRGSensitivity_Rising);

  /* Master Mode selection: Update event */
  TIM1_SelectOutputTrigger(TIM1_TRGOSource_Update);

  /* Enable TIM1 */
  TIM1_Cmd(ENABLE);
  /* Enable Interrupts */
  enableInterrupts();

   /* GPIO configuration -------------------------------------------*/
  

   /* TIM1 configuration -------------------------------------------*/
 

  while (1)
  {
    PWM++;
    /* Toggle LEDs LD1..LD4 */
    GPIO_ToggleBits(LED_GPIO_PORT, GPIO_Pin_6);
   // for (PWM = 0x1FFF; PWM < 0xFFF0; PWM++) {
      TIM1->CCR1H = (uint8_t)(PWM >> 8);
      TIM1->CCR1L = (uint8_t)(PWM);
   //   Delay(0xFF);
   // }
   // for (PWM = 0xF000; PWM > 0xFF0; PWM--) {
   //   TIM1->CCR1H = (uint8_t)(PWM >> 8);
   //   TIM1->CCR1L = (uint8_t)(PWM);
   //   Delay(0xFF);
   // }
      
    //PWM += 0xF;
    Delay(0xFFFF);
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: specifies the delay time length.
  * @retval None
  */
void Delay(__IO uint16_t nCount)
{
  /* Decrement nCount value */
  while (nCount != 0)
  {
    nCount--;
  }
}

/**
  * @brief  Configure peripherals Clock   
  * @param  None
  * @retval None
  */
static void CLK_Config(void)
{
  /* Enable TIM1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM1, ENABLE);
  CLK_PeripheralClockConfig(CLK_Peripheral_TIM2, ENABLE);
  /* Enable ADC1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_ADC1, ENABLE);

  /* Enable DMA1 clock */
  CLK_PeripheralClockConfig(CLK_Peripheral_DMA1, ENABLE);

}


/**
  * @brief  Configure ADC peripheral 
  * @param  None
  * @retval None
  */
static void ADC_Config(void)
{
  /* Initialize and configure ADC1 */
  ADC_Init(ADC1, ADC_ConversionMode_Single, ADC_Resolution_12Bit, ADC_Prescaler_2);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_SlowChannels, ADC_SamplingTime_384Cycles);
  ADC_SamplingTimeConfig(ADC1, ADC_Group_FastChannels, ADC_SamplingTime_384Cycles);

  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);

  /* Enable ADC1 Channels 3 */
  ADC_ChannelCmd(ADC1, ADC_Channel_22, ENABLE); /* connected to Potentiometer RV */
  /* Enable ADC1 Channels 24 */
  ADC_ChannelCmd(ADC1, ADC_Channel_21, ENABLE); /* connected to BNC */
}

/**
  * @brief  Configure DMA peripheral 
  * @param  None
  * @retval None
  */
static void DMA_Config(void)
{
  /* Connect ADC to DMA channel 0 */
  SYSCFG_REMAPDMAChannelConfig(REMAP_DMA1Channel_ADC1ToChannel0);

  DMA_Init(DMA1_Channel0, BUFFER_ADDRESS,
           ADC1_DR_ADDRESS,
           BUFFER_SIZE,
           DMA_DIR_PeripheralToMemory,
           DMA_Mode_Circular,
           DMA_MemoryIncMode_Inc,
           DMA_Priority_High,
           DMA_MemoryDataSize_HalfWord);

  /* DMA Channel0 enable */
  DMA_Cmd(DMA1_Channel0, ENABLE);

  /* Enable DMA1 channel0 Transfer complete interrupt */
  DMA_ITConfig(DMA1_Channel0, DMA_ITx_TC, ENABLE);

  /* DMA enable */
  DMA_GlobalCmd(ENABLE);
}


/**
  * @brief  Configure TIM Channels GPIO 
  * @param  None
  * @retval None
  */
static void GPIO_Config(void)
{
  /* TIM1 Channels 1, 2, 3 and 1N configuration: PD2, PD4, PD5 and PD7 */
  GPIO_Init(GPIOD, GPIO_Pin_2 | GPIO_Pin_7, GPIO_Mode_Out_PP_Low_Fast);
  /* TIM1 Break input pin configuration: PD6 */
  GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_In_PU_No_IT);
}
/**
  * @brief  Configure TIM1 peripheral 
  * @param  None
  * @retval None
  */
static void TIM1_Config(void)
{
  /* TIM1 configuration:
   - TIM1CLK is set to 2 MHz, the TIM2 Prescaler is equal to 0 so the TIM1 counter
   clock used is 2 MHz / (0+1) = 2 MHz
   - Channels output frequency = TIM1CLK / (TIM1_PERIOD + 1) * (TIM1_PRESCALER + 1)
                              = 2 000 000 / 65536 * 1 = 30.51 Hz
    
   Time Base configuration */
  TIM1_TimeBaseInit(TIM1_PRESCALER, TIM1_CounterMode_Up, TIM1_PERIOD, TIM1_REPTETION_COUNTER);

  /* Channels 1, 1N, 2, 2N, 3 and 3N Configuration in PWM2 mode */
  TIM1_OC1Init(TIM1_OCMode_PWM2, TIM1_OutputState_Enable, TIM1_OutputNState_Enable, CCR1_VAL,
               TIM1_OCPolarity_High, TIM1_OCNPolarity_High, TIM1_OCIdleState_Reset, TIM1_OCNIdleState_Reset);
  TIM1_OC2Init(TIM1_OCMode_PWM2, TIM1_OutputState_Enable, TIM1_OutputNState_Enable, CCR2_VAL,
 //              TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);
  TIM1_OC3Init(TIM1_OCMode_PWM2, TIM1_OutputState_Enable, TIM1_OutputNState_Enable, CCR3_VAL,
 //              TIM1_OCPolarity_Low, TIM1_OCNPolarity_Low, TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set);

  /* Automatic Output Enable, Break, dead time and lock configuration */
  /* Enable the break feature with polarity low so applying a low lovel on PD6
     sets the non-inverted channels and inverted channels in high level 
    (TIM1_OCIdleState_Set, TIM1_OCNIdleState_Set).
    Once the PD6 is connected to high level, the non-inverted channels and 
    inverted channels return to PWM state at the next update (TIM1_AutomaticOutput_Enable).
    Non-inverted Channels and inverted channels rising edge are delayed 
    by 58.5 ?s = DEADTIME / TIM1CLK = 117 / 2 MHz
    As the lock level is level 1, Channels Idle state, break enable and polarity 
    bits can no longer be written.
  */
  //TIM1_BDTRConfig(TIM1_OSSIState_Disable, TIM1_LockLevel_1, DEADTIME,
                  TIM1_BreakState_Enable, TIM1_BreakPolarity_High, TIM1_AutomaticOutput_Enable);

  /* Main Output Enable */
  TIM1_CtrlPWMOutputs(ENABLE);

  /* TIM1 counter enable */
  TIM1_Cmd(ENABLE);
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
