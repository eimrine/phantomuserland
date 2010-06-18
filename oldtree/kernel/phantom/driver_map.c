#define DEBUG_MSG_PREFIX "driver"
#include "debug_ext.h"
#define debug_level_flow 1
#define debug_level_error 10
#define debug_level_info 10

#include "config.h"

#include <phantom_libc.h>

#include "i386/pci.h"
#include "driver_map.h"
#include "device.h"

#include "misc.h"

#include <virtio_blk.h>
#include <virtio_net.h>
#include <virtio_rng.h>

#define SPECIAL_VIRTIO_MAP 0

// very simple drivers to devices mapping structure

typedef struct
{
    const char *name;
    phantom_device_t * (*probe_f)( pci_cfg_t *pci, int stage );        // Driver probe func
    int minstage; // How early we can be called
    int vendor; // PCI Vendor or 0 to ignore (must have device == 0 too)
    int device; // PCI device ID or 0 to ignore
    int dclass; // PCI device class or 0 to ignore (must have dev == 0)

} pci_probe_t;

#define VIRTIO_VENDOR 0x1AF4

// NB! No network drivers on stage 0!
static pci_probe_t pci_drivers[] =
{
    //    { "VirtIO Disk", 	driver_virtio_disk_probe, 	2, VIRTIO_VENDOR, 0, 1 },
    //    { "VirtIO Baloon", driver_virtio_baloon_probe, 2, VIRTIO_VENDOR, 0, 5 },
    //    { "VirtIO Random",  driver_virtio_random_probe, 2, 0x1AF4, 0, 1 }, // TODO dev/dclass?

#if 1 && HAVE_NET
    { "VirtIO Net",  	driver_virtio_net_probe, 	1, 0x1AF4, 0x1000, 0 }, // TODO dev/dclass?
    { "AMD PcNet",   	driver_pcnet_pchome_probe, 	1, AMD_VENDORID, PCNET_DEVICEID, 0 },
    { "AMD PcHome",  	driver_pcnet_pchome_probe, 	1, AMD_VENDORID, PCHOME_DEVICEID, 0 },
    { "RTL 8139", 	driver_rtl_8139_probe, 		1, RTL8139_VENDORID, RTL8139_DEVICEID, 0 },
    { "Intel i82559er", driver_intel_82559_probe, 	3, INTEL_VENDORID, 0x1209, 0 },
#endif // HAVE_NET
    { "Ne2000 PCI", 	driver_pci_ne2000_probe, 	1, 0x10ec, 0x8029, 0 },

#if COMPILE_OHCI
    { "USB OHCI",       driver_ohci_probe, 		3, 0, 0, OHCI_BASE_CLASS },
#endif

    { "USB UHCI",       driver_uhci_probe, 		3, 0, 0, UHCI_BASE_CLASS },

    // Chipset drivers, etc

    // Do not work, seem to be uninited by BIOS - no IO port defined
    //{ "PIC/Mem 1237", 	driver_intel_1237_bridge_probe,	3, INTEL_VENDORID, 0x1237, 0 },
    //{ "PIIX4 PM", 	driver_intel_PIIX4_pm_probe,	3, INTEL_VENDORID, 0x7113, 0 },

};


#if SPECIAL_VIRTIO_MAP
// This one for virtio

typedef struct
{
    const char *name;
    phantom_device_t * (*probe_f)( pci_cfg_t *pci, int stage );        // Driver probe func
    int minstage; // How early we can be called
    int subdev; // Subsystem dev id

} virtio_probe_t;

// NB! No network drivers on stage 0!
static virtio_probe_t virtio_drivers[] =
{
    { "VirtIO Disk", driver_virtio_disk_probe, 2, VIRTIO_ID_BLOCK },
    { "VirtIO Net",  driver_virtio_net_probe, 2, VIRTIO_ID_NET }, 
    { "VirtIO Random",  driver_virtio_random_probe, 2, VIRTIO_ID_RNG }, 

};
#endif


typedef struct
{
    const char *name;
    phantom_device_t * (*probe_f)( int port, int irq, int stage );        // Driver probe func
    int minstage; // How early we can be called
    int port; // can be zero, driver supposed to find itself
    int irq; // can be zero, driver supposed to find itself
} isa_probe_t;

// NB! No network drivers on stage 0!
static isa_probe_t isa_drivers[] =
{
    { "LPT1", 		driver_isa_lpt_probe, 	2, 0x378, 7 },
    { "LPT2", 		driver_isa_lpt_probe, 	2, 0x278, 5 },

    { "COM1",		driver_isa_com_probe,   2, 0x3F8, 4 },
    { "COM2",		driver_isa_com_probe,   2, 0x2F8, 3 },
    { "COM3",		driver_isa_com_probe,   2, 0x3E8, 4 },
    { "COM4",		driver_isa_com_probe,   2, 0x2E8, 3 },

    { "CGA", 		driver_isa_vga_probe, 	0, 0x3D4, -1 },
    { "MDA", 		driver_isa_vga_probe, 	0, 0x3B4, -1 },

    { "PS2 Keyboard", 	driver_isa_ps2k_probe, 	1, -1, 1 },
    { "PS2 Mouse", 	driver_isa_ps2m_probe, 	1, -1, 12 },

    { "Beep",           driver_isa_beep_probe,  0, 0x42, -1 },

#if HAVE_NET
    { "NE2000", 	driver_isa_ne2000_probe,1, 0x280, 11 },
    { "NE2000", 	driver_isa_ne2000_probe,1, 0x300, 11 },
    { "NE2000", 	driver_isa_ne2000_probe,1, 0x320, 11 },
    { "NE2000", 	driver_isa_ne2000_probe,1, 0x340, 11 },
    { "NE2000", 	driver_isa_ne2000_probe,1, 0x360, 11 },
#endif

//    { "SB16",         driver_isa_sb16_probe,  3, 0x220, 5 },
};



