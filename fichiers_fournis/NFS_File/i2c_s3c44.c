/*
 *************************************************************
 * Projet  : drivers/i2c/S3C44/
 * Fichier : $RCSfile: i2c_s3c44.c,v $
 * Auteur  : $Author: xmontagne $
 * Version : $Revision: 1.7 $
 * Date    : $Date: 2006-12-02 $
 * Tag     : $Name:  $
 *************************************************************
*/

#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
#include <linux/modversions.h>
#define MODVERSIONS
#endif


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <linux/proc_fs.h>

#define U32    unsigned int
#define U8     unsigned char

#define WRDATA	      (1)
#define POLLACK       (2)
#define RDDATA	      (3)
#define SETRDADDR     (4)

#define BIT_IIC       (1<<5)
#define IICBUFSIZE    0x20
#define I2C_VVERSION  0x400
#define I2C_TTIMEOUT  0x500

#ifdef I2C_DEBUG
        #define I2CMSG(fmt,args...) printk("I2C : " fmt,##args)
#else
        #define I2CMSG(fmt,args...)
#endif

/* Device name as it will appear in /proc/devices */
#define DEVICE_NAME "I2C-S3C44B0X"

U8 _iicData[IICBUFSIZE];
volatile int _iicDataCount;
volatile int _iicStatus;
volatile int _iicMode;
int _iicPt;
unsigned char SLAVADDR = 0;
unsigned char POLL_TIMEOUT = 10;

static struct timer_list timeout_timer;
static DECLARE_WAIT_QUEUE_HEAD(timeout_wait_queue);


void i2c_handler(int irq, void *dev_id, struct pt_regs *regs);
ssize_t i2c_read(struct file *file, char *buffer, size_t length, loff_t *offset);
ssize_t i2c_write(struct file *file, const char *buffer, size_t length, loff_t *offset);
int i2c_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
int write(U32 slvAddr,U32 addr,U8 *data);
int read(U32 slvAddr,U32 addr,U8 *data);
int  i2c_open(struct inode *inode, struct file *file);
int  i2c_close(struct inode *inode, struct file *file);
unsigned int  Delay(unsigned int val);


struct file_operations i2c_fops =
{
        owner:          THIS_MODULE,
        read:           i2c_read,
        write:          i2c_write,
        open:           i2c_open,
	ioctl:          i2c_ioctl,
        release:        i2c_close,
};


MODULE_LICENSE("GPL");

#define STATUS_OK           0
#define STATUS_KO           1
#define IIC_ITEN            (1<<5)
#define IIC_ACKEN           (1<<7)
#define IIC_PEND            (1<<4)

#define I2C_SPEED_30K       0x30
#define I2C_SPEED_60K       0x60
#define I2C_SPEED_120K      0x120
#define I2C_SPEED_238K      0x238
#define I2C_SPEED_380K      0x380

unsigned char STATUS = STATUS_OK;
unsigned char NOACK  = 0;
unsigned char IIC_SPEED = 0;

unsigned char VERSION = 9;

int init_module(void)
{
	unsigned int res;

        printk("Loading I2C driver for S3C440X v1.%02d "__DATE__"\n", VERSION);

	writel(readl(S3C44B0X_PCONF) | 0x0a, S3C44B0X_PCONF);
	writel(readl(S3C44B0X_PUPF)  | 0x03, S3C44B0X_PUPF);

	IIC_SPEED = (1<<6) | (0);
	writeb(IIC_SPEED, S3C44B0X_IICCON); // 120 KHz - CLOCK=61MHz
        I2CMSG("S3C44B0X_IICCON = 0x%08x\n", readb(S3C44B0X_IICCON));
        I2CMSG("S3C44B0X_IICADD = 0x%08x\n", readb(S3C44B0X_IICADD));

	/* Enable TX/RX */
	writeb(0x10, S3C44B0X_IICSTAT);
        I2CMSG("S3C44B0X_IICSTAT = 0x%08x\n", readb(S3C44B0X_IICSTAT));

	writel(readl(S3C44B0X_INTMSK) & ~(BIT_IIC), S3C44B0X_INTMSK);
	I2CMSG("S3C44B0X_INTMSK = 0x%08x\n", readl(S3C44B0X_INTMSK));

        res = register_chrdev(I2C_MAJOR, DEVICE_NAME, &i2c_fops);  // 89 in include/linux/i2c.h
        if (res < 0)
        {
                printk("Error registering driver.\n");
                return -ENODEV;
        }

	/* Attaching IRQ handler */
	res = request_irq(S3C44B0X_INTERRUPT_IIC, i2c_handler, SA_INTERRUPT, "I2C", 0);
	if (res)
	{
		printk("Unable to use IRQ%u\n", S3C44B0X_INTERRUPT_IIC);
		return(1);
	}

	printk("I2c driver registered !\n");

	return 0;
}

