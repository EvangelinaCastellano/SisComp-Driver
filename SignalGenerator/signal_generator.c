/*
    Este driver generará dos señales cuadradas:
    1) señal de 0.25 Hz por el pin 18.
    2) señal de 0.5 Hz por el pin 23.
*/

//TO DO: CAMBIAR NOMBRES DE LAS VARIABLES QUE DICEN LA FRECUENCIA!!
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define LED_PIN_1HZ 18
#define LED_PIN_2HZ 23
#define INTERVAL_MS_1HZ 4000
#define INTERVAL_MS_2HZ 2000

static struct timer_list led_timer_1hz;
static struct timer_list led_timer_2hz;

static void toggle_led_1hz(struct timer_list *timer);
static void toggle_led_2hz(struct timer_list *timer);

static int __init led_driver_init(void){

    // Se solicita el control de los GPIO necesarios:
    if(gpio_request(LED_PIN_1HZ, "led_1hz") < 0){
        printk(KERN_INFO "ERROR: gpio_request function.\n");
        return -1;
    }
    if(gpio_request(LED_PIN_2HZ, "led_2hz") < 0){
        printk(KERN_INFO "ERROR: gpio_request function.\n");
        return -1;
    }

    // Se configuran los pines como salida:
    if(gpio_direction_output(LED_PIN_1HZ, 0) < 0){
        printk(KERN_INFO "ERROR: gpio_direction_output function. \n");
        gpio_free(LED_PIN_1HZ); // Si ocurre un fallo se libera el pin solicitado.
        return -1;
    }
    if(gpio_direction_output(LED_PIN_2HZ, 0) < 0){
        printk(KERN_INFO "ERROR: gpio_direction_output function. \n");
        gpio_free(LED_PIN_2HZ); // Si ocurre un fallo se libera el pin solicitado.
        return -1;
    }

    // Se configuran los timers:
    timer_setup(&led_timer_1hz, toggle_led_1hz, 0);
    mod_timer(&led_timer_1hz, jiffies + msecs_to_jiffies(INTERVAL_MS_1HZ));

    timer_setup(&led_timer_2hz, toggle_led_2hz, 0);
    mod_timer(&led_timer_2hz, jiffies + msecs_to_jiffies(INTERVAL_MS_2HZ));

    printk(KERN_INFO "GPIO's configurados exitosamente.\n");
    return 0;
}

static void toggle_led_1hz(struct timer_list *timer){
 
    gpio_set_value(LED_PIN_1HZ, !gpio_get_value(LED_PIN_1HZ)); // Se cambia el valor del pin.
    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_MS_1HZ)); // Se reinicia el timer.
}

static void toggle_led_2hz(struct timer_list *timer){

    gpio_set_value(LED_PIN_2HZ, !gpio_get_value(LED_PIN_2HZ)); // Se cambia el valor del pin.
    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_MS_2HZ)); // Se reinicia el timer.
}

static void __exit led_driver_exit(void){

    del_timer(&led_timer_1hz);
    gpio_set_value(LED_PIN_1HZ, 0);
    gpio_free(LED_PIN_1HZ);

    del_timer(&led_timer_2hz);
    gpio_set_value(LED_PIN_2HZ, 0);
    gpio_free(LED_PIN_2HZ);
    printk(KERN_INFO "GPIO's liberados exitosamente\nDriver descargado.\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CAPA 8");
MODULE_DESCRIPTION("TP5 - Sistemas de Computación");