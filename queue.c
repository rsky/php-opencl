/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "queue.h"

#define get_info ((phpcl_get_info_func_t)_get_command_queue_info)
#define get_info_ex NULL

/* {{{ globals */

static const phpcl_info_param_t command_queue_info_params[] = {
	{ NULL, 0, 0 }
};

/* }}} */
/* {{{ _get_command_queue_info() */

static cl_int _get_command_queue_info(cl_command_queue command_queue,
                                      void *reserved __attribute__ ((unused)),
                                      cl_command_queue_info name,
                                      size_t value_size,
                                      void *value,
                                      size_t *value_size_ret)
{
	return clGetCommandQueueInfo(command_queue, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ _get_command_queue_info_all() */

static void _get_command_queue_info_all(INTERNAL_FUNCTION_PARAMETERS,
	cl_command_queue command_queue)
{
	const phpcl_info_param_t *param = command_queue_info_params;
	char buf[32] = {0};

	array_init_size(return_value, 8);
	snprintf(buf, sizeof(buf), "%p", command_queue);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(get_info, get_info_ex,
		                             command_queue, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */
/* {{{ _get_command_queue_info_by_name() */

static void _get_command_queue_info_by_name(INTERNAL_FUNCTION_PARAMETERS,
	cl_command_queue command_queue, cl_int name)
{
	const phpcl_info_param_t *param = command_queue_info_params;

	RETVAL_NULL();

	while (param->key != NULL) {
		if (param->name == name) {
			zval *entry = phpcl_get_info(get_info, get_info_ex,
			                             command_queue, NULL, param TSRMLS_CC);
			if (entry) {
				RETVAL_ZVAL(entry, 0, 1);
			}
			return;
		}
		param++;
	}
}

/* }}} */
/* {{{ mixed cl_get_command_queue_info(resource cl_command_queue command_queue[, int name]) */

PHP_FUNCTION(cl_get_command_queue_info)
{
	zval *zid = NULL;
	cl_command_queue command_queue = NULL;
	long name = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|l", &zid, &name) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(command_queue, cl_command_queue, &zid, -1,
	                    "cl_command_queue", phpcl_le_command_queue());

	if (ZEND_NUM_ARGS() == 2) {
		_get_command_queue_info_by_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, command_queue, (cl_int)name);
	} else {
		_get_command_queue_info_all(INTERNAL_FUNCTION_PARAM_PASSTHRU, command_queue);
	}
}

/* }}} */
/* {{{ resource cl_command_queue cl_create_command_queue(resource cl_context context[, resource cl_device device[, array properties[, int &errcode]]]) */

PHP_FUNCTION(cl_create_command_queue)
{
	cl_int errcode = CL_SUCCESS;
	cl_command_queue command_queue = NULL;
	zval *zcontext = NULL;
	phpcl_context_t *ctx = NULL;
	zval *zdevice = NULL;
	cl_device_id device = NULL;
	long properties = 0;
	zval *zerrcode = NULL;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|r!lz", &zcontext, &zdevice,
	                          &properties, &zerrcode) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(ctx, phpcl_context_t *, &zcontext, -1,
	                    "cl_context", phpcl_le_context());

	if (zdevice) {
		ZEND_FETCH_RESOURCE(device, cl_device_id, &zdevice, -1,
		                    "cl_device", phpcl_le_device());
	} else {
		cl_device_id *devices = phpcl_context_get_devices(ctx->context, NULL, NULL);
		device = devices[0];
		efree(devices);
	}

	command_queue = clCreateCommandQueue(ctx->context, device,
	                                     (cl_command_queue_properties)properties,
	                                     &errcode);

	if (zerrcode) {
		zval_dtor(zerrcode);
		ZVAL_LONG(zerrcode, (long)errcode);
	}

	if (command_queue) {
		ZEND_REGISTER_RESOURCE(return_value, command_queue, phpcl_le_command_queue());
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
