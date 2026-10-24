/* LM35 Temperature Monitor + Buzzer
   LPC1768 - Keil uVision4

   LM35:
   VCC  -> 5V
   VOUT -> CND pin 1 (P0.23 / AD0.0)
   GND  -> CND pin 10 / board GND

   LED bar graph:
   CNA -> CNA1
   P0.4 to P0.11 = LED1 to LED8

   LCD:
   CNC -> CNAD
   D4-D7 = P0.15-P0.18
   RS = P0.19, EN = P0.20

   BUZZER:
   P2.0 = separate buzzer control output.
   Connect it only to the verified buzzer signal input.

   Alarm ON above 40 C.
   Alarm OFF at or below 39 C.
*/

#include <LPC17xx.h>
#include <stdio.h>

#define HIGH_T       400u
#define HYST          10u
#define BASE_T       260u
#define STEP_T        20u

#define LED_SHIFT      4
#define LED_ALL   0x00000FF0u

#define BUZZ_MASK (1u << 0)  /* P2.0 */

#define LCD_D_SHIFT 15
#define LCD_D_MASK (0xFu << 15)
#define LCD_RS (1u << 19)
#define LCD_EN (1u << 20)

unsigned int raw;

void delay(volatile unsigned long n)
{
    while (n--);
}

/* ---------- LCD ---------- */

void lcd_nibble(unsigned char n, unsigned char rs)
{
    LPC_GPIO0->FIOCLR = LCD_D_MASK | LCD_RS;

    LPC_GPIO0->FIOSET =
        ((unsigned long)(n & 0x0F) << LCD_D_SHIFT) |
        (rs ? LCD_RS : 0);

    LPC_GPIO0->FIOSET = LCD_EN;
    delay(300);
    LPC_GPIO0->FIOCLR = LCD_EN;
    delay(3000);
}

void lcd_cmd(unsigned char c)
{
    lcd_nibble(c >> 4, 0);
    lcd_nibble(c & 0x0F, 0);

    if (c < 4)
        delay(60000);
}

void lcd_data(unsigned char d)
{
    lcd_nibble(d >> 4, 1);
    lcd_nibble(d & 0x0F, 1);
}

void lcd_puts(const char *s)
{
    while (*s)
        lcd_data((unsigned char)*s++);
}

void lcd_init(void)
{
    LPC_PINCON->PINSEL0 &= ~(3u << 30);
    LPC_PINCON->PINSEL1 &= ~0x3FFu;

    LPC_GPIO0->FIODIR |= LCD_D_MASK | LCD_RS | LCD_EN;

    delay(400000);

    lcd_nibble(0x3, 0);
    delay(100000);

    lcd_nibble(0x3, 0);
    delay(10000);

    lcd_nibble(0x3, 0);
    delay(10000);

    lcd_nibble(0x2, 0);

    lcd_cmd(0x28);
    lcd_cmd(0x0C);
    lcd_cmd(0x06);
    lcd_cmd(0x01);
}

/* ---------- ADC ---------- */

void adc_init(void)
{
    LPC_SC->PCONP |= (1u << 12);

    /* P0.23 = AD0.0 */
    LPC_PINCON->PINSEL1 &= ~(3u << 14);
    LPC_PINCON->PINSEL1 |=  (1u << 14);

    /* No pull-up or pull-down */
    LPC_PINCON->PINMODE1 &= ~(3u << 14);
    LPC_PINCON->PINMODE1 |=  (2u << 14);
}

unsigned int adc_read(void)
{
    unsigned long r;

    LPC_ADC->ADCR =
        (1u << 0)  |   /* Channel AD0.0 */
        (5u << 8)  |   /* ADC clock divider */
        (1u << 21) |   /* ADC operational */
        (1u << 24);    /* Start conversion */

    do
    {
        r = LPC_ADC->ADGDR;
    }
    while (!(r & (1u << 31)));

    LPC_ADC->ADCR &= ~(7u << 24);

    return (unsigned int)((r >> 4) & 0xFFFu);
}

