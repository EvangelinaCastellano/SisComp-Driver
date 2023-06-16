#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/gpio.h>

#define BUFFER_LENGTH PAGE_SIZE
#define GPIO_PIN_17 17
#define GPIO_PIN_22 22
#define INTERVAL_MS 1000

static struct timer_list timer_1hz;
static struct proc_dir_entry *proc_entry;
static char *clipboard; // Space for the "clipboard"
static int signal = 0;

static void sensed_gpio(struct timer_list *timer)
{
    int value;

    if (signal == 1)
    {
        value = gpio_get_value(GPIO_PIN_17);
    }
    else
    {
        value = gpio_get_value(GPIO_PIN_22);
    }
    if (value == 1)
    {
        strncpy(clipboard, "1", BUFFER_LENGTH - 1);
    }

    else
    {
        strncpy(clipboard, "0", BUFFER_LENGTH - 1);
    }

    clipboard[BUFFER_LENGTH - 1] = '\0';

    mod_timer(timer, jiffies + msecs_to_jiffies(INTERVAL_MS));
}

static ssize_t clipboard_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
    int available_space = BUFFER_LENGTH - 1;

    if ((*off) > 0) /* The application can write in this entry just once !! */
        return 0;

    if (len > available_space)
    {
        printk(KERN_INFO "clipboard: not enough space!!\n");
        return -ENOSPC;
    }

    /* Transfiere data desde el espacio de usuario al espacio de kernel */
    /* echo un mensaje > /proc/clipboard                                */

    if (copy_from_user(&clipboard[0], buf, len))
        return -EFAULT;

    clipboard[len] = '\0'; /* Add the `\0' */
    *off += len;           /* Update the file pointer */

    signal = (int)clipboard[0] - 48;

    return len;
}

static ssize_t clipboard_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{

    int nr_bytes;

    if ((*off) > 0) /* Tell the application that there is nothing left to read */
        return 0;

    nr_bytes = strlen(clipboard);

    if (len < nr_bytes)
        return -ENOSPC;

    /* Transfiere data desde el espacio de kernel al espacio de usuario */
    /* cat /proc/clipboard                                              */

    if (copy_to_user(buf, clipboard, nr_bytes))
        return -EINVAL;

    (*off) += len; /* Update the file pointer */

    return nr_bytes;
}

static const struct proc_ops proc_entry_fops = {
    .proc_read = clipboard_read,
    .proc_write = clipboard_write,
};

int init_sensed(void)
{
    int ret;

    ret = gpio_request(GPIO_PIN_17, "pin_gpio_17");
    if (ret < 0)
    {
        printk(KERN_INFO "Error requesting GPIO %d\n", GPIO_PIN_17);
        return ret;
    }

    ret = gpio_request(GPIO_PIN_22, "pin_gpio_22");
    if (ret < 0)
    {
        printk(KERN_INFO "Error requesting GPIO %d\n", GPIO_PIN_22);
        return ret;
    }

    ret = gpio_direction_input(GPIO_PIN_17);
    if (ret < 0)
    {
        printk(KERN_ERR "Error setting GPIO direction\n");
        gpio_free(GPIO_PIN_17);
        return ret;
    }

    ret = gpio_direction_input(GPIO_PIN_22);
    if (ret < 0)
    {
        printk(KERN_ERR "Error setting GPIO direction\n");
        gpio_free(GPIO_PIN_22);
        return ret;
    }

    timer_setup(&timer_1hz, sensed_gpio, 0);
    mod_timer(&timer_1hz, jiffies + msecs_to_jiffies(INTERVAL_MS));

    return 0;
}

int init_clipboard_module(void)
{
    int ret = 0;
    int err = 0;
    clipboard = (char *)vmalloc(BUFFER_LENGTH);

    if (!clipboard)
    {
        ret = -ENOMEM;
    }
    else
    {
        memset(clipboard, 0, BUFFER_LENGTH);
        proc_entry = proc_create("clipboard", 0666, NULL, &proc_entry_fops);
        if (proc_entry == NULL)
        {
            ret = -ENOMEM;
            vfree(clipboard);
            printk(KERN_INFO "Clipboard: No puede crear entrada en /proc..!!\n");
        }
        else
        {
            printk(KERN_INFO "Clipboard: Modulo cargado..!!\n");
        }
    }

    err = init_sensed();
    if (err < 0)
    {
        remove_proc_entry("clipboard", NULL);
        vfree(clipboard);
        return err;
    }

    return ret;
}

void exit_clipboard_module(void)
{
    del_timer(&timer_1hz);
    gpio_free(GPIO_PIN_17);
    gpio_free(GPIO_PIN_22);

    remove_proc_entry("clipboard", NULL);
    vfree(clipboard);
    printk(KERN_INFO "Clipboard: Modulo descargado..!!\n");
}

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Clipboard Kernel Module - FDI-UCM");
MODULE_AUTHOR("Juan Carlos Saez");

module_init(init_clipboard_module);
module_exit(exit_clipboard_module);
