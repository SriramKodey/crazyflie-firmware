#include "config.h"
#include "debug.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "static_mem.h"
#include "task.h"

#include "deck_spi.h"
#include "deck.h"
#include "stm32f4xx_spi.h"
#include "deck_constants.h"

#include "disc_spi.h"

static xQueueHandle inputQueue;
STATIC_MEM_QUEUE_ALLOC(inputQueue, 1, sizeof(int));

static void discSpiTask(void *);
STATIC_MEM_TASK_ALLOC(discSpiTask, DISC_SPI_TASK_STACKSIZE);

static deckPin_t cs_Pin;

static uint16_t spiSpeed = SPI_BAUDRATE_21MHZ;
static uint8_t spiTxBuffer[100];
static uint8_t spiRxBuffer[100];
static bool isInit = false;

static discPacket_t writePacket;

void discSpiTaskInit() {
    inputQueue = STATIC_MEM_QUEUE_CREATE(inputQueue);

    writePacket.amplitude = 2501.5678f;
    writePacket.delta_amplitude = 20.500f;
    writePacket.offset = 50.900f;
    
    /* Setup SPI */
    // Init Pins
    cs_Pin = DECK_GPIO_IO3;
    pinMode(cs_Pin, OUTPUT);
    digitalWrite(cs_Pin, HIGH);
    spiBegin();

    STATIC_MEM_TASK_CREATE(discSpiTask, discSpiTask, DISC_SPI_TASK_NAME, NULL, DISC_SPI_TASK_PRI);
    isInit = true;
}

bool discSpiTaskTest() {
    return isInit;
}

static void discSpiTask(void * parameters) {
    DEBUG_PRINT("DISC_SPI_TASK main function is running");
    while(true) {
        int input;
        if (pdTRUE == xQueueReceive(inputQueue, &input, 0)) {
            // set current data packet values
        }
        vTaskDelay(2); // Set speed, Talk to Daksh
        spiBeginTransaction(spiSpeed);
        memcpy(spiTxBuffer, &writePacket, 12);
        digitalWrite(cs_Pin, LOW);
        spiExchange(sizeof(discPacket_t), spiTxBuffer, spiRxBuffer);
        digitalWrite(cs_Pin, HIGH);
        spiEndTransaction();
    }
}

void discSpiTaskEnqueueInput(int value) {
    xQueueOverwrite(inputQueue, &value);
}