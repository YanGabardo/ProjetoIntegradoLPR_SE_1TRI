/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Controle de Sala de Aula
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "st7735\st7735.h"
#include <stdlib.h>
#include <stdio.h>

/* USER CODE BEGIN PD */
#define LerB1 HAL_GPIO_ReadPin(Botao1_GPIO_Port, Botao1_Pin)
#define LerB2 HAL_GPIO_ReadPin(Botao2_GPIO_Port, Botao2_Pin)
#define LerB3 HAL_GPIO_ReadPin(Botao3_GPIO_Port, Botao3_Pin)
#define LerB4 HAL_GPIO_ReadPin(Botao4_GPIO_Port, Botao4_Pin)

#define LIGA_L1    HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_SET)
#define DESLIGA_L1 HAL_GPIO_WritePin(Led1_GPIO_Port, Led1_Pin, GPIO_PIN_RESET)
#define INVERTE_L1 HAL_GPIO_TogglePin(Led1_GPIO_Port, Led1_Pin)

#define LIGA_L2    HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, GPIO_PIN_SET)
#define DESLIGA_L2 HAL_GPIO_WritePin(Led2_GPIO_Port, Led2_Pin, GPIO_PIN_RESET)
#define INVERTE_L2 HAL_GPIO_TogglePin(Led2_GPIO_Port, Led2_Pin)

#define LIGA_L3    HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_SET)
#define DESLIGA_L3 HAL_GPIO_WritePin(Led3_GPIO_Port, Led3_Pin, GPIO_PIN_RESET)
#define INVERTE_L3 HAL_GPIO_TogglePin(Led3_GPIO_Port, Led3_Pin)

#define LIGA_L4    HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, GPIO_PIN_SET)
#define DESLIGA_L4 HAL_GPIO_WritePin(Led4_GPIO_Port, Led4_Pin, GPIO_PIN_RESET)
#define INVERTE_L4 HAL_GPIO_TogglePin(Led4_GPIO_Port, Led4_Pin)
/* USER CODE END PD */

/* USER CODE BEGIN PV */
int maxAlunos       = 10;
int maxFora         = 3;
int alunosPresentes = 0;
int alunosFora      = 0;
int totalSaidas     = 0;
int senhaGerada = 0;
int senhaDigitada   = 0;
int tentativas      = 0;
int digitos[4]      = {0, 0, 0, 0};
int posicao         = 0;
int limpouTela      = 0;
int matriculas[50];
char texto[50];
volatile int novaMatricula = 0;
GPIO_PinState B1, B2, B3, B4;

uint32_t inicioAula     = 0;
uint32_t fimAula        = 0;
uint32_t inicioSaida    = 0;
uint32_t tempoTotalFora = 0;
uint32_t duracaoAula    = 0;
/* USER CODE END PV */

SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

/* USER CODE BEGIN 0 */
void MostrarSenhaNosLEDs(void);
void ControleSaida(void);
void InterfaceSala(void);
void EsperaBotao(void);
void RelatorioFinal(void);
void ControlePresenca(void);

