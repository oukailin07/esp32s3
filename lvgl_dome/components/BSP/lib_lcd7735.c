#include <stdio.h>
#include "lib_lcd7735.h"
#include "ascii_font.h"
//#include "LCD_ST7735.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

spi_device_handle_t tft_hspi = NULL;					///<spi�豸���?
static void lcdSelectRegister(unsigned char data);
static void lcdWriteDataU8(unsigned char data);
static void lcdWriteDataU16(unsigned short data);

static void SpiSend(uint8_t *data, uint8_t dataLength);

/**
  * @brief spi��������
  * @note  ���޸������Լ���ͬ��Ӳ��
  * @param data ��Ҫ���͵�����
  * @param dataLength ���ݵĸ���(1BYTE Ϊ��λ)
  * @return ����״̬
  */
static void SpiSend(uint8_t *data, uint8_t dataLength)
{

	//spi��������
	spi_transaction_t ext;  		///<����̶����Ƚṹ��?
	memset(&ext, 0, sizeof(ext));  	///<�����ڴ�
	//ext.command_bits = 0;  		///<
	ext.rxlength = 0 ;				///<����
	ext.length = 8 * dataLength;		///<д��λ��
	ext.tx_buffer =	data;
	ext.user = (void*)1; 
	// esp_err_t  lcd_err = spi_device_transmit(tft_hspi, &ext); 			///<�жϴ���
	esp_err_t  lcd_state = spi_device_polling_transmit(tft_hspi, &ext); 	///<��ѯ����
	if (lcd_state != ESP_OK)
		printf("lcd_err	-->%d\n", lcd_state);
}



/**
  * @brief spi gpio��ʼ��
  * @note  ���޸������Լ���ͬ��Ӳ��
  */
void LcdGpioSpiInit(void)
{
	//--gpio ���� add your code
	gpio_config_t lcd_io = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1 << LCD_PIN_RES)  | (1 << 16) | (1<<15)| (1<<6)| (1<<5),	///<RES GPIO
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en	= GPIO_PULLDOWN_DISABLE,
	};
	gpio_config(&lcd_io);

	//��������  add your code
	spi_bus_config_t buscfg = {
		.miso_io_num = -1,   			///<gpio12->miso
		.mosi_io_num = LCD_PIN_SDA,    	///<gpio13->mosi
		.sclk_io_num = LCD_PIN_SCL,   	///<gpio14-> sclk
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.flags = SPICOMMON_BUSFLAG_MASTER , //SPI��ģʽ
		.max_transfer_sz = 4096 * 8,
	};
		//buscfg.intr_flags = 0;  							///<�����������SPIͨѶ����ص��жϺ������ж����ȼ���?0��Ĭ�ϡ�
	esp_err_t tft_spi_f = spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);		///<���߳�ʼ��������ʹ��DMA����
	if (tft_spi_f != ESP_OK) {
		printf("--tft--spi--bus--initialize--err,%d\n", tft_spi_f);						///<������Ϣ��ӡ
	}

	//�豸���ýṹ�� add your code
	spi_device_interface_config_t interface_config = {
		.address_bits = 0,
		.input_delay_ns = 0,
		.command_bits = 0,
		.dummy_bits = 0,
		.clock_speed_hz = 40 * 1000 * 1000,
		.mode = LCD_SPI_MODE, 				///<����SPIͨѶ����λ���ԺͲ������ء�������mode0-3���֡�Ҫ�����豸�ܹ�ʹ������ģʽ
#if LCD_HARDWARE_CS						///<��ѡ��Ӳ��CS
		///����ǰ��Ƭѡ ����ʱ���ٴ����ݲ�Ȼ����ʧ��
		// .cs_ena_pretrans = 2,
		// .cs_ena_posttrans = 2,
		.spics_io_num = -1,	//LCD_PIN_CS,			///<����Ƭѡ��
#endif
		.pre_cb=NULL,
		.post_cb=NULL,//NULL
		.duty_cycle_pos = 0,
		.queue_size = 6 					///<������еĳ��ȣ���ʾ������ͨѶ��ʱ�������ٸ�spiͨѶ�����ж�ͨѶģʽ��ʱ���ѵ�ǰspiͨѶ���̹��𵽶�����
	};

	tft_spi_f = spi_bus_add_device(LCD_SPI_HOST, &interface_config, &tft_hspi);		///<����spi�豸
	if (tft_spi_f != ESP_OK) {
		printf("--tft--spi--deiver--config--err,%d\n", tft_spi_f);					///<������Ϣ��ӡ
	}
}


