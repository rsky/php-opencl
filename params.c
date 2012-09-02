/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "params.h"

/* {{{ phpcl_is_callable() */

zend_bool phpcl_is_callable(zval *zv, int argno TSRMLS_DC)
{
	if (!zend_is_callable(zv, 0, NULL TSRMLS_CC)) {
		if (argno > 0) {
			php_error(E_WARNING,
				"%s() expects parameter %d to be a valid callback",
				get_active_function_name(TSRMLS_C), argno);
		}
		return 0;
	}

	return 1;
}

/* }}} */
/* {{{ phpcl_get_devicecs() */

cl_device_id *phpcl_get_devicecs(zval *zv, int argno, cl_uint *num_devices_ret TSRMLS_DC)
{
	cl_device_id *devices = NULL;
	cl_uint num_devices = 0;

	if (Z_TYPE_P(zv) == IS_RESOURCE) {
		cl_device_id device = NULL;
		ZEND_FETCH_RESOURCE_NO_RETURN(device, cl_device_id, &zv, -1,
		                              "cl_device", phpcl_le_device());
		if (device) {
			devices = emalloc(sizeof(cl_device_id));
			devices[0] = device;
			num_devices = 1;
		}
	} else {
		/* TODO: support multiple devices */
		if (argno > 0) {
			php_error(E_WARNING,
				"%s() expects parameter %d to be a valid resource",
				get_active_function_name(TSRMLS_C), argno);
		}
	}

	if (num_devices_ret) {
		*num_devices_ret = num_devices;
	}
	return devices;
}

/* }}} */
/* {{{ phpcl_convert_zval_to_c() */

zend_bool phpcl_convert_zval_to_c(zval *zv, phpcl_c_type_t type,
	void **value_ptr_ret, size_t *size_ret TSRMLS_DC)
{
	/* not yet implemented */
	return 0;
}

/* }}} */
/* {{{ phpcl_convert_c_to_zval() */

zval *phpcl_convert_c_to_zval(void *value_ptr, phpcl_c_type_t type TSRMLS_DC)
{
	/* not yet implemented */
	return NULL;
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
