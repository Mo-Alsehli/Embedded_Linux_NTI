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
	{ 0x30a38ba5, "module_layout" },
	{ 0xa5c1dc06, "proc_remove" },
	{ 0xedc03953, "iounmap" },
	{ 0xc8a42ec4, "proc_create" },
	{ 0x6b4b2933, "__ioremap" },
	{ 0xaf56600a, "arm64_use_ng_mappings" },
	{ 0x8da6585d, "__stack_chk_fail" },
	{ 0x56470118, "__warn_printk" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x12a4e128, "__arch_copy_from_user" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "C1F666F882258F3C851B59D");
