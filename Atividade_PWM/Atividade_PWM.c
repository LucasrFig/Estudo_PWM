#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h" 

#define PWM_pin 11
#define wrap_led 19999.0
#define divisor_led 125.0
//Duty Cycles  
#define dutyc_1 2400
#define dutyc_2 1470
#define dutyc_3 500
#define jump 10 //velocidade na qual o pwm aumenta/diminui


uint get_pwm_feq();//Frequência PWM
uint get_pwm_active_time_us();//tempo ativo no período
uint get_pwm_duty_cycle();//Porcentagem do duty cycle
uint get_pwm_period();//período total PWM


void wrapHandler(){
    static int fade = 500; //nível do PWM
    static bool rise = false; //flag para elevar ou reduzir o nível
    pwm_clear_irq(pwm_gpio_to_slice_num(PWM_pin)); //resetar o flag de interrupção

    if(rise){ //caso a iluminação seja elevada
        fade+=jump; 
        if(fade > 2400){ //caso o fade seja maior que 255
            fade = 2400; 
            rise = false;//passa a reduzir agora
        }
    }
    else{ //caso a iluminação seja reduzida
        fade-=jump; //reduz o nível de brilho
        if(fade < 500){ //Caso o fade seja menor que 0
            fade = 500; 
            rise = true; //Passa a elevar agora
        }
    }
    //printf("%d\n",fade);
    pwm_set_gpio_level(PWM_pin, fade);
}

//Configura o pwm
uint gpio_pwm_config(uint gpio, float divisor,uint wrap,uint dutycycle){
    gpio_set_function(gpio,GPIO_FUNC_PWM);//Habilitando pino como PWM
    uint slice = pwm_gpio_to_slice_num(gpio);//Obter canal PWM da GPIO

    //Configurar interrupção
    pwm_clear_irq(slice); 
    pwm_set_irq_enabled(slice, false); 
    irq_set_exclusive_handler(PWM_IRQ_WRAP, wrapHandler); 
    irq_set_enabled(PWM_IRQ_WRAP, true); //Habilitar ou desabilitar uma interrupção específica
    
    //Configura 
    pwm_set_clkdiv(slice,divisor);//Define o valor do divisor de clock 
    pwm_set_wrap(slice,wrap);//Define Valor de Wrap
    pwm_set_gpio_level(gpio,dutycycle);//Define valor Duty Cycle
    
    return slice;
}

int main()
{
    //inicializar sistema de entrada e saída padrão.
    stdio_init_all();
    
    //configurar pwm
    uint slice1 = gpio_pwm_config(PWM_pin,divisor_led,wrap_led,dutyc_1);
    pwm_set_enabled(slice1, true); //habilita o pwm no slice correspondente
    
    

    for(uint i = 2400;i>=500;i--){
        pwm_set_gpio_level(PWM_pin,i);
        if(i==dutyc_1||i==dutyc_2||i==dutyc_3){
            sleep_ms(5000);
        }
    }
    
    
        //smooth_movment();
        /*double frequency_pwm = (clock_get_hz(clk_sys))/((wrap_led+1.0)*divisor_led);
        double period_pwm = ((wrap_led + 1)*divisor_led*1000)/(clock_get_hz(clk_sys));
        double dc_pwm = ((dutyc_1*100)/(wrap_led));
        double activet_pwm = ((dutyc_1*divisor_led*1000000)/(clock_get_hz(clk_sys)));
        printf("Frequency: [%.0lfHz]\n",frequency_pwm);
        printf("Period: [%.0lfms]\n",period_pwm);
        printf("Duty Cycle: [%lf%%]\n",dc_pwm);
        printf("Active time in 'us': [%.0lfus]\n",activet_pwm);
        printf("================================================\n");*/

    while (1) {
        pwm_set_irq_enabled(slice1, true);
    }
}



