## Semáforo Inteligente com FreeRTOS no Raspberry Pi Pico

Este projeto implementa um **semáforo virtual** usando **FreeRTOS** em uma Raspberry Pi Pico, com saída em LEDs, matriz 5×5 via PIO, display OLED e buzzer. Há dois modos de operação (Normal e Noturno), que podem ser alternados em tempo real.

---

## ⚙️ Funcionalidades

- **Gerenciamento por FreeRTOS**  
  Quatro tarefas simultâneas controlam LEDs, buzzer, display e matriz de LEDs 5×5.

- **Modo Normal**  
  - Sequência de semáforo tradicional: Vermelho → Amarelo → Verde  
  - LED vermelho e verde piscam alternadamente  
  - Matriz 5×5 exibe ícones (X para vermelho, linha para amarelo, círculo para verde)  
  - Display OLED mostra “MODO: NORMAL” e qual sinal está ativo  
  - Buzzer emite tons diferentes para cada fase

- **Modo Noturno**  
  - Toque intermitente de atenção: LEDs vermelho e verde piscam juntos  
  - Matriz 5×5 pisca em amarelo  
  - Display OLED mostra “MODO: NOTURNO” e “ATENÇÃO!”  
  - Buzzer emite tom único intermitente

- **Botões**  
  - **Botão A (GPIO 5)**: alterna entre Modo Normal e Noturno  
  - **Botão B (GPIO 6)**: entra em modo BOOTSEL para atualização do firmware
  
---

## 🧰 Hardware e Bibliotecas

- **Placa**: Raspberry Pi Pico (RP2040)  
- **Display OLED**: SSD1306 via I2C (`lib/ssd1306.h`, `lib/font.h`)  
- **LEDs indicadores**: GPIO 13 (vermelho), GPIO 11 (verde)  
- **Matriz de LEDs 5×5**: controlada por PIO (`Semaforo.pio`) em GPIO 7  
- **Buzzer PWM**: GPIO 21, driver via `hardware/pwm.h`  
- **Botões**: GPIO 5 (A), GPIO 6 (B/BOOTSEL)  
- **RTOS**: FreeRTOS (`FreeRTOS.h`, `task.h`)

---

## 📌 Mapeamento de Pinos

| Função                    | GPIO |
|---------------------------|------|
| I2C SDA (OLED)            | 14   |
| I2C SCL (OLED)            | 15   |
| LED Vermelho (Semáforo)   | 13   |
| LED Verde (Semáforo)      | 11   |
| Matriz 5×5 (PIO)          | 7    |
| Buzzer PWM                | 21   |
| Botão A (modo)            | 5    |
| Botão B (BOOTSEL)         | 6    |

---
## 📋 Estrutura do Código
