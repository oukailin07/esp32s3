
#include <stdio.h>
#include "lib_lcd7735.h"
#include "ascii_font.h"
//#include "LCD_ST7735.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"

spi_device_handle_t tft_hspi = NULL;					///<spi锟借备锟斤拷锟?
static void lcdSelectRegister(unsigned char data);
static void lcdWriteDataU8(unsigned char data);
static void lcdWriteDataU16(unsigned short data);

static void SpiSend(uint8_t *data, uint8_t dataLength);

/**
  * @brief spi锟斤拷锟斤拷锟斤拷锟斤拷
  * @note  锟斤拷锟睫革拷锟斤拷锟斤拷锟皆硷拷锟斤拷同锟斤拷硬锟斤拷
  * @param data 锟斤拷要锟斤拷锟酵碉拷锟斤拷锟斤拷
  * @param dataLength 锟斤拷锟捷的革拷锟斤拷(1BYTE 为锟斤拷位)
  * @return 锟斤拷锟斤拷状态
  */
static void SpiSend(uint8_t *data, uint8_t dataLength)
{

	//spi锟斤拷锟斤拷锟斤拷锟斤拷
	spi_transaction_t ext;  		///<锟斤拷锟斤拷潭锟斤拷锟斤拷冉峁癸拷锟?
	memset(&ext, 0, sizeof(ext));  	///<锟斤拷锟斤拷锟节达拷
	//ext.command_bits = 0;  		///<
	ext.rxlength = 0 ;				///<锟斤拷锟斤拷
	ext.length = 8 * dataLength;		///<写锟斤拷位锟斤拷
	ext.tx_buffer =	data;
	ext.user = (void*)1; 
	// esp_err_t  lcd_err = spi_device_transmit(tft_hspi, &ext); 			///<锟叫断达拷锟斤拷
	esp_err_t  lcd_state = spi_device_polling_transmit(tft_hspi, &ext); 	///<锟斤拷询锟斤拷锟斤拷
	if (lcd_state != ESP_OK)
		printf("lcd_err	-->%d\n", lcd_state);
}



/**
  * @brief spi gpio锟斤拷始锟斤拷
  * @note  锟斤拷锟睫革拷锟斤拷锟斤拷锟皆硷拷锟斤拷同锟斤拷硬锟斤拷
  */
