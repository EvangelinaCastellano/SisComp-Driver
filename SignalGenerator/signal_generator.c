/*
    Este driver generará dos señales cuadradas:
    1) señal de 6 s por el pin 18.
    2) señal de 5 s por el pin 23.
*/

//TO DO: CAMBIAR NOMBRES DE LAS VARIABLES QUE DICEN LA FRECUENCIA!!
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define LED_PIN_6s 18
#define LED_PIN_5s 23
#define INTERVAL_6_MS 6000
#define INTERVAL_5_MS 5000

static struct timer_list led_timer_6s;
static struct timer_list led_timer_5s;

static void toggle_led_6s(struct timer_list *timer);
static void toggle_led_5s(struct timer_list *timer);

static int __init led_driver_init(void){

    // Se solicita el control de los GPIO necesarios:
    if(gpio_request(LED_PIN_6s, "led_6s") < 0){
        printk(KERN_INFO "ERROR: gpio_request function.\n");
        return -1;
    }
    if(gpio_request(LED_PIN_5s, "led_5s") < 0){
        printk(KERN_INFO "ERROR: gpio_request function.\n");
        return -1;
    }

    // Se configuran los pines como salida:
    if(gpio_direction_output(LED_PIN_6s, 0) < 0){
        printk(KERN_INFO "ERROR: gpio_direction_output function. \n");
        gpio_free(LED_PIN_6s); // Si ocurre un fallo se libera el pin solicitado.
        return -1;
    }
    if(gpio_direction_output(LED_PIN_5s, 0) < 0){
        printk(KERN_INFO "ERROR: gpio_direction_output function. \n");
        gpio_free(LED_PIN_5s); // Si ocurre un fallo se libera el pin solicitado.
        return -1;
    }

    // Se configuran los timers:
    timer_setup(&led_timer_6s, toggle_led_6s, 0);
    mod_timer(&led_timer_6s, jiffies + msecs_to_jiffies(INTERVAL_6_MS));

    timer_setup(&led_timer_5s, toggle_led_5s, 0);
    mod_timer(&led_timer_5s, jiffies + msecs_to_jiffies(INTERVAL_5_MS));

    printk(KERN_INFO "GPIO's configurados exitosamente.\n");
    return 0;
}

static void toggle_led_6s(struct timer_list *timer){
 
    gpio_set_value(LED_PIN_6s, !gpio_get_value(LED_PIN_6s)); // Se cambia el valor del pin.
    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_6_MS)); // Se reinicia el timer.
}

static void toggle_led_5s(struct timer_list *timer){

    gpio_set_value(LED_PIN_5s, !gpio_get_value(LED_PIN_5s)); // Se cambia el valor del pin.
    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_5_MS)); // Se reinicia el timer.
}

static void __exit led_driver_exit(void){

    del_timer(&led_timer_6s);
    gpio_set_value(LED_PIN_6s, 0);
    gpio_free(LED_PIN_6s);

    del_timer(&led_timer_5s);
    gpio_set_value(LED_PIN_5s, 0);
    gpio_free(LED_PIN_5s);
    printk(KERN_INFO "GPIO's liberados exitosamente\nDriver descargado.\n");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CAPA 8");
MODULE_DESCRIPTION("TP5 - Sistemas de Computación");