void timeout_sequence_finished (unsigned long parameters)
{
        unsigned long flags;

        save_flags(flags); cli();
        //h8_monitor_timer_active = 0;
        restore_flags(flags);

        //wake_up(&h8_monitor_wait);
        //wake_up_interruptible (&timeout_wait_queue);
	//printk("TIMEOUT : jiffies = %d\n", jiffies);
	wake_up_interruptible(&timeout_wait_queue);
}

void cleanup_module(void)
{
	free_irq(S3C44B0X_INTERRUPT_IIC, NULL);
	unregister_chrdev(I2C_MAJOR, DEVICE_NAME);
	writel(readl(S3C44B0X_PCONF) & ~(0x0f), S3C44B0X_PCONF); // as input
	writel(readl(S3C44B0X_PUPF)  & ~(0x03), S3C44B0X_PUPF); // pull-up enabled
        printk("Unloading I2C driver successfully\n");
}


int i2c_open(struct inode *inode, struct file *file)
{
        I2CMSG("Device opened.\n");
        MOD_INC_USE_COUNT;

        writel(readl(S3C44B0X_PCONF) | 0x0a, S3C44B0X_PCONF);
        writel(readl(S3C44B0X_PUPF)  | 0x03, S3C44B0X_PUPF);

        IIC_SPEED = (1<<6) | (0);
        writeb(IIC_SPEED, S3C44B0X_IICCON); // 120 KHz - CLOCK=61MHz
        I2CMSG("S3C44B0X_IICCON = 0x%08x\n", readb(S3C44B0X_IICCON));
        I2CMSG("S3C44B0X_IICADD = 0x%08x\n", readb(S3C44B0X_IICADD));
        return 0;
}

int i2c_close(struct inode *inode, struct file *file)
{
        I2CMSG("Device closed.\n");
        MOD_DEC_USE_COUNT;
	writel(readl(S3C44B0X_PCONF) & ~(0x0f), S3C44B0X_PCONF); // as input
	writel(readl(S3C44B0X_PUPF)  & ~(0x0f), S3C44B0X_PUPF); // pull-up enabled
        return 0;
}

ssize_t i2c_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
        unsigned int ret;
        unsigned char data[256];
	I2CMSG("write called : length = %d\n", length);

        if (length > 256)
        {
                printk("You try to write to many bytes ! \n");
                return 0;
        }

        if (SLAVADDR == 0x00)
        {
                printk("Not any slave address selected ! \n");
                printk("Please use the IOCTL command to do it... \n");
                return 0;
        }

	copy_from_user(data, buffer, length);
	ret = write(SLAVADDR, length, &data[0]);
	I2CMSG("i2c_write returns %d\n", ret);
	return ret;
}

int write(U32 slvAddr, U32 len, U8 *data)
{
        int i;
	volatile int timeout;

        _iicMode = WRDATA;
        _iicPt = 0;
        _iicDataCount = len;

	for (i = 0; i < len; i++)
                _iicData[i] = *data++;

	//printk("WR(%02d) addr=0x%02x: ", _iicDataCount, slvAddr);

	writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
        writeb(slvAddr, S3C44B0X_IICDS);
        writeb(0xf0, S3C44B0X_IICSTAT); // Start TX mode

	//writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
	STATUS  = STATUS_OK;
	NOACK   = 0;

        timeout = _iicDataCount * 500;

        while(_iicDataCount != -1)
        {
                timeout--;
                //udelay(100);
                if (timeout == 0)
		{
			writeb(0xd0, S3C44B0X_IICSTAT); // Stop mode
			writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
			Delay(10);
                        STATUS = STATUS_KO;
			printk("WR(%02d) addr=0x%02x: TIMEOUT ! \n", _iicDataCount, slvAddr);
                        I2CMSG("STATUS ERROR (%d)\n", STATUS);
                        return (-1);
		}
        }

        //printk("[timeout=%d] [status=%d]\n", timeout, STATUS);

	if (STATUS > STATUS_OK)
	{
		writeb(0xd0, S3C44B0X_IICSTAT); // Stop RX mode
		writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
		Delay(10);
		I2CMSG("STATUS ERROR (%d)\n", STATUS);
		return (-1);
	}

	// !!!!!!!!!!!!!!!!!
	// Il faudrait attendre le POLLACK !

	writeb(0xd0, S3C44B0X_IICSTAT); // Stop RX mode
	writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
	Delay(10);
	return (len);
}