void LcdGpioSpiInit(void)
{
	//--gpio 锟斤拷锟斤拷 add your code
	gpio_config_t lcd_io = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1 << LCD_PIN_RES)  | (1 << 16) | (1<<15)| (1<<6)| (1<<5),	///<RES GPIO
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en	= GPIO_PULLDOWN_DISABLE,
	};
	gpio_config(&lcd_io);

	//锟斤拷锟斤拷锟斤拷锟斤拷  add your code
	spi_bus_config_t buscfg = {
		.miso_io_num = -1,   			///<gpio12->miso
		.mosi_io_num = LCD_PIN_SDA,    	///<gpio13->mosi
		.sclk_io_num = LCD_PIN_SCL,   	///<gpio14-> sclk
		.quadwp_io_num = -1,
		.quadhd_io_num = -1,
		.flags = SPICOMMON_BUSFLAG_MASTER , //SPI锟斤拷模式
		.max_transfer_sz = 4096 * 8,
	};
		//buscfg.intr_flags = 0;  							///<锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟絊PI通讯锟斤拷锟斤拷氐锟斤拷卸虾锟斤拷锟斤拷锟斤拷卸锟斤拷锟斤拷燃锟斤拷锟?0锟斤拷默锟较★拷
	esp_err_t tft_spi_f = spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);		///<锟斤拷锟竭筹拷始锟斤拷锟斤拷锟斤拷锟斤拷使锟斤拷DMA锟斤拷锟斤拷
	if (tft_spi_f != ESP_OK) {
		printf("--tft--spi--bus--initialize--err,%d\n", tft_spi_f);						///<锟斤拷锟斤拷锟斤拷息锟斤拷印
	}

	//锟借备锟斤拷锟矫结构锟斤拷 add your code
	spi_device_interface_config_t interface_config = {
		.address_bits = 0,
		.input_delay_ns = 0,
		.command_bits = 0,
		.dummy_bits = 0,
		.clock_speed_hz = 40 * 1000 * 1000,
		.mode = LCD_SPI_MODE, 				///<锟斤拷锟斤拷SPI通讯锟斤拷锟斤拷位锟斤拷锟皆和诧拷锟斤拷锟斤拷锟截★拷锟斤拷锟斤拷锟斤拷mode0-3锟斤拷锟街★拷要锟斤拷锟斤拷锟借备锟杰癸拷使锟斤拷锟斤拷锟斤拷模式
#if LCD_HARDWARE_CS						///<锟斤拷选锟斤拷硬锟斤拷CS
		///锟斤拷锟斤拷前锟斤拷片选 锟斤拷锟斤拷时锟斤拷锟劫达拷锟斤拷锟捷诧拷然锟斤拷锟斤拷失锟斤拷
		// .cs_ena_pretrans = 2,
		// .cs_ena_posttrans = 2,
		.spics_io_num = -1,	//LCD_PIN_CS,			///<锟斤拷锟斤拷片选锟斤拷
#endif
		.pre_cb=NULL,
		.post_cb=NULL,//NULL
		.duty_cycle_pos = 0,
		.queue_size = 6 					///<锟斤拷锟斤拷锟斤拷械某锟斤拷龋锟斤拷锟绞撅拷锟斤拷锟斤拷锟酵ㄑ讹拷锟绞憋拷锟斤拷锟斤拷锟斤拷俑锟絪pi通讯锟斤拷锟斤拷锟叫讹拷通讯模式锟斤拷时锟斤拷锟窖碉拷前spi通讯锟斤拷锟教癸拷锟金到讹拷锟斤拷锟斤拷
	};

	tft_spi_f = spi_bus_add_device(LCD_SPI_HOST, &interface_config, &tft_hspi);		///<锟斤拷锟斤拷spi锟借备
	if (tft_spi_f != ESP_OK) {
		printf("--tft--spi--deiver--config--err,%d\n", tft_spi_f);					///<锟斤拷锟斤拷锟斤拷息锟斤拷印
	}
}


//锟斤拷锟斤拷锟斤拷dc锟斤拷锟斤拷c
static void lcdSelectRegister(unsigned char com)
{
	CLR_LCD_CS;
	CLR_LCD_A0;  ///<锟斤拷锟斤拷
	SpiSend(&com, 1); 	///<锟斤拷8bit
	SET_LCD_CS;
}
//锟斤拷锟斤拷锟斤拷dc-d
static void lcdWriteDataU8(unsigned char data)
{
	CLR_LCD_CS;
	SET_LCD_A0;
	SpiSend(&data, 1);
	SET_LCD_CS;
}

