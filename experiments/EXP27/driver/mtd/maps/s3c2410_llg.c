/*
 * Normal mappings of chips on Samsung s3c2410 in physical memory
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <asm/io.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/config.h>

#define WINDOW_ADDR 0x01000000 //»ù
#define WINDOW_SIZE 0x01600000   //flash´ó16M
#define BUSWIDTH 2

static struct mtd_info *mymtd;

__u8 s3c2410_read8(struct map_info *map, unsigned long ofs)
{
        return readb(map->map_priv_1 + ofs);
}

__u16 s3c2410_read16(struct map_info *map, unsigned long ofs)
{
        return readw(map->map_priv_1 + ofs);
               }

__u32 s3c2410_read32(struct map_info *map, unsigned long ofs)
{
        return readl(map->map_priv_1 + ofs);
}

void s3c2410_copy_from(struct map_info *map, void *to, unsigned long from, ssize_t len)
{
        memcpy(to, (void *)(map->map_priv_1 + from), len);
}

void s3c2410_write8(struct map_info *map, __u8 d, unsigned long adr)
{
        writeb(d, map->map_priv_1 + adr);
}

void s3c2410_write16(struct map_info *map, __u16 d, unsigned long adr)
{
        writew(d, map->map_priv_1 + adr);
}
void s3c2410_write32(struct map_info *map, __u32 d, unsigned long adr)
{
        writel(d, map->map_priv_1 + adr);
}

void s3c2410_copy_to(struct map_info *map, unsigned long to, const void *from, ssize_t len)
{
        memcpy((void *)(map->map_priv_1 + to), from, len);
}

struct map_info s3c2410_map = {
        name: "s3c2410 flash device",
        size: WINDOW_SIZE,
        buswidth: BUSWIDTH,
        read8: s3c2410_read8,
        read16: s3c2410_read16,
        read32: s3c2410_read32,
        copy_from: s3c2410_copy_from,
        write8: s3c2410_write8,
        write16: s3c2410_write16,
             write32: s3c2410_write32,
        copy_to: s3c2410_copy_to,

        map_priv_1:     WINDOW_ADDR,
        map_priv_2:     -1,
};
//ÒÏÊ·ÖøÈ£¬µ±ȻҪ¸ù×¼ºµÄèȷ¶¨Á
static struct mtd_partition s3c2410_partitions[] = {
    {
            name: "reserved for bootloader",
            size: 0x040000,
            offset: 0x0,
                        mask_flags: MTD_WRITEABLE,
    },
    {
            name: "reserved for kernel",
            size: 0x0100000,
            offset: 0x040000,
                        mask_flags: MTD_WRITEABLE,
    },
    {
            name: "reserved for ramdisk",
 	    size: 0x400000,
            offset: 0x140000,
                        mask_flags: MTD_WRITEABLE,

    },
	{
                name: "jffs2(8M)",
                size: 0x700000,
                offset: 0x800000,
        },
	{
		name: "cramfs(2.75M)",
		size:0x2c0000,
		offset:0x540000,
	}

};

int __init init_s3c2410(void)
{
    printk(KERN_NOTICE "s3c2410 flash device: %x at %x\n", WINDOW_SIZE, WINDOW_ADDR);
        s3c2410_map.map_priv_1 = (unsigned long)ioremap(WINDOW_ADDR, WINDOW_SIZE);
        //printk("0\n");
        if (!s3c2410_map.map_priv_1) {
                printk("Failed to ioremap/n");
                return -EIO;
                         }
        //printk("1\n");
        mymtd = do_map_probe("jedec_probe", &s3c2410_map);
        if (!mymtd)
        mymtd = do_map_probe("cfi_probe", &s3c2410_map);
        //printk("2\n");
        if (mymtd) {
                mymtd->module = THIS_MODULE;
                mymtd->erasesize = 0x20000; //²sý¡ INTEL E28F128J3A-150 Ê128kb
                return add_mtd_partitions(mymtd, s3c2410_partitions, sizeof(s3c2410_partitions) / sizeof(struct mtd_partition));
        }
        //printk("3\n");
        iounmap((void *)s3c2410_map.map_priv_1);
        return -ENXIO;
}

static void __exit cleanup_s3c2410(void)
{
        if (mymtd) {
                del_mtd_partitions(mymtd);
                  map_destroy(mymtd);
        }
        if (s3c2410_map.map_priv_1) {
                iounmap((void *)s3c2410_map.map_priv_1);
                s3c2410_map.map_priv_1 = 0;
        }
}

module_init(init_s3c2410);
module_exit(cleanup_s3c2410);
 
