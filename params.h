/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "php_opencl.h"

#ifndef PHPCL_PARAMS_H
#define PHPCL_PARAMS_H

zend_bool phpcl_is_callable(zval *zv, int argno TSRMLS_DC);

cl_device_id *phpcl_get_devicecs(zval *zv, int argno,
	cl_uint *num_devices_ret TSRMLS_DC);

zend_bool phpcl_convert_zval_to_c(zval *zv, phpcl_c_type_t type,
	void **value_ptr_ret, size_t *size_ret TSRMLS_DC);

zval *phpcl_convert_c_to_zval(void *value_ptr, phpcl_c_type_t type TSRMLS_DC);

#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