ssize_t i2c_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
	unsigned int ret;
	unsigned char data[256];
	I2CMSG("read called\n");

	if (length > 256)
	{
		printk("You try to read to many bytes ! \n");
		return 0;
	}

	if (SLAVADDR == 0x00)
        {
                printk("Not any slave address selected ! \n");
                printk("Please use the IOCTL command to do it... \n");
                return 0;
        }
	
	ret = read(SLAVADDR, length, &data[0]);
	copy_to_user(buffer, data, length);
	return ret;
}

int read(U32 slvAddr, U32 len, U8 *data)
{
	int i;
	volatile int timeout;

	_iicMode = RDDATA;
        _iicPt = 0;
        _iicDataCount =  len - 1; // ???

        writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
        writeb(slvAddr, S3C44B0X_IICDS);
        writeb(0xb0, S3C44B0X_IICSTAT); // Start RX mode


        STATUS  = STATUS_OK;
        NOACK   = 0;

	timeout = _iicDataCount * 500;

	while(_iicDataCount != -1)
	{
		timeout--;
                //udelay(100);
		if (timeout == 0)
		{
			STATUS = STATUS_KO;
			printk("RD(%02d) addr=0x%02x: TIMEOUT ! \n", _iicDataCount, slvAddr);
			I2CMSG("STATUS ERROR (%d)\n", STATUS);
			return (-1);
		}
	}

	//printk("[timeout=%d] [status=%d]\n", timeout, STATUS);

	if (STATUS > STATUS_OK)
	{
		I2CMSG("STATUS ERROR (%d)\n", STATUS);
		return (-1);
	}

	for (i = 1; i <= len; i++)
		*data++ = _iicData[i];

	return (len);
}

int i2c_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	unsigned char data[256];
	unsigned int i, j;

        I2CMSG("i2c-%d ioctl, cmd: 0x%x, arg: %lx.\n", MINOR(inode->i_rdev),cmd, arg);

        switch ( cmd ) 
	{
	case I2C_VVERSION:
		//printk("Get VERSION\n");
		return VERSION;
		break;
	case I2C_TTIMEOUT:
		if (arg < 1 || arg > 200)
			return -EINVAL;
		//printk("Set POLL_TIMEOUT at %d\n", arg);
		POLL_TIMEOUT = arg;
		break;
        case I2C_SLAVE:
                if (arg > 0x3ff) 
                        return -EINVAL;
                SLAVADDR = arg;
                break;
        case I2C_RDWR:
        	printk("Write & read test data into KS24C080\n");
		printk("[Byte 5 is read from the 0xb5 address]\n");

        	for (i = 0; i < 256; i++)
                	write(0xa0, (U8)i, (unsigned char*)(&i));
        	for (i = 0; i < 256; i++)
                	data[i] = 0;

        	for (i = 0; i < 256; i++)
                	read(0xa0, (U8)i, &(data[i]));
        	/* Test to verify a wrong address gives 0xff */
        	read(0xb5, (U8)5, &(data[5]));

        	for (i = 0; i < 16; i++)
        	{
                	for (j = 0; j < 16; j++)
                        	printk("%2x ", data[i*16+j]);
                	printk("\n");
        	}
		break;
	case I2C_SPEED_238K :
		//printk("I2C driver set speed at 238K\n");
		IIC_SPEED = 0x0f;
		break;
	case I2C_SPEED_380K :
		//printk("I2C driver set speed at 380K\n");
		IIC_SPEED = 0x09;
		break;
	case I2C_SPEED_120K :
		//printk("I2C driver set speed at 120K\n");
		IIC_SPEED = 0x40;
		break;
	case I2C_SPEED_60K  :
		//printk("I2C driver set speed at 60K\n");
		IIC_SPEED = 0x41;
		break;
	case I2C_SPEED_30K  :
		//printk("I2C driver set speed at 30K\n");
		IIC_SPEED = 0x43;
		break;
	default:
		printk("Unknown IOCTL command [0x%08x] !\n", cmd);
		return (-1);
	}
        return 0;
}

