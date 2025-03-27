#pragma once

#ifndef _LIB_ST7735_H_
#define _LIB_ST7735_H_

///�豸ѡ��
#define ESP32_SPI   1

///��Ļ����
#define     LCD_DIR_VAERTICAL1      0           ///<����1
#define     LCD_DIR_VAERTICAL2      1           ///<����2
#define     LCD_DIR_TRANSVERSET1    2           ///<����1
#define     LCD_DIR_TRANSVERSET2    3           ///<����2

#define     LCD_DIR  LCD_DIR_VAERTICAL2         ///<��ʾ����


#if (LCD_DIR==LCD_DIR_VAERTICAL1||LCD_DIR==LCD_DIR_VAERTICAL2)
#define LCD_ROW_SIZE        128                 ///<��Ļ������
#define LCD_COLUMN_SIZE     160                 ///<��Ļ������
#else
#define LCD_ROW_SIZE        160                 ///<��Ļ������
#define LCD_COLUMN_SIZE     128                 ///<��Ļ������
#endif



#define LCD_DELAY(t)    vTaskDelay(t/portTICK_PERIOD_MS)       ///<��ʱ����

///spi
#define LCD_HARDWARE_CS 1                   ///< 1ѡ�� Ӳ��CS 0ѡ�� ����gpio CS
#if ESP32_SPI
#define LCD_SPI_HOST    SPI3_HOST           //spi
#define LCD_SPI_MODE    0                   ///<spiģʽ 4�� 0~3
//HSPI
// #define LCD_PIN_CS      GPIO_NUM_15         ///< Ƭѡ
// #define LCD_PIN_RES     GPIO_NUM_2          ///< ��λ
// #define LCD_PIN_A0      GPIO_NUM_3          ///< ����/����
// #define LCD_PIN_SDA     GPIO_NUM_13         ///< MOSI
// #define LCD_PIN_SCL     GPIO_NUM_14         ///< spi CLK
//VSPI
#define LCD_PIN_CS      GPIO_NUM_16         ///< Ƭѡ
#define LCD_PIN_RES     GPIO_NUM_7          ///< ��λ
#define LCD_PIN_A0      GPIO_NUM_15          ///< ����/���� DC
#define LCD_PIN_SDA     GPIO_NUM_6         ///< MOSI
#define LCD_PIN_SCL     GPIO_NUM_5         ///< spi CLK

/*io����*/
#define SET_LCD_CS 	    gpio_set_level(LCD_PIN_CS, 1)
#define SET_LCD_RES     gpio_set_level(LCD_PIN_RES, 1)
#define SET_LCD_A0 	    gpio_set_level(LCD_PIN_A0, 1)
#define SET_LCD_SDA     gpio_set_level(LCD_PIN_SDA, 1)
#define SET_LCD_SCL     gpio_set_level(LCD_PIN_SCL, 1)

#define	CLR_LCD_CS      gpio_set_level(LCD_PIN_CS, 0)
#define	CLR_LCD_RES     gpio_set_level(LCD_PIN_RES, 0)
#define	CLR_LCD_A0      gpio_set_level(LCD_PIN_A0, 0)
#define	CLR_LCD_SDA     gpio_set_level(LCD_PIN_SDA, 0)
#define	CLR_LCD_SCL     gpio_set_level(LCD_PIN_SCL, 0)
#endif


/*--��ɫ--*/
#define WHITE	    0xFFFF
#define BLACK	    0x0000
#define BLUE	    0x001F
#define BRED	    0XF81F
#define GRED	    0XFFE0
#define GBLUE	    0X07FF
#define RED		    0xF800
#define MAGENTA	    0xF81F
#define GREEN	    0x07E0
#define CYAN	    0x7FFF
#define YELLOW	    0xFFE0
#define BROWN	    0XBC40 //��ɫ
#define BRRED	    0XFC07 //�غ�ɫ
#define GRAY	    0X8430 //��ɫ
#define DARKBLUE	0X01CF	//����ɫ
#define LIGHTBLUE	0X7D7C	//ǳ��ɫ
#define GRAYBLUE	0X5458 //����ɫ


void LcdGpioSpiInit(void);
/// @brief ��Ļ��ʼ��
/// @param  None
void lcdInit(void);
void lcdSetAddress(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2);
/// @brief ����
/// @param color �����ɫ
void lcdClear(unsigned short color);

/// @brief ����
/// @param x λ��
/// @param y λ��
/// @param color ��ɫ

void LCD_DisplayOn(void); //����ʾ
void LCD_DisplayOff(void); //����ʾ
void LCD_Fill(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short color);
void LCD_flush(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short *color_p); //�������
void lcdClear(uint16_t color);
static void lcdSetRowAddress(uint16_t y);
static void SpiSendBlock(const void* data, size_t length);
static void LCD_WriteBlock(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height, uint16_t *color_p);

#endif

