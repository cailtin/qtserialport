/*
    License...
*/

#include "serialportinfo.h"
#include "serialportinfo_p.h"

#include <sys/param.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>

#include <IOKit/serial/IOSerialKeys.h>
#if defined(MAC_OS_X_VERSION_10_3) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_3)
#include <IOKit/serial/ioss.h>
#endif
#include <IOKit/IOBSD.h>

#include <QtCore/QVariant>
#include <QtCore/QStringList>


/* Public methods */


QList<SerialPortInfo> SerialPortInfo::availablePorts()
{
    QList<SerialPortInfo> ports;

    CFMutableDictionaryRef classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    if (!classesToMatch)
        return ports;

    CFDictionaryAddValue(classesToMatch,
                         CFSTR(kIOSerialBSDTypeKey),
                         CFSTR(kIOSerialBSDAllTypes));

    io_iterator_t serialPortIterator = 0;
    kern_return_t kernResult = IOServiceGetMatchingServices(kIOMasterPortDefault,
                                                            classesToMatch,
                                                            &serialPortIterator);
    if (kernResult == KERN_SUCCESS) {

        io_object_t serialService;
        while ((serialService = IOIteratorNext(serialPortIterator))) {

            SerialPortInfo info;

            //Device name
            CFTypeRef nameRef = IORegistryEntryCreateCFProperty(serialService,
                                                                CFSTR(kIOCalloutDeviceKey),
                                                                kCFAllocatorDefault,
                                                                0);
            bool result = false;
            QByteArray bsdPath(MAXPATHLEN, 0);
            if (nameRef) {
                result = CFStringGetCString(CFStringRef(nameRef), bsdPath.data(), bsdPath.size(),
                                            kCFStringEncodingUTF8);
                CFRelease(nameRef);
            }

            // If device name (path) found.
            if (result) {

                io_registry_entry_t parent;
                kernResult = IORegistryEntryGetParentEntry(serialService, kIOServicePlane, &parent);

                CFTypeRef descriptionRef = 0;
                CFTypeRef manufacturerRef = 0;

                //
                while ((kernResult == KERN_SUCCESS)) {

                    // Description ref
                    if (!descriptionRef)
                        descriptionRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                         CFSTR("USB Product Name"),
                                                                         kCFAllocatorDefault, 0);
                    // Manufacturer ref
                    if (!manufacturerRef)
                        manufacturerRef = IORegistryEntrySearchCFProperty(parent, kIOServicePlane,
                                                                          CFSTR("USB Vendor Name"),
                                                                          kCFAllocatorDefault, 0);

                    ////// Next info
                    //.....

                    io_registry_entry_t currParent = parent;
                    kernResult = IORegistryEntryGetParentEntry(currParent, kIOServicePlane, &parent);
                    IOObjectRelease(currParent);
                }

                QByteArray stringValue(MAXPATHLEN, 0);

                //name
                info.d_ptr->portName = QString(bsdPath); // FIXME: With regexp, remove /dev/
                //location
                info.d_ptr->device = QString(bsdPath);

                //description
                if (descriptionRef) {
                    if (CFStringGetCString(CFStringRef(descriptionRef),
                                           stringValue.data(), stringValue.size(),
                                           kCFStringEncodingUTF8)) {

                        info.d_ptr->description = QString(stringValue);

                    }
                    CFRelease(descriptionRef);
                }

                //manufacturer
                if (manufacturerRef) {
                    if (CFStringGetCString(CFStringRef(manufacturerRef),
                                           stringValue.data(), stringValue.size(),
                                           kCFStringEncodingUTF8)) {

                        info.d_ptr->manufacturer = QString(stringValue);
                    }
                    CFRelease(manufacturerRef);
                }


                ports.append(info);
            }
            (void) IOObjectRelease(serialService);
        }
    }

    // Here delete classetToMach ?

    return ports;
}

QList<int> SerialPortInfo::standardRates() const
{
    QList<int> rates;

    /*
      MacX implementation detect supported rates list
      or append standart Unix rates.
    */

    return rates;
}

bool SerialPortInfo::isBusy() const
{
    // Try open and close port by location: systemLocation().
    //...
    //...

    return false;
}

bool SerialPortInfo::isValid() const
{
    // Impl me
    return false;
}