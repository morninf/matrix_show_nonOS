#include <string.h>

#include "sys.h"

#include "word.h"
#include "delay.h"
#include "matrix_show.h"

#ifdef _WIN32
#include <stdio.h> /* for putchar */
#endif

uint8_t matrix_out_data[FONT_LEN][AS_MATRIX_MAX_LEN];
//uint8_t **matrix_out_data;

/*
	A------>PA0
	B------>PA1
	R1----->PF0
	G1----->PF1
	B1----->PF2
	R2----->PF3
	G2----->PF4
	B2----->PF5
	CLK---->PF6
	LAT---->PF7
	OE----->PF8
*/

/* defined the address A pin: PA0 */
#define MATRIX_ADDR_PORT	GPIOA
#define MATRIX_ADDR_A_PIN    GPIO_Pin_0
/* defined the address A pin: PA1 */
#define MATRIX_ADDR_B_PIN    GPIO_Pin_1

/* defined the R1 pin: PF0 */
#define MATRIX_DATA_PORT	GPIOF

#define MATRIX_R1_PIN    GPIO_Pin_0
/* defined the G1 pin: PF1 */
#define MATRIX_G1_PIN    GPIO_Pin_1
/* defined the B1 pin: PF2 */
#define MATRIX_B1_PIN    GPIO_Pin_2

/* defined the R2 pin: PF3 */
#define MATRIX_R2_PIN    GPIO_Pin_3
/* defined the G2 pin: PF4 */
#define MATRIX_G2_PIN    GPIO_Pin_4
/* defined the B2 pin: PF5 */
#define MATRIX_B2_PIN    GPIO_Pin_5


/* defined the CLK pin: PF6 */
#define MATRIX_CLK_PIN    GPIO_Pin_6
/* defined the LAT pin: PF7 */
#define MATRIX_LAT_PIN    GPIO_Pin_7
/* defined the OE pin: PF8 */
#define MATRIX_OE_PIN    GPIO_Pin_8


#define MATRIX_CLK_H			GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_CLK_PIN)
#define MATRIX_CLK_L			GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_CLK_PIN)

#define MATRIX_LAT_H			GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_LAT_PIN)
#define MATRIX_LAT_L			GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_LAT_PIN)

#define MATRIX_OE_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_OE_PIN)
#define MATRIX_OE_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_OE_PIN)

#define MATRIX_R1_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_R1_PIN)
#define MATRIX_R1_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_R1_PIN)
#define MATRIX_G1_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_G1_PIN)
#define MATRIX_G1_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_G1_PIN)
#define MATRIX_B1_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_B1_PIN)
#define MATRIX_B1_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_B1_PIN)

#define MATRIX_R2_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_R2_PIN)
#define MATRIX_R2_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_R2_PIN)
#define MATRIX_G2_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_G2_PIN)
#define MATRIX_G2_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_G2_PIN)
#define MATRIX_B2_H				GPIO_SetBits(MATRIX_DATA_PORT,MATRIX_B2_PIN)
#define MATRIX_B2_L				GPIO_ResetBits(MATRIX_DATA_PORT,MATRIX_B2_PIN)

static void matrix_io_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟
	
	//GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = MATRIX_ADDR_A_PIN | MATRIX_ADDR_B_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(MATRIX_ADDR_PORT, &GPIO_InitStructure);//初始化


	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);//使能GPIOF时钟
	
	//GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = 0x1FF;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(MATRIX_DATA_PORT, &GPIO_InitStructure);//初始化
	
}

static void matrix_set_line(uint8_t line)
{
	//two address port include 0b'00 01 10 11.
	GPIO_Write(MATRIX_ADDR_PORT,line&0x03);
}

static void matrix_show( uint8_t matrix_out[][AS_MATRIX_MAX_LEN],int start)
{
	uint8_t dat_low;
	uint8_t dat_low1;
	
	uint8_t dat_high;
	uint8_t dat_high1;

	int start_temp = start;
	
	for(int n = 0; n<4; n++)
	{
		matrix_set_line((uint8_t)(n));
		start_temp = start;
		for( int i = 0; i< SHOW_LEN; i++)
		{
			MATRIX_OE_H;
			MATRIX_LAT_L;
			
			if( start_temp  >= (TRUE_WORD_LEN +  BLANK_MATRIX_LEN))
				start_temp = 0;
			
			dat_high = matrix_out[n][start_temp];
			dat_high1 = matrix_out[n+4][start_temp];
			
			dat_low = matrix_out[n+8][start_temp];
			dat_low1 = matrix_out[n+12][start_temp];

			start_temp++;
			for( int k = 0; k < 8; k++)
			{
				MATRIX_CLK_L;
				if( dat_high1 & (1<<(7-k)))
				{
					MATRIX_R1_H;
				}
				else
				{
					MATRIX_R1_L;
				}
				
				if( dat_low1 & (1<<(7-k)))
				{
					MATRIX_R2_H;
				}
				else
				{
					MATRIX_R2_L;
				}
			MATRIX_CLK_H;
		}
			
		for( int k = 0; k < 8; k++)
		{

				MATRIX_CLK_L;
				if( dat_high & (1<<k))
				{
					MATRIX_R1_H;
				}
				else
				{
					MATRIX_R1_L;
				}
				if( dat_low & (1<<k))
				{
					MATRIX_R2_H;
				}
				else
				{
					MATRIX_R2_L;
				}
			MATRIX_CLK_H;
		}

		}
		MATRIX_LAT_H;
		MATRIX_LAT_L;
		MATRIX_OE_L;
		delay_ms(2);
	}
}

static void matrix_buffer_prepare(void)
{
	for(int i =0; i < FONT_LEN; i++)
	{
		for( int j = 0; j < TRUE_WORD_LEN; j++)
		{
			matrix_out_data[i][j+BLANK_MATRIX_LEN] = word_date[j/2][i*2 + (j%2) ];
		}
	}
}

void matrix_show_round(void)
{
	static int start = 0;
	//while(1)
	{
		for(int i =0 ; i<40 ;i++)
			matrix_show(matrix_out_data,start);
		start +=1;
		if(start >= TRUE_WORD_LEN +  BLANK_MATRIX_LEN)
			start = 0;
	}

}

int matrix_show_init(void)
{
	matrix_io_init();

	matrix_buffer_prepare();

	return 0;

}


