/*
 * capall.c
 * $Id$
 *
 * Demo kernel module. Gives a process all capabilities and resets the
 * cap bound to -1. jammod helps you to regain "real" superuser
 * rights.
 *
 * (c) 2003-2004 Max Kellermann (max@linuxtag.org)
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>

int pid = 0;

extern kernel_cap_t cap_bset;

int init_module(void);
int init_module(void) {
    struct task_struct *task;

    cap_bset = -1;

    if (pid == 0) {
        task = current->parent;

        if (task == NULL) {
            printk(KERN_ERR "capall: current process has no parent\n");
            return -EINVAL;
        }
    } else {
        task = find_task_by_pid(pid);

        if (task == NULL) {
            printk(KERN_ERR "capall: pid %u not found\n",
                   pid);
            return -EINVAL;
        }
    }

    task->cap_effective = -1;
    task->cap_inheritable = -1;
    task->cap_permitted = -1;
    task->keep_capabilities = 1;
    
    return -1;
}
