/* tiny210_led_drv.c */
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <linux/slab.h> /* kmalloc, memset etc. */
#include <linux/list.h> /* kernel linkables */
#include <linux/device.h>
#include <asm/uaccess.h>
//#include <linux/ioport.h>
#include <asm/io.h>

#define DEV_NAME "Tiny210_LED"
#define GPJ2_CONTROL_REG 0xe0200280
#define GPJ2_DATA_REG 0xe0200284
#define REG_INIT_VAL 0x0ff0000
#define REG_INIT_OR_VAL 0x00001111

#define MAGIC 0x05
#define CMD0 _IO(MAGIC,0)
#define CMD1 _IO(MAGIC,1)
#define CMD2 _IO(MAGIC,2)
#define CMD3 _IO(MAGIC,3)
#define CMD4 _IO(MAGIC,4)

unsigned int unControlReg;
unsigned long * pLedCtrlAddr;
unsigned long * pLedDataAddr;

static int myData=0;

static ssize_t TestRead(struct file *,char *,size_t,loff_t *);
static ssize_t TestWrite(struct file *,const char *,size_t,loff_t *);
static long led_ioctl(struct file * filp,unsigned int cmd,unsigned long arg);

struct file_operations test_fops=
{
   .read = TestRead,
   .write = TestWrite,
   .unlocked_ioctl = led_ioctl,
};

static struct miscdevice misc=
{
   .minor=MISC_DYNAMIC_MINOR,//动态分配此设备号
   .name = DEV_NAME,         //设备名：/dev/Tiny210_LED 
   .fops = &test_fops,       //文件操作函数指针
};

static int __init Tiny210_Led_init(void)
{
    int ret;
    printk("<1>tiny210_led_drv initing...\n");

    ret = misc_register(&misc);//注册misc设备
    if(ret<0)printk("<1>Register failed!\n");
    else printk("<1>Misc device registered!\n");
    //物理地址——虚拟地址映射
    pLedCtrlAddr = ioremap(GPJ2_CONTROL_REG,4); /* 0xe0912280 */
    pLedDataAddr = ioremap(GPJ2_DATA_REG,4);
    printk("<1>the led control register address is %p \n",pLedCtrlAddr);
    printk("<1>the led Data register address is %p \n",pLedDataAddr);
    unControlReg = ioread32(pLedCtrlAddr);
    printk("<1>Control reg value is %x \n",unControlReg);
    unControlReg = unControlReg & ~REG_INIT_VAL;//位清空（0x0ff0000）
    unControlReg = unControlReg | REG_INIT_OR_VAL;//位设置（0x00001111）
    printk("<1>now the control reg value is about to changed to %x \n",unControlReg);
    iowrite32(unControlReg,pLedCtrlAddr);
    iowrite32(0x0000000f,pLedDataAddr);/* 全灭 */
    printk("<1>Leds 0-3 turn off now!\n" );

    return ret;
}

static void __exit Tiny210_Led_exit(void)
{
    printk("<1>tiny210_led_drv exit!\n");

    misc_deregister(&misc);
    return;
}

static ssize_t TestRead(struct file *filp, char *buf,size_t len,loff_t *off)
{
    printk("<1>tiny210_led_drv Reading...\n");
    if(copy_to_user(buf,&myData,sizeof(int)))return -EFAULT;
    return sizeof(int);
}

static ssize_t TestWrite(struct file *filp, const char *buf, size_t len, loff_t *off)
{
    printk("<1>tiny210_led_drv Writing...\n");
    if (copy_from_user(&myData, buf, sizeof(int)))
        return -EFAULT;
    if (myData == 2) { // 全亮
        iowrite32(0x00000000, pLedDataAddr);
    } else if (myData == 1) { // 全灭
        iowrite32(0x0000000f, pLedDataAddr);
    } else {
        // 其他情况保持原样或默认处理
        iowrite32(0x0000000f, pLedDataAddr);
    }
    return sizeof(int);
}

static long led_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    printk("<1> ioctl is running... \n");

    printk("<1> cmd is %u \n",cmd );
    switch(cmd){
        case CMD0:/* led 0 打开*/
        {
            printk("<1> CMD0, the led 0 will turn on. \n");
            iowrite32(0x0000000e,pLedDataAddr);
            break;
        }
        case CMD1:/* led 1 打开 */
        {
            printk("<1> CMD1, the led 1 will turn on. \n");
            iowrite32(0x0000000d,pLedDataAddr);
            break;
        }
        case CMD2:/* led 2 打开 */
        {
            printk("<1> CMD2, the led 2 will turn on. \n");
            iowrite32(0x0000000b,pLedDataAddr);
            break;
        }
        case CMD3:/* led 3 打开 */
        {
            printk("<1> CMD3, the led 3 will turn on. \n");
            iowrite32(0x00000007,pLedDataAddr);
            break;
        }
        case CMD4:/*参数将直接传递给LED数据寄存器*/
        {
            printk("<1> CMD4,the arg will be delivered to Data Register directly. \n");
            printk("<1> the input parameter arg is %lx \n",arg);
            iowrite32(arg,pLedDataAddr);
            break;
        }
        default:
        {
            printk("<1> CMD can't be recognized by driver. \n");
            return -1;
        }
    }

    return 0;
}

module_init(Tiny210_Led_init);
module_exit(Tiny210_Led_exit);
