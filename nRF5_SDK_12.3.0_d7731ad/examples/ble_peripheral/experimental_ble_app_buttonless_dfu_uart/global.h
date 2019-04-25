
#define TIMER1_CIRCLE_MS 100

#define MS_TO_CNT(NEED_MS, CIRCLE_MS)  (((NEED_MS) / (CIRCLE_MS)) + ((((NEED_MS) % (CIRCLE_MS)) != 0) ? 1 : 0))

#define LED_PIN_NUMBER              21

