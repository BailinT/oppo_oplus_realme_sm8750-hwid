// SPDX-License-Identifier: GPL-2.0-only
#include <linux/bootconfig.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/panic.h>
#include <linux/string.h>

#include "hwid_allowlist.h"

extern char boot_command_line[];

static const char *const hwid_keys[] __initconst = {
	"androidboot.chipid",
	"oplusboot.serialno",
};

static const char *__init hwid_from_cmdline(const char *key, char *value,
					     size_t value_size)
{
	const char *cursor = boot_command_line;
	size_t key_len = strlen(key);

	while ((cursor = strstr(cursor, key)) != NULL) {
		const char *start;
		size_t len;

		if (cursor != boot_command_line && cursor[-1] != ' ') {
			cursor += key_len;
			continue;
		}

		start = cursor + key_len;
		if (*start != '=') {
			cursor += key_len;
			continue;
		}

		start++;
		if (*start == '"')
			start++;
		len = strcspn(start, " \"");
		if (!len || len >= value_size)
			return NULL;

		memcpy(value, start, len);
		value[len] = '\0';
		return value;
	}

	return NULL;
}

/* Read one key; the caller checks every available key against the allowlist. */
static const char *__init hwid_read_key(const char *key, char *value,
					 size_t value_size)
{
#ifdef CONFIG_BOOT_CONFIG
	struct xbc_node *value_node;
	const char *bootconfig_value = xbc_find_value(key, &value_node);

	if (bootconfig_value && *bootconfig_value) {
		if (strscpy(value, bootconfig_value, value_size) < 0)
			return NULL;
		return value;
	}
#endif
	return hwid_from_cmdline(key, value, value_size);
}

static bool __init hwid_allowed(const char *hwid)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(hwid_allowlist); i++) {
		if (!strcasecmp(hwid, hwid_allowlist[i]))
			return true;
	}

	return false;
}

void __init hwid_lock_verify(void)
{
	char value[HWID_LOCK_MAX_ID_LEN];
	bool found_any = false;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(hwid_keys); i++) {
		const char *detected = hwid_read_key(hwid_keys[i], value,
						     sizeof(value));

		if (!detected)
			continue;
		found_any = true;
		if (hwid_allowed(detected)) {
			pr_info("HWID lock: device authorized via %s\n", hwid_keys[i]);
			return;
		}
	}

	panic_timeout = 1;
	if (!found_any)
		panic("HWID lock: androidboot.chipid/oplusboot.serialno is missing");
	panic("HWID lock: this kernel is not authorized for this device");
}
