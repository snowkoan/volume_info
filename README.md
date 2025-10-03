# volume_info
What volume information is available during the [Filter Manager instance setup callback](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/fltkernel/nc-fltkernel-pflt_instance_setup_callback)?

Originally forked from @zoadiacon: https://github.com/zodiacon/windowskernelprogrammingbook2e/tree/master/Chapter12/KBackup2

# building
1. This should build against WDK 10.0.22621.0 and later. I'm not sure about earlier versions.
2. [ktl](https://github.com/snowkoan/ktl) is a submodule so you will have problems if you don't initialize it somehow. The easiest way is likely: `git clone https://github.com/snowkoan/kernel_copy_detect.git --recurse-submodules`

# installing & running
Do not run this on your dev machine. It's kernel code of the POC variety. Here's what I do:

1. Copy all binaries to a test VM with [test signing enabled](https://learn.microsoft.com/en-us/windows-hardware/drivers/install/the-testsigning-boot-configuration-option).
2. To install, just create a service entry for the driver: `sc create volume_info type= kernel binPath="%cd%\aamini.sys" start= demand`
3. Run `fltmc load volume_info`. This will set up the necessary minifilter registry values etc (thanks @zodiacon)
4. Run `aaminiexe.exe` to see messages from the driver when volumes are mounted.

ex:

```
C:\temp\aamini>aaminiexe
Connected to port \com.snowkoan.volume_info.port. Waiting for messages.

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Volume detached: \Device\HarddiskVolume9

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

Skipping volume (unsupported filesystem 1 or device type 8)

--- New Volume Info: \Device\HarddiskVolume13 ---
FileSystemDriverName: \FileSystem\Ntfs (DeviceName: \Ntfs)
FileSystemType: 2 (NTFS)
DeviceType: 0x00000008 (DISK_FILE_SYSTEM)
Characteristics: 0x00060001 (FILE_REMOVABLE_MEDIA | FILE_DEVICE_ALLOW_APPCONTAINER_TRAVERSAL | FILE_PORTABLE_DEVICE)
Storage Bus Type : 7 (USB), Removable Media? Yes
```

To uninstall:
1. Run `sc delete volume_info`

# testing
So far, I've only played with this on Windows 1124H2.

