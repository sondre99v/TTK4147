#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"
#include <avr32/io.h>

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


__attribute__((__interrupt__)) static void interrupt_J3(void);

void init(){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);
    
    cpu_irq_disable();
    INTC_init_interrupts();
    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

uint8_t flags[3] = {0};

__attribute__((__interrupt__)) static void interrupt_J3(void) {
	if(gpio_get_pin_interrupt_flag(TEST_A)){
		gpio_clear_pin_interrupt_flag(TEST_A);
		//flags[0] = 1;
		gpio_set_pin_low(RESPONSE_A);
	}
	if(gpio_get_pin_interrupt_flag(TEST_B)){
		busy_delay_us(100);
		gpio_clear_pin_interrupt_flag(TEST_B);
		//flags[1] = 1;
		gpio_set_pin_low(RESPONSE_B);
	}
	if(gpio_get_pin_interrupt_flag(TEST_C)){
		gpio_clear_pin_interrupt_flag(TEST_C);
		//flags[2] = 1;
		gpio_set_pin_low(RESPONSE_C);
	}
	busy_delay_us(5);
	gpio_set_pin_high(RESPONSE_A);
	gpio_set_pin_high(RESPONSE_B);
	gpio_set_pin_high(RESPONSE_C);
}

int main (void) {
    init();
	
	gpio_configure_pin (TEST_A, GPIO_DIR_INPUT);
	gpio_configure_pin (TEST_B, GPIO_DIR_INPUT);
	gpio_configure_pin (TEST_C, GPIO_DIR_INPUT);
    
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
	
	gpio_configure_pin (RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin (RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	gpio_configure_pin (RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
	
    while(1){
		
		if(flags[0]){
			gpio_set_pin_low(RESPONSE_A);
			busy_delay_us(5);
			gpio_set_pin_high(RESPONSE_A);
			flags[0] = 0;
		}
		if(flags[1]){
			//busy_delay_us(100);
			gpio_set_pin_low(RESPONSE_B);
			busy_delay_us(5);
			gpio_set_pin_high(RESPONSE_B);
			flags[1] = 0;
		}
		if(flags[2]){
			gpio_set_pin_low(RESPONSE_C);
			busy_delay_us(5);
			gpio_set_pin_high(RESPONSE_C);
			flags[2] = 0;
		}
		
		continue;
		int a,b,c;
		do {
			a = gpio_get_pin_value(TEST_A);
			b = gpio_get_pin_value(TEST_B);
			c = gpio_get_pin_value(TEST_C);
		} while(a&&b&&c);
		
		if (!a) {
			gpio_set_pin_low(RESPONSE_A);
		}
		if (!b) {
			gpio_set_pin_low(RESPONSE_B);
		}
		if (!c) {
			gpio_set_pin_low(RESPONSE_C);
		}
		busy_delay_us(5);
		gpio_set_pin_high(RESPONSE_A);
		gpio_set_pin_high(RESPONSE_B);
		gpio_set_pin_high(RESPONSE_C);
		//gpio_toggle_pin(LED0_GPIO);

        //printf("tick\n");
        
        //busy_delay_ms(500);
    }
}
