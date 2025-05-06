#include <stdio.h>
#include "pico/stdlib.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "Semaforo.pio.h"
#include "pico/bootrom.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

//---------------------------------------------------
// DEFINES
//---------------------------------------------------
#define I2C_PORT        i2c1
#define I2C_SDA         14
#define I2C_SCL         15
#define ENDERECO        0x3C
#define LED_RED         13
#define LED_GREEN        11
#define LED_MATRIX      7
#define BUZZER          21
#define BUTTON_A        5
#define botaoB 6

//---------------------------------------------------
// VARIAVEIS GLOBAIS
//---------------------------------------------------

volatile bool modo = true;
volatile uint8_t controle = 0;

void Controle_Matriz(){
    // Configura o PIO para controlar a matriz de LEDs
    PIO pio = pio0;
    bool clk = set_sys_clock_khz(128000, false);
    uint offset = pio_add_program(pio, &Semaforo_program);
    uint sm_pio = pio_claim_unused_sm(pio, true);
    Semaforo_program_init(pio, sm_pio, offset, LED_MATRIX);

    static const uint32_t VERDE   = 0x99000000;
    static const uint32_t VERMELHO =  0x00990000;
    static const uint32_t AMARELO  = 0x99990000;

    uint32_t desenhos[3][25] = {
        {VERMELHO, 0, 0, 0, VERMELHO,
        0, VERMELHO, 0, VERMELHO, 0,
        0, 0, VERMELHO, 0, 0,
        0, VERMELHO, 0, VERMELHO, 0,
        VERMELHO, 0, 0, 0, VERMELHO
        },
        {0, 0, AMARELO, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, AMARELO, 0, 0,
        0, 0, AMARELO, 0, 0,
        0, 0, AMARELO, 0, 0,
        },
        {0, VERDE, VERDE, VERDE, 0,
        VERDE, 0, 0, 0, VERDE,
        VERDE, 0, 0, 0, VERDE,
        VERDE, 0, 0, 0, VERDE,
        0, VERDE, VERDE, VERDE, 0}};

    while(true){
        if (modo){
            for (int i = 0; i < 25; i++){
                pio_sm_put_blocking(pio, sm_pio, desenhos[controle][i]);
            }vTaskDelay(pdMS_TO_TICKS(200));
        }else{
            for (int i = 0; i < 25; i++){
                pio_sm_put_blocking(pio, sm_pio, desenhos[1][i]);
            }vTaskDelay(pdMS_TO_TICKS(1000));
            for (int i = 0; i < 25; i++){
            pio_sm_put_blocking(pio, sm_pio, 0);
            }vTaskDelay(pdMS_TO_TICKS(1000));
        }

    }

}
void Controle_Display(){
    uint controle_aux = 4;
    bool norturno = true;
    ssd1306_t ssd;
    uint8_t largura = 40;
    uint8_t altura = 19;
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // Inicializa o display via I2C
    i2c_init(I2C_PORT, 400000); // 0.4 MHz
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);
    while (true)
    {
        if (modo){
            printf("CONTROLE %d | CONTROLE AUX %d \n", controle, controle_aux);
            if (controle_aux != controle){
                ssd1306_fill(&ssd, false);
                ssd1306_rect(&ssd, 4, 4, largura, 57, 1,0);
                ssd1306_rect(&ssd, 23, 4, largura, altura, 1,0);
                switch (controle)
                {
                    case 0:
                    ssd1306_rect(&ssd, 4, 4, largura, altura, 1,1);
                    ssd1306_draw_string(&ssd, "VERMELHO", 50, 45);
                    break;
                    case 1:
                    ssd1306_rect(&ssd, 4 + altura, 4, largura, altura, 1,1);
                    ssd1306_draw_string(&ssd, "AMARELO", 50, 45);
                    break;
                    case 2:
                    ssd1306_rect(&ssd, 4 + 2*altura, 4, largura, altura, 1,1);
                    ssd1306_draw_string(&ssd, "VERDE", 50, 45);
                    break;
                    default:
                    break;
            }
                ssd1306_draw_string(&ssd, "MODO:", 50, 10);
                ssd1306_draw_string(&ssd, "NORMAL", 50, 20);
                ssd1306_draw_string(&ssd, "SINAL:", 50, 35);
                ssd1306_send_data(&ssd);
        }controle_aux = controle;
        }else{
            ssd1306_fill(&ssd, false);
            ssd1306_rect(&ssd, 4, 4, largura, 57, 1,0);
            ssd1306_rect(&ssd, 23, 4, largura, altura, 1,0);
            ssd1306_rect(&ssd, 4 + altura, 4, largura, altura, 1,norturno);
            ssd1306_draw_string(&ssd, "MODO:", 50, 10);
            ssd1306_draw_string(&ssd, "NOTURNO", 50, 20);
            ssd1306_draw_string(&ssd, "ATENCAO!", 50, 30);
            ssd1306_send_data(&ssd);
            norturno =!norturno;
        }
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    

}