//锟斤拷16位锟斤拷锟斤拷
static void lcdWriteDataU16(unsigned short data)
{
	lcdWriteDataU8(data >> 8);
	lcdWriteDataU8(data);

	// SET_LCD_A0;
	// #if !(LCD_HARDWARE_CS)

	//--*ESP32 锟斤拷锟斤拷小锟斤拷芯片锟斤拷锟斤拷 uint16_t 锟斤拷 uint32_t 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫?位锟芥储锟斤拷锟斤拷小锟侥碉拷址锟斤拷锟斤拷耍锟斤拷锟斤拷 uint16_t 锟芥储锟斤拷锟节达拷锟叫ｏ拷锟斤拷锟斤拷锟饺凤拷锟斤拷位 [7:0]锟斤拷锟斤拷锟斤拷锟轿? [15:8]锟斤拷*/
	/*idf锟斤拷锟斤拷植锟? :锟斤拷某些锟斤拷锟斤拷拢锟揭?锟斤拷锟斤拷锟斤拷锟斤拷荽锟叫★拷锟? uint8_t 锟斤拷锟介不同锟斤拷锟斤拷使锟斤拷锟斤拷锟铰宏将锟斤拷锟斤拷转锟斤拷为锟斤拷锟斤拷 SPI 锟斤拷锟斤拷直锟接凤拷锟酵的革拷式锟斤拷
		锟借传锟斤拷锟斤拷锟斤拷荩锟绞癸拷锟? SPI_SWAP_DATA_TX
		锟斤拷锟秸碉拷锟斤拷锟斤拷锟捷ｏ拷使锟斤拷 SPI_SWAP_DATA_RX
		*/
	// 	CLR_LCD_CS;	 ///<使锟斤拷锟借备
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
	//锟斤拷位
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

	///<锟斤拷示锟斤拷锟斤拷/
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

//锟斤拷锟矫伙拷锟狡碉拷址
void lcdSetAddress(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2)
{
	
#if (LCD_DIR==LCD_DIR_VAERTICAL1||LCD_DIR==LCD_DIR_VAERTICAL2)
	lcdSelectRegister(0x2A);   ///
	//锟斤拷始锟叫碉拷址
	lcdWriteDataU16(x1); ///st7735s锟斤拷幕锟斤拷锟斤拷锟叫碉拷偏锟狡ｏ拷+2锟斤拷锟斤拷
	//锟斤拷锟斤拷锟叫碉拷址
	lcdWriteDataU16(x2);
	lcdSelectRegister(0x2B); 	///
	//锟斤拷始
	lcdWriteDataU16(y1);
	//锟斤拷锟斤拷锟斤拷
	lcdWriteDataU16(y2);
#else
	lcdSelectRegister(0x2A);   ///
	//锟斤拷始锟叫碉拷址
	lcdWriteDataU16(x1); ///锟斤拷幕锟斤拷锟斤拷锟叫碉拷偏锟狡ｏ拷+2锟斤拷锟斤拷
	//锟斤拷锟斤拷锟叫碉拷址
	lcdWriteDataU16(x2);
	lcdSelectRegister(0x2B); 	///
	//锟斤拷始
	lcdWriteDataU16(y1 + 2);
	//锟斤拷锟斤拷锟斤拷
	lcdWriteDataU16(y2 + 2);
#endif

	lcdSelectRegister(0x2C);	///<Memory Write
}

//锟斤拷锟斤拷
void lcdClear(unsigned short color)
{
	lcdSetAddress(0, 0, LCD_ROW_SIZE, LCD_COLUMN_SIZE);///<锟斤拷锟斤拷锟斤拷幕锟斤拷小

	for (unsigned char i = 0; i <= LCD_ROW_SIZE; i++)
	{
		for (unsigned char j = 0; j <= LCD_COLUMN_SIZE; j++)
		{
			lcdWriteDataU16(color);
		}
	}
}

//LCD锟斤拷锟斤拷
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	lcdSetAddress(x, y, x, y);//锟斤拷锟矫癸拷锟轿伙拷锟?
	lcdWriteDataU16(color);
}

//锟斤拷锟劫伙拷锟斤拷
//x,y:锟斤拷锟斤拷
//color:锟斤拷色
void LCD_Fast_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
	lcdWriteDataU16(color);
}

//LCD锟斤拷锟斤拷锟斤拷示
void LCD_DisplayOn(void)
{
	lcdSelectRegister(0x29);	//锟斤拷锟斤拷锟斤拷示
}
//LCD锟截憋拷锟斤拷示
void LCD_DisplayOff(void)
{
	lcdSelectRegister(0x28);	//锟截憋拷锟斤拷示
}

//锟斤拷指锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷涞ワ拷锟斤拷锟缴?
//(sx,sy),(ex,ey):锟斤拷锟斤拷锟轿对斤拷锟斤拷锟斤拷,锟斤拷锟斤拷锟叫∥?:(ex-sx+1)*(ey-sy+1)
//color:要锟斤拷锟斤拷锟斤拷色
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

