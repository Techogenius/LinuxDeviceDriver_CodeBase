/*

Linux Usb Device Driver

*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>

#define USB_VENDOR_ID 0xaaaa
#define USB_PRODUCT_ID 0x8816

static struct usb_class_driver usb_cd;

static int usb_drv_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	struct usb_host_interface *intface_desc;
	struct usb_endpoint_descriptor * endpoint;
	int ret;
	
	intface_desc = interface->cur_altsetting;

	printk(KERN_INFO " USB info %d now probed: %04X ,  %04X\n",intface_desc->desc.bInterfaceNumber, id->idVendor, id->idProduct);
	printk(KERN_INFO " ID->bNumEndpoints : %02X\n", intface_desc->desc.bNumEndpoints);
	printk(KERN_INFO " ID-> bInterfaceClass: %02X\n",intface_desc->desc.bInterfaceClass);
	

	usb_cd.name = "usb%d";
	ret = usb_register_dev(interface,&usb_cd);
	if(ret)
	{
		printk(KERN_INFO " Not able to get the minor number..\n");
		return ret;	
	}
	else
	{	
		printk(KERN_INFO " Minor number = %d\n",interface->minor);
	}
	
}

static void usb_drv_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "Disconnected and Release the MINOR number %d\n", interface->minor);
	usb_deregister_dev(interface, &usb_cd);
}

	

static struct usb_device_id usb_drv_table[] = {
	{
		USB_DEVICE(USB_VENDOR_ID,USB_PRODUCT_ID)
	},
	{}
};
MODULE_DEVICE_TABLE(usb, usb_drv_table);

static struct usb_driver usb_drv_struct = {
	.name 		= "TechoGenius USB Driver",
	.probe 		= usb_drv_probe,
	.disconnect 	= usb_drv_disconnect,
	.id_table 	= usb_drv_table,
};


static int __init usb_test_init(void)
{
	printk(KERN_INFO " Register the usb driver with the usb subsystem\n");
	usb_register(&usb_drv_struct);
}

static void __exit usb_test_exit(void)
{
	printk(KERN_INFO " DeRegister the usb driver with the usb subsystem\n");
	usb_deregister(&usb_drv_struct);
}

module_init(usb_test_init);
module_exit(usb_test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechoGenius Academy <techogenius7519@gmail.com>");
MODULE_DESCRIPTION("USB test Driver");