void Controle_LED()
{   
    gpio_init(LED_RED);
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_init(LED_GREEN);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    uint16_t intervalo = 3000;

    while (true)
    {
        if (modo){
            gpio_put(LED_RED  , true);
            vTaskDelay(pdMS_TO_TICKS(intervalo)); //Vermelho
            controle = (controle + 1) %3;
        }if (modo){
            gpio_put(LED_GREEN  , true);
            vTaskDelay(pdMS_TO_TICKS(intervalo)); //Amarelo
            controle = (controle + 1) %3;
            }
        if (modo){
            gpio_put(LED_RED  , false);
            vTaskDelay(pdMS_TO_TICKS(intervalo)); //Verde
            controle = (controle + 1) %3;
            gpio_put(LED_GREEN  , false);}
        else{
        gpio_put(LED_RED  , true);
        gpio_put(LED_GREEN  , true);
        vTaskDelay(pdMS_TO_TICKS(1000));
        gpio_put(LED_RED  ,false);
        gpio_put(LED_GREEN  , false);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }}}

void Controle_Buzzer(){
// PWM BUZZER
    gpio_set_function(BUZZER, GPIO_FUNC_PWM);
    uint slice = pwm_gpio_to_slice_num(BUZZER); 
    const uint16_t wrap = 1000;   // Valor de wrap do PWM
    pwm_set_enabled(slice, true);
    pwm_set_wrap(slice, wrap);
    pwm_set_clkdiv(slice, 128); //1Khz 

    while(true){
        if (modo){
                switch (controle)
            {
                case 0:
                    pwm_set_gpio_level(BUZZER, 500);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    pwm_set_gpio_level(BUZZER,0);
                    vTaskDelay(pdMS_TO_TICKS(1000));
                    break;
                case 1:
                    pwm_set_gpio_level(BUZZER, 300);
                    break;
                case 2:
                    pwm_set_gpio_level(BUZZER, 200);
                    vTaskDelay(pdMS_TO_TICKS(500));
                    pwm_set_gpio_level(BUZZER,0);
                    vTaskDelay(pdMS_TO_TICKS(1500));
                    break;
                default:
                    pwm_set_enabled(slice, false);
                    break;
            }}
            else{
                pwm_set_gpio_level(BUZZER, 300);
                vTaskDelay(pdMS_TO_TICKS(2000));
                pwm_set_gpio_level(BUZZER,0);
                vTaskDelay(pdMS_TO_TICKS(2000));

            }

    }

}

void gpio_irq_handler(uint gpio, uint32_t events)
{
    static absolute_time_t last_time_A = 0;
    static absolute_time_t last_time_B = 0;
    absolute_time_t now = get_absolute_time();

    if (gpio == BUTTON_A) {
        if (absolute_time_diff_us(last_time_A, now) > 200000) {
            modo = !modo;
            printf("MODO DE OPERAÇÂO: %d \n", modo);
            last_time_A = now;
        }
    } else if (gpio == botaoB) {
        if (absolute_time_diff_us(last_time_B, now) > 200000) {
            reset_usb_boot(0, 0);
            last_time_B = now;
        }

}}


int main()
{
    // Para ser utilizado o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_init(BUTTON_A);
    gpio_set_dir(BUTTON_A, GPIO_IN);
    gpio_pull_up(BUTTON_A);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled(BUTTON_A, GPIO_IRQ_EDGE_FALL, true);
    // Fim do trecho para modo BOOTSEL com botão B


    stdio_init_all();
    xTaskCreate(Controle_LED, "Controle LED", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(Controle_Buzzer, "Controle BUZZER", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(Controle_Display, "Controle DISPLAY", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL);
    xTaskCreate(Controle_Matriz, "Controle MATRIZ", configMINIMAL_STACK_SIZE, 
        NULL, tskIDLE_PRIORITY, NULL);
    vTaskStartScheduler();
    panic_unsupported();
}
