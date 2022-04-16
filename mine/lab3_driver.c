#include <linux/module.h>  //структура модуля ядра
#include <linux/kernel.h>  // функция printk
#include <linux/fs.h>  
//указание лецензии
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kurcha");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.1");

//make all
int init_module(void){
    printk("Module loaded");
    return 0;
}
//clean 
void cleanup_module(void){
    printk("Module unloaded");
}
