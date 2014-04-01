#ifndef HASHDEV_IOCTL_H
#define HASHDEV_IOCTL_H

#ifdef __KERNEL__
#include <linux/kernel.h>
#else
#include <stdint.h>
#endif

#include <linux/ioctl.h>

struct hashdev_ioctl_get_result {
	uint8_t hash[20];
};
#define HASHDEV_IOCTL_GET_RESULT _IOR('C', 0x00, struct hashdev_ioctl_get_result)

#endif
