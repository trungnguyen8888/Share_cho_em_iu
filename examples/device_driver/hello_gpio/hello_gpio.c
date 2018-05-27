#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h> 
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/gpio.h>
#include <linux/cdev.h>

const int gpio_led = 102;
int my_value = 0;

static char command[256];
static char response[256];
int have_command = 0;

int my_read( struct file *filep, char *buffer, size_t count, loff_t *offp ) 
{
    int ret;

    if( 0==have_command )
        return 0; // we already have sent a response and wait for a "write command" operation

    if( count > strlen(response) )
        count = strlen(response);

    ret = copy_to_user( buffer, response, strlen(response) );
    if( ret != 0 )
        return -EINVAL;

    have_command = 0;
    return count;
}

int my_write( struct file *filep, const char *buffer, size_t count, loff_t *offp )
{
    int ret;

    if( count > sizeof(command)-1 )
        count = sizeof(command)-1;

    ret = copy_from_user( command, buffer, count );
    if( ret != 0 )
        return -EINVAL;
    
    command[count] = '\0';

    // parse command here and execute it
    switch( command[0] )
    {
        case 's': 
            // TODO: set led
            strcpy( response, "OKd\n" );
            gpio_set_value( gpio_led, 1 );
            break;
        case 'c': 
            // TODO: clear led
            strcpy( response, "OK\n" );
            gpio_set_value( gpio_led, 0 );
            break;
        default: 
            strcpy( response, "ERROR: unknown command\n" ); 
    }

    have_command = 1;
    return count;
} 

static struct file_operations my_fops =
{
    .read = my_read,
    .write = my_write,
};

/* leds driver major number */
static dev_t leds_dev_number;
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static int __init my_init(void)
{
    int ret;
    int rc;
        
    printk(KERN_INFO "my module: init\n");
    
    /* request device major number */
    if ((ret = alloc_chrdev_region(&leds_dev_number, 0, 1, "test") < 0)) {
        printk(KERN_DEBUG "Error registering device!\n");
        return ret;
    }
    
    if ((cl = class_create(THIS_MODULE, "chardrv_class")) == NULL)
    {
        unregister_chrdev_region(leds_dev_number, 1);
        return -1;
    }

    if (device_create(cl, NULL, leds_dev_number, NULL, "gpio_module") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(leds_dev_number, 1);
        return -1;
    }

    cdev_init(&c_dev, &my_fops);
    if (cdev_add(&c_dev, leds_dev_number, 1) == -1)
    {
        device_destroy(cl, leds_dev_number);
        class_destroy(cl);
        unregister_chrdev_region(leds_dev_number, 1);
        return -1;
    }
	
    rc = gpio_request( gpio_led, "my_gpio_label" );
    if(rc < 0)
    {
        printk(KERN_ERR "gpio_request failed, error %d\n", rc );
        return -1;
    }

    rc = gpio_direction_output( gpio_led, 0 );
    if(rc < 0)
    {
        printk(KERN_ERR "gpio_direction_output failed, error %d\n", rc );
        return -1;
    }

    return 0;
}

static void __exit my_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, leds_dev_number);
    class_destroy(cl);
    
    /* release major number */
    unregister_chrdev_region(leds_dev_number, 0);
    printk(KERN_INFO "my module: exit\n" );
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("F.B.");
MODULE_DESCRIPTION("my char device driver");
