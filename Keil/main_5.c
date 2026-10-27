#include <LPC213x.h>
#include <stdio.h>

/* =====================================================
   IoT-Based Patient Temperature Monitor
   LPC2138 + LM35 + ADC0804 + LCD + Buzzer + UART
   ===================================================== */

/* LCD PINS */
#define RS      (1UL << 16)
#define EN      (1UL << 17)
#define D4      (1UL << 18)
#define D5      (1UL << 19)
#define D6      (1UL << 20)
#define D7      (1UL << 21)

#define LCD_MASK (RS | EN | D4 | D5 | D6 | D7)

/* BUZZER */
#define BUZZER  (1UL << 22)

/* ADC0804 CONTROL PINS */
#define ADC_WR   (1UL << 24)
#define ADC_RD   (1UL << 25)
#define ADC_INTR (1UL << 26)

/* =====================================================
   DELAY
   ===================================================== */

void delay_ms(unsigned int ms)
{
    unsigned int i, j;

    for(i = 0; i < ms; i++)
        for(j = 0; j < 6000; j++);
}

/* =====================================================
   LCD FUNCTIONS
   ===================================================== */

void lcd_pulse(void)
{
    IO0SET = EN;
    delay_ms(1);

    IO0CLR = EN;
    delay_ms(1);
}

void lcd_nibble(unsigned char data)
{
    IO0CLR = D4 | D5 | D6 | D7;

    if(data & 0x01) IO0SET = D4;
    if(data & 0x02) IO0SET = D5;
    if(data & 0x04) IO0SET = D6;
    if(data & 0x08) IO0SET = D7;

    lcd_pulse();
}

void lcd_cmd(unsigned char cmd)
{
    IO0CLR = RS;

    lcd_nibble(cmd >> 4);
    lcd_nibble(cmd & 0x0F);

    delay_ms(2);
}

void lcd_data(unsigned char data)
{
    IO0SET = RS;

    lcd_nibble(data >> 4);
    lcd_nibble(data & 0x0F);

    delay_ms(2);
}

void lcd_string(char *str)
{
    while(*str)
    {
        lcd_data(*str);
        str++;
    }
}

void lcd_init(void)
{
    IO0DIR |= LCD_MASK;

    delay_ms(20);

    IO0CLR = RS | EN;

    lcd_nibble(0x03);
    delay_ms(5);

    lcd_nibble(0x03);
    delay_ms(1);

    lcd_nibble(0x03);

    lcd_nibble(0x02);

    lcd_cmd(0x28);   /* 4-bit, 2 line */
    lcd_cmd(0x0C);   /* Display ON */
    lcd_cmd(0x06);   /* Cursor increment */
    lcd_cmd(0x01);   /* Clear LCD */

    delay_ms(5);
}

/* =====================================================
   UART0 FUNCTIONS
   9600 BAUD
   ===================================================== */

void uart_init(void)
{
    /* P0.0 = TXD0, P0.1 = RXD0 */
    PINSEL0 &= ~0x0000000F;
    PINSEL0 |=  0x00000005;

    /* PCLK = CCLK = 12 MHz */
    VPBDIV = 0x01;

    U0LCR = 0x83;      /* 8-bit, 1 stop, DLAB = 1 */

    /* 9600 baud for PCLK = 12 MHz */
    U0DLL = 78;
    U0DLM = 0;

    U0LCR = 0x03;      /* DLAB = 0 */
}

void uart_send_char(char ch)
{
    while(!(U0LSR & 0x20));

    U0THR = ch;
}

void uart_send_string(char *str)
{
    while(*str)
    {
        uart_send_char(*str);
        str++;
    }
}

/* =====================================================
   ADC0804 FUNCTIONS
   ===================================================== */

void adc0804_init(void)
{
    /* D0-D7 INPUT */
    IO1DIR &= ~(0xFFUL << 16);

    /* WR and RD OUTPUT */
    IO1DIR |= ADC_WR | ADC_RD;

    /* INTR INPUT */
    IO1DIR &= ~ADC_INTR;

    /* Initial state */
    IO1SET = ADC_WR | ADC_RD;
}

unsigned char adc0804_read(void)
{
    unsigned char adc_value;
    unsigned long timeout = 500000;

    /* RD HIGH */
    IO1SET = ADC_RD;

    /* Start conversion */
    IO1CLR = ADC_WR;

    delay_ms(1);

    IO1SET = ADC_WR;

    /* Wait for INTR LOW with timeout */
    while((IO1PIN & ADC_INTR) && timeout)
    {
        timeout--;
    }

    /* If ADC conversion failed */
    if(timeout == 0)
    {
        return 0xFF;
    }

    /* RD LOW */
    IO1CLR = ADC_RD;

    delay_ms(1);

    /* Read D0-D7 */
    adc_value = (unsigned char)((IO1PIN >> 16) & 0xFF);

    /* RD HIGH */
    IO1SET = ADC_RD;

    return adc_value;
}

/* =====================================================
   MAIN PROGRAM
   ===================================================== */

int main(void)
{
    unsigned char adc_value;
		unsigned int temperature;
    /* INITIALIZE */

    IO0DIR |= BUZZER;

    IO0CLR = BUZZER;

    lcd_init();

    uart_init();

    adc0804_init();

    lcd_cmd(0x80);
    lcd_string("Patient Temp");

    lcd_cmd(0xC0);
    lcd_string("Initializing...");

    uart_send_string("\r\nPatient Temperature Monitor\r\n");

    delay_ms(1);

			while(1)
	{
			adc_value = adc0804_read();

			/* ADC0804: 0–5V, LM35 = 10mV/°C */
			temperature = ((unsigned long)adc_value * 500) / 255;

			lcd_cmd(0x01);
			delay_ms(5);

			lcd_cmd(0x80);
			lcd_string("Temp: ");

			if(temperature >= 100)
			{
					lcd_data((temperature / 100) + '0');
			}

			if(temperature >= 10)
			{
					lcd_data(((temperature / 10) % 10) + '0');
			}

			lcd_data((temperature % 10) + '0');

			lcd_string(" C");

			lcd_cmd(0xC0);

			if(temperature < 35)
			{
					IO0SET = BUZZER;
					lcd_string("LOW TEMP!");
					uart_send_string("LOW TEMP\r\n");
			}
			else if(temperature >= 38)
			{
					IO0SET = BUZZER;
					lcd_string("HIGH TEMP!");
					uart_send_string("HIGH TEMP\r\n");
			}
			else
			{
					IO0CLR = BUZZER;
					lcd_string("NORMAL");
					uart_send_string("NORMAL\r\n");
			}

			delay_ms(1);
	}
}