//锟斤拷锟斤拷
//x1,y1:锟斤拷锟斤拷锟斤拷锟?
//x2,y2:锟秸碉拷锟斤拷锟斤拷
void LCD_DrawLine(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
	unsigned short t;
	int xerr = 0, yerr = 0, delta_x, delta_y, distance;
	int incx, incy, uRow, uCol;
	delta_x = x2 - x1; //锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	delta_y = y2 - y1;
	uRow = x1;
	uCol = y1;

	//锟斤拷锟矫碉拷锟斤拷锟斤拷锟斤拷
	if ( delta_x > 0 )
	{
		incx = 1;
	}
	else if ( delta_x == 0 ) //锟斤拷直锟斤拷
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
	else if ( delta_y == 0 ) //水平锟斤拷
	{
		incy = 0;
	}
	else
	{
		incy = -1;
		delta_y = -delta_y;
	}

	if ( delta_x > delta_y ) //选取锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
	{
		distance = delta_x;
	}
	else
	{
		distance = delta_y;
	}

	for (t = 0; t <= distance + 1; t++ ) //锟斤拷锟斤拷锟斤拷锟?
	{
		LCD_DrawPoint(uRow, uCol, color);//锟斤拷锟斤拷
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
/// @brief 锟斤拷锟狡撅拷锟斤拷
/// @param x1 	stort x
/// @param y1 	stort y
/// @param x2 	end	x
/// @param y2 	end y
/// @param color 锟斤拷色
void LCD_DrawRectangle(unsigned short x1, unsigned short y1, unsigned short x2, unsigned short y2, unsigned short color)
{
	LCD_DrawLine(x1, y1, x2, y1, color);
	LCD_DrawLine(x1, y1, x1, y2, color);
	LCD_DrawLine(x1, y2, x2, y2, color);
	LCD_DrawLine(x2, y1, x2, y2, color);
}

//锟斤拷指锟斤拷位锟矫伙拷一锟斤拷指锟斤拷锟斤拷小锟斤拷圆
//(x,y):锟斤拷锟侥碉拷
//r    :锟诫径
void LCD_Draw_Circle(unsigned short x0, unsigned short y0, unsigned char r, unsigned short color)
{
	int a, b;
	int di;
	a = 0;
	b = r;
	di = 3 - ( r << 1 );           //锟叫讹拷锟铰革拷锟斤拷位锟矫的憋拷志
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
		//使锟斤拷Bresenham锟姐法锟斤拷圆
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

//锟斤拷指锟斤拷位锟矫伙拷一锟斤拷指锟斤拷锟斤拷小锟斤拷圆
//(x,y):锟斤拷锟侥碉拷
//r    :锟诫径
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
//锟斤拷指锟斤拷位锟斤拷锟斤拷示一锟斤拷锟街凤拷
//x,y:锟斤拷始锟斤拷锟斤拷
//num:要锟斤拷示锟斤拷锟街凤拷:" "--->"~"
//size:锟斤拷锟斤拷锟叫? 12/16/24
//mode:锟斤拷锟接凤拷式(1)锟斤拷锟角非碉拷锟接凤拷式(0)
void LCD_ShowChar(unsigned short x, unsigned short y, unsigned char num, unsigned char size, unsigned char mode, unsigned short pen_color, unsigned short back_color)
{
    unsigned char temp, t1, t;
	unsigned short y0 = y;
	unsigned char csize = ( size / 8 + ( (size % 8) ? 1 : 0)) * (size / 2); //锟矫碉拷锟斤拷锟斤拷一锟斤拷锟街凤拷锟斤拷应锟斤拷锟斤拷锟斤拷占锟斤拷锟街斤拷锟斤拷
 	num = num - ' ';//锟矫碉拷偏锟狡猴拷锟街碉拷锟紸SCII锟街匡拷锟角从空革拷始取模锟斤拷锟斤拷锟斤拷-' '锟斤拷锟角讹拷应锟街凤拷锟斤拷锟街库）

	for(t = 0; t < csize; t++)
	{
		// if(size == 8)//锟斤拷锟斤拷0804锟斤拷锟藉（锟斤拷锟斤拷锟矫ｏ拷
		// {
		// 	temp = asc2_0804[num][t];
		// }
		if(size == 12)//锟斤拷锟斤拷1206锟斤拷锟斤拷
		{
			temp = asc2_1206[num][t];
		}
		else if(size == 16)//锟斤拷锟斤拷1608锟斤拷锟斤拷
		{
			temp=asc2_1608[num][t];
		}
		else if(size == 24)	//锟斤拷锟斤拷2412锟斤拷锟斤拷
		{
			temp=asc2_2412[num][t];
		}
		else
			return; //没锟叫碉拷锟街匡拷

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

			if(y >= LCD_COLUMN_SIZE)//锟斤拷锟斤拷锟斤拷
			{
				return;
			}

			if((y-y0) == size)
			{
				y = y0;
				x++;
				if(x>=LCD_ROW_SIZE)//锟斤拷锟斤拷锟斤拷
				{
					return;
				}
				break;
			}
		}
	}
}

//锟斤拷示锟街凤拷锟斤拷
//x,y:锟斤拷锟斤拷锟斤拷锟?
//width,height:锟斤拷锟斤拷锟叫?
//size:锟斤拷锟斤拷锟叫?
//*p:锟街凤拷锟斤拷锟斤拷始锟斤拷址
void LCD_ShowString(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char size, unsigned char *p, unsigned short pen_color, unsigned short back_color)
{
	unsigned char x0 = x;
	width += x;
	height += y;
    while((*p<='~')&&(*p>=' '))//锟叫讹拷锟角诧拷锟角非凤拷锟街凤拷!
    {
        if(x >= width)
		{
			x = x0;
			y += size;
		}

        if(y >= height)//锟剿筹拷
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
//锟斤拷示锟斤拷锟斤拷
//x,y:锟斤拷锟斤拷锟斤拷锟?
//len:锟斤拷锟街碉拷位锟斤拷
//size:锟斤拷锟斤拷锟叫?
//num:要锟斤拷示锟斤拷锟斤拷锟斤拷,0~4294967295
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

// //锟斤拷锟斤拷锟斤拷锟斤拷示
// //说锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷取模锟斤拷锟斤拷锟斤拷锟斤拷锟矫碉拷 指锟斤拷锟侥猴拷锟斤拷 锟斤拷锟街达拷小锟斤拷16x16	 锟斤拷32锟斤拷锟街节存储一锟斤拷锟斤拷锟斤拷
// // 		锟解部锟街猴拷锟斤拷锟角达拷锟节筹拷锟斤拷占锟? 锟斤拷锟皆猴拷锟街的讹拷锟斤拷直锟斤拷影锟斤拷锟斤拷锟秸硷拷锟绞ｏ拷锟斤拷锟?
// //      锟斤拷要锟斤拷锟斤拷锟节撅拷锟斤拷示锟斤拷锟斤拷指锟斤拷锟侥猴拷锟斤拷锟斤拷说锟酵诧拷锟矫碉拷锟斤拷锟街匡拷锟斤拷
// //x  y  要锟斤拷示锟斤拷锟街碉拷锟斤拷锟斤拷
// //c[2]  锟斤拷锟街碉拷锟斤拷锟斤拷 一锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街节憋拷示
// //dcolor 锟斤拷锟街碉拷锟斤拷色   bgcolor 锟斤拷锟斤拷锟斤拷色
// void GUI_sprintf_hz1616(unsigned short x, unsigned short y, unsigned char c[2], unsigned short pen_color, unsigned short back_color)
// {
// 	unsigned char i, j, k, m; //锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷
// 	unsigned short x0, y0;
// 	x0 = x;
// 	y0=y;
// 	lcdSetAddress(x, y, x + 16 - 1, y + 16 - 1); //选锟斤拷锟斤拷锟斤拷位锟斤拷

// 	for (k = 0; k < 64; k++) //64锟斤拷示锟皆斤拷锟斤拷锟街匡拷锟叫的革拷锟斤拷锟斤拷锟界超锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
// 	{
// 		if ((code_GB_16[k].Index[0]==c[0]) && (code_GB_16[k].Index[1]==c[1])) //寻锟揭讹拷应锟斤拷锟斤拷	 一锟斤拷锟斤拷锟斤拷锟斤拷要锟斤拷锟斤拷锟街斤拷
// 		{
// 			for(i = 0; i < 32; i++) //32锟斤拷锟街斤拷 每锟斤拷锟街节讹拷要一锟斤拷锟斤拷一锟斤拷锟姐处锟斤拷 锟斤拷锟斤拷锟角达拷锟斤拷锟斤拷32X8锟斤拷
// 			{
// 				m = code_GB_16[k].Msk[i];							//锟斤拷取锟斤拷应锟街斤拷锟斤拷锟斤拷
// 				for(j = 0; j < 8; j++) 									//锟斤拷示一锟斤拷锟街斤拷  一锟斤拷锟街斤拷8位 也锟斤拷锟斤拷8锟斤拷锟斤拷
// 				{
// 					//锟叫讹拷锟角凤拷锟斤拷要写锟斤拷锟?
// 					if( (m&0x80) == 0x80)
// 					{
// 						LCD_Fast_DrawPoint(x, y, pen_color); // 锟斤拷锟斤拷锟? 锟斤拷锟斤拷锟斤拷锟斤拷色
// 					}
// 					else
// 					{
// 						LCD_Fast_DrawPoint(x, y, back_color); //锟斤拷锟斤拷锟斤拷锟? 为锟斤拷锟斤拷色  锟斤拷锟斤拷色
// 					}
// 					m <<= 1; //锟斤拷锟斤拷一位  锟叫讹拷锟斤拷一锟斤拷
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

// //锟斤拷示锟街凤拷锟斤拷
// //x1 y1 锟斤拷示锟侥筹拷始位锟斤拷
// //*str 要锟斤拷示锟斤拷锟斤拷锟斤拷
// //dcolor 锟斤拷示锟街凤拷锟斤拷锟斤拷色
// //bgcolor 锟斤拷示锟街凤拷锟侥憋拷锟斤拷锟斤拷色
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

// //锟斤拷示32x32锟斤拷锟斤拷锟斤拷锟斤拷
// //(x锟斤拷y) 锟斤拷锟斤拷示锟斤拷锟捷碉拷锟斤拷锟较斤拷锟斤拷锟疥，c[2]锟斤拷示锟斤拷锟街碉拷锟斤拷锟斤拷锟街斤拷
// //dcolor 锟斤拷锟街碉拷锟斤拷色   bgcolor 锟斤拷锟斤拷锟斤拷色
// //说锟斤拷锟斤拷锟斤拷指锟斤拷位锟斤拷锟斤拷锟斤拷示一锟斤拷32x32锟侥猴拷锟斤拷
// void GUI_sprintf_hz3232(unsigned short x, unsigned short y, unsigned char c[2], unsigned short pen_color, unsigned short back_color)
// {
// 	unsigned char i, j, k;
// 	unsigned short x0, y0;
// 	x0 = x;
// 	y0 = y;

// 	lcdSetAddress(x, y, x + 32 - 1, y + 32 - 1); //选锟斤拷锟斤拷锟斤拷位锟斤拷

// 	for (k = 0; k < 100; k++) //锟皆斤拷锟斤拷锟街库（k锟斤拷锟街碉拷秃锟斤拷挚锟斤拷泻锟斤拷指锟斤拷锟斤拷泄兀锟斤拷锟窖?锟斤拷锟斤拷询锟斤拷锟斤拷
// 	{
// 		if ((code_GB_32[k].Index[0]==c[0])&&(code_GB_32[k].Index[1]==c[1])) //寻锟斤拷要锟斤拷示锟侥猴拷锟斤拷
// 		{
// 			for(i = 0; i < 128; i++) //1锟斤拷锟斤拷锟斤拷要128锟斤拷锟街节碉拷锟斤拷锟斤拷示
// 			{
// 				unsigned char m = code_GB_32[k].Msk[i]; //一锟斤拷锟街斤拷一锟斤拷锟街节碉拷锟斤拷示
// 				for(j = 0; j < 8; j++)
// 				{
// 					//锟叫讹拷锟角凤拷锟斤拷要写锟斤拷锟?
// 					if((m&0x80)==0x80) // 锟斤拷锟斤拷锟?,锟斤拷锟斤拷锟斤拷锟斤拷色
// 					{
// 						LCD_Fast_DrawPoint(x,y,pen_color);
// 					}
// 					else //锟斤拷锟斤拷锟斤拷锟?,为锟斤拷锟斤拷色
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

// //锟斤拷示32x32锟斤拷锟斤拷 锟斤拷32x16 锟斤拷母锟斤拷
// //x1锟斤拷y1锟斤拷锟斤拷示锟斤拷锟捷碉拷锟斤拷锟较斤拷锟斤拷锟疥，
// //dcolor 锟斤拷锟街碉拷锟斤拷色   bgcolor 锟斤拷锟斤拷锟斤拷色
// //*str 要锟斤拷示锟斤拷锟斤拷锟斤拷
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
