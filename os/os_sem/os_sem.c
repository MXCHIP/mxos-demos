/**
 ******************************************************************************
 * @file    os_sem.c
 * @author  William Xu
 * @version V1.0.0
 * @date    21-May-2015
 * @brief   MiCO RTOS thread control demo.
 ******************************************************************************
 *
 *  The MIT License
 *  Copyright (c) 2016 MXCHIP Inc.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is furnished
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in
 *  all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 *  IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 ******************************************************************************
 */

#include "mxos.h"

#define os_sem_log(M, ...) custom_log("OS", M, ##__VA_ARGS__)

static mos_semphr_id_t os_sem = NULL;

void release_thread(void *arg)
{
    UNUSED_PARAMETER(arg);

    while (1)
    {
        os_sem_log("release semaphore");
        mos_semphr_release(os_sem);
        mos_sleep(1);
    }

    mos_thread_delete(NULL);
}

int main(void)
{
    merr_t err = kNoErr;
    int semphr_fd = -1;
    mos_thread_id_t thread;

    os_sem_log("test binary semaphore");

    os_sem = mos_semphr_new(1); //0/1 binary semaphore || 0/N semaphore
    require(os_sem != NULL, exit);

    thread = mos_thread_new(MOS_APPLICATION_PRIORITY, "release sem", release_thread, 0x500, NULL);
    require(thread != NULL, exit);

    semphr_fd = mos_event_fd_new(os_sem);
    fd_set readfds;
    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(semphr_fd, &readfds);
        os_sem_log("selecting ...");
        select(24, &readfds, NULL, NULL, NULL);
        os_sem_log("selected");

        if (FD_ISSET(semphr_fd, &readfds))
        {
            mos_semphr_acquire(os_sem, MOS_WAIT_FOREVER); //wait until get semaphore
            os_sem_log("get semaphore");
        }
    }

exit:
    if (err != kNoErr)
        os_sem_log("Thread exit with err: %d", err);

    if (os_sem != NULL)
    {
        mos_semphr_delete(os_sem);
    }

    if (semphr_fd != -1)
    {
        mos_event_fd_delete(semphr_fd);
    }

    return err;
}
