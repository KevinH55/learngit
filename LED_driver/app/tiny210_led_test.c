/* tiny210_led_test.c */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define DEV_NAME "/dev/Tiny210_LED"
#define MAGIC 0x05
#define CMD0 _IO(MAGIC,0)
#define CMD1 _IO(MAGIC,1)
#define CMD2 _IO(MAGIC,2)
#define CMD3 _IO(MAGIC,3)
#define CMD4 _IO(MAGIC,4)

int main()
{
    int fd, num, i,j;
    fd = open(DEV_NAME, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        printf("Open Tiny210_LED failed!\n");
        return -1;
    }

    for (i = 0; i < 10; i++) {
        printf("请输入操作：1(全灭), 2(全亮), 3(跑马灯): ");
        if (scanf("%d", &num) != 1) {
            // 输入非数字，清空输入缓冲区
            while (getchar() != '\n');
            printf("输入无效，请重新输入！\n");
            continue;
        }

        if (num == 1 || num == 2) {
            write(fd, &num, sizeof(int));
            read(fd, &num, sizeof(int));
            printf("当前状态: %d\n", num);
        } else if (num == 3) {
            // 执行跑马灯
            for (j = 0; j < 5; j++) { // 跑马灯循环5次
                ioctl(fd, CMD0, 0); // LED0亮
                sleep(1);
                ioctl(fd, CMD1, 0); // LED1亮
                sleep(1);
                ioctl(fd, CMD2, 0); // LED2亮
                sleep(1);
                ioctl(fd, CMD3, 0); // LED3亮
                sleep(1);
            }
            // 跑马灯结束后全灭
            num = 1;
            write(fd, &num, sizeof(int));
        } else {
            printf("无效输入，请输入1、2或3！\n");
        }
    }

    close(fd);
    return 0;
}