/* USER CODE END 0 */
int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_SPI1_Init();


    ST7735_Init();
    DESLIGA_L1; DESLIGA_L2; DESLIGA_L3; DESLIGA_L4;


    uint32_t seed = HAL_GetTick() ^ SysTick->VAL ^ SCB->ICSR;
    srand(seed);
    senhaGerada = rand() % 9000 + 1000;



    ST7735_FillScreen(BLACK);
    sprintf(texto, "Senha:%04d", senhaGerada);
    ST7735_WriteString(2, 10, "INICIO AULA",  Font_7x10, WHITE,  BLACK);
    ST7735_WriteString(2, 55, "P11=Iniciar",  Font_7x10, WHITE,  BLACK);
    ST7735_WriteString(2, 70, "P9=Ver Senha",  Font_7x10, GREEN,  BLACK);

    while(1)

    {
    	B1 = LerB1;
        if(B1 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            MostrarSenhaNosLEDs();
        }

        B3 = LerB3;
        if(B3 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            break;
        }
    }

    tentativas = 0;

    while(tentativas < 3)
    {
        for(int i = 0; i < 4; i++) digitos[i] = 0;
        posicao       = 0;
        senhaDigitada = 0;

        ST7735_FillScreen(BLACK);

        int confirmado = 0;
        while(!confirmado)
        {
            senhaDigitada =
                  digitos[0]
                + digitos[1] * 10
                + digitos[2] * 100
                + digitos[3] * 1000;

            ST7735_WriteString(2,  5, "Confirmar senha", Font_7x10, WHITE, BLACK);

            sprintf(texto, "Dig:%04d", senhaDigitada);
            ST7735_WriteString(2, 25, texto, Font_7x10, GREEN, BLACK);

            sprintf(texto, "Casa:%d", posicao + 1);
            ST7735_WriteString(2, 45, texto, Font_7x10, WHITE, BLACK);

            sprintf(texto, "Tent:%d", 3 - tentativas);
            ST7735_WriteString(2, 65, texto, Font_7x10, WHITE, BLACK);

            B1 = LerB1;
            B2 = LerB2;
            B3 = LerB3;
            B4 = LerB4;
            if(B1 == GPIO_PIN_RESET)
            {
                if(digitos[posicao] < 9) digitos[posicao]++;
                EsperaBotao();
            }
            else if(B2 == GPIO_PIN_RESET)
            {
                if(digitos[posicao] > 0) digitos[posicao]--;
                EsperaBotao();
            }

            else if(B4 == GPIO_PIN_RESET)
            {
                posicao++;
                if(posicao > 3) posicao = 0;
                EsperaBotao();
            }

            else if(B3 == GPIO_PIN_RESET)
            {
                EsperaBotao();
                confirmado = 1;
            }
        }

        if(senhaDigitada == senhaGerada)
        {
            inicioAula = HAL_GetTick();
            ST7735_FillScreen(BLACK);
            ST7735_WriteString(2, 35, "AULA INICIADA", Font_7x10, GREEN, BLACK);
            LIGA_L1;
            HAL_Delay(1500);
            DESLIGA_L1;
            break;
        }
        else
        {
            tentativas++;
            ST7735_FillScreen(BLACK);

            if(tentativas < 3)
            {
                ST7735_WriteString(2, 35, "SENHA ERRADA", Font_7x10, RED, BLACK);
                sprintf(texto, "Restam:%d", 3 - tentativas);
                ST7735_WriteString(2, 55, texto, Font_7x10, RED, BLACK);
                HAL_Delay(1500);
            }
        }
    }


    if(tentativas >= 3)
    {
        ST7735_FillScreen(BLACK);
        ST7735_WriteString(2, 25, "ACESSO NEGADO", Font_7x10, RED, BLACK);
        ST7735_WriteString(2, 45, "3 ERROS",       Font_7x10, RED, BLACK);
        LIGA_L4;
        while(1) { }
    }
    ST7735_FillScreen(BLACK);

    while(1)
    {
        sprintf(texto, "Max:%02d", maxAlunos);
        ST7735_WriteString(2,  10, "CONFIG TURMA",   Font_7x10, WHITE, BLACK);
        ST7735_WriteString(2,  35, texto,             Font_7x10, GREEN, BLACK);
        ST7735_WriteString(2,  55, "P11=Conf",        Font_7x10, WHITE, BLACK);
        ST7735_WriteString(65, 55, "P9=Registrar",    Font_7x10, WHITE, BLACK);
        ST7735_WriteString(2,  70, "P12=Mais",        Font_7x10, GREEN, BLACK);
        ST7735_WriteString(65, 70, "P10=Menos",       Font_7x10, RED,   BLACK);

        B1 = LerB1;
        if(B1 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            if(alunosPresentes < maxAlunos)
            {
                alunosPresentes++;
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 40, "REGISTRADO", Font_7x10, GREEN, BLACK);
                LIGA_L2; HAL_Delay(1000); DESLIGA_L2;
            }
            else
            {
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 30, "TURMA LOTADA", Font_7x10, RED, BLACK);
                HAL_Delay(1000);
            }
            ST7735_FillScreen(BLACK);
        }

        B3 = LerB3;
        B4 = LerB4;
        B2 = LerB2;
        if(B4 == GPIO_PIN_RESET)
        {
            if(maxAlunos < 50) maxAlunos++;
            EsperaBotao();
        }
        else if(B2 == GPIO_PIN_RESET)
        {
            if(maxAlunos > 1) maxAlunos--;
            EsperaBotao();
        }
        else if(B3 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            break;
        }
    }
    ST7735_FillScreen(BLACK);

    while(1)
    {
        ST7735_WriteString(2,  5, "MENU AULA",    Font_7x10, WHITE, BLACK);
        ST7735_WriteString(2, 22, "P9=Presenca",  Font_7x10, GREEN, BLACK);
        ST7735_WriteString(2, 38, "P10=Banheiro", Font_7x10, GREEN, BLACK);
        ST7735_WriteString(2, 54, "P11=Status",   Font_7x10, WHITE, BLACK);
        ST7735_WriteString(2, 70, "P11+P12=Fim",  Font_7x10, RED,   BLACK);

        B1 = LerB1;
        if(B1 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            ControlePresenca();
            ST7735_FillScreen(BLACK);
        }

        B2 = LerB2;
        if(B2 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            ControleSaida();
            ST7735_FillScreen(BLACK);
        }

        B3 = LerB3;
        B4 = LerB4;
        if(B3 == GPIO_PIN_RESET)
        {
            B4 = LerB4;
            HAL_Delay(200);
            if(B4 == GPIO_PIN_RESET)
            {
            	B3 = LerB3;
            	B4 = LerB4;
            	while(LerB3 == GPIO_PIN_RESET || LerB4 == GPIO_PIN_RESET) { }
            	        HAL_Delay(150);
            	        RelatorioFinal();
            	        break;
            }
            else
            {
                EsperaBotao();
                InterfaceSala();
                ST7735_FillScreen(BLACK);
            }
        }
    }


    while(1) { }
}

