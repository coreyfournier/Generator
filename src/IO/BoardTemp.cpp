#include <soc/rtc.h>
#include <stdio.h>
#include "Arduino.h"
#include <Syslog.h>

namespace IO
{       
    #define M2_CALPOINT1_CELSIUS 23.0f
    #define M2_CALPOINT1_RAW 163600.0f
    #define M2_CALPOINT2_CELSIUS -20.0f
    #define M2_CALPOINT2_RAW 183660.0f
    #define M1_CALPOINT1_CELSIUS 23.0f
    #define M1_CALPOINT1_RAW 128253742.0f
    #define M1_CALPOINT2_CELSIUS -20.0f
    #define M1_CALPOINT2_RAW 114261758.0f

    /***
     * Not sure how great this code is. Got it from https://github.com/Torxgewinde/ESP32-Temperature
     * 
     */
    class BoardTemp
    {
        private :        
        
        public:

        BoardTemp()
        {

        }

        float ToFahrenheit(float c)
        {
            return  (c * 9/5) + 32;
        } 

        void Setup()
        {
            Serial.begin(115200);
            Serial.print("===================================================\r\n");
            
            //configure RTC slow clock to internal oscillator, fast clock to XTAL divided by 4
            rtc_clk_slow_freq_set(RTC_SLOW_FREQ_RTC);
            rtc_clk_fast_freq_set(RTC_FAST_FREQ_XTALD4);
            
            //read CPU speed
            rtc_cpu_freq_config_t freq_config;
            rtc_clk_cpu_freq_get_config(&freq_config);
            //should be "0 -- 150000 -- 240", internal oscillator running at ~150kHz and CPU at 240 MHz
            printf("%d -- %d -- %d\r\n", (int)rtc_clk_slow_freq_get(), rtc_clk_slow_freq_get_hz(), freq_config.freq_mhz);

        }
      
        float ReadTemp1(bool printRaw = false) 
        {
            uint64_t value = 0;
            int rounds = 100;

            for(int i=1; i<=rounds; i++) {
                value += rtc_clk_cal_ratio(RTC_CAL_RTC_MUX, 100);
                yield();
            }
            value /= (uint64_t)rounds;

            if(printRaw) {
                printf("%s: raw value is: %llu\r\n", __FUNCTION__, value);
            }

            return ((float)value - M1_CALPOINT1_RAW) * (M1_CALPOINT2_CELSIUS - M1_CALPOINT1_CELSIUS) / (M1_CALPOINT2_RAW - M1_CALPOINT1_RAW) + M1_CALPOINT1_CELSIUS;
        }

        float ReadTemp2() {
            uint64_t value = rtc_time_get();
            delay(100);
            value = (rtc_time_get() - value);

            return ((float)value*10.0 - M2_CALPOINT1_RAW) * (M2_CALPOINT2_CELSIUS - M2_CALPOINT1_CELSIUS) / (M2_CALPOINT2_RAW - M2_CALPOINT1_RAW) + M2_CALPOINT1_CELSIUS;
        }
    };
} // namespace IO
