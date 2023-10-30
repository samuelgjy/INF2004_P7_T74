#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Define Infrared Sensor
#define GPIO_PIN_INFRARED 27

// Define constraints for sensor thresholds & barcode patterns
#define THRESHOLD_BLACK_BAR 1900
#define THRESHOLD_WHITE_BAR 1600
#define NUM_OF_BARS_IN_CHAR 9
#define NUM_OF_WHITE_BARS 4
#define NUM_OF_BLACK_BARS 5
#define TIMER_THRESHOLD 200000 // MICROSECONDS
#define DEBOUNCE_TIME_MS 50 // MICROSECONDS

typedef struct {
    uint32_t timeDiff;
    uint32_t edgeTimestamp;
    uint32_t previousEdgeTimestamp;
    bool scanningCharacter;
    bool delimiterDetected;
    bool scannerActive;
    bool firstBlackBarDetected;
    int barCounts;
    int currentWhiteIndex;
    int currentBlackIndex;
    char blackBar[NUM_OF_BLACK_BARS];
    char whiteBar[NUM_OF_WHITE_BARS];
    bool saveBlack;
} BarcodeScanner;

BarcodeScanner scanner;

void saveBlackBars(uint32_t timeDiff, int *currentIndex, char *blackbars, BarcodeScanner *scanner)
{
    printf("Saving Black Bars\n");
    if (*currentIndex <= NUM_OF_BLACK_BARS)
    {
        if (timeDiff < TIMER_THRESHOLD)
        {
            printf("Narrow Black Bar Detected!\n");
            //printf("Thin Black Bar Detected, Time diff: %d\n", timeDiff);
            blackbars[*currentIndex] = 'N';
        }
        else
        {
            printf("Wide Black Bar Detected!\n");
            //printf("Thick Black Bar Detected , Time diff: %d\n", timeDiff);
            blackbars[*currentIndex] = 'W';
        }

        (*currentIndex)++;
    }
}

void saveWhiteBars(uint32_t timeDiff, int *currentIndex, char *whitebars, BarcodeScanner *scanner)
{
    // printf("Saving White Bars\n");
    if (*currentIndex <= NUM_OF_WHITE_BARS)
    {
        if (timeDiff < TIMER_THRESHOLD)
        {
            printf("Narrow White Bar Detected!\n");
            //printf("Thin White Bar Detected , Time diff: %d\n", timeDiff);
            whitebars[*currentIndex] = 'N';
        }
        else
        {   
            printf("Wide White Bar Detected!\n");
            //printf("Thick White Bar Detected , Time diff: %d\n", timeDiff);
            whitebars[*currentIndex] = 'W';
        }

        (*currentIndex)++;
    }
}

char decodeCode39(char *blackBars, char *whiteBars, BarcodeScanner *scanner)
{
    // Define the Code 39 character set
    const char code39_set[47] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ-. $/+%*";

    // Define the corresponding Code 39 patterns
    const char *white_bar_patterns[] = {"NWNN", "NNWN", "NNNW", "WNNN"};
    const char *black_bar_patterns[] = {"WNNNW", "NWNNW", "WWNNN", "NNWNW", "WNWNN", "NWWNN", "NNNWW", "WNNWN", "NWNWN", "NNWWN"};

    printf("Black Bars: %s\n", blackBars);
    printf("White Bars: %s\n", whiteBars);

    int even_bits = 0;
    int odd_bits = 0;

    for (int i = 0; i < 4; i++)
    {
        if (strcmp(whiteBars, white_bar_patterns[i]) == 0)
        {
            even_bits = i * 10;
            break;
        }
    }

    for (int i = 0; i < 10; i++)
    {
        if (strcmp(blackBars, black_bar_patterns[i]) == 0)
        {
            odd_bits = i + 1;
            break;
        }
    }

    int index = even_bits + odd_bits;

    return code39_set[index];
}

void init_IR_sensor()
{
    // Initialize ADC
    adc_init();
    adc_gpio_init(GPIO_PIN_INFRARED);
    adc_select_input(0);
}

int read_IR_sensor()
{
    // Read ADC value
    uint16_t adc_value = adc_read();
    //printf("ADC Value: %d\n", adc_value);
    return adc_value;
}

