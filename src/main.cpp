#include <M5Unified.h>

// M5Stack Core2 PortB : 32
// M5Stack Basic PortB : 26
#define FLASH_EN_PIN 32 // 26

const int MAX_FLASH_MODE = 16;
uint8_t mode_flash = 0;

// timer interrupt
hw_timer_t* timer = NULL;
volatile uint32_t counter = 0;
volatile uint32_t current_count = 0;

void update_flash_timeout(uint64_t alarm_value)
{
    timerAlarmDisable(timer);
    timerAlarmWrite(timer, alarm_value, true);
    timerAlarmEnable(timer);
}

const String flash_config_message[MAX_FLASH_MODE+1] = {
    "0:OFF",
    " 1:100%, 220ms",
    " 2: 90%, 220ms",
    " 3: 80%, 220ms",
    " 4: 70%, 220ms",
    " 5: 60%, 220ms",
    " 6: 50%, 220ms",
    " 7: 40%, 220ms",
    " 8: 30%, 220ms",
    " 9:100%,  1.3s",
    "10: 90%,  1.3s",
    "11: 80%,  1.3s",
    "12: 70%,  1.3s",
    "13: 60%,  1.3s",
    "14: 50%,  1.3s",
    "15: 40%,  1.3s",
    "16: 30%,  1.3s"
};

void unit_flash_init(void)
{
    pinMode(FLASH_EN_PIN, OUTPUT);
    digitalWrite(FLASH_EN_PIN, LOW);
}

void unit_flash_set_brightness(uint8_t mode)
{
    const uint8_t interval_ms = 4;

    // timer
    uint64_t alarm_value = 0;
    if((mode > 0) && (mode <= MAX_FLASH_MODE))
    {
        alarm_value = 220000;
    } else {
        alarm_value = 1300000;
    }
    update_flash_timeout(alarm_value);

    // flash
    if ((mode >= 1) && (mode <= MAX_FLASH_MODE))
    {
        for (int i = 0; i < mode; i++)
        {
            digitalWrite(FLASH_EN_PIN, LOW);
            delayMicroseconds(interval_ms);
            digitalWrite(FLASH_EN_PIN, HIGH);
            delayMicroseconds(interval_ms);
        }
    }
    else
    {
        digitalWrite(FLASH_EN_PIN, LOW);
    }

}

void update_flash_message(int mode)
{
    M5.Lcd.fillScreen(BLACK);
    M5.Display.setTextColor(WHITE);
    M5.Display.drawString(flash_config_message[mode_flash], 150, 100);
}

// timer
void OnTimer()
{
    counter++;
    current_count = millis();

    unit_flash_set_brightness(0);   // OFF
}

void setup()
{
    auto cfg = M5.config();
    M5.begin(cfg);

    // display
    M5.Lcd.fillScreen(BLACK);
    M5.Display.setTextDatum(MC_DATUM);
    M5.Display.setColorDepth(8);
    M5.Display.setFont(&fonts::efontCN_14);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(ORANGE);
    M5.Display.drawString("Flash Light!!", 160, 10);

    // timer interrupt
    timer = timerBegin(0, 80, true);    // divider:80
    timerAttachInterrupt(timer, &OnTimer, true);
    timerAlarmWrite(timer, 220000, true);   // 220ms
    timerAlarmDisable(timer);

    // flash
    mode_flash = 0;
    unit_flash_init();
}

void loop()
{
    M5.update();

    if (M5.BtnA.wasPressed())
    {
        // M5.Lcd.print('A');
        // M5.Lcd.print(mode_flash);
        mode_flash++;
        if (mode_flash > 16)
        {
            mode_flash = 1;
        }
        update_flash_message(mode_flash);
    }
    else if (M5.BtnB.wasReleased())
    {
        // M5.Lcd.print('B');
        unit_flash_set_brightness(mode_flash);
    }
    else if (M5.BtnC.wasPressed())
    {
        // M5.Lcd.print('C');
        unit_flash_set_brightness(0);
        timerAlarmDisable(timer);
        // mode_flash = 0;
        // update_flash_message(mode_flash);
        // unit_flash_set_brightness(mode_flash);
    }

    // unit_flash_set_brightness(mode_flash);
}
