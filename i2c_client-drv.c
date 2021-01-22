/* 

   I2C client Driver 
   written by @techogenius academy

 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

struct i2c_data {
	struct i2c_client *client;
	dev_t dev; // For major and minor number
	u8 *buf;
	u16 value;
	struct cdev cdev;
	struct class *class;
};

static ssize_t my_read(struct file *f, char *buf, size_t count, loff_t *off)
{
	struct i2c_data *dev = (struct i2c_data*)(f->private_data);
	struct i2c_adapter *adap = dev->client->adapter;
	struct i2c_msg msg;
	char *temp;
	int ret;

	temp = kmalloc(count, GFP_KERNEL);

	msg.addr = 0x68;
	msg.flags = 0;
	msg.flags |= I2C_M_RD;
	msg.len = count;
	msg.buf = temp;

	ret = i2c_transfer(adap, &msg, 1);
	if (ret >=0)
		ret = copy_to_user(buf, temp, count) ? -EFAULT : count;
	kfree(temp);
	return ret;
}

static ssize_t my_write(struct file *f, const char *buf, size_t count, loff_t *off)
{
	struct i2c_data *dev = (struct i2c_data*)(f->private_data);
	struct i2c_adapter *adap = dev->client->adapter;
	struct i2c_msg msg;
	char *temp;
	int ret;

	temp = memdup_user(buf, count);

	msg.addr = 0x68;
	msg.flags = 0;
	msg.len = count;
	msg.buf = temp;

	ret = i2c_transfer(adap,&msg,1);
	kfree(temp);
	return(ret == 1? count :ret);
}


static int my_open (struct inode *i, struct file *f)
{
	struct i2c_data *dev = container_of(i->i_cdev,struct i2c_data,cdev);
	if(dev == NULL)
	{
		printk(KERN_ALERT" There is no data...\n");
		return -1;
	}
	f->private_data = dev;

	return 0;
}

static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

struct file_operations fops = {
	.open		= my_open,
	.release 	= my_close,
	.read		= my_read,
	.write		= my_write,
};


/* Remove Function */

static int ds3231_remove (struct i2c_client *client)
{
	struct i2c_data *data;
	printk(KERN_INFO"Remove Function is invoked..\n");
	data = i2c_get_clientdata(client);
	cdev_del(&data->cdev);
	device_destroy(data->class, data->dev);
	class_destroy(data->class);
	unregister_chrdev_region(data->dev, 1);
	return 0;
}

/* Probe Function to invoke the I2c Driver */

static int ds3231_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct i2c_data *data;
	int result;

	printk(KERN_INFO " Probe Function is invoked..\n");
	data = devm_kzalloc(&client->dev, sizeof(struct i2c_data), GFP_KERNEL);
	data->value = 30;
	data->buf = devm_kzalloc(&client->dev, data->value,GFP_KERNEL);
	i2c_set_clientdata(client, data);
	result = alloc_chrdev_region(&data->dev,0,1,"i2c_drv");

	if(result < 0)
	{
		printk(KERN_ALERT"Unable to do Device Registration...\n");
		unregister_chrdev_region(data->dev, 1);
		return -1;
	}

	printk("Major Number = %d\n", MAJOR(data->dev));

	if ((data->class = class_create(THIS_MODULE,"i2cdriver")) == NULL)
	{
		printk(KERN_ALERT"Unable to create the device class...\n");
		unregister_chrdev_region(data->dev, 1);
		return -1;
	}
	if(device_create(data->class, NULL, data->dev, NULL, "i2c_drv%d",0) == NULL)
	{
		printk(KERN_ALERT"Unable to create the device...\n");
		class_destroy(data->class);
		unregister_chrdev_region(data->dev, 1);
		return -1;
	}

	cdev_init(&data->cdev, &fops);

	if(cdev_add(&data->cdev, data->dev, 1) == -1)
	{
		printk(KERN_ALERT" Unable to add the Device..\n");
		device_destroy(data->class, data->dev);
		class_destroy(data->class);
		unregister_chrdev_region(data->dev, 1);
		return -1;
	}
	return 0;
}


static const struct i2c_device_id i2c_ids[] = {
	{"ds3231",0},
	{"ds32",0},
	{ }
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static struct i2c_driver ds3231_I2C_drv = {
	.driver = {
		.name = "ds32",
		.owner = THIS_MODULE,
	},
	.probe = ds3231_probe,
	.remove = ds3231_remove,
	.id_table = i2c_ids,
};

/* Initialization Module */

static int __init i2c_client_drv_init(void)
{
	// Register with i2c-core 
	return i2c_add_driver(&ds3231_I2C_drv);
}

module_init(i2c_client_drv_init);
/* Exit module */

static void __exit i2c_client_drv_exit(void)
{
	i2c_del_driver(&ds3231_I2C_drv);
}

module_exit(i2c_client_drv_exit);

MODULE_DESCRIPTION("I2C Client Driver");
MODULE_AUTHOR("TechoGenius Academy");
MODULE_LICENSE("GPL");
