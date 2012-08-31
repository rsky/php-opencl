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

static const phpcl_platform_info_param_t platform_info_list[] = {
	{ "profile",    CL_PLATFORM_PROFILE    },
	{ "version",    CL_PLATFORM_VERSION    },
	{ "name",       CL_PLATFORM_NAME       },
	{ "vendor",     CL_PLATFORM_VENDOR     },
	{ "extensions", CL_PLATFORM_EXTENSIONS },
	{ NULL, 0 }
};

/* }}} */
/* {{{ function prototypes */

static zval *phpcl_get_platform_info(cl_platform_id platform TSRMLS_DC);

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
	zval *zid = NULL;
	cl_platform_id platform = NULL;
	zval *platform_info = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r", &zid) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(platform, cl_platform_id, &zid, -1,
	                    "cl_platform_id", phpcl_le_platform());

	platform_info = phpcl_get_platform_info(platform TSRMLS_CC);
	if (platform_info) {
		RETURN_ZVAL(platform_info, 0, 1);
	}
}

/* }}} */
/* {{{ phpcl_get_platform_info() */

static zval *phpcl_get_platform_info(cl_platform_id platform TSRMLS_DC)
{
	const phpcl_platform_info_param_t *param = platform_info_list;
	cl_int err = CL_SUCCESS;
	char buf[1024] = { 0 };
	size_t len = 0;
	zval *zinfo;

	MAKE_STD_ZVAL(zinfo);
	array_init_size(zinfo, 8);

	snprintf(buf, sizeof(buf), "%p", platform);
	add_assoc_string(zinfo, "id", buf, 1);

	while (param->key != NULL) {
		err = clGetPlatformInfo(platform, param->name, sizeof(buf), buf, &len);
		if (err == CL_SUCCESS) {
			add_assoc_stringl(zinfo, param->key, buf, len, 1);
		} else {
			add_assoc_null(zinfo, param->key);
		}
		param++;
	}

	return zinfo;
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
