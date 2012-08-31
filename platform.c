/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "platform.h"

/* {{{ type definitions */

typedef struct {
	const char *key;
	cl_platform_info name;
} phpcl_platform_info_param_t;

/* }}} */
/* {{{ globals */

static const phpcl_info_param_t platform_info_params[] = {
	{ "profile",    CL_PLATFORM_PROFILE,    INFO_TYPE_STRING },
	{ "version",    CL_PLATFORM_VERSION,    INFO_TYPE_STRING },
	{ "name",       CL_PLATFORM_NAME,       INFO_TYPE_STRING },
	{ "vendor",     CL_PLATFORM_VENDOR,     INFO_TYPE_STRING },
	{ "extensions", CL_PLATFORM_EXTENSIONS, INFO_TYPE_STRING },
	{ NULL, 0 }
};

/* }}} */
/* {{{ phpcl_get_platform_info() */

static cl_int phpcl_get_platform_info(cl_platform_id platform,
                                      void *reserved __attribute__ ((unused)),
                                      cl_platform_info name,
                                      size_t value_size,
                                      void *value,
                                      size_t *value_size_ret)
{
	return clGetPlatformInfo(platform, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ array cl_get_platform_ids(void) */

PHP_FUNCTION(cl_get_platform_ids)
{
	cl_int err = CL_SUCCESS;
	cl_uint num_entries = 0;
	cl_platform_id *platforms = NULL;
	cl_uint index = 0;

	RETVAL_FALSE;

	if (ZEND_NUM_ARGS() != 0) {
		WRONG_PARAM_COUNT;
	}

	err = clGetPlatformIDs(0, NULL, &num_entries);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetPlatformIDs() failed [%s]", phpcl_errstr(err));
	}

	platforms = ecalloc(num_entries, sizeof(cl_platform_id));
	if (!platforms) {
		return;
	}

	err = clGetPlatformIDs(num_entries, platforms, NULL);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetPlatformIDs() failed [%s]", phpcl_errstr(err));
		efree(platforms);
		return;
	}

	array_init_size(return_value, num_entries);

	for (index = 0; index < num_entries; index++) {
		cl_platform_id platform = platforms[index];
		zval *entry;
		MAKE_STD_ZVAL(entry);
		ZEND_REGISTER_RESOURCE(entry, platform, phpcl_le_platform());
		add_next_index_zval(return_value, entry);
	}

	efree(platforms);
}

/* }}} */
/* {{{ array cl_get_platform_info(resource cl_platform_id platform) */

PHP_FUNCTION(cl_get_platform_info)
{
	const phpcl_info_param_t *param = platform_info_params;
	zval *zid = NULL;
	cl_platform_id platform = NULL;
	char buf[64] = {0};

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r", &zid) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(platform, cl_platform_id, &zid, -1,
	                    "cl_platform_id", phpcl_le_platform());

	array_init_size(return_value, 8);
	snprintf(buf, sizeof(buf), "%p", platform);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(
			(phpcl_get_info_func_t)phpcl_get_platform_info, NULL,
			platform, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
