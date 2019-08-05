#include "libze/libze.h"
#include "system_linux.h"
#include <string.h>

/**
 * @brief Concatenate two strings with a separator
 * @param[in] prefix Prefix string
 * @param[in] separator Separator string
 * @param[in] suffix Suffix string
 * @param[in] buflen Length of buffer
 * @param[out] buf Resulting concatenated string
 * @return Nonzero if the resulting string is longer than the buffer length
 */
int
libze_util_concat(const char *prefix, const char *separator, const char *suffix,
                  size_t buflen, char buf[buflen]) {
    if ((strlcat(buf, prefix, buflen) >= buflen) ||
        (strlcat(buf, separator, buflen) >= buflen) ||
        (strlcat(buf, suffix, buflen) >= buflen)) {
        return -1;
    }

    return 0;
}

/**
 * @brief Cut a string at the last instance of a delimiter
 * @param[in] path String to cut
 * @param[in] buflen Length of buffer
 * @param[out] buf  Prefix before last instance of delimiter
 * @param[in] delimiter Delimiter to cut string at
 * @return Nonzero if buffer is too short, or there is no instance of delimiter
 */
int
libze_util_cut(const char path[static 1], size_t buflen, char buf[buflen], char delimiter) {
    char *slashp = NULL;

    if (strlcpy(buf, path, buflen) >= buflen) {
        return -1;
    }

    /* Get pointer to last instance of '/' */
    if ((slashp = strrchr(buf, delimiter)) == NULL) {
        return -1;
    }

    /* terminate string at '/' */
    *slashp = '\0';
    return 0;
}

/**
 * @brief Given a dataset, return just the portion after the root of boot environments
 * @param[in] root Root of boot environments
 * @param[in] dataset Full dataset to get suffix of
 * @param[in] buflen Length of buffer
 * @param[out] buf Buffer to place suffix in
 * @return Non-zero if there is no parent (path is just the name of the pool),
 *         or if the length of the buffer is exceeded
 */
int
libze_util_suffix_after_string(const char root[static 1], const char dataset[static 1], size_t buflen,
                               char buf[buflen]) {

    if (strlcpy(buf, dataset, buflen) >= buflen) {
        return -1;
    }

    size_t loc = strlen(root)+1;
    if (loc >= buflen) {
        return -1;
    }

    /* get substring after next '/' */
    if (strlcpy(buf, buf+loc, buflen) >= buflen) {
        return -1;
    }

    return 0;
}

/**
 * @brief Given a dataset, get the name of the boot environment
 * @param[in] dataset Dataset to get the boot environment of
 * @param[in] buflen Length of buffer
 * @param[out] buf Buffer to place boot environment in
 * @return Non-zero if the length of the buffer is exceeded,
 *         or if there is no / contained in the data set
 */
int
libze_boot_env_name(const char *dataset, size_t buflen, char buf[buflen]) {
    char *slashp = NULL;

    if (strlcpy(buf, dataset, buflen) >= buflen) {
        return -1;
    }

    /* Get pointer to last instance of '/' */
    if ((slashp = strrchr(buf, '/')) == NULL) {
        return -1;
    }

    /* get substring after last '/' */
    if (strlcpy(buf, slashp+1, buflen) >= buflen) {
        return -1;
    }

    return 0;
}

/**
 * @brief Check if boot environment is active
 * @param[in] lzeh Initialized @p libze_handle
 * @param[in] be_dataset Dataset to check if active
 * @return @p B_TRUE if active else @p B_FALSE
 */
boolean_t
libze_is_active_be(libze_handle *lzeh, const char be_dataset[static 1]) {
    return ((strcmp(lzeh->bootfs, be_dataset) == 0) ? B_TRUE : B_FALSE);
}

/**
 * @brief Check if boot environment is root dataset
 * @param[in] lzeh Initialized @p libze_handle
 * @param[in] be_dataset Dataset to check if root dataset
 * @return @p B_TRUE if active else @p B_FALSE
 */
boolean_t
libze_is_root_be(libze_handle *lzeh, const char be_dataset[static 1]) {
    return ((strcmp(lzeh->rootfs, be_dataset) == 0) ? B_TRUE : B_FALSE);
}

/**
 * @brief Free an nvlist and one level down of it's children
 * @param[in] nvl nvlist to free
 */
void
libze_list_free(nvlist_t *nvl) {
    if (nvl == NULL) {
        return;
    }

    nvpair_t *pair = NULL;
    for (pair = nvlist_next_nvpair(nvl, NULL); pair != NULL;
         pair = nvlist_next_nvpair(nvl, pair)) {
        nvlist_t *ds_props = NULL;
        nvpair_value_nvlist(pair, &ds_props);
        fnvlist_free(ds_props);
    }

    nvlist_free(nvl);
}

/**
 * @brief Get the root dataset
 * @param[in] lzeh Initialized @p libze_handle
 * @return Non-zero on success
 *
 * @pre lzeh != NULL
 */
int
libze_get_root_dataset(libze_handle *lzeh) {
    zfs_handle_t *zh;
    int ret = 0;

    char rootfs[LIBZE_MAXPATHLEN];

    // Make sure type is ZFS
    if (libze_dataset_from_mountpoint("/", LIBZE_MAXPATHLEN, rootfs) != SYSTEM_ERR_SUCCESS) {
        return -1;
    }

    if ((zh = zfs_path_to_zhandle(lzeh->lzh, "/", ZFS_TYPE_FILESYSTEM)) == NULL) {
        return -1;
    }

    if (strlcpy(lzeh->rootfs, zfs_get_name(zh), LIBZE_MAXPATHLEN) >= LIBZE_MAXPATHLEN) {
        ret = -1;
    }

    zfs_close(zh);
    return ret;
}