void i2c_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned char iicSt,i;

	I2CMSG("i2c_hanlder catched !\n");
	Delay(10);

	iicSt = readb(S3C44B0X_IICSTAT);
	I2CMSG("S3C44B0X_IICSTAT = 0x%02x\n", iicSt);
	if(iicSt & 0x8) { I2CMSG("I2C : lost arbitration !\n"); STATUS = 8;} // when bus arbitration is failed.
	if(iicSt & 0x4) { I2CMSG("I2C : slave addr is matching !\n"); STATUS = 4;} // when a slave address is matched with IICADD
	if(iicSt & 0x2) { I2CMSG("I2C : slave addr is 0x00 !\n"); STATUS = 2;} // when a slave address is 00000000b
	if(iicSt & 0x1) { I2CMSG("I2C : ACK not received !\n"); if (NOACK & !STATUS) STATUS = STATUS_OK; else STATUS = STATUS_KO;} // when ACK isn't received
	I2CMSG("STATUS = %d\n", STATUS);
	if (STATUS > STATUS_OK)
	{
		I2CMSG("I2C : STATUS KO dans l'IT\n");
		_iicDataCount=0;
	}

	switch (_iicMode)
	{
		case POLLACK:
			I2CMSG("POLLACK\n");
			_iicStatus = iicSt;
			break;

		case RDDATA:
			I2CMSG("RDDATA (_iicDataCount=%d)\n", _iicDataCount);
			Delay(100);
			if (_iicPt == 0) //  ACK of the address -  not yet data received
			{
				_iicData[_iicPt] = 0x74;
				_iicPt++;
				for (i = 0; i < 100; i++) //for setup time until rising edge of IICSCL
					writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
		//		if (STATUS == STATUS_KO) // No ack received
		//			_iicDataCount = -1;
				break;
			}
			if ((_iicDataCount--) == 0)
			{
				_iicData[_iicPt++] = readl(S3C44B0X_IICDS);
	    
				writeb(0x90, S3C44B0X_IICSTAT);
				writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
				I2CMSG("Data read = 0x%02x\n", _iicData[_iicPt-1]);
				Delay(100);	//wait until stop condtion is in effect.
				//too long time... 
				//The pending bit will not be set after issuing stop condition.
				break;    
			}	     
			_iicData[_iicPt++] = readb(S3C44B0X_IICDS);
			if ((_iicDataCount) == 0)
			{
				NOACK = 1;
				for (i = 0; i < 10; i++)
					writeb(IIC_SPEED | IIC_ITEN, S3C44B0X_IICCON);
			}
			else
			{
				NOACK = 0;
				for (i = 0; i < 10; i++)
					writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
			}
			I2CMSG("Data read = 0x%02x\n", _iicData[_iicPt-1]);
			break;

		case WRDATA:
			I2CMSG("WRDATA (_iicDataCount=%d)\n", _iicDataCount);
			if ((_iicDataCount--) == 0)
			{
				writeb(0xd0, S3C44B0X_IICSTAT);
				writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
				Delay(100);	//wait until stop condtion is in effect.
				//The pending bit will not be set after issuing stop condition.
				break;    
			}
			writeb(_iicData[_iicPt++], S3C44B0X_IICDS);
			for (i = 0; i < 10; i++) //for setup time until rising edge of IICSCL
				writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
			//Delay(100); //for setup time until rising edge of IICSCL
			break;

		case SETRDADDR:
			/* No ACK in case of SETRDADDR ?? */
			I2CMSG("SETRDADDR [S%d]\n", _iicDataCount);
			if ((_iicDataCount--) == 0)
			{
				//printk("SETADDR wake_up\n");
				break;  //IIC operation is stopped because of IICCON[4]    
			}
			writeb(_iicData[_iicPt++], S3C44B0X_IICDS);
			for (i = 0; i < 10; i++);  //for setup time until rising edge of IICSCL
				writeb(IIC_SPEED | IIC_ITEN | IIC_ACKEN, S3C44B0X_IICCON);
			break;

		default:
			break;	  
	}
}

unsigned int  Delay(unsigned int val)
{
	volatile unsigned int counter;
	counter = 0;
/*
	val = val << 8;
	while (counter < val)
		counter++;
*/
	return (counter);
}


