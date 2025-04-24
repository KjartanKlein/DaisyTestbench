#include "daisy_seed.h"
#include "daisysp.h"
#include "dev/oled_ssd130x.h"
using namespace daisy;
using namespace daisysp;

using myOled = OledDisplay<SSD130xI2c128x64Driver>;
DaisySeed hw;
myOled display;
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}
}

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);
	
    
	GPIO my_button;
	my_button.Init(seed::D28, GPIO::Mode::INPUT, GPIO::Pull::PULLUP);
	
	GPIO my_led;
	my_led.Init(seed::D27, GPIO::Mode::OUTPUT);
	bool led_state = false;

	Led pwm_led;
	pwm_led.Init(seed::D15,false);
	pwm_led.SetSampleRate(1000.0);
	const int adc_channels =2;
	AdcChannelConfig adc_config[adc_channels];
	adc_config[0].InitSingle(seed::A6);
	adc_config[1].InitSingle(seed::A5);
	hw.adc.Init(adc_config,adc_channels);
	
	// Starts
	hw.adc.Start();
	hw.StartLog();
	int i = 0;
	myOled::Config disp_cfg;
	if (true){
			disp_cfg.driver_config.transport_config.i2c_config.pin_config.scl =seed::D11;
			disp_cfg.driver_config.transport_config.i2c_config.pin_config.sda =seed::D12;
			//disp_cfg.driver_config.transport_config.i2c_address= 0x7A;
			
	}
	display.Init(disp_cfg);
	
    char strbuff[128];

	while(1) {
		bool button_state = my_button.Read();
		my_led.Write(!button_state);
		led_state = !led_state;
		int adc_value1 = hw.adc.Get(0);
		int adc_value2 = hw.adc.Get(1);
		if (i>=1000){
			hw.PrintLine("1000 loops past of 1ms delay");
			hw.PrintLine("Analog reading : %d", adc_value1);
			hw.PrintLine("Analog reading : %d", adc_value2);
			i=0;

			display.Fill(false);
			display.SetCursor(0,0);
			sprintf(strbuff,"ADC readout");
			display.WriteString(strbuff,Font_11x18,true);
			display.SetCursor(0,18);
			sprintf(strbuff,"ADC1 %d", adc_value1);
			display.WriteString(strbuff,Font_11x18,true);
			display.SetCursor(0,18*2);
			sprintf(strbuff,"ADC2 %d", adc_value2);
			display.WriteString(strbuff,Font_11x18,true);		
			display.Update();

		}else{
			i++;
		}
		hw.SetLed(button_state);
		pwm_led.Set((float)adc_value1/65535.0f);
		pwm_led.Update();
		System::Delay(1);	
	}
}