typedef struct
{
    const char *name;
    phantom_device_t * (*probe_f)( const char *name, int stage );        // Driver probe func
    int minstage; // How early we can be called

} etc_probe_t;

// NB! No network drivers on stage 0!
static etc_probe_t etc_drivers[] =
{
    { "SMBios", 		driver_etc_smbios_probe, 	0 },

};






typedef struct
{
    pci_cfg_t   pci;
    int 	filled; // not empty rec
    int 	used; // already found a driver
} pci_record_t;




static void show_pci( pci_cfg_t* pci )
{
    printf("PCI %d.%d: %s (%s) %X:%X class %X:%X (%s) if %X\n",
           //pci->bus,
           pci->dev, pci->func,
           get_pci_device( pci->vendor_id, pci->device_id ),
           get_pci_vendor(pci->vendor_id), 
           pci->vendor_id, pci->device_id,
           pci->base_class, pci->sub_class, get_pci_class(pci->base_class, pci->sub_class),
           pci->interface
          );

}



#define MAXPCI 512
static pci_record_t allpci[MAXPCI];
#if SPECIAL_VIRTIO_MAP
static pci_record_t virtio_pci[MAXPCI];
#endif
static int load_once = 0;

static int loadpci()
{
    int bus,dev,func;
    int pci_slot = 0;

#if SPECIAL_VIRTIO_MAP
    int virtio_slot = 0;
#endif

    if(load_once) return 0;
    load_once = 1;

    SHOW_FLOW0( 1, "Loading PCI config:");

    // TODO we need to count buses ourself and
    // not to check all the world each time
    for(bus=0;bus<255;bus++){
        for(dev=0;dev<32;dev++) {

            if(pci_slot >= MAXPCI)
            {
                SHOW_ERROR0( 1, "Warning: too much PCI devs, some are missed");
                return 0;
            }

            if(phantom_pci_probe(bus,dev,0, &allpci[pci_slot].pci ))
                continue;

            show_pci(&allpci[pci_slot].pci);

            allpci[pci_slot].used = 0;
            allpci[pci_slot].filled = 1;

            int is_multifunc = allpci[pci_slot].pci.header_type & 0x80;

            pci_slot++;

            if(is_multifunc)
            {
                for( func = 1; func < 8; func++ )
                {
                    if(!phantom_pci_probe(bus,dev,func, &allpci[pci_slot].pci ) )
                    {
                        show_pci(&allpci[pci_slot].pci);

#if SPECIAL_VIRTIO_MAP
                        if( allpci[pci_slot].pci.vendor_id == 0x1AF4 )
                        {
                            virtio_pci[virtio_slot].pci = allpci[pci_slot].pci;
                            virtio_pci[virtio_slot].filled = 1;
                            virtio_pci[virtio_slot].used = 0;
                            virtio_slot++;
                        }
                        else
#endif
                        {
                            allpci[pci_slot].used = 0;
                            allpci[pci_slot].filled = 1;
                            pci_slot++;
                        }
                    }
                }
            }
        }
    }
//getchar();
    return 0;
}


static int probe_pci( int stage, pci_cfg_t *pci )
{

    SHOW_FLOW( 2, "%d PCI check vend %X dev %X cl %X", stage, pci->vendor_id, pci->device_id, pci->base_class );

               unsigned int i;
    for( i = 0; i < sizeof(pci_drivers)/sizeof(pci_probe_t); i++ )
    {
        pci_probe_t *dp = &pci_drivers[i];
        SHOW_FLOW( 3, "-- against %X : %X cl %X stg %d", dp->vendor, dp->device, dp->dclass, dp->minstage );
        if( stage < dp->minstage )
            continue;
        if( dp->device && pci->device_id != dp->device )
            continue;
        if( dp->vendor && pci->vendor_id != dp->vendor )
            continue;
        if( dp->dclass && pci->base_class != dp->dclass )
            continue;

        SHOW_FLOW( 1, "Probing driver '%s' for PCI dev %d:%d (%s:%s %X:%X class %X:%X)",
                   dp->name, pci->dev, pci->func,
                   get_pci_vendor(pci->vendor_id), get_pci_device( pci->vendor_id, pci->device_id ),
                   pci->vendor_id, pci->device_id,
                   pci->base_class, pci->sub_class
                  );
        phantom_device_t *dev = dp->probe_f( pci, stage );

        if( dev != 0 )
        {
            SHOW_INFO( 0, "Driver '%s' attached to PCI dev %d:%d (%s:%s)",
                   dp->name, pci->dev, pci->func,
                   get_pci_vendor(pci->vendor_id), get_pci_device( pci->vendor_id, pci->device_id )
                  );

            phantom_bus_add_dev( &pci_bus, dev );

            return 1;
        }
        else
            SHOW_ERROR0( 1, "Failed");
    }
//getchar();
    return 0;
}


