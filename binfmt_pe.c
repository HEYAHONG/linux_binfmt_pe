#include <linux/module.h>
#include <linux/string.h>
#include <linux/stat.h>
#include <linux/binfmts.h>
#include <linux/init.h>
#include <linux/file.h>
#include <linux/err.h>
#include <linux/fs.h>

#define WINE_PATH "/usr/bin/wine"

static int load_pe(struct linux_binprm *bprm)
{
	struct file *file;
	int retval;

	/* Not ours to exec if we don't start with "MZ". */
	if ((bprm->buf[0] != 'M') || (bprm->buf[1] != 'Z'))
		return -ENOEXEC;

	retval = remove_arg_zero(bprm);
	if (retval)
		return retval;
	retval = copy_string_kernel(bprm->interp, bprm);
	if (retval < 0)
		return retval;
	bprm->argc++;

	retval = copy_string_kernel(WINE_PATH, bprm);
	if (retval)
		return retval;
	bprm->argc++;


	retval = bprm_change_interp(WINE_PATH, bprm);
	if (retval < 0)
		return retval;



	/*
	 * OK, now restart the process with the interpreter's dentry.
	 */
	file = open_exec(WINE_PATH);
	if (IS_ERR(file))
		return PTR_ERR(file);

	bprm->interpreter = file;
	return 0;
}

static struct linux_binfmt pe_format = {
	.module		= THIS_MODULE,
	.load_binary	= load_pe,
};

static int __init init_pe_binfmt(void)
{
	register_binfmt(&pe_format);
	return 0;
}

static void __exit exit_pe_binfmt(void)
{
	unregister_binfmt(&pe_format);
}

core_initcall(init_pe_binfmt);
module_exit(exit_pe_binfmt);
MODULE_LICENSE("MIT");
