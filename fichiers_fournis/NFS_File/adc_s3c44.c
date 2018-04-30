/*
 *************************************************************
 * Projet  : drivers/adc
 * Fichier : $RCSfile: adc_s3c44.c,v $
 * Auteur  : $Author: xmontagne $
 * Version : $Revision: 1.01 $
 * Date    : $Date: 2006-12-16 $
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
#include <asm/hardware.h>
#include <asm/io.h>
#include <linux/proc_fs.h>

#ifdef ADC_DEBUG
        #define ADCMSG(fmt,args...) printk("ADC : " fmt,##args)
#else
        #define ADCMSG(fmt,args...)
#endif

/* Device name as it will appear in /proc/devices */
#define DEVICE_NAME           "ADC-S3C44B0X"
#define ADC_MAJOR             70
#define ADC_VERSION           0x400
#define ADC_SAMPLING          0x800
#define ADC_CHANNEL           0x500

static DECLARE_WAIT_QUEUE_HEAD(timeout_wait_queue);


ssize_t adc_read(struct file *file, char *buffer, size_t length, loff_t *offset);
ssize_t adc_write(struct file *file, const char *buffer, size_t length, loff_t *offset);
int adc_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
int  adc_open(struct inode *inode, struct file *file);
int  adc_close(struct inode *inode, struct file *file);
static int adc_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data);
static int adc_write_proc(struct file *file, const char *buffer, unsigned long count, void *data);
void adc_handler(int irq, void *dev_id, struct pt_regs *regs);

struct file_operations adc_fops =
{
        owner:          THIS_MODULE,
        read:           adc_read,
        write:          adc_write,
        open:           adc_open,
	ioctl:          adc_ioctl,
        release:        adc_close,
};


MODULE_LICENSE("GPL");


unsigned char VERSION = 1;
unsigned char ADCPSR  = 100;
unsigned int Sampling;
int channel[8];
int channelID = 0;
unsigned char TIMER3_VAL = 100;

int init_module(void)
{
	unsigned int res, i_ms;
	struct proc_dir_entry *proc_ADC;
        struct proc_dir_entry *proc_Channel1;
	struct proc_dir_entry *proc_Channel2;
	struct proc_dir_entry *proc_Channel3;
	struct proc_dir_entry *proc_Channel4;
	struct proc_dir_entry *proc_Channel5;
	struct proc_dir_entry *proc_Channel6;
	struct proc_dir_entry *proc_Channel7;
	struct proc_dir_entry *proc_Channel8;
	struct proc_dir_entry *proc_Sampling;

	i_ms = TIMER3_VAL; // 10 ms by default

        printk("Loading ADC driver for S3C440X v1.%02d "__DATE__"\n", VERSION);

	writel ( readl(S3C44B0X_CLKCON) | (1<<12), S3C44B0X_CLKCON );// Enable ADC clock
	//writel ( 0x01 | (0<<2), S3C44B0X_ADCCON ); // Enable ADC
	writel ( ADCPSR, S3C44B0X_ADCPSR );

	Sampling = (61000000/(2*(ADCPSR+1))/16);
	printk("ADCPSR = 0x%08x\n", readl(S3C44B0X_ADCPSR));
	ADCMSG("ADC conv freq = %dHz\n", (int)(Sampling));

	// clear manual update bit, stop Timer3 (f d<82>cal<82> de 16: timer 3, 12: timer 2 (voir S3C44.pdf)
        writel ( readl ( S3C44B0X_TCON  ) & ( ~ ( 0xf << 16 ) ), S3C44B0X_TCON );
        writel ( readl ( S3C44B0X_TCFG0 ) & 0xffff00ff, S3C44B0X_TCFG0 ); // (presc=190) => 320000 KHz
        writel ( readl ( S3C44B0X_TCFG0 ) | 0x0000BE00, S3C44B0X_TCFG0 );
        writel ( readl ( S3C44B0X_TCFG1 ) & 0xffff0fff, S3C44B0X_TCFG1 ); // set Timer 3 MUX 1/32
        writel ( readl ( S3C44B0X_TCFG1 ) | 0x00004000, S3C44B0X_TCFG1 ); // => 10000 KHz
        writel ( i_ms * 10, S3C44B0X_TCNTB3 );                 // en ms.
        writel (         1, S3C44B0X_TCMPB3 );
        writel ( readl ( S3C44B0X_TCON ) & ( ~ ( 0x0f << 16 ) ), S3C44B0X_TCON ); // REPEAT + OFF
        writel ( readl ( S3C44B0X_TCON ) |     ( 0x0a << 16 )  , S3C44B0X_TCON );

        /* Attaching IRQ handler */
        res = request_irq(S3C44B0X_INTERRUPT_TIMER3, adc_handler, SA_INTERRUPT, "TIMER3_ADC", 0);
        if (res)
        {
                printk("Unable to use IRQ%u\n", S3C44B0X_INTERRUPT_TIMER3);
                return(1);
        }

        res = register_chrdev(ADC_MAJOR, DEVICE_NAME, &adc_fops);
        if (res < 0)
        {
                printk("Error registering driver.\n");
                return -ENODEV;
        }

	proc_ADC = proc_mkdir("ADC", 0);
	proc_Channel1 = create_proc_read_entry("Channel1", 0, proc_ADC, adc_read_proc, (void*)(&channel[0]));
	proc_Channel2 = create_proc_read_entry("Channel2", 0, proc_ADC, adc_read_proc, (void*)(&channel[1]));
	proc_Channel3 = create_proc_read_entry("Channel3", 0, proc_ADC, adc_read_proc, (void*)(&channel[2]));
	proc_Channel4 = create_proc_read_entry("Channel4", 0, proc_ADC, adc_read_proc, (void*)(&channel[3]));
	proc_Channel5 = create_proc_read_entry("Channel5", 0, proc_ADC, adc_read_proc, (void*)(&channel[4]));
	proc_Channel6 = create_proc_read_entry("Channel6", 0, proc_ADC, adc_read_proc, (void*)(&channel[5]));
	proc_Channel7 = create_proc_read_entry("Channel7", 0, proc_ADC, adc_read_proc, (void*)(&channel[6]));
	proc_Channel8 = create_proc_read_entry("Channel8", 0, proc_ADC, adc_read_proc, (void*)(&channel[7]));
	proc_Sampling = create_proc_read_entry("Sampling", 0, proc_ADC, adc_read_proc, (void*)(&Sampling));
	proc_Sampling->write_proc = adc_write_proc;

        writel ( readl ( S3C44B0X_TCON ) & ( ~ ( 0x0f << 16 ) ), S3C44B0X_TCON );  // TIMER3 Start
        writel ( readl ( S3C44B0X_TCON ) |     ( 0x09 << 16 )  , S3C44B0X_TCON );

	printk("ADC driver registered !\n");
	return 0;
}

void cleanup_module(void)
{
	writel ( 0x00 | (0<<2), S3C44B0X_ADCCON ); // Disable ADC
        writel ( readl ( S3C44B0X_TCON ) & ( ~ ( 0x0f << 16 ) ), S3C44B0X_RTCCON ); // TIMER3 Stop
        writel ( readl ( S3C44B0X_TCON ) |     ( 0x0a << 16 )  , S3C44B0X_TCON );
	free_irq(S3C44B0X_INTERRUPT_TIMER3, NULL);
	remove_proc_entry("ADC", NULL);
	unregister_chrdev(ADC_MAJOR, DEVICE_NAME);
        printk("Unloading ADC driver successfully\n");
}

