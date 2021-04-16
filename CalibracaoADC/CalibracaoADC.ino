#include <esp_adc_cal.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <esp_log.h>
 
 
esp_adc_cal_characteristics_t adc_cal;//Estrutura que contem as informacoes para calibracao
 
 
void app_main()
{
    adc1_config_width(ADC_WIDTH_BIT_12);//Configura a resolucao
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);//Configura a atenuacao
 
    
    esp_adc_cal_value_t adc_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_cal);//Inicializa a estrutura de calibracao
 
    if (adc_type == ESP_ADC_CAL_VAL_EFUSE_VREF)
    {
        ESP_LOGI("ADC CAL", "Vref eFuse encontrado: %umV", adc_cal.vref);
    }
    else if (adc_type == ESP_ADC_CAL_VAL_EFUSE_TP)
    {
        ESP_LOGI("ADC CAL", "Two Point eFuse encontrado");
    }
    else
    {
        ESP_LOGW("ADC CAL", "Nada encontrado, utilizando Vref padrao: %umV", adc_cal.vref);
    }
    
 
 
    while (1)
    {
        /*
            Obtem a leitura RAW do ADC para depois ser utilizada pela API de calibracao
 
            Media simples de 100 leituras intervaladas com 30us
        */
        uint32_t voltage = 0;
        for (int i = 0; i < 100; i++)
        {
            voltage += adc1_get_raw(ADC1_CHANNEL_0);//Obtem o valor RAW do ADC
            ets_delay_us(30);
        }
        voltage /= 100;
 
 
        voltage = esp_adc_cal_raw_to_voltage(voltage, &adc_cal);//Converte e calibra o valor lido (RAW) para mV
        ESP_LOGI("ADC CAL", "Read mV: %u", voltage);//Mostra a leitura calibrada no Serial Monitor
 
 
 
        vTaskDelay(pdMS_TO_TICKS(1000));//Delay 1seg
    }
 
}
