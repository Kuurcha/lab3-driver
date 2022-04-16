#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x8e6402a9, "module_layout" },
	{ 0x37a0cba, "kfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xfe5cf5ad, "cdev_del" },
	{ 0x64b60eb0, "class_destroy" },
	{ 0xf595d267, "device_destroy" },
	{ 0xd9da0486, "device_create" },
	{ 0xa946dcde, "__class_create" },
	{ 0xc0fdf3d5, "cdev_add" },
	{ 0x82046fe4, "cdev_alloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xe78dfe6d, "kmem_cache_alloc_trace" },
	{ 0x595451f1, "kmalloc_caches" },
	{ 0xffee12e9, "try_module_get" },
	{ 0x49b3b968, "module_put" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x167e7f9d, "__get_user_1" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "E4951A07A1148B51A4A8A8C");
