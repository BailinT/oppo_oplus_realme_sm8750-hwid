# HWID-bound kernel builds

`allowlist.txt` contains every device that may boot the resulting kernel. All
non-comment entries are compiled into the same kernel image. Add one HWID per
line and rebuild after changing the list.

At early boot the kernel reads, in order:

1. `androidboot.chipid`
2. `oplusboot.serialno`

The lookup supports both Android bootconfig and the legacy kernel command
line. A missing value, or a value not present in the allowlist, triggers a
kernel panic with a one-second automatic reboot before Android userspace
starts.

The supplied device is registered as `abbe0d2c`.

Recovery from a mismatch requires restoring a known-good boot image from
fastboot/recovery. Test with an available recovery path and keep the original
boot image before flashing.
