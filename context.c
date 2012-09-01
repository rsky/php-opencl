/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "context.h"

#define get_info ((phpcl_get_info_func_t)_get_context_info)
#define get_info_ex NULL

/* {{{ type definitions */

typedef void (*create_context_callback_func_t)(const char *, const void *, size_t, void *);

/* }}} */
/* {{{ globals */

static const phpcl_info_param_t context_info_params[] = {
	{ NULL, 0, 0 }
};

/* }}} */
/* {{{ _get_context_info() */

static cl_int _get_context_info(cl_context context,
                                void *reserved __attribute__ ((unused)),
                                cl_context_info name,
                                size_t value_size,
                                void *value,
                                size_t *value_size_ret)
{
	return clGetContextInfo(context, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ _get_context_info_all() */

static void _get_context_info_all(INTERNAL_FUNCTION_PARAMETERS,
	cl_context context)
{
	const phpcl_info_param_t *param = context_info_params;
	char buf[32] = {0};

	array_init_size(return_value, 8);
	snprintf(buf, sizeof(buf), "%p", context);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(get_info, get_info_ex,
		                             context, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */
/* {{{ _get_context_info_by_name() */

static void _get_context_info_by_name(INTERNAL_FUNCTION_PARAMETERS,
	cl_context context, cl_int name)
{
	const phpcl_info_param_t *param = context_info_params;

	RETVAL_NULL();

	while (param->key != NULL) {
		if (param->name == name) {
			zval *entry = phpcl_get_info(get_info, get_info_ex,
			                             context, NULL, param TSRMLS_CC);
			if (entry) {
				RETVAL_ZVAL(entry, 0, 1);
			}
			return;
		}
		param++;
	}
}

/* }}} */
/* {{{ mixed cl_get_context_info(resource cl_context context[, int name]) */

PHP_FUNCTION(cl_get_context_info)
{
	zval *zid = NULL;
	cl_context context = NULL;
	long name = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|l", &zid, &name) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(context, cl_context, &zid, -1,
	                    "cl_context", phpcl_le_context());

	if (ZEND_NUM_ARGS() == 2) {
		_get_context_info_by_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, context, (cl_int)name);
	} else {
		_get_context_info_all(INTERNAL_FUNCTION_PARAM_PASSTHRU, context);
	}
}

/* }}} */
/* {{{ resource cl_context cl_create_context(mixed device[, array properties[, callback callback[, mixed user_data[, int &errcode]]]] */

PHP_FUNCTION(cl_create_context)
{
	cl_int errcode = CL_SUCCESS;
	cl_context context = NULL;
	zval *zdevice = NULL;
	cl_device_id *devices_ptr = NULL;
	cl_device_id devices[1] = {NULL};
	cl_uint num_devices = 0;
	HashTable *properties_ht = NULL;
	cl_context_properties *properties = NULL;
	zval *zcallback = NULL;
	create_context_callback_func_t pfn_notify = NULL;
	zval *zdata = NULL;
	void *user_data = NULL;
	zval *zerrcode = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "z/|h!z/z/z", &zdevice, &properties_ht,
	                          &zcallback, &zdata, &zerrcode) == FAILURE) {
		return;
	}

	if (Z_TYPE_P(zdevice) == IS_RESOURCE) {
		cl_device_id device;
		ZEND_FETCH_RESOURCE(device, cl_device_id, &zdevice, -1,
		                    "cl_device", phpcl_le_device());
		devices[0] = device;
		devices_ptr = devices;
		num_devices = 1;
	} else {
		/* TODO: support multiple devices */
		php_error(E_WARNING,
			"%s() expects parameter 1 to be a valid resource",
			get_active_function_name(TSRMLS_C));
		return;
	}

	if (properties_ht) {
		/* TODO: support properties */
	}

	if (zcallback) {
		if (!zend_is_callable(zcallback, 0, NULL TSRMLS_CC)) {
			php_error(E_WARNING,
				"%s() expects parameter 3 to be a valid callback",
				get_active_function_name(TSRMLS_C));
			return;
		}
	}

	context = clCreateContext(properties, num_devices, devices_ptr,
	                          pfn_notify, user_data, &errcode);

	if (zerrcode) {
		zval_dtor(zerrcode);
		ZVAL_LONG(zerrcode, (long)errcode);
	}

	if (context) {
		phpcl_context_t *ctx = emalloc(sizeof(phpcl_context_t));
		ctx->context = context;
		if (zcallback) {
			Z_ADDREF_P(zcallback);
			ctx->callback = zcallback;
			if (zdata) {
				Z_ADDREF_P(zdata);
				ctx->data = zdata;
			}
		}
		ZEND_REGISTER_RESOURCE(return_value, ctx, phpcl_le_context());
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