//������dc����c
static void lcdSelectRegister(unsigned char com)
{
	CLR_LCD_CS;
	CLR_LCD_A0;  ///<����
	SpiSend(&com, 1); 	///<��8bit
	SET_LCD_CS;
}
//������dc-d
static void lcdWriteDataU8(unsigned char data)
{
	CLR_LCD_CS;
	SET_LCD_A0;
	SpiSend(&data, 1);
	SET_LCD_CS;
}

//��16λ����
static void lcdWriteDataU16(unsigned short data)
{
	lcdWriteDataU8(data >> 8);
	lcdWriteDataU8(data);

	// SET_LCD_A0;
	// #if !(LCD_HARDWARE_CS)

	//--*ESP32 ����С��оƬ���� uint16_t �� uint32_t ������������?λ�洢����С�ĵ�ַ����ˣ���� uint16_t �洢���ڴ��У������ȷ���λ [7:0]��������? [15:8]��*/
	/*idf����ֲ�? :��ĳЩ����£��?��������ݴ�С��? uint8_t ���鲻ͬ����ʹ�����º꽫����ת��Ϊ���� SPI ����ֱ�ӷ��͵ĸ�ʽ��
		�贫������ݣ�ʹ��? SPI_SWAP_DATA_TX
		���յ������ݣ�ʹ�� SPI_SWAP_DATA_RX
		*/
	// 	CLR_LCD_CS;	 ///<ʹ���豸
	// #endif
	// //LCD_DELAY(1);
	// SpiSend((uint8_t*)&data,2);
	// //LCD_DELAY(1);
	// #if !(LCD_HARDWARE_CS)
	// 	SET_LCD_CS;
	// #endif

}


void lcdInit(void)
{
	//gpio
	LcdGpioSpiInit();
	gpio_set_level(4,1);
	//��λ
	SET_LCD_RES;
	LCD_DELAY(10);
	CLR_LCD_RES;
	LCD_DELAY(100);
	SET_LCD_RES;
	LCD_DELAY(200);
	//
	lcdSelectRegister(0x11); //Sleep out
	LCD_DELAY(120);

	lcdSelectRegister(0xB1);
	lcdWriteDataU8(0x05);
	lcdWriteDataU8(0x3C);
	lcdWriteDataU8(0x3C);

	lcdSelectRegister(0xB2);
	lcdWriteDataU8(0x05);
	lcdWriteDataU8(0x3C);
	lcdWriteDataU8(0x3C);

	lcdSelectRegister(0xB3);
	lcdWriteDataU8(0x05);
	lcdWriteDataU8(0x3C);
	lcdWriteDataU8(0x3C);
	lcdWriteDataU8(0x05);
	lcdWriteDataU8(0x3C);
	lcdWriteDataU8(0x3C);

	lcdSelectRegister(0xB4);
	lcdWriteDataU8(0x03);

	lcdSelectRegister(0xC0);
	lcdWriteDataU8(0x28);
	lcdWriteDataU8(0x08);
	lcdWriteDataU8(0x04);

	lcdSelectRegister(0xC1);
	lcdWriteDataU8(0XC0);

	lcdSelectRegister(0xC2);
	lcdWriteDataU8(0x0D);
	lcdWriteDataU8(0x00);

	lcdSelectRegister(0xC3);
	lcdWriteDataU8(0x8D);
	lcdWriteDataU8(0x2A);

	lcdSelectRegister(0xC4);
	lcdWriteDataU8(0x8D);
	lcdWriteDataU8(0xEE);

	lcdSelectRegister(0xC5);
	lcdWriteDataU8(0x1A);

	///<��ʾ����/
	lcdSelectRegister(0x36);
#if LCD_DIR==LCD_DIR_VAERTICAL1
	lcdWriteDataU8(0x00);
#elif (LCD_DIR==LCD_DIR_VAERTICAL2)
	lcdWriteDataU8(0xC0);
#elif (LCD_DIR==LCD_DIR_TRANSVERSEt1)
	lcdWriteDataU8(0x70);
#else
	lcdWriteDataU8(0xA0);
#endif
	lcdSelectRegister(0xE0);
	lcdWriteDataU8(0x04);
	lcdWriteDataU8(0x22);
	lcdWriteDataU8(0x07);
	lcdWriteDataU8(0x0A);
	lcdWriteDataU8(0x2E);
	lcdWriteDataU8(0x30);
	lcdWriteDataU8(0x25);
	lcdWriteDataU8(0x2A);
	lcdWriteDataU8(0x28);
	lcdWriteDataU8(0x26);
	lcdWriteDataU8(0x2E);
	lcdWriteDataU8(0x3A);
	lcdWriteDataU8(0x00);
	//lcdWriteDataU8(0x05);
	lcdWriteDataU8(0x01);
	lcdWriteDataU8(0x03);
	lcdWriteDataU8(0x13);

	lcdSelectRegister(0xE1);
	lcdWriteDataU8(0x04);
	lcdWriteDataU8(0x16);
	lcdWriteDataU8(0x06);
	lcdWriteDataU8(0x0D);
	lcdWriteDataU8(0x2D);
	lcdWriteDataU8(0x26);
	lcdWriteDataU8(0x23);
	lcdWriteDataU8(0x27);
	lcdWriteDataU8(0x27);
	lcdWriteDataU8(0x25);
	lcdWriteDataU8(0x2D);
	lcdWriteDataU8(0x3B);
	lcdWriteDataU8(0x00);
	lcdWriteDataU8(0x01);
	lcdWriteDataU8(0x04);
	lcdWriteDataU8(0x13);

	lcdSelectRegister(0x3A);
	lcdWriteDataU8(0x05);

	lcdSelectRegister(0x29);
}

