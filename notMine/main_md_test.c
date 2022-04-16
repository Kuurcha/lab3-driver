#include <linux/module.h>  //структура модуля ядра
#include <linux/kernel.h>  // функция printk
#include <linux/fs.h>      // структура файловых операций
#include <linux/cdev.h>    // структура и функции символьных устройств
#include <linux/uaccess.h> // функции copy_to_user и copy_from_user
#include <linux/slab.h>    // функции выделения/освобождения памяти kmalloc kfree
// #include <linux/init.h>

//  Прототипы функций, обычно их выносят в заголовочный файл (.h)
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "ttyIIVS_HVI3" // Имя устройства, будет отображаться в /proc/devices
#define DEVICE_CLASS "CLASS_HVI3"
#define MEM_CNT 255 // Максимальная длина сообщения

// Макросы

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vika");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.1");

// Глобальные переменные, объявлены как static, воизбежание конфликтов имен.
static struct device *device; // Указатель на новый класс
static struct class *class;   // Указатель на новый класс
static struct cdev *cdev;     //структура символьного устройства
static dev_t dev;             // ссылка на переменную dev_t, в которую будет записан номер для нового устройства
static char *BufLinck;        // Указатель на выделенный буфер
static int Major;             // Старший номер устройства нашего драйвера
static int Minor;             // Младший номер устройства нашего драйвера
static int Device_Open = 0;   // Устройство открыто?
                              // используется для предотвращения одновременного
                              // обращения из нескольких процессов
static char msg[MEM_CNT];     // Здесь будет собираться текст сообщения
static char *msg_Ptr;

// Заполнение структуры файловых операций
static struct file_operations fops = {
    .read = device_read,   // ссылка на функцию чтения
    .write = device_write, // ссылка на функцию записи
    .owner = THIS_MODULE,  // //владелец
    .open = device_open,
    .release = device_release};

// Функции

// Вызывается ядром Linux при запуске модуля.
// Функция должна возвратить 0 в случае успешного запуска
int init_module(void)
{
    printk("<1>Start init_module\n");

    // Выделение памяти под сообщение
    BufLinck = kmalloc(sizeof(msg), GFP_KERNEL);
    if (BufLinck == 0)
    {
        printk("<1>Kmalloc failed\n");
        return -1;
    }
    printk("<1>Kmalloc done\n");

    // Выделение номера символьного устройства
    int allocChrdevRegionRes = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (allocChrdevRegionRes < 0)
    {
        printk("<1>alloc_chrdev_region failed\n");
        return allocChrdevRegionRes;
    }
    printk("<1>alloc_chrdev_region done %d\n", allocChrdevRegionRes);

    Major = MAJOR(dev);
    printk("<1>Major is %d\n", Major);
    Minor = MINOR(dev);
    printk("<1>Minor is %d\n", Minor);

    // Инициализация структуры символьного устройства
    cdev = cdev_alloc();
    if (!cdev)
    {
        printk("<1>cdev_alloc failed\n");
    }
    printk("<1>cdev_alloc done\n");

    // Заполнение структуры символьного устройства
    cdev->owner = THIS_MODULE;
    cdev->ops = &fops;

    // Регистрирация структуры символьного устройства в ядре
    int cdevAddRes = cdev_add(cdev, dev, 1);
    if (cdevAddRes != 0)
    {
        printk("<1>cdev_add failed with error %d\n", cdevAddRes);
    }
    printk("<1>cdev_add done %d\n", cdevAddRes);

    // создания нового класса
    class = class_create(THIS_MODULE, DEVICE_CLASS);
    if (!class)
    {
        printk("<1>class_create failed\n");
    }
    printk("<1>class_create done\n");

    // непосредственно создать файл устройства
    device = device_create(class, NULL, dev, NULL, DEVICE_NAME);
    if (!device)
    {
        printk("<1>device_create failed\n");
    }
    printk("<1>device_create done\n");

    return 0;
}

// вызывается ядром Linux при выгрузке модуля из ядра Linux
void cleanup_module(void)
{
    // Освободить созданное устройство
    device_destroy(class, Major);

    // Дерегистрация класса устройства !
    class_destroy(class);

    // Удаление структуры символьного устройства
    cdev_del(cdev);

    // Дерегистрация номера символьного устройства
    unregister_chrdev_region(dev, 1);

    // Освобождение памяти под сообщение
    kfree(BufLinck);

    // ready
    // unregister_chrdev(Major, DEVICE_NAME);

    // последнее сообщение
    printk("<1>cleanup_module done\n");
}

// Обработчики

// Вызывается, когда процесс пытается открыть файл устройства, например командой
static int device_open(struct inode *inode, struct file *file)
{
    printk("<1>device_open start (open file)\n");

    if (Device_Open)
        return -EBUSY;
    Device_Open++;

    try_module_get(THIS_MODULE);
    return SUCCESS;
}

// Вызывается, когда процесс закрывает файл устройства.
static int device_release(struct inode *inode, struct file *file)
{
    printk("<1>device_release start (close file)\n");

    // Теперь мы готовы обслужить другой процесс
    Device_Open--;

    // Уменьшить счетчик обращений
    module_put(THIS_MODULE);

    return SUCCESS;
}

// Вызывается, когда процесс пытается прочитать уже открытый файл устройства
static ssize_t device_read(struct file *file, // file
                           char *buffer,      // буфер, куда надо положить данные
                           size_t bufLen,     // размер буфера
                           loff_t *offset)
{
    printk("<1>device_read start\n");

    printk("device_read(%p,%p,%d)\n", file, buffer, bufLen);

    // Количество байт, фактически записанных в буфер
    int bytes_read = 0;

    // Если достигли конца сообщения,
    // вернуть 0, как признак конца файла
    if (*msg_Ptr == 0)
        return 0;

    // Перемещение данных в буфер
    while (bufLen && *msg_Ptr)
    {
        // Буфер находится в пространстве пользователя (в сегменте данных),
        // а не в пространстве ядра, поэтому простое присваивание здесь недопустимо.
        // Для того, чтобы скопировать данные, мы используем функцию put_user,
        // которая перенесет данные из пространства ядра в пространство пользователя.
        put_user(*(msg_Ptr++), buffer++);
        bufLen--;
        bytes_read++;
    }

    // В большинстве своем, функции чтения возвращают количество байт, записанных в буфер.
    return bytes_read;
}

// Вызывается, когда процесс пытается записать в устройство,
// например так: echo "hi" > /dev/chardev
static ssize_t device_write(struct file *file,  // file
                            const char *buffer, // буфер, куда надо положить данные
                            size_t bufLen,      // размер буфера
                            loff_t *offset)
{
    printk("<1>device_write start\n");

    printk("device_write(%p,%s,%d)", file, buffer, bufLen);

    int i;
    for (i = 0; i < bufLen && i < MEM_CNT; i++)
    {
        get_user(msg[i], buffer + i);
    }
    msg_Ptr = msg;
    return i;
}
