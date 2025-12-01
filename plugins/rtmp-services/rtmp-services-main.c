#include <util/text-lookup.h>
#include <util/threading.h>
#include <util/platform.h>
#include <util/dstr.h>
#include <obs-module.h>
#include <file-updater/file-updater.h>

#include "rtmp-format-ver.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE("rtmp-services", "en-US")
MODULE_EXPORT const char *obs_module_description(void)
{
	return "OBS core RTMP services";
}

#if defined(ENABLE_SERVICE_UPDATES)
static const char *RTMP_SERVICES_LOG_STR = "[rtmp-services plugin] ";
static const char *RTMP_SERVICES_URL = (const char *)SERVICES_URL;
#endif

extern struct obs_service_info rtmp_common_service;
extern struct obs_service_info rtmp_custom_service;

static update_info_t *update_info = NULL;
static struct dstr module_name = {0};

const char *get_module_name(void)
{
	return module_name.array;
}

static bool confirm_service_file(void *param, struct file_download_data *file)
{
	if (astrcmpi(file->name, "services.json") == 0) {
		obs_data_t *data;
		int format_version;

		data = obs_data_create_from_json((char *)file->buffer.array);
		if (!data)
			return false;

		format_version = (int)obs_data_get_int(data, "format_version");
		obs_data_release(data);

		if (format_version != RTMP_SERVICES_FORMAT_VERSION)
			return false;
	}

	UNUSED_PARAMETER(param);
	return true;
}

bool obs_module_load(void)
{
	dstr_copy(&module_name, "rtmp-services plugin (libobs ");
	dstr_cat(&module_name, obs_get_version_string());
	dstr_cat(&module_name, ")");

#if defined(ENABLE_SERVICE_UPDATES)
	char *local_dir = obs_module_file("");
	char *cache_dir = obs_module_config_path("");
	char update_url[128];
	snprintf(update_url, sizeof(update_url), "%s/v%d", RTMP_SERVICES_URL, RTMP_SERVICES_FORMAT_VERSION);

	if (cache_dir) {
		update_info = update_info_create(RTMP_SERVICES_LOG_STR, module_name.array, update_url, local_dir,
						 cache_dir, confirm_service_file, NULL);
	}

	bfree(local_dir);
	bfree(cache_dir);
#endif

	obs_register_service(&rtmp_common_service);
	obs_register_service(&rtmp_custom_service);
	return true;
}

void obs_module_unload(void)
{
	update_info_destroy(update_info);
	dstr_free(&module_name);
}
