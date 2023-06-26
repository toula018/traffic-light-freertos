#include <FreeRTOS.h>
#include <task.h>
#include <stdlib.h>
#include <stdio.h>
#include <semphr.h>

SemaphoreHandle_t car_counter_mutex;


const char* getMax(int num1, const char* name1, int num2, const char* name2, int num3, const char* name3, int num4, const char* name4) {
    int max = num1;
    const char* maxName = name1;

    if (num2 > max) {
        max = num2;
        maxName = name2;
    }

    if (num3 > max) {
        max = num3;
        maxName = name3;
    }

    if (num4 > max) {
        max = num4;
        maxName = name4;
    }

    return maxName;
}
int getMaxNumCar(int num1, int num2, int num3,  int num4) {
    int max = num1;

    if (num2 > max) {
        max = num2;
    }

    if (num3 > max) {
        max = num3;

    if (num4 > max) {
        max = num4;
    }

    return max;
}
}

int getRandomNumber(int n){
      int car_num= rand() % n;
      return car_num;

}
QueueHandle_t queue;



void sensor(void *pvParameters)
{
    car_counter_mutex = xSemaphoreCreateMutex();
    while (1)
    {
       
        xSemaphoreTake(car_counter_mutex, portMAX_DELAY);
        int north = getRandomNumber(6);
        int south = getRandomNumber(6);
        int east = getRandomNumber(6);
        int west = getRandomNumber(6);
        printf("cars towards :\n");
        printf("North: %d            south: %d              east:  %d           west:  %d\n", north,south,east,west);

        
        vTaskDelay(pdMS_TO_TICKS(2000));
        xQueueSend(queue, &north, portMAX_DELAY);
        xQueueSend(queue, &south, portMAX_DELAY);
        xQueueSend(queue, &east, portMAX_DELAY);
        xQueueSend(queue, &west, portMAX_DELAY);
        xSemaphoreGive(car_counter_mutex);
        vTaskDelay(pdMS_TO_TICKS(5000));

    }
}

void controller(void *pvParameters)
{
    while (1)
    {   int northCars,southCars,eastCars,westCars;

        xSemaphoreTake(car_counter_mutex, portMAX_DELAY);
        xQueueReceive(queue, &northCars, portMAX_DELAY);
        xQueueReceive(queue, &southCars, portMAX_DELAY);
        xQueueReceive(queue, &eastCars, portMAX_DELAY);
        xQueueReceive(queue, &westCars, portMAX_DELAY);

        const char* N = "north_dir";
        const char* S = "south_dir";
        const char* E = "east_dir";
        const char* W = "west_dir";

        const char* maxCarsDirection= getMax(northCars, N, southCars, S, eastCars, E, westCars, W);
        int maxCars= getMaxNumCar(northCars,southCars,eastCars, westCars);

        xQueueSend(queue, &maxCarsDirection, portMAX_DELAY);
        xQueueSend(queue, &maxCars, portMAX_DELAY);
        xSemaphoreGive(car_counter_mutex);
        vTaskDelay(pdMS_TO_TICKS(5000));
        
    }
}
void actuator(void *pvParameters)
{
    while (1)
    {   const char* maxCarDir;
        int maxCar;
        xSemaphoreTake(car_counter_mutex, portMAX_DELAY);
        xQueueReceive(queue, &maxCarDir, portMAX_DELAY);
        xQueueReceive(queue, &maxCar, portMAX_DELAY);

        if (maxCar>5){
            int timer=maxCar*2000;
            if(maxCarDir=="north_dir" || maxCarDir=="south_dir" ){

            printf("North and south GREEN light is : ON  \n");
            printf("east and west RED light is : ON  \n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            printf("east and west GREEN light is : ON  \n");
            printf("North and south RED light is : ON  \n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            xSemaphoreGive(car_counter_mutex);


        }else{
            if(maxCarDir=="east_dir" || maxCarDir=="west_dir" )
            printf("east and west GREEN light is : ON  \n");
            printf("North and south RED light is : ON  \n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            printf("North and south GREEN light is : ON  \n");
            printf("east and west RED light is : ON  \n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            xSemaphoreGive(car_counter_mutex);

        }
        }else{

            printf("North and south GREEN light is : ON  \n");
            printf("east and west RED light is : ON  \n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            printf("east and west GREEN light is : ON  \n");
            printf("North and south RED light is : ON  \n");
            vTaskDelay(pdMS_TO_TICKS(2000));
            xSemaphoreGive(car_counter_mutex);

        }

        
    }
}


int main()
{
    queue = xQueueCreate(10, sizeof(int));
    xTaskCreate(sensor, "sensor", 1024, NULL, 3, NULL);
    xTaskCreate(controller, "controller", 1024, NULL, 2, NULL);
    xTaskCreate(actuator, "actuator", 1024, NULL, 1, NULL);

    
    vTaskStartScheduler();
    return 0;
}