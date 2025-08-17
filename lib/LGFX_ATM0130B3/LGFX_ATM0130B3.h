#ifndef GMT130_H
#define GMT130_H

#include <LovyanGFX.hpp>

#define PIN_SCLK 4
#define PIN_MOSI 5
#define PIN_MISO -1
#define PIN_DC 7
#define PIN_CS 6
#define PIN_RST 10
#define PIN_BUSY -1
// #define PIN_LIGHT 35

class LGFX : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    // lgfx::Light_PWM _light_instance;

public:
    LGFX(void)
    {
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 3;
            cfg.freq_write = 40000000;
            cfg.freq_read = 16000000;

            cfg.pin_sclk = PIN_SCLK;
            cfg.pin_mosi = PIN_MOSI;
            cfg.pin_miso = PIN_MISO;
            cfg.pin_dc   = PIN_DC;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs   = PIN_CS;
            cfg.pin_rst  = PIN_RST;
            cfg.pin_busy = PIN_BUSY;

            cfg.panel_width = 240;
            cfg.panel_height = 240;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
      
            cfg.invert = true;
            
            _panel_instance.config(cfg);
        }
        // {
        //     auto cfg = _light_instance.config();

        //     cfg.pin_bl = PIN_LIGHT;
        //     cfg.freq = 1200;
        //     cfg.pwm_channel = 0;

        //     _light_instance.config(cfg);
        //     _panel_instance.setLight(&_light_instance);
        // }
        setPanel(&_panel_instance);
    }
};

#endif