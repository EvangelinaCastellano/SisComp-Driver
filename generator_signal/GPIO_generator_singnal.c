#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define LED_PIN_1HZ 18
#define LED_PIN_2HZ 23
#define INTERVAL_MS_1HZ 1000
#define INTERVAL_MS_2HZ 500

static struct timer_list led_timer_1hz;
static struct timer_list led_timer_2hz;

static void toggle_led_1hz(struct timer_list *timer)
{
    int value = gpio_get_value(LED_PIN_1HZ);
    gpio_set_value(LED_PIN_1HZ, !value);
    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_MS_1HZ));
}

static void toggle_led_2hz(struct timer_list *timer)
{
    int value = gpio_get_value(LED_PIN_2HZ);
    gpio_set_value(LED_PIN_2HZ, !value);
    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_MS_2HZ));
}

static int __init led_driver_init(void)
{
    int ret;

    ret = gpio_request(LED_PIN_1HZ, "led_1");
    if (ret < 0)
    {
        printk(KERN_ERR "Error requesting GPIO %d\n");
        return ret;
    }

    ret = gpio_request(LED_PIN_2HZ, "led_1");
    if (ret < 0)
    {
        printk(KERN_ERR "Error requesting GPIO %d\n");
        return ret;
    }

    ret = gpio_direction_output(LED_PIN_1HZ, 0);
    if (ret < 0)
    {
        printk(KERN_ERR "Error setting GPIO direction\n");
        gpio_free(LED_PIN_1HZ);
        return ret;
    }

    ret = gpio_direction_output(LED_PIN_2HZ, 0);
    if (ret < 0)
    {
        printk(KERN_ERR "Error setting GPIO direction\n");
        gpio_free(LED_PIN_2HZ);
        return ret;
    }

    timer_setup(&led_timer_1hz, toggle_led_1hz, 0);
    mod_timer(&led_timer_1hz, jiffies + msecs_to_jiffies(INTERVAL_MS_1HZ));

    timer_setup(&led_timer_2hz, toggle_led_2hz, 0);
    mod_timer(&led_timer_2hz, jiffies + msecs_to_jiffies(INTERVAL_MS_2HZ));

    printk(KERN_INFO "GPIO tomado exitosamente");
    return 0;
}

static void __exit led_driver_exit(void)
{
    del_timer(&led_timer_1hz);
    gpio_set_value(LED_PIN_1HZ, 0);
    gpio_free(LED_PIN_1HZ);

    del_timer(&led_timer_2hz);
    gpio_set_value(LED_PIN_2HZ, 0);
    gpio_free(LED_PIN_2HZ);
    printk(KERN_INFO "GPIO liberado exitosamente, eliminando driver");
}

module_init(led_driver_init);
module_exit(led_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CAPA8");
MODULE_DESCRIPTION("TP5 SdeComp");