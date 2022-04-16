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
	{ 0x9f593a9b, "module_layout" },
	{ 0x37a0cba, "kfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x36d04f2b, "cdev_del" },
	{ 0xb50c5cd2, "class_destroy" },
	{ 0x676d16c2, "device_destroy" },
	{ 0xfba270b5, "device_create" },
	{ 0x827ba31, "__class_create" },
	{ 0x56b94690, "cdev_add" },
	{ 0xb088826d, "cdev_alloc" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xed5f6e, "kmem_cache_alloc_trace" },
	{ 0x25931311, "kmalloc_caches" },
	{ 0xf73cafe, "try_module_get" },
	{ 0x1a4937cb, "module_put" },
	{ 0xc3aaf0a9, "__put_user_1" },
	{ 0x167e7f9d, "__get_user_1" },
	{ 0xc5850110, "printk" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "4E981252C8F7C4BEC60A96B");
