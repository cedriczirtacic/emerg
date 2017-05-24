/*
 * Copyright (c) 2017, cedric zirtacic (https://github.com/cedriczirtacic)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Gibson nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/keyboard.h>
#include <linux/notifier.h>
#include <linux/version.h>

/* parameters */
static char *phrase;
static char *exec;
module_param(phrase, charp, S_IRUGO);
module_param(exec, charp, S_IRUGO);

/* module vars */
static int matches = 0;
static char *env[] = {
    "USER=root",
    "HOME=/",
    "TERM=linux",
    "SHELL=/bin/bash",
    "PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin",
    NULL
};

static int shell_exec(char *e) {
    printk(KERN_INFO "executing %s", e);
    matches = 0; //start all over again
    
    struct subprocess_info *info;
    char *argv[] = {e, NULL};

#if LINUX_VERSION_CODE > KERNEL_VERSION(4, 0, 0)
    info = call_usermodehelper_setup(argv[0], argv, env, GFP_ATOMIC, NULL, NULL, NULL);
#elif
    info = call_usermodehelper_setup(argv[0], argv, env, GFP_ATOMIC);
#endif
    if (info == NULL){
        printk(KERN_ERR "error! *info struct is NULL");
        return -ENOMEM;
    }
    //lets execute
    return call_usermodehelper_exec(info, UMH_NO_WAIT);
}

int _kb_call(struct notifier_block *nb, unsigned long code, void *p) {
    struct keyboard_notifier_param *params = p;

    // we've got some key pressed but not pressure
    if (code == KBD_POST_KEYSYM && !params->down) {
        char p_char = params->value^0xfb00; // clean
        
        if (p_char == *(phrase+matches)) {
            if (++matches == strlen(phrase)) // all the keys matched
                shell_exec(exec);
            return NOTIFY_OK;
        }else
            // character mismatch, start again
            matches = 0;
    }
    
    return NOTIFY_OK;
}

static struct notifier_block nblock = {
    .notifier_call = _kb_call
};

static int __init emerg_init(void) {
    printk(KERN_INFO "emerg loaded...");

    if (phrase == NULL || exec == NULL)
        return -EINVAL;

    register_keyboard_notifier(&nblock);
    return 0;
}

static void __exit emerg_exit(void) {
    unregister_keyboard_notifier(&nblock);
}

module_init( emerg_init );
module_exit( emerg_exit );
MODULE_LICENSE("GPL");

