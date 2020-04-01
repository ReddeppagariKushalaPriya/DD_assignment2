
#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/usb.h>
#include <linux/slab.h>
#define READ_CAPACITY_LENGTH   0x08
#define REQUEST_SENSE_LENGTH   0x12
#define BOMS_GET_MAX_LUN       0xFE
#define CARD_READER_VID  0x0781
#define CARD_READER_PID  0x559d
#define data_length      0x24
#define SAMSUNG_MEDIA_VID  0x04e8
#define SAMSUNG_MEDIA_PID  0x6860
#define REQ_TYPE	   0xA1
#define NXP_KEIL_VID 0xc251
#define NXP_MSD_PID 0x1303
struct command_block_wrapper {
	u8 dCBWSignature[4];
	u32 dCBWTag;
	u32 dCBWDataTransferLength;
	u8 bmCBWFlags;
	u8 bCBWLUN;
	u8 bCBWCBLength;
	u8 CBWCB[16];
};
// Section 5.2: Command Status Wrapper (CSW)
struct command_status_wrapper {
	u8 dCSWSignature[4];
	u32 dCSWTag;
	u32 dCSWDataResidue;
	u8 bCSWStatus;
};


struct usb_device *udev;
int flag=0;
u8 lun;
u8 cdb[16];
u8 buffer[64];
u32 ret_tag;
u8 ep_in1,ep_out1;

int logical_unit_number(void);
int send_cbw(void);
int read_capacity(void);
static __u8 cdb_length[256] = {
//	 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
	06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  0
	06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,06,  //  1
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  2
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  3
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  4
	10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,  //  5
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  6
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  7
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  8
	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,  //  9
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  A
	12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,  //  B
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  C
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  D
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  E
	00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,  //  F
};


int send_cbw()
{	
	
	__u64 big;
	int i, r, size,size1;
	u8 cdb_len;
	
	u32  max_lba,block_size;
	
	unsigned long device_size;
	struct command_status_wrapper csw;

	static u32 tag = 1;
	int ct=0;
	
	struct command_block_wrapper cbw;
	memset(&csw, 0, sizeof(csw));
	
	
	
	cbw.dCBWSignature[0] = 'U';
	cbw.dCBWSignature[1] = 'S';
	cbw.dCBWSignature[2] = 'B';
	cbw.dCBWSignature[3] = 'C';
	
	cbw.bCBWLUN = 0x00;









	//read_capacity
	__u32 buffer1[64];
	memset(cdb, 0, sizeof(cdb));
	memset(buffer1, 0, sizeof(buffer1));
	cdb[0] = 0x25;
	//printk(KERN_INFO"KUSHALA");
	
	size=0;
	
	ret_tag = tag;
	cbw.dCBWTag = tag+1;
	cbw.dCBWDataTransferLength = READ_CAPACITY_LENGTH; //
	cbw.bmCBWFlags = 0x80; //since data transfer is in
	cdb_len = cdb_length[cdb[0]];	
	
	cbw.bCBWCBLength = cdb_len;
	memcpy(cbw.CBWCB, cdb, cdb_len);

	r = usb_bulk_msg(udev, usb_sndbulkpipe(udev,ep_out1),(void *)&cbw, 31, &size, 10000);
	
	//printk(KERN_INFO "   sent %d CDB bytes error %d actual sent=%d\n", cdb_len,r,size);
	
	
	i=usb_bulk_msg(udev, usb_rcvbulkpipe (udev,ep_in1), (void *)&buffer1,  READ_CAPACITY_LENGTH, &size1, 10000);
	//printk(KERN_INFO "   received %d bytes\n", size1);
	//printk(KERN_INFO"KUSHALA");
	

	//printk(KERN_INFO"KUSHALA");
	
	
	max_lba = (__u32*)(&buffer1[0]);
	block_size =(__u32*)(& buffer1[4]);

	device_size=(block_size/(1024*1024*1024));
	printk(KERN_INFO "in hexa:   Max LBA: %08X, Block Size: %08X therefore total size: (%llx GB)\n", max_lba, block_size, device_size);

	
	
return 0;


}


