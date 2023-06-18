/*
    Este driver sensa dos señales por los pines 17 y 22.
    La señal a sensar la determina la variable "signal", que debe ser indicada a través del archivo /proc/signal_file.
    El valor resultante del sensado se guarda en la variable "value", que se escribe también en el archivo /proc/signal_file.
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h>

int init_signal_module(void);
int init_sensed(void);
static void gpio_sensor(struct timer_list *timer);
void exit_signal_module(void);
static ssize_t signal_file_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);

#define BUFFER_LENGTH PAGE_SIZE
#define GPIO_PIN_17 17   // Pin para sensar señal de 1Hz
#define GPIO_PIN_22 22   // Pin para sensar señal de 2Hz
#define INTERVAL_MS 1000 // Tiempo de sensado

static struct timer_list timer_1hz;
static struct proc_dir_entry *proc_entry;
static char *signal_file;
static int signal = 0;

static const struct proc_ops proc_entry_fops = {
    //.proc_read = signal_file_read,
    .proc_write = signal_file_write,
};

int init_signal_module(void){
    
    int ret = 0;
    int err = 0;
    signal_file = (char *)vmalloc(BUFFER_LENGTH);

    if (!signal_file){
        ret = -ENOMEM;
    }else{
        memset(signal_file, 0, BUFFER_LENGTH);
        proc_entry = proc_create("signal_file", 0666, NULL, &proc_entry_fops);
        if (proc_entry == NULL){
            ret = -ENOMEM;
            vfree(signal_file);
            printk(KERN_INFO "ERROR: No se pudo crear la entrada en /proc.\n");
        }
        else{
            printk(KERN_INFO "Modulo cargado.\n");
        }
    }

    err = init_sensed();
    if (err < 0){
        remove_proc_entry("signal_file", NULL);
        vfree(signal_file);
        return err;
    }

    return ret;
}

int init_sensed(void){
    
    // Se solicita el control de los GPIO necesarios:
    if(gpio_request(GPIO_PIN_17, "pin_gpio_17") < 0){
        printk(KERN_INFO "ERROR: gpio_request function.\n");
        return -1;
    }
    if(gpio_request(GPIO_PIN_22, "pin_gpio_22") < 0){
        printk(KERN_INFO "ERROR: gpio_request function.\n");
        return -1;
    }
    
    // Se configuran los pines como salida:
    if(gpio_direction_input(GPIO_PIN_17) < 0){
        printk(KERN_INFO "ERROR: gpio_direction_input function.\n");
        gpio_free(GPIO_PIN_17);
        return -1;
    }
    if(gpio_direction_input(GPIO_PIN_22) < 0){
        printk(KERN_INFO "ERROR: gpio_direction_input function.\n");
        gpio_free(GPIO_PIN_22);
        return -1;
    }

    // Se configura el timer:
    timer_setup(&timer_1hz, gpio_sensor, 0);
    mod_timer(&timer_1hz, jiffies + msecs_to_jiffies(INTERVAL_MS));

    return 0;
}

static void gpio_sensor(struct timer_list *timer){
    
    int value;

    if(signal == 1){
        value = gpio_get_value(GPIO_PIN_17);
    }else{
        value = gpio_get_value(GPIO_PIN_22);
    }
    if(value == 1){
        strncpy(signal_file, "1", BUFFER_LENGTH - 1);
    }else{
        strncpy(signal_file, "0", BUFFER_LENGTH - 1);
    }

    signal_file[BUFFER_LENGTH - 1] = '\0';

    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_MS));
}

//Transfiere datos desde el espacio de usuario al espacio de kernel, echo "hola" > /proc/signal_file
static ssize_t signal_file_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
    
    int available_space = BUFFER_LENGTH - 1;

    if((*off) > 0) // The application can write in this entry just once
        return 0;

    if(len > available_space){
        printk(KERN_INFO "ERROR: No hay espacio disponible.\n");
        return -ENOSPC;
    }

    if(copy_from_user(&signal_file[0], buf, len))
        return -EFAULT;

    signal_file[len] = '\0';
    *off += len; // Update the file pointer

    signal = (int)signal_file[0] - 48; // Se resta 48 porque su valor esta en ASCII.

    return len;
}

void exit_signal_module(void){
    
    del_timer(&timer_1hz); //Se elimina el timer.
    //Se liberan los GPIO's:
    gpio_free(GPIO_PIN_17); 
    gpio_free(GPIO_PIN_22);

    remove_proc_entry("signal_file", NULL); // Se elimina el file.
    vfree(signal_file); // Se libera la memoria.
    printk(KERN_INFO "Modulo descargado.\n");
}

module_init(init_signal_module);
module_exit(exit_signal_module);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("CAPA 8");
MODULE_DESCRIPTION("TP5 - Sistemas de Computación");