#include "main.h"

file_operations_t g_fops = { .owner          = THIS_MODULE,
                             .open           = file_operation_open,
                             .release        = file_operation_release,
                             .unlocked_ioctl = file_operation_ioctl };
static bool g_bOpenDevice       = false;

mm_t* mm_access(task_t* task, unsigned int mode)
{
    mm_t* mm;

    typedef mm_t* (*mm_access_t)(task_t*, unsigned int);

    static mm_access_t p_mm_access = NULL;

    if (p_mm_access == NULL)
    {
        p_mm_access = (mm_access_t)kallsyms_lookup_name("mm_access");
    }

    if (p_mm_access == NULL)
    {
        c_printk_error("couldn't find mm_access\n");
        return NULL;
    }

    mm = p_mm_access(task, mode);

    return mm;
}

int check_permissions(task_t* task)
{
    mm_t* mm;

    // This is used for process_vm_readv/process_vm_writev calls.
    mm = mm_access(task, PTRACE_MODE_ATTACH_REALCREDS);

    // Do not allow access to process that can't attach to others
    // processes.
    if (mm == NULL || IS_ERR(mm))
    {
        return -EACCES;
    }

    // Do not forget to release mm,
    // Otherwhise the current task's file binary will get always busy.
    mmput(mm);

    return 0;
}

int file_operation_open(inode_t* i, file_t* f)
{
    int ret;
    c_printk_info("pid %i open %s\n",
                  get_current()->pid,
                  DEVICE_FILE_NAME);

    ret = check_permissions(get_current());

    if (g_bOpenDevice)
    {
        return -EACCES;
    }

    g_bOpenDevice = true;

    return ret;
}

int file_operation_release(inode_t* i, file_t* f)
{
    int ret;
    c_printk_info("pid %i release %s\n",
                  get_current()->pid,
                  DEVICE_FILE_NAME);

    ret = check_permissions(get_current());

    if (!g_bOpenDevice)
    {
        return -EACCES;
    }

    g_bOpenDevice = false;

    return ret;
}

long file_operation_ioctl(file_t* f, unsigned int n, unsigned long p)
{
    communicate_error_t error    = COMMUNICATE_ERROR_NONE;
    communicate_cmd_t cmd_number = (communicate_cmd_t)n;

    if (check_permissions(get_current()) < 0)
    {
        error = COMMUNICATE_ERROR_ACCESS_DENIED;
        goto out;
    }

    c_printk_info("pid %i ioctl %s with cmd %i\n",
                  get_current()->pid,
                  DEVICE_FILE_NAME,
                  cmd_number);

    switch (cmd_number)
    {
        case COMMUNICATE_CMD_READ:
        {
            error = communicate_process_cmd_read(get_current(), p);
            break;
        }
        case COMMUNICATE_CMD_WRITE:
        {
            error = communicate_process_cmd_write(get_current(), p);
            break;
        }
        case COMMUNICATE_CMD_REMOTE_MMAP:
        {
            error = communicate_process_cmd_remote_mmap(get_current(), p);
            break;
        }
        case COMMUNICATE_CMD_REMOTE_MUNMAP:
        {
            error = communicate_process_cmd_remote_munmap(get_current(),
                                                          p);
            break;
        }
        case COMMUNICATE_CMD_REMOTE_CLONE:
        {
            error = communicate_process_cmd_remote_clone(get_current(),
                                                         p);
            break;
        }
        default:
        {
            c_printk_error("communicate error, unknown command %i\n",
                           cmd_number);
            error = COMMUNICATE_ERROR_UNKNOWN_CMD;
            break;
        }
    }

out:

    return error;
}