int read_capacity()
{
send_cbw();
return 0;
}


struct usbdev_private
{
	//struct usb_device *udev;
	unsigned char class;
	unsigned char subclass;
	unsigned char protocol;
	unsigned char ep_in;
	unsigned char ep_out;
};



	
struct usbdev_private *p_usbdev_info;

static void usbdev_disconnect(struct usb_interface *interface)
{
	printk(KERN_INFO "USBDEV Device Removed\n");
	return;
}

static struct usb_device_id usbdev_table [] = {
	{USB_DEVICE(CARD_READER_VID, CARD_READER_PID)},
	{USB_DEVICE(SAMSUNG_MEDIA_VID, SAMSUNG_MEDIA_PID)},
	{USB_DEVICE(NXP_KEIL_VID, NXP_MSD_PID)},
	{} /*terminating entry*/	
};


static int usbdev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
	int i;
	u8 epAddr, epAttr;
	
	struct usb_endpoint_descriptor *ep_desc;
	udev = interface_to_usbdev(interface);
	if(id->idProduct == CARD_READER_PID)
	{
		printk(KERN_INFO "Camera Plugged in\n");
	}
	else if(id->idProduct == SAMSUNG_MEDIA_PID)
	{
		printk(KERN_INFO "Media Plugged in\n");
	}
	else if(id->idVendor == NXP_KEIL_VID)
	{
		printk(KERN_INFO "Blueboard Plugged in\n");
	}

	printk(KERN_INFO"VID:%4X\n",CARD_READER_VID);
	printk(KERN_INFO"PID:%X\n",CARD_READER_PID);
	//if_desc = interface->cur_altsetting;
	printk(KERN_INFO "No. of Altsettings = %d\n",interface->num_altsetting);

	printk(KERN_INFO "No. of Endpoints = %d\n", interface->cur_altsetting->desc.bNumEndpoints);

	for(i=0;i<interface->cur_altsetting->desc.bNumEndpoints;i++)
	{
		ep_desc = &interface->cur_altsetting->endpoint[i].desc;
		epAddr = ep_desc->bEndpointAddress;
		epAttr = ep_desc->bmAttributes;
	
		if((epAttr & USB_ENDPOINT_XFERTYPE_MASK)==USB_ENDPOINT_XFER_BULK)
		{
			if(epAddr & USB_DIR_IN)
				{
				ep_in1 = epAddr;
				printk(KERN_INFO "EP %d is Bulk IN\n address=%c", i,ep_in1);
				}
			else	{
				ep_out1 = epAddr;
				printk(KERN_INFO "EP %d is Bulk OUT\n address=%c", i,ep_out1);
				
				}
	
		}
	}
	
	//p_usbdev_info->class = interface->cur_altsetting->desc.bInterfaceClass;
	
	printk(KERN_INFO "USB DEVICE CLASS : %x", interface->cur_altsetting->desc.bInterfaceClass);
	printk(KERN_INFO "USB DEVICE SUB CLASS : %x", interface->cur_altsetting->desc.bInterfaceSubClass);
	printk(KERN_INFO "USB DEVICE Protocol : %x", interface->cur_altsetting->desc.bInterfaceProtocol);
printk(KERN_INFO "READING cap:\n");
	flag=1;
	read_capacity();

return 0;
}/*


/*Operations structure*/
static struct usb_driver usbdev_driver = {
	name: "usbdev",  //name of the device
	probe: usbdev_probe, // Whenever Device is plugged in
	disconnect: usbdev_disconnect, // When we remove a device
	id_table: usbdev_table, //  List of devices served by this driver
};




int device_init(void)
{
	usb_register(&usbdev_driver);
	if (flag)
{

	read_capacity();
	flag=0;
}		
	

	return 0;
}

void device_exit(void)
{
	usb_deregister(&usbdev_driver);
	printk(KERN_NOTICE "Leaving Kernel\n");
	//return 0;
}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("KUSHALA PRIYA");
MODULE_DESCRIPTION("USB DEVICE DRIVER");