/* USER CODE BEGIN 4 */

void PiscarLED(int led, int vezes)
{
    if(vezes == 0)
    {
        switch(led)
        {
            case 1: LIGA_L1; HAL_Delay(600); DESLIGA_L1; break;
            case 2: LIGA_L2; HAL_Delay(600); DESLIGA_L2; break;
            case 3: LIGA_L3; HAL_Delay(600); DESLIGA_L3; break;
            case 4: LIGA_L4; HAL_Delay(600); DESLIGA_L4; break;
        }
        HAL_Delay(300);
        return;
    }

    for(int i = 0; i < vezes; i++)
    {
        switch(led)
        {
            case 1: LIGA_L1; HAL_Delay(200); DESLIGA_L1; break;
            case 2: LIGA_L2; HAL_Delay(200); DESLIGA_L2; break;
            case 3: LIGA_L3; HAL_Delay(200); DESLIGA_L3; break;
            case 4: LIGA_L4; HAL_Delay(200); DESLIGA_L4; break;
        }
        HAL_Delay(150);
    }
    HAL_Delay(400);
}
void MostrarSenhaNosLEDs(void)
{
    DESLIGA_L1; DESLIGA_L2; DESLIGA_L3; DESLIGA_L4;
    HAL_Delay(500);

    int d3 = (senhaGerada / 1000) % 10;
    int d2 = (senhaGerada / 100)  % 10;
    int d1 = (senhaGerada / 10)   % 10;
    int d0 = (senhaGerada / 1)    % 10;

    PiscarLED(1, d3);
    HAL_Delay(400);

    PiscarLED(2, d2);
    HAL_Delay(400);

    PiscarLED(3, d1);
    HAL_Delay(400);

    PiscarLED(4, d0);
    HAL_Delay(400);

    DESLIGA_L1; DESLIGA_L2; DESLIGA_L3; DESLIGA_L4;
}
void EsperaBotao(void)
{
    HAL_Delay(150);

    while(1)
    {
        B1 = LerB1;
        B2 = LerB2;
        B3 = LerB3;
        B4 = LerB4;

        if(B1 == GPIO_PIN_SET &&
           B2 == GPIO_PIN_SET &&
           B3 == GPIO_PIN_SET &&
           B4 == GPIO_PIN_SET)
        {
            break;
        }
    }
}
void ControlePresenca(void)
{
    ST7735_FillScreen(BLACK);

    while(1)
    {
        sprintf(texto, "Max:%d", maxAlunos);
        ST7735_WriteString(2,  5, texto,      Font_7x10, WHITE, BLACK);

        sprintf(texto, "Alunos:%d", alunosPresentes);
        ST7735_WriteString(2, 25, texto,      Font_7x10, GREEN, BLACK);

        ST7735_WriteString(2, 45, "P10=Criar", Font_7x10, WHITE, BLACK);
        ST7735_WriteString(2, 65, "P12=Sair",  Font_7x10, WHITE, BLACK);

        B1 = LerB1;
        B2 = LerB2;
        B3 = LerB3;
        B4 = LerB4;

        if(B2 == GPIO_PIN_RESET)
        {
            EsperaBotao();

            if(alunosPresentes < maxAlunos)
            {
                matriculas[alunosPresentes] = alunosPresentes + 1;
                alunosPresentes++;

                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 15, "MATRICULA CRIADA", Font_7x10, GREEN, BLACK);
                sprintf(texto, "Matricula:%d", matriculas[alunosPresentes - 1]);
                ST7735_WriteString(2, 35, texto, Font_7x10, YELLOW, BLACK);
                HAL_Delay(1500);
                ST7735_FillScreen(BLACK);
            }
            else
            {
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 35, "TURMA LOTADA", Font_7x10, RED, BLACK);
                HAL_Delay(1000);
                ST7735_FillScreen(BLACK);
            }
        }

        if(B4 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            break;
        }
    }     HAL_Delay(1000);
}
void ControleSaida(void)
{
    ST7735_FillScreen(BLACK);

    while(1)
    {
        sprintf(texto, "Fora:%d/%d", alunosFora, maxFora);
        ST7735_WriteString(2,  5, texto,         Font_7x10, WHITE, BLACK);
        ST7735_WriteString(2, 25, "P9=Sair",     Font_7x10, GREEN, BLACK);
        ST7735_WriteString(2, 45, "P10=Volta",   Font_7x10, GREEN, BLACK);
        ST7735_WriteString(2, 65, "P12=Menu",    Font_7x10, WHITE, BLACK);

        B1 = LerB1;
        B2 = LerB2;
        B3 = LerB3;
        B4 = LerB4;

        if(B1 == GPIO_PIN_RESET)
        {
            EsperaBotao();

            if(alunosPresentes == 0)
            {
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 35, "SEM ALUNOS", Font_7x10, RED, BLACK);
                LIGA_L4; HAL_Delay(1000); DESLIGA_L4;
                ST7735_FillScreen(BLACK);
            }
            else if(alunosFora >= alunosPresentes)
            {
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 25, "TODOS JA",    Font_7x10, RED, BLACK);
                ST7735_WriteString(2, 45, "ESTAO FORA",  Font_7x10, RED, BLACK);
                LIGA_L4; HAL_Delay(1000); DESLIGA_L4;
                ST7735_FillScreen(BLACK);
            }
            else if(alunosFora >= maxFora)
            {
                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 35, "LIMITE ATINGIDO", Font_7x10, RED, BLACK);
                LIGA_L4; HAL_Delay(1000); DESLIGA_L4;
                ST7735_FillScreen(BLACK);
            }
            else
            {
                alunosFora++;
                inicioSaida = HAL_GetTick();
                totalSaidas++;

                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 35, "SAIDA LIBERADA", Font_7x10, GREEN, BLACK);
                sprintf(texto, "Fora:%d/%d", alunosFora, maxFora);
                ST7735_WriteString(2, 55, texto, Font_7x10, WHITE, BLACK);
                LIGA_L3; HAL_Delay(1000); DESLIGA_L3;
                ST7735_FillScreen(BLACK);
            }
        }
        else if(B2 == GPIO_PIN_RESET)
        {
            EsperaBotao();

            if(alunosFora > 0)
            {
                uint32_t tempoFora = (HAL_GetTick() - inicioSaida) / 1000;
                alunosFora--;
                tempoTotalFora += tempoFora;

                ST7735_FillScreen(BLACK);
                ST7735_WriteString(2, 15, "ALUNO VOLTOU",   Font_7x10, GREEN, BLACK);
                sprintf(texto, "Tempo:%lds", tempoFora);
                ST7735_WriteString(2, 40, texto,            Font_7x10, WHITE, BLACK);
                HAL_Delay(2000);
            }
            ST7735_FillScreen(BLACK);
        }
        else if(B4 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            break;
        }
    }
}
void InterfaceSala(void)
{
    ST7735_FillScreen(BLACK);
    ST7735_WriteString(15, 5, "STATUS SALA", Font_7x10, WHITE, BLACK);

    while(1)
    {
        sprintf(texto, "Presentes:%d/%d", alunosPresentes, maxAlunos);
        ST7735_WriteString(2, 25, texto, Font_7x10, GREEN, BLACK);

        int barra = (alunosPresentes * 96) / maxAlunos;
        ST7735_FillRectangle(10, 45, 100, 10, WHITE);
        ST7735_FillRectangle(12, 47, barra,  6, GREEN);

        sprintf(texto, "Fora:%d/%d", alunosFora, maxFora);
        ST7735_WriteString(2, 60, texto, Font_7x10, WHITE, BLACK);
        B1 = LerB1;
        B2 = LerB2;
        B3 = LerB3;
        B4 = LerB4;
        if(alunosFora >= maxFora)
            ST7735_WriteString(80, 60, "OCUPADO", Font_7x10, RED,   BLACK);
        else
            ST7735_WriteString(80, 60, "LIVRE  ", Font_7x10, GREEN, BLACK);

        if(B4 == GPIO_PIN_RESET)
        {
            EsperaBotao();
            break;
        }
        HAL_Delay(200);
    }
}
void RelatorioFinal(void)
{
    ST7735_FillScreen(BLACK);

    fimAula     = HAL_GetTick();
    duracaoAula = (fimAula - inicioAula) / 1000;

    while(1)
    {
        sprintf(texto, "Alunos:%d",   alunosPresentes);
        ST7735_WriteString(2,  5, texto, Font_7x10, GREEN, BLACK);

        sprintf(texto, "Saidas:%d",   totalSaidas);
        ST7735_WriteString(2, 20, texto, Font_7x10, WHITE, BLACK);

        sprintf(texto, "Tempo:%lds",  duracaoAula);
        ST7735_WriteString(2, 35, texto, Font_7x10, WHITE, BLACK);

        sprintf(texto, "TempoTotal:%lds", tempoTotalFora);
        ST7735_WriteString(2, 50, texto, Font_7x10, WHITE, BLACK);
    }
}
/* USER CODE END 4 */


void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSI_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL16;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) Error_Handler();

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                     | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) Error_Handler();
}

static void MX_SPI1_Init(void)
{
    hspi1.Instance               = SPI1;
    hspi1.Init.Mode              = SPI_MODE_MASTER;
    hspi1.Init.Direction         = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize          = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity       = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase          = SPI_PHASE_1EDGE;
    hspi1.Init.NSS               = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi1.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode            = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial     = 10;
    if(HAL_SPI_Init(&hspi1) != HAL_OK) Error_Handler();
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(ST7735_CS_GPIO_Port, ST7735_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, ST7735_DC_Pin | ST7735_RES_Pin |
                             Led1_Pin | Led2_Pin | Led3_Pin | Led4_Pin, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = ST7735_CS_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(ST7735_CS_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = ST7735_DC_Pin | ST7735_RES_Pin |
                          Led1_Pin | Led2_Pin | Led3_Pin | Led4_Pin;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin  = Botao1_Pin | Botao2_Pin | Botao3_Pin | Botao4_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Error_Handler(void)
{
    __disable_irq();
    while(1) { }
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) { }
#endif
