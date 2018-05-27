#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h> 
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>

static char command[256];
static char response[256];
int have_command = 0;

/*Khai báo số type cho số định danh lệnh dùng trong ioctl() */ 
#define HELLOWORLD_DEV_MAGIC 'B' /*Số type là số 66 trong mã ascii*/ 
/*Số định danh lệnh cho hàm add*/ 
#define HELLOWORLD_ADD _IOWR(HELLOWORLD_DEV_MAGIC, 10,unsigned long) 
/*Số định danh lệnh cho hàm sub*/ 
#define HELLOWORLD_SUB _IOWR(HELLOWORLD_DEV_MAGIC, 11,unsigned long) 
/*Số định danh lệnh cho hàm mul*/ 
#define HELLOWORLD_MUL _IOWR(HELLOWORLD_DEV_MAGIC, 12,unsigned long) 
/*Số định danh lệnh cho hàm div*/ 
#define HELLOWORLD_DIV _IOWR(HELLOWORLD_DEV_MAGIC, 13,unsigned long) 

/*Cấu trúc ioctl() thực hiện các phép toán add, sub, mul và div*/ 
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long lArg) 
{  /*Biến lưu mã lỗi trảvềcủa ioctl()*/
	int retval; 
	
	unsigned long *arg = (unsigned long *)lArg; 
	
	/*Phân biệt các trường hợp lệnh khác nhau*/ 
	switch (cmd) 
	{ 
		case HELLOWORLD_ADD: 
		/*Thông báo quá trình tính toán bắt đầu*/ 
		  printk ("Driver: Start ...\n"); 
		/*Thực hiện tính toán, chép qua user qua biến arg[2]*/ 
		  arg[2] = arg[0] + arg [1]; 
		/*In thông báo tính toán kết thúc*/ 
		  printk ("Driver: Calculating is complete, Result = %ld \n", arg[2]); 
		break; 
		case HELLOWORLD_SUB: 
		/*Thông báo quá trình tính toán bắt đầu*/ 
		  printk ("Driver: Start ...\n"); 
		/*Thực hiện tính toán, chép qua user qua biến arg[2]*/
		  arg[2] = arg[0] - arg [1]; 
		/*In thông báo tính toán kết thúc*/ 
		  printk ("Driver: Calculating is complete, Result = %ld \n", arg[2]);
		break; 
		case HELLOWORLD_MUL: 
		/*Thông báo quá trình tính toán bắt đầu*/ 
		  printk ("Driver: Start ...\n"); 
		/*Thực hiện tính toán, chép qua user qua biến arg[2]*/ 
		  arg[2] = arg[0] * arg [1]; 
		/*In thông báo tính toán kết thúc*/ 
		  printk ("Driver: Calculating is complete, Result = %ld \n", arg[2]); 
		break; 
		case HELLOWORLD_DIV: 
		/*Thông báo quá trình tính toán bắt đầu*/ 
		  printk ("Driver: Start ...\n"); 
		/*Thực hiện tính toán, chép qua user qua biến arg[2]*/ 
		  arg[2] = arg[0] / arg [1]; 
		  /*In thông báo tính toán kết thúc*/ 
		  printk ("Driver: Calculating is complete, Result = %ld \n", arg[2]); 
		break; 
		default: 
		/*Thông báo lỗi cho người sử dụng, không có lệnh hỗ trợ*/ 
		  printk ("Driver: I don't have this operation!\n"); 
		retval = -EINVAL; 
		break; 
		} 
	/*Trả về mã lỗi cho hàm ioctl() */ 
	return retval; 
} 

static ssize_t my_read( struct file *filep, char *buffer, size_t count, loff_t *offp ) 
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
static ssize_t my_write( struct file *filep, const char *buffer, size_t count, loff_t *offp )
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
            break;
        case 'c':
            // TODO: clear led
            strcpy( response, "OK\n" );
            break;
        default:
            strcpy( response, "ERROR: unknown command\n" ); 
    }
    
    have_command = 1;
    
    return count;
}
static struct file_operations my_fops =
{
	.unlocked_ioctl = my_ioctl,
    .read = my_read,
    .write = my_write,
};


static dev_t my_device; // driver major number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static int __init my_init(void)
{
    printk(KERN_INFO "my module: init\n");
    
    /* request device major number */
    if (alloc_chrdev_region(&my_device, 0, 1, "calc") < 0) {
        printk(KERN_DEBUG "Error registering device!\n");
        return -1;
    }

    if ((cl = class_create(THIS_MODULE, "chardrv")) == NULL)
    {
        unregister_chrdev_region(my_device, 1);
        return -1;
    }

    if (device_create(cl, NULL, my_device, NULL, "my_module") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(my_device, 1);
        return -1;
    }

    cdev_init(&c_dev, &my_fops);

    if (cdev_add(&c_dev, my_device, 1) == -1)
    {
        device_destroy(cl, my_device);
        class_destroy(cl);
        unregister_chrdev_region(my_device, 1);
        return -1;
    }

    return 0;
}

static void __exit my_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, my_device);
    class_destroy(cl);
    
    /* release major number */
    unregister_chrdev_region(my_device, 0);
    printk(KERN_INFO "my module: exit\n" );
}

module_init(my_init);
module_exit(my_exit);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("F.B.");
MODULE_DESCRIPTION("my char device driver");
