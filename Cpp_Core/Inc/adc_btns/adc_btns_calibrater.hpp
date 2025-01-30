#ifndef __ADC_BTNS_CALIBRATER_HPP__
#define __ADC_BTNS_CALIBRATER_HPP__

#include <stdio.h>
#include "adc_values_mapping.hpp"
#include "stm32h750xx.h"
#include "constant.hpp"
#include "storagemanager.hpp"
#include "adc.h"
#include "message_center.hpp"
#include "adc_btns_error.hpp"

class ADCBtnsCalibrator {
    public:
        ADCBtnsCalibrator(ADCBtnsCalibrator const&) = delete;
        void operator=(ADCBtnsCalibrator const&) = delete;

        static __attribute__((section("._RAM_D1_Area"))) uint32_t ADC_Values[NUM_ADC_BUTTONS];

        static ADCBtnsCalibrator& getInstance() {
            static ADCBtnsCalibrator instance;
            return instance;
        }
        ADCBtnsError setup(const char* mapping_name);
        ADCBtnsError start();
        ADCBtnsError stop();
        ADCBtnsError loop();

    private:
        ADCBtnsCalibrator();
        bool is_dma_started = false;
        char mapping_name[16];
        uint32_t min_value[NUM_ADC_BUTTONS];
        uint32_t max_value[NUM_ADC_BUTTONS];
        
};

#define ADC_BTNS_CALIBRATOR ADCBtnsCalibrator::getInstance()

#endif