#include <linux/module.h>  //структура модуля ядра
#include <linux/kernel.h>  // функция printk
#include <linux/fs.h>      // структура файловых операций
#include <linux/cdev.h>    // структура и функции символьных устройств
#include <linux/uaccess.h> // функции copy_to_user и copy_from_user
#include <linux/slab.h>    // функции выделения/освобождения памяти kmalloc kfree

#define DEVICE_NAME "ttyIIVS_TMP"
#define DEVICE_CLASS "CLASS_TMP"
#define MEM_CNT 255

//все функци кроме init_mdoule и cleanuo_module static
static char messageContent[MEM_CNT]; //содержание сообщения
static char* messageBufferPointer;// Указатель на выделенный буфер для kmalloc сообщения
static dev_t dev; //переменная в которую будет записан номер для нового устройства
static int returnResult; //перменная, хранящая результат выполнения функциий
static long returnResultLong;
static int majorAdress; //старший номер устройства
static int minorAdress; //младший номер устройства
static struct cdev *cdev;  //структура символьного устройства
static struct class *class; //указатель на класс файла устройства
static struct device *device; // указатель на файл устройства
static int amountOfBytesWritten; //количество записанных байтов
static int amountOfBytesRead; //количество прочитанные байтов
//Прототип функции чтения
static ssize_t read_func(struct file *file, char *buf, size_t bufLen, loff_t *offset);
//Прототип функции записи
static ssize_t write_func(struct file *file, const char *buf, size_t bufLen, loff_t *offset);
//Структура файловых операций, ссылки на функции
static struct file_operations fileOperationStruct = {
    .read = read_func,   // ссылка на функцию чтения
    .write =  write_func, // ссылка на функцию записи
    .owner = THIS_MODULE
};
MODULE_LICENSE("GPL");
//The kernel ring buffer is line buffered, which means it's not flushed until it encounters a newline. Add a \n to the end of your printk strings: https://askubuntu.com/questions/1111529/printk-message-doesnt-display-till-next-event
int init_module(void){
    printk("Module loaded\n");
    //6.1.3, выделение памяти под сообщение
    messageBufferPointer = kmalloc(sizeof(messageContent), GFP_KERNEL);
    if (messageBufferPointer == 0){
         printk("Something went wrong with kmalloc when allocating message\n");
         return -1;
    }
    //6.1.4 Выделение номера символьного устройства.
    returnResult = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (returnResult!= 0){
        printk("Something went wrong with alloc_chrdev_region when allocating number for the device\n");
    }
    majorAdress = MAJOR(dev);
    minorAdress = MINOR(dev);
    printk("allocated device number is %d  %d\n", majorAdress, minorAdress);
    //6.1.5 Инициализация структуры символьного устройства
    cdev = cdev_alloc();
    if (cdev == NULL){
        printk("struct allocation with cdevAlloc failed\n");
    }
    //заполнение, используя указатель на эту структуру
    cdev->owner = THIS_MODULE;
    cdev->ops = &fileOperationStruct;

    //регистрация символьного устройства в ядре
    returnResult = cdev_add(cdev, dev, 1);
    if (returnResult!=0){
        printk("cdev registartion with cdev_add failed with error code %d\n", returnResult);
    }

    //6.1.6 Создание файла устройства
    //создание класса нового устройства
    class = class_create(THIS_MODULE, DEVICE_CLASS);
    if (class == NULL){
        printk("something went wrong with class_create");
    }
    //создание файла устройства
    device = device_create(class, NULL, dev, NULL, DEVICE_NAME);
    if (device == NULL){
        printk("something went wrong with device_create\n");
    }

    printk ("module initialisation complete\n");
    return 0;
}

//clean 
void cleanup_module(void){
     printk("Module unload start\n");
    //6.2.3.1 Освободить созданное устройство
    device_destroy(class, dev);
    //6.2.3.2 Дерегистрация класса устройства
    class_destroy(class);
    //6.2.3.3 Удаление структуры символьного устройства
    cdev_del(cdev);
    //6.2.3.4 Дерегистрация номера символьного устройства
    unregister_chrdev_region(dev, 1);
    //6.2.3.5 Освобождение памяти под сообщение
    kfree(messageBufferPointer);

    printk("Module unloaded\n");
}

//Функция записи в устройство
//6.2 описание файловых операций
//6.2.1 реализация функции записи
static ssize_t write_func(struct file *file, const char *buf,size_t bufLen, loff_t *offst)
{

    printk("device_write started \n");

    printk("arguments(%p, %s, %ld)\n", file, buf, bufLen);
    //записать данные от пользователя во внутренний буфер, который был выделен при помощи kmalloc при инициализации. 
    returnResultLong = copy_from_user(messageBufferPointer, buf, bufLen);
    if (returnResultLong!=0){
        printk("something went wrong with copy_from_user, can't write %ld bytes\n", returnResultLong);
        return 0;
    }
    printk("write ok!\n");
    //сохранить количество записанных байт в поле модуля
    amountOfBytesWritten = bufLen;
    //увеличить *offst на количество записанных байт
    offst += bufLen;
    //возвратить из функции количество записанных байт.
    return bufLen;
}
//6.2.2 Реализация функции чтения
static ssize_t read_func(struct file *file, char *buf, size_t bufLen, loff_t * offst){
    printk("reading func called \n");

    printk("arguments(file: %p, buf: %s, bufLen: %ld, offset %lld)\n", file, buf, bufLen, *offst);


    amountOfBytesRead = 0;
    //если значение *offst != 0 - возвратить 0, что определит окончание чтения
    if( *offst != 0){
        printk("reading finished!\n");
        return 0;
    }
    //записать данные из внутреннего буфера в выходной буфер пользователя 
    returnResultLong = copy_to_user(buf, messageBufferPointer, amountOfBytesWritten );
        if (returnResultLong!=0){
        printk("something went wrong with copy_to_user, can't write %ld bytes\n", returnResultLong);
        return 0;
    }
    //увеличить *offst на количество прочитанных байт
    *offst += amountOfBytesWritten;
    //возвратить из функции количество прочитанных байт.
    return amountOfBytesWritten;

}
/*
*insmod ./lab3_driver.ko 
*ls -l /dev/ttyIIVS_TMP
*echo "text_of_message" > /dev/ttyIIVS_TMP
*cat /dev/ttyIIVS_TMP
*rmmod ./lab3_driver.ko
*
*
*/