unsigned int read_mv(void)
{
    unsigned long sum = 0;
    unsigned char i;

    adc_read(); /* Discard first reading */

    for (i = 0; i < 16; i++)
        sum += adc_read();

    raw = (unsigned int)(sum >> 4);

    return (unsigned int)((raw * 3300UL) / 4095UL);
}

/* ---------- LED LEVEL ---------- */

unsigned int led_level(unsigned int t)
{
    unsigned int lv;

    if (t > HIGH_T)
        return 7;

    if (t < BASE_T + STEP_T)
        return 0;

    lv = (t - BASE_T) / STEP_T;

    if (lv > 6)
        lv = 6;

    return lv;
}

/* ---------- MAIN ---------- */

int main(void)
{
    unsigned int mv, t, lv;
    unsigned int hb = 0;
    unsigned int alarm = 0;

    unsigned long leds;
    unsigned char fault;

    char line[24];
    const char *status;

    SystemInit();
    SystemCoreClockUpdate();

    /* LEDs: P0.4-P0.11 */
    LPC_PINCON->PINSEL0 &= 0xFF0000FFu;

    LPC_GPIO0->FIODIR |= LED_ALL;
    LPC_GPIO0->FIOCLR = LED_ALL;

    /* Separate buzzer output: P2.0 */
    LPC_PINCON->PINSEL4 &= ~3u;
    LPC_GPIO2->FIODIR |= BUZZ_MASK;

    /* Start with buzzer OFF (active-HIGH assumption) */
    LPC_GPIO2->FIOCLR = BUZZ_MASK;

    lcd_init();
    adc_init();

    /* Startup indication */
    lcd_cmd(0x80);
    lcd_puts("LM35 Monitor    ");

    lcd_cmd(0xC0);
    lcd_puts("System starting ");

    /* Short buzzer self-test */
    LPC_GPIO2->FIOSET = BUZZ_MASK;
    delay(1000000);
    LPC_GPIO2->FIOCLR = BUZZ_MASK;

    delay(1000000);

    while (1)
    {
        mv = read_mv();

        /*
           LM35: 10 mV per degree C.
           Therefore, millivolts numerically equal
           temperature in tenths of a degree C.
           Example: 405 mV = 40.5 C.
        */
        t = mv;

        /* Basic ADC disconnected/rail fault check */
        fault = (raw < 10 || raw > 4085);

        /* Alarm hysteresis */
        if (fault)
        {
            alarm = 0;
        }
        else if (t > HIGH_T)
        {
            alarm = 1;
        }
        else if (t <= HIGH_T - HYST)
        {
            alarm = 0;
        }

        hb ^= 1u;

        LPC_GPIO0->FIOCLR = LED_ALL;

        if (fault)
        {
            status = "FAULT  ";

            if (hb)
                LPC_GPIO0->FIOSET = LED_ALL;
        }
        else
        {
            if (t < 330u)
                status = "NORMAL ";
            else if (t <= HIGH_T)
                status = "WARM   ";
            else
                status = "HIGH!  ";

            lv = led_level(t);

            /* LEDs below the current level stay ON */
            leds = ((1UL << lv) - 1UL) << LED_SHIFT;

            /* Current LED blinks */
            if (hb)
                leds |= (1UL << (lv + LED_SHIFT));

            LPC_GPIO0->FIOSET = leds;
        }

        /* Buzzer beeps in time with the blinking LED8 */
        if (!fault && alarm && hb)
            LPC_GPIO2->FIOSET = BUZZ_MASK;
        else
            LPC_GPIO2->FIOCLR = BUZZ_MASK;

        /* LCD line 1: temperature and sensor voltage */
        sprintf(line, "T:%2u.%u C %4umV ",
                t / 10, t % 10, mv);

        lcd_cmd(0x80);
        lcd_puts(line);

        /* LCD line 2: raw ADC and status */
        sprintf(line, "Raw:%4u %s", raw, status);

        lcd_cmd(0xC0);
        lcd_puts(line);

        delay(2000000);
    }
}
