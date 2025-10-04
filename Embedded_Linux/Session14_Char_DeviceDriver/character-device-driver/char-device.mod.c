#include <linux/module.h>
#include <linux/export-internal.h>
#include <linux/compiler.h>

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



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0xa61fd7aa, "__check_object_size" },
	{ 0x092a35a2, "_copy_from_user" },
	{ 0xe8213e80, "_printk" },
	{ 0xd272d446, "__x86_return_thunk" },
	{ 0x90a48d82, "__ubsan_handle_out_of_bounds" },
	{ 0xd272d446, "__stack_chk_fail" },
	{ 0x092a35a2, "_copy_to_user" },
	{ 0x9f222e1e, "alloc_chrdev_region" },
	{ 0xb06a91bd, "cdev_init" },
	{ 0x7db91808, "cdev_add" },
	{ 0x3d568d84, "class_create" },
	{ 0xa2e1228b, "device_create" },
	{ 0x88b4fdc1, "device_destroy" },
	{ 0xfbc10eaa, "class_destroy" },
	{ 0x6ce3748e, "cdev_del" },
	{ 0x0bc5fb0d, "unregister_chrdev_region" },
	{ 0xd272d446, "__fentry__" },
	{ 0x70eca2ca, "module_layout" },
};

static const u32 ____version_ext_crcs[]
__used __section("__version_ext_crcs") = {
	0xa61fd7aa,
	0x092a35a2,
	0xe8213e80,
	0xd272d446,
	0x90a48d82,
	0xd272d446,
	0x092a35a2,
	0x9f222e1e,
	0xb06a91bd,
	0x7db91808,
	0x3d568d84,
	0xa2e1228b,
	0x88b4fdc1,
	0xfbc10eaa,
	0x6ce3748e,
	0x0bc5fb0d,
	0xd272d446,
	0x70eca2ca,
};
static const char ____version_ext_names[]
__used __section("__version_ext_names") =
	"__check_object_size\0"
	"_copy_from_user\0"
	"_printk\0"
	"__x86_return_thunk\0"
	"__ubsan_handle_out_of_bounds\0"
	"__stack_chk_fail\0"
	"_copy_to_user\0"
	"alloc_chrdev_region\0"
	"cdev_init\0"
	"cdev_add\0"
	"class_create\0"
	"device_create\0"
	"device_destroy\0"
	"class_destroy\0"
	"cdev_del\0"
	"unregister_chrdev_region\0"
	"__fentry__\0"
	"module_layout\0"
;

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "8DD53526C7A63685784AACE");