static int adc_write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
        int value;
        int *ptr;

        if (count<0)
                return -EINVAL;
        if (count == 0)
                return 0;

        sscanf(buffer, "%d", &value);

        if ((data == &Sampling) && (value > 8000 && value < 100000))
        {
                ptr = data;
                *ptr = value;
		ADCPSR = 61000000 / (value*16);
		ADCPSR = (ADCPSR / 2) - 1;
		writel ( ADCPSR, S3C44B0X_ADCPSR );
		//Sampling = (61000000/(2*(ADCPSR+1))/16);
        }

        return count;
}

static int adc_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
        char *out;
        int len;
        int *value;

        value = data;
        if (value == NULL)
                return -ENODEV;

        out = page;
        out += sprintf(out, "%d\n", *value);

        len = out - page - off;
        if (len < count) 
	{
		*eof = 1;
                if (len <= 0) return 0;
        } else {
                len = count;
        }
        *start = page + off;
        return len;
}


int adc_open(struct inode *inode, struct file *file)
{
        ADCMSG("Device opened.\n");
        MOD_INC_USE_COUNT;
	writel ( 0x01 | (0<<2), S3C44B0X_ADCCON ); // Enable ADC
        return 0;
}

int adc_close(struct inode *inode, struct file *file)
{
        ADCMSG("Device closed.\n");
        MOD_DEC_USE_COUNT;
	writel ( 0x00 | (0<<2), S3C44B0X_ADCCON ); // Disable ADC
        return 0;
}

ssize_t adc_write(struct file *file, const char *buffer, size_t length, loff_t *offset)
{
        unsigned int ret;
	ADCMSG("write called : length = %d\n", length);
	ret = 0;
	return ret;
}

ssize_t adc_read(struct file *file, char *buffer, size_t length, loff_t *offset)
{
	unsigned int ret, i;
	int value;
	ADCMSG("read called : length = %d\n", length);
        writel ( 0x00 | (channelID<<2), S3C44B0X_ADCCON );
	for (i = 0; i<500; i++);
	writel ( 0x01 | (channelID<<2), S3C44B0X_ADCCON );
	while(readl(S3C44B0X_ADCCON) & 0x01);  // To avoid the first flag error case
	while(!(readl(S3C44B0X_ADCCON) & 0x40));  // To avoid the second flag error case
	for (i = 0; i < ADCPSR; i++);
	value = readl(S3C44B0X_ADCDAT);
	ADCMSG("channel[%d] = 0x%08x\n", channelID, value);
	channel[channelID] = value;
	copy_to_user(buffer, &value, length);
	return length;
}

int adc_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
        ADCMSG("timer3-%d ioctl, cmd: 0x%x, arg: %lx.\n", MINOR(inode->i_rdev),cmd, arg);

        switch ( cmd )
        {
        case ADC_VERSION:
                return VERSION;
                break;
        case ADC_SAMPLING:
                if (arg < 8000 || arg > 100000)
                        return -EINVAL;
                ADCPSR = 61000000 / (arg*16);
                ADCPSR = (ADCPSR / 2) - 1;
                writel ( ADCPSR, S3C44B0X_ADCPSR );
                break;
	case ADC_CHANNEL:
		if (arg < 1 || arg > 8)
			return -EINVAL;
		channelID = arg - 1; // 0...7
	default :
		break;
	}

        return 0;
}

void adc_handler(int irq, void *dev_id, struct pt_regs *regs)
{
	int i, value;
        writel ( 0x00 | (channelID<<2), S3C44B0X_ADCCON );
        for (i = 0; i<500; i++);
        writel ( 0x01 | (channelID<<2), S3C44B0X_ADCCON );
        while(readl(S3C44B0X_ADCCON) & 0x01);  // To avoid the first flag error case
        while(!(readl(S3C44B0X_ADCCON) & 0x40));  // To avoid the second flag error case
        for (i = 0; i < ADCPSR; i++);
        value = readl(S3C44B0X_ADCDAT);
        channel[channelID] = value;
}


/* End of File : adc_s3c44.c */