//���û��Ƶ�ַ
void lcdSetAddress(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
	
#if (LCD_DIR==LCD_DIR_VAERTICAL1||LCD_DIR==LCD_DIR_VAERTICAL2)
	lcdSelectRegister(0x2A);   ///
	//��ʼ�е�ַ
	lcdWriteDataU16(x1); ///st7735s��Ļ�����е�ƫ�ƣ�+2����
	//�����е�ַ
	lcdWriteDataU16(x2);
	lcdSelectRegister(0x2B); 	///
	//��ʼ
	lcdWriteDataU16(y1);
	//������
	lcdWriteDataU16(y2);
#else
	lcdSelectRegister(0x2A);   ///
	//��ʼ�е�ַ
	lcdWriteDataU16(x1); ///��Ļ�����е�ƫ�ƣ�+2����
	//�����е�ַ
	lcdWriteDataU16(x2);
	lcdSelectRegister(0x2B); 	///
	//��ʼ
	lcdWriteDataU16(y1 + 2);
	//������
	lcdWriteDataU16(y2 + 2);
#endif

	lcdSelectRegister(0x2C);	///<Memory Write
}

//����
void lcdClear(unsigned short color)
{
	lcdSetAddress(0, 0, LCD_ROW_SIZE, LCD_COLUMN_SIZE);///<������Ļ��С

	for (unsigned char i = 0; i <= LCD_ROW_SIZE; i++)
	{
		for (unsigned char j = 0; j <= LCD_COLUMN_SIZE; j++)
		{
			lcdWriteDataU16(color);
		}
	}
}

//LCD����
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	lcdSetAddress(x, y, x, y);//���ù��λ��?
	lcdWriteDataU16(color);
}

//���ٻ���
//x,y:����
//color:��ɫ
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	lcdWriteDataU16(color);
}

//LCD������ʾ
void LCD_DisplayOn(void)
{
	lcdSelectRegister(0x29);	//������ʾ
}
//LCD�ر���ʾ
void LCD_DisplayOff(void)
{
	lcdSelectRegister(0x28);	//�ر���ʾ
}

//��ָ����������䵥�����?
//(sx,sy),(ex,ey):�����ζԽ�����,�����С�?:(ex-sx+1)*(ey-sy+1)
//color:Ҫ������ɫ
void LCD_flush(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short *color_p)
{
	unsigned short i, j;
	unsigned short xlen = 0;
	unsigned short ylen = 0;

	xlen = ex - sx + 1;
	ylen = ey - sy + 1;

	lcdSetAddress(sx, sy, ex, ey);
	for (i = 0; i < xlen; i++)
	{
		for (j = 0; j < ylen; j++)
		{
			uint16_t color = color_p[i * ylen + j];
			lcdWriteDataU16(color);
		}
	}
}

