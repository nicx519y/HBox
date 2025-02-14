class ADCManager {
    public:
        // ... 其他代码 ...

        // 修改 getter 方法，确保读取前清理缓存
        static const uint32_t* getADCValues() { 
            SCB_CleanInvalidateDCache_by_Addr((uint32_t*)ADCButtonValues, sizeof(ADCButtonValues));
            return ADCButtonValues; 
        }
        
        static uint32_t getADCValue(uint8_t index) { 
            if(index >= NUM_ADC_BUTTONS) return 0;
            SCB_CleanInvalidateDCache_by_Addr((uint32_t*)&ADCButtonValues[index], sizeof(uint32_t));
            return ADCButtonValues[index]; 
        }

        // ... 其他代码 ...
}; 