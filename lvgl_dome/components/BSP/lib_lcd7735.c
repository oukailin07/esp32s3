#include <stdio.h>
#include "lib_lcd7735.h"
#include "ascii_font.h"
//#include "LCD_ST7735.h"
#include "esp_log.h"
#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
// 定义屏幕尺寸和块大小
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 160
#define BLOCK_HEIGHT  80  // 每次传输20行数据
static uint16_t block_buf[BLOCK_HEIGHT][SCREEN_WIDTH];
spi_device_handle_t tft_hspi = NULL;					///<spi�豸���?

static const char *TAG = "st7735";
static void lcdSelectRegister(unsigned char data);
static void lcdWriteDataU8(unsigned char data);
static void lcdWriteDataU16(unsigned short data);

static void SpiSend(uint8_t *data, uint8_t dataLength);

// 新增优化后的SPI发送函数（支持大块数据传输）
static void SpiSendLarge(const void* data, size_t length)
{
    spi_transaction_t trans = {
        .length = length * 8,  // 位数
        .tx_buffer = data,
        .flags = 0  // 移除SPI_TRANS_USE_TXDATA标志
    };
    
    esp_err_t ret = spi_device_polling_transmit(tft_hspi, &trans);
    if(ret != ESP_OK) {
       // ESP_LOGE("LCD", "SPI transmit failed: %d", ret);
    }
}
// 优化后的地址设置函数（支持行模式）
static void lcdSetRowAddress(uint16_t y)
{
    lcdSelectRegister(0x2A);  // 列地址设置
    lcdWriteDataU16(0);
    lcdWriteDataU16(LCD_ROW_SIZE);
    
    lcdSelectRegister(0x2B);  // 行地址设置
    lcdWriteDataU16(y);
    lcdWriteDataU16(y);
    
    lcdSelectRegister(0x2C);  // 内存写入
}

// 清屏函数
void lcdClear(uint16_t color)
{
    // 填充块缓冲区
    for(int y = 0; y < BLOCK_HEIGHT; y++) {
        for(int x = 0; x < SCREEN_WIDTH; x++) {
            block_buf[y][x] = color;
        }
    }

    // 设置全屏地址
    lcdSetAddress(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1);
    
    // 分块发送
    uint16_t full_blocks = SCREEN_HEIGHT / BLOCK_HEIGHT;
    for(uint16_t block = 0; block < full_blocks; block++) {
        SpiSendBlock(block_buf, sizeof(block_buf));
    }
    
    // 处理剩余行
    uint16_t remaining_lines = SCREEN_HEIGHT % BLOCK_HEIGHT;
    if(remaining_lines > 0) {
        SpiSendBlock(block_buf, sizeof(uint16_t) * SCREEN_WIDTH * remaining_lines);
    }
}