void LCD_Fill(unsigned short sx, unsigned short sy, unsigned short ex, unsigned short ey, unsigned short color)
{
	unsigned short i, j;
	unsigned short xlen = 0;
	unsigned short ylen = 0;

	xlen = ex - sx + 1;
	ylen = ey - sy + 1;

	lcdSetAddress(sx, sy, ex, ey);
	for (i = 0; i < xlen; i++)
	{
		for (j = 0; j < ylen; j++)
		{
			lcdWriteDataU16(color);
		}
	}
}

//����
//x1,y1:�������?
//x2,y2:�յ�����
void LCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
	unsigned short t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //������������
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

	//���õ�������
	if ( delta_x > 0 )
	{
		incx = 1;
	}
	else if ( delta_x == 0 ) //��ֱ��
	{
		incx = 0;
	}
	else
	{
		incx = -1;
		delta_x = -delta_x;
	}


	if ( delta_y > 0 )
	{
		incy = 1;
	}
	else if ( delta_y == 0 ) //ˮƽ��
	{
		incy = 0;
	}
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}

	if ( delta_x > delta_y ) //ѡȡ��������������
	{
		distance = delta_x;
	}
	else
	{
		distance = delta_y;
	}

	for (t = 0; t <= distance + 1; t++ ) //�������?
	{
		LCD_DrawPoint(uRow, uCol, color);//����
		xerr += delta_x ;
		yerr += delta_y ;
		if ( xerr > distance )
		{
			xerr -= distance;
			uRow += incx;
		}

		if ( yerr > distance )
		{
			yerr -= distance;
			uCol += incy;
		}
	}
}
/// @brief ���ƾ���
/// @param x1 	stort x
/// @param y1 	stort y
/// @param x2 	end	x
/// @param y2 	end y
/// @param color ��ɫ
void LCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
	LCD_DrawLine(x1, y1, x2, y1, color);
	LCD_DrawLine(x1, y1, x1, y2, color);
	LCD_DrawLine(x1, y2, x2, y2, color);
	LCD_DrawLine(x2, y1, x2, y2, color);
}

//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_Draw_Circle(unsigned short x0, unsigned short y0, unsigned char r, unsigned short color)
{
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - ( r << 1 );           //�ж��¸���λ�õı�־
	while ( a <= b )
	{
		LCD_DrawPoint(x0 + a, y0 - b, color);
		LCD_DrawPoint(x0 + b, y0 - a, color);
		LCD_DrawPoint(x0 + b, y0 + a, color);
		LCD_DrawPoint(x0 + a, y0 + b, color);
		LCD_DrawPoint(x0 - a, y0 + b, color);
		LCD_DrawPoint(x0 - b, y0 + a, color);
		LCD_DrawPoint(x0 - a, y0 - b, color);
		LCD_DrawPoint(x0 - b, y0 - a, color);
		a++;
		//ʹ��Bresenham�㷨��Բ
		if ( di < 0 )
		{
			di += 4 * a + 6;
		}
		else
		{
			di += 10 + 4 * ( a - b );
			b--;
		}
	}
}

//��ָ��λ�û�һ��ָ����С��Բ
//(x,y):���ĵ�
//r    :�뾶
void LCD_DrawFullCircle(unsigned short Xpos, unsigned short Ypos, unsigned short Radius, unsigned short Color)
{
	uint16_t x, y, r = Radius;
	for (y = Ypos - r; y < Ypos + r; y++)
	{
		for (x = Xpos - r; x < Xpos + r; x++)
		{
			if (((x - Xpos) * (x - Xpos) + (y - Ypos) * (y - Ypos)) <= r * r)
			{
				LCD_DrawPoint(x, y, Color);
			}
		}
	}
}

// void LCD_ShowChar8(unsigned short x, unsigned short y, unsigned char ch, unsigned char font_size,  unsigned short pen_color, unsigned short back_color)
// {
// 	int i = 0, j = 0;
// 	unsigned char temp = 0;
// 	unsigned char size = 0;


// 	 if((x > (LCD_COLUMN_SIZE - font_size / 2)) || (y > (LCD_ROW_SIZE - font_size)))
// 		 return;

// 	 ch = ch - ' ';
// 	 if(font_size == 8)
// 	 {

//  			size = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size / 2);

// 			for(i = 0; i < size; i++)
// 			{
// 				  temp = asc2_0804[ch][i];

// 					for(j = 0; j < 4; j++)
// 					{
// 							if(temp & 0x80)
// 							LCD_DrawPoint(x, y, pen_color);
// 							else
// 							LCD_DrawPoint(x, y, back_color);