void handleTimestamp(BarcodeScanner *scanner)
{
    scanner->previousEdgeTimestamp = scanner->edgeTimestamp;
    scanner->edgeTimestamp = time_us_32();
}

void handleDebounce(BarcodeScanner *scanner)
{
    if (scanner->edgeTimestamp - scanner->previousEdgeTimestamp < DEBOUNCE_TIME_MS * 100)
        return;
}

void handleSensorValue(int currentValue, BarcodeScanner *scanner)
{
    if (!scanner->firstBlackBarDetected && currentValue > THRESHOLD_BLACK_BAR)
    {
        scanner->firstBlackBarDetected = true;
        scanner->scannerActive = true;
    }

    if (scanner->scanningCharacter)
    {
        if (currentValue > THRESHOLD_BLACK_BAR)
        {
            scanner->timeDiff = time_us_32() - scanner->previousEdgeTimestamp;
            if (scanner->saveBlack)
                saveBlackBars(scanner->timeDiff, &scanner->currentBlackIndex, scanner->blackBar, scanner);
            else
                saveWhiteBars(scanner->timeDiff, &scanner->currentWhiteIndex, scanner->whiteBar, scanner);

            scanner->barCounts++;
            scanner->saveBlack = !scanner->saveBlack;
        }
    }
    else if (scanner->scannerActive && currentValue > THRESHOLD_BLACK_BAR)
    {
        scanner->timeDiff = time_us_32() - scanner->previousEdgeTimestamp;
        if (scanner->saveBlack)
            saveBlackBars(scanner->timeDiff, &scanner->currentBlackIndex, scanner->blackBar, scanner);
        else
            saveWhiteBars(scanner->timeDiff, &scanner->currentWhiteIndex, scanner->whiteBar, scanner);

        scanner->barCounts++;
        scanner->saveBlack = !scanner->saveBlack;
    }
    else if (!scanner->scannerActive && currentValue > THRESHOLD_BLACK_BAR)
    {
        scanner->scannerActive = true;
    }
}

void handleScanningCharacter(BarcodeScanner *scanner)
{
    if (scanner->scanningCharacter && scanner->barCounts >= NUM_OF_BARS_IN_CHAR)
    {
        char character = decodeCode39(scanner->blackBar, scanner->whiteBar, scanner);
        printf("Decoded Character: %c\n", character);
        scanner->barCounts = 0;
        scanner->currentBlackIndex = 0;
        scanner->currentWhiteIndex = 0;
        scanner->firstBlackBarDetected = false;
        memset(scanner->blackBar, 0, NUM_OF_BLACK_BARS);
        memset(scanner->whiteBar, 0, NUM_OF_WHITE_BARS);
    }
    else if (!scanner->scanningCharacter && scanner->barCounts >= NUM_OF_BARS_IN_CHAR)
    {
        char delimiter = decodeCode39(scanner->blackBar, scanner->whiteBar, scanner);
        printf("Decoded Delimiter: %c\n", delimiter);

        if (delimiter == '*')
        {
            printf("Start delimiter detected\n");
            scanner->delimiterDetected = true;
            scanner->scanningCharacter = true;
        }

        scanner->barCounts = 0;
        scanner->currentBlackIndex = 0;
        scanner->currentWhiteIndex = 0;
        scanner->firstBlackBarDetected = false;
        memset(scanner->blackBar, 0, NUM_OF_BLACK_BARS);
        memset(scanner->whiteBar, 0, NUM_OF_WHITE_BARS);
    }
}

void edge_interrupt(uint gpio, uint32_t events)
{
    handleTimestamp(&scanner);

    handleDebounce(&scanner);

    int currentValue = read_IR_sensor();

    //printf("Current Value: %d\n", currentValue);

    handleSensorValue(currentValue, &scanner);

    handleScanningCharacter(&scanner);
}

int main()
{
    stdio_init_all();
    // Initialize IR sensor
    init_IR_sensor();

    // Initialise GPIO for edge detection
    gpio_init(GPIO_PIN_INFRARED);
    gpio_set_dir(GPIO_PIN_INFRARED, GPIO_IN);
    gpio_set_irq_enabled_with_callback(GPIO_PIN_INFRARED, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &edge_interrupt);

    while (true)
    {
        tight_loop_contents();
    }
}