static void SpiSend(uint8_t *data, uint8_t dataLength)
{
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


void LcdGpioSpiInit(void)
{
	gpio_config_t lcd_io = {
		.intr_type = GPIO_INTR_DISABLE,
		.mode = GPIO_MODE_OUTPUT,
		.pin_bit_mask = (1 << LCD_PIN_RES)  | (1 << 16) | (1<<15)| (1<<6)| (1<<5),	///<RES GPIO
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.pull_up_en	= GPIO_PULLDOWN_DISABLE,
	};
	gpio_config(&lcd_io);

   // 修改SPI总线配置
   spi_bus_config_t buscfg = {
	.miso_io_num = -1,
	.mosi_io_num = LCD_PIN_SDA,
	.sclk_io_num = LCD_PIN_SCL,
	.quadwp_io_num = -1,
	.quadhd_io_num = -1,
	.max_transfer_sz = sizeof(block_buf),  // 增加最大传输尺寸
	.flags = SPICOMMON_BUSFLAG_MASTER ,
};
	esp_err_t tft_spi_f = spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);		///<���߳�ʼ��������ʹ��DMA����
	if (tft_spi_f != ESP_OK) {
		printf("--tft--spi--bus--initialize--err,%d\n", tft_spi_f);						///<������Ϣ��ӡ
	}
	spi_device_interface_config_t interface_config = {
		.address_bits = 0,
		.input_delay_ns = 0,
		.command_bits = 0,
		.dummy_bits = 0,
		.clock_speed_hz = 80 * 1000 * 1000,
		.mode = LCD_SPI_MODE, 			
#if LCD_HARDWARE_CS	
		.spics_io_num = -1,	//LCD_PIN_CS,			
#endif
		.pre_cb=NULL,
		.post_cb=NULL,//NULL
		.duty_cycle_pos = 0,
		.queue_size = 7, 	
		.flags = SPI_DEVICE_NO_DUMMY,				///<������еĳ��ȣ���ʾ������ͨѶ��ʱ�������ٸ�spiͨѶ�����ж�ͨѶģʽ��ʱ���ѵ�ǰspiͨѶ���̹��𵽶�����
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

// 块写入函数（一次写入20行）
static void LCD_WriteBlock(uint16_t sx, uint16_t sy, uint16_t width, uint16_t height, uint16_t *color_p)
{
    // 设置块区域地址
    lcdSetAddress(sx, sy, sx + width - 1, sy + height - 1);
	CLR_LCD_CS;
	SET_LCD_A0;   
    // 发送整个块数据
    SpiSendBlock(color_p, sizeof(uint16_t) * width * height);
	SET_LCD_CS;
}


// 实现块传输函数
static void SpiSendBlock(const void* data, size_t length)
{
    spi_transaction_t trans = {
        .length = length * 8,  // 位数
        .tx_buffer = data,
        .flags = 0
    };
    
    esp_err_t ret = spi_device_polling_transmit(tft_hspi, &trans);
    if(ret != ESP_OK) {
        ESP_LOGE("LCD", "SPI block transmit failed: %d", ret);
    }
}
// 区域填充函数
void LCD_Fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t color)
{
    const uint16_t width = ex - sx + 1;
    const uint16_t height = ey - sy + 1;

    // 填充块缓冲区
    for(int y = 0; y < BLOCK_HEIGHT && y < height; y++) {
        for(int x = 0; x < width; x++) {
            block_buf[y][x] = color;
        }
    }

    // 分块处理
    uint16_t full_blocks = height / BLOCK_HEIGHT;
    uint16_t remaining_lines = height % BLOCK_HEIGHT;
    
    for(uint16_t block = 0; block < full_blocks; block++) {
        uint16_t current_y = sy + block * BLOCK_HEIGHT;
        LCD_WriteBlock(sx, current_y, width, BLOCK_HEIGHT, (uint16_t*)block_buf);
    }
    
    // 处理剩余行
    if(remaining_lines > 0) {
        uint16_t current_y = sy + full_blocks * BLOCK_HEIGHT;
        LCD_WriteBlock(sx, current_y, width, remaining_lines, (uint16_t*)block_buf);
    }
}

// 刷屏函数
void LCD_flush(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint16_t *color_p)
{
    const uint16_t width = ex - sx + 1;
    const uint16_t total_height = ey - sy + 1;
    
    // 计算完整块数量
    uint16_t full_blocks = total_height / BLOCK_HEIGHT;
    // 剩余行数
    uint16_t remaining_lines = total_height % BLOCK_HEIGHT;
    
    // 处理完整块
    for(uint16_t block = 0; block < full_blocks; block++) {
        uint16_t current_y = sy + block * BLOCK_HEIGHT;
        LCD_WriteBlock(sx, current_y, width, BLOCK_HEIGHT, 
                      color_p + block * BLOCK_HEIGHT * width);
    }
    
    // 处理剩余行
    if(remaining_lines > 0) {
        uint16_t current_y = sy + full_blocks * BLOCK_HEIGHT;
        LCD_WriteBlock(sx, current_y, width, remaining_lines,
                      color_p + full_blocks * BLOCK_HEIGHT * width);
    }
}