// 							temp <<= 1;
// 					}
// 			}
// 	 }
// 	}
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//size:������? 12/16/24
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)
void LCD_ShowChar(unsigned short x, unsigned short y, unsigned char num, unsigned char size, unsigned char mode, unsigned short pen_color, unsigned short back_color)
{
    unsigned char temp, t1, t;
	unsigned short y0 = y;
	unsigned char csize = ( size / 8 + ( (size % 8) ? 1 : 0)) * (size / 2); //�õ�����һ���ַ���Ӧ������ռ���ֽ���
 	num = num - ' ';//�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩

	for(t = 0; t < csize; t++)
	{
		// if(size == 8)//����0804���壨�����ã�
		// {
		// 	temp = asc2_0804[num][t];
		// }
		if(size == 12)//����1206����
		{
			temp = asc2_1206[num][t];
		}
		else if(size == 16)//����1608����
		{
			temp=asc2_1608[num][t];
		}
		else if(size == 24)	//����2412����
		{
			temp=asc2_2412[num][t];
		}
		else
			return; //û�е��ֿ�

		for(t1 = 0; t1 < 8; t1++)
		{
			if( temp & 0x80 )
			{
				LCD_DrawPoint(x, y, pen_color);
			}
			else if( mode == 0)
			{
				LCD_DrawPoint(x, y, back_color);
			}
			temp <<= 1;
			y++;

			if(y >= LCD_COLUMN_SIZE)//������
			{
				return;
			}

			if((y-y0) == size)
			{
				y = y0;
				x++;
				if(x>=LCD_ROW_SIZE)//������
				{
					return;
				}
				break;
			}
		}
	}
}

//��ʾ�ַ���
//x,y:�������?
//width,height:������?
//size:������?
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char size, unsigned char *p, unsigned short pen_color, unsigned short back_color)
{
	unsigned char x0 = x;
	width += x;
	height += y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
        if(x >= width)
		{
			x = x0;
			y += size;
		}

        if(y >= height)//�˳�
		{
			break;
		}

        LCD_ShowChar(x, y, *p, size, 0, pen_color, back_color);
        x += size / 2;
        p++;
    }
}

uint32_t LCD_Pow(uint8_t m,uint8_t n)
{
	uint32_t result=1;
	while(n--)result*=m;
	return result;
}
//��ʾ����
//x,y:�������?
//len:���ֵ�λ��
//size:������?
//num:Ҫ��ʾ������,0~4294967295
void LCD_ShowNum(unsigned short x, unsigned short y, unsigned short len, unsigned short size, unsigned long num, unsigned short pen_color, unsigned short back_color)
{
	unsigned short t,temp;
	for(t=0;t<len;t++)
	{
		temp=(num/LCD_Pow(10,len-t-1))%10;
			if(temp==0)
			{
				LCD_ShowChar(x+(size/2)*t,y,'0',size,0, pen_color, back_color);
		  }
			else
	 	LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0, pen_color, back_color);
	}
}

// //��������ʾ
// //˵������������ȡģ���������õ� ָ���ĺ��� ���ִ�С��16x16	 ��32���ֽڴ洢һ������
// // 		�ⲿ�ֺ����Ǵ��ڳ���ռ�? ���Ժ��ֵĶ���ֱ��Ӱ�����ռ��ʣ����?
// //      ��Ҫ�����ھ���ʾ����ָ���ĺ�����˵�Ͳ��õ����ֿ���
// //x  y  Ҫ��ʾ���ֵ�����
// //c[2]  ���ֵ����� һ�����������ֽڱ�ʾ
// //dcolor ���ֵ���ɫ   bgcolor ������ɫ
// void GUI_sprintf_hz1616(unsigned short x, unsigned short y, unsigned char c[2], unsigned short pen_color, unsigned short back_color)
// {
// 	unsigned char i, j, k, m; //������ʱ����
// 	unsigned short x0, y0;
// 	x0 = x;
// 	y0=y;
// 	lcdSetAddress(x, y, x + 16 - 1, y + 16 - 1); //ѡ������λ��

// 	for (k = 0; k < 64; k++) //64��ʾ�Խ����ֿ��еĸ������糬������������
// 	{
// 		if ((code_GB_16[k].Index[0]==c[0]) && (code_GB_16[k].Index[1]==c[1])) //Ѱ�Ҷ�Ӧ����	 һ��������Ҫ�����ֽ�
// 		{
// 			for(i = 0; i < 32; i++) //32���ֽ� ÿ���ֽڶ�Ҫһ����һ���㴦�� �����Ǵ�����32X8��
// 			{
// 				m = code_GB_16[k].Msk[i];							//��ȡ��Ӧ�ֽ�����
// 				for(j = 0; j < 8; j++) 									//��ʾһ���ֽ�  һ���ֽ�8λ Ҳ����8����
// 				{
// 					//�ж��Ƿ���Ҫд���?
// 					if( (m&0x80) == 0x80)
// 					{
// 						LCD_Fast_DrawPoint(x, y, pen_color); // �����? ��������ɫ
// 					}
// 					else
// 					{
// 						LCD_Fast_DrawPoint(x, y, back_color); //�������? Ϊ����ɫ  ����ɫ
// 					}
// 					m <<= 1; //����һλ  �ж���һ��
// 					x++;

// 					if(x - x0 == 16)
// 					{
// 						x = x0;
// 						y++;
// 						if(y - y0 >= 16)
// 						{
// 							return;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// }

// //��ʾ�ַ���
// //x1 y1 ��ʾ�ĳ�ʼλ��
// //*str Ҫ��ʾ������
// //dcolor ��ʾ�ַ�����ɫ
// //bgcolor ��ʾ�ַ��ı�����ɫ
// void GUI_sprintf_hz16x(unsigned short x1, unsigned short y1, unsigned char *str, unsigned short pen_color, unsigned short back_color)
// {
// 	unsigned char l = 0;
// 	while(*str)
// 	{
// 		if(*str>=0x80)
// 		{
// 			GUI_sprintf_hz1616(x1 + l * 8, y1, (unsigned char*)str, pen_color, back_color);
// 			str += 2;
// 			l += 2;
// 		}
// 	}
// }

// //��ʾ32x32��������
// //(x��y) ����ʾ���ݵ����Ͻ����꣬c[2]��ʾ���ֵ������ֽ�
// //dcolor ���ֵ���ɫ   bgcolor ������ɫ
// //˵������ָ��λ������ʾһ��32x32�ĺ���
// void GUI_sprintf_hz3232(unsigned short x, unsigned short y, unsigned char c[2], unsigned short pen_color, unsigned short back_color)
// {
// 	unsigned char i, j, k;
// 	unsigned short x0, y0;
// 	x0 = x;
// 	y0 = y;

// 	lcdSetAddress(x, y, x + 32 - 1, y + 32 - 1); //ѡ������λ��

// 	for (k = 0; k < 100; k++) //�Խ����ֿ⣨k���ֵ�ͺ��ֿ��к��ָ����йأ����?����ѯ����
// 	{
// 		if ((code_GB_32[k].Index[0]==c[0])&&(code_GB_32[k].Index[1]==c[1])) //Ѱ��Ҫ��ʾ�ĺ���
// 		{
// 			for(i = 0; i < 128; i++) //1������Ҫ128���ֽڵ�����ʾ
// 			{
// 				unsigned char m = code_GB_32[k].Msk[i]; //һ���ֽ�һ���ֽڵ���ʾ
// 				for(j = 0; j < 8; j++)
// 				{
// 					//�ж��Ƿ���Ҫд���?
// 					if((m&0x80)==0x80) // �����?,��������ɫ
// 					{
// 						LCD_Fast_DrawPoint(x,y,pen_color);
// 					}
// 					else //�������?,Ϊ����ɫ
// 					{
// 						LCD_Fast_DrawPoint(x,y,back_color);
// 					}
// 					m<<=1;
// 					x++;
// 					if(x - x0 == 32)
// 					{
// 						x = x0;
// 						y++;
// 						if(y - y0 >= 32)
// 						{
// 							return;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// }

// //��ʾ32x32���� ��32x16 ��ĸ��
// //x1��y1����ʾ���ݵ����Ͻ����꣬
// //dcolor ���ֵ���ɫ   bgcolor ������ɫ
// //*str Ҫ��ʾ������
// void GUI_sprintf_hz32x(unsigned short x1, unsigned short y1, unsigned char *str, unsigned short pen_color, unsigned short back_color)
// {
// 	unsigned char l = 0;
// 	while(*str)
// 	{
// 		if(*str>=0x80)
// 		{
// 			GUI_sprintf_hz3232(x1 + l * 8, y1, (unsigned char*)str, pen_color, back_color);
// 			l += 4;
// 			str += 2;
// 		}
// 	}
// }