#if SPECIAL_VIRTIO_MAP
static int probe_virtio( int stage, pci_cfg_t *pci )
{
    printf("VirtIO look for driver for PCI dev %d:%d (%s:%s %X:%X class %X:%X)\n",
                   pci->dev, pci->func,
                   get_pci_vendor(pci->vendor_id), get_pci_device( pci->vendor_id, pci->device_id ),
                   pci->vendor_id, pci->device_id,
                   pci->base_class, pci->sub_class
                  );


    int i;
    for( i = 0; i < sizeof(virtio_drivers)/sizeof(virtio_probe_t); i++ )
    {
        virtio_probe_t *dp = &virtio_drivers[i];

        if( stage < dp->minstage )
            continue;
        if( pci->device_id != dp->subdev )
            continue;

        phantom_device_t *dev = dp->probe_f( pci, stage );

        if( dev != 0 )
        {
            printf("VirtIO driver '%s' attached to PCI dev %d:%d (%s:%s %X:%X class %X:%X)\n",
                   dp->name, pci->dev, pci->func,
                   get_pci_vendor(pci->vendor_id), get_pci_device( pci->vendor_id, pci->device_id ),
                   pci->vendor_id, pci->device_id,
                   pci->base_class, pci->sub_class
                  );

            phantom_bus_add_dev( &pci_bus, dev );

            return 1;
        }
    }

    return 0;
}
#endif


static int probe_isa( int stage )
{
    unsigned int i;
    for( i = 0; i < sizeof(isa_drivers)/sizeof(isa_probe_t); i++ )
    {
        isa_probe_t *dp = &isa_drivers[i];

        SHOW_FLOW( 2, "check %s stage %d minstage %d", dp->name, stage, dp->minstage );
        if( stage < dp->minstage )
            continue;

        SHOW_FLOW( 2, "probe %s @ 0x%x", dp->name, dp->port );

        phantom_device_t *dev = dp->probe_f( dp->port, dp->irq, stage );

        if( dev != 0 )
        {
            SHOW_INFO( 0, "Driver '%s' attached to ISA at 0x%X (IRQ %d)",
                       dp->name, dp->port, dp->irq );
            phantom_bus_add_dev( &isa_bus, dev );
        }
        dp->minstage = 1000; // prevent it from being processed next time
    }

    return 0;
}



static int probe_etc( int stage )
{
    unsigned int i;
    for( i = 0; i < sizeof(etc_drivers)/sizeof(etc_probe_t); i++ )
    {
        etc_probe_t *dp = &etc_drivers[i];

        if( stage < dp->minstage )
            continue;

        phantom_device_t *dev = dp->probe_f( dp->name, stage );

        if( dev != 0 )
        {
            SHOW_INFO( 0, "Driver '%s' attached", dp->name );
            phantom_bus_add_dev( &etc_bus, dev );
            dp->minstage = ~0; // prevent it from being processed next time
        }
    }

    return 0;
}


// Stage is:
//   0 - very early in the boot - interrupts can be used only
//   1 - boot, most of kernel infrastructure is there
//   2 - disks which Phantom will live in must be found here
//   3 - late and optional and slow junk

int phantom_pci_find_drivers( int stage )
{
    if(loadpci())
    {
        SHOW_ERROR0( 0, "Can not load PCI devices table" );
        return -1;
    }
    SHOW_FLOW( 0, "Look for PCI devices, stage %d", stage );

    int i;

    for(i = 0; i <= MAXPCI; i++ )
    {
        if( (!allpci[i].filled) || allpci[i].used )
            continue;

        if( probe_pci(stage, &allpci[i].pci ) )
            allpci[i].used = 1;
    }
    SHOW_FLOW( 2, "Finished looking for PCI devices, stage %d", stage );


#if SPECIAL_VIRTIO_MAP
    printf("Look for VirtIO PCI devices, stage %d\n", stage );
    for(i = 0; i <= MAXPCI; i++ )
    {
        if( !(virtio_pci[i].filled) || virtio_pci[i].used )
            continue;

        if( probe_virtio(stage, &allpci[i].pci ) )
            virtio_pci[i].used = 1;
    }
//getchar();
    printf("Finished looking for VirtIO PCI devices, stage %d", stage );
#endif

    probe_isa( stage );
    SHOW_FLOW( 2, "Finished looking for ISA devices, stage %d", stage );
    //getchar();
    probe_etc( stage );
    SHOW_FLOW( 2, "Finished looking for other devices, stage %d", stage );

    return -1;
}



