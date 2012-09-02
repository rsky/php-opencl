/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "kernel.h"
#include "params.h"

#define get_info ((phpcl_get_info_func_t)_get_kernel_info)
#define get_info_ex NULL

/* {{{ globals */

static const phpcl_info_param_t kernel_info_params[] = {
	{ NULL, 0 }
};

/* }}} */
/* {{{ _get_kernel_info() */

static cl_int _get_kernel_info(cl_kernel kernel,
                               void *reserved __attribute__ ((unused)),
                               cl_kernel_info name,
                               size_t value_size,
                               void *value,
                               size_t *value_size_ret)
{
	return clGetKernelInfo(kernel, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ _get_kernel_info_all() */

static void _get_kernel_info_all(
	INTERNAL_FUNCTION_PARAMETERS,
	cl_kernel kernel)
{
	const phpcl_info_param_t *param = kernel_info_params;
	char buf[32] = {0};

	array_init_size(return_value, 8);
	snprintf(buf, sizeof(buf), "%p", kernel);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(get_info, get_info_ex,
		                             kernel, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */
/* {{{ _get_kernel_info_by_name() */

static void _get_kernel_info_by_name(
	INTERNAL_FUNCTION_PARAMETERS,
	cl_kernel kernel, cl_int name)
{
	const phpcl_info_param_t *param = kernel_info_params;

	RETVAL_NULL();

	while (param->key != NULL) {
		if (param->name == name) {
			zval *entry = phpcl_get_info(get_info, get_info_ex,
			                             kernel, NULL, param TSRMLS_CC);
			if (entry) {
				RETVAL_ZVAL(entry, 0, 1);
			}
			return;
		}
		param++;
	}
}

/* }}} */
/* {{{ mixed cl_get_kernel_info(resource cl_kernel kernel[, int name]) */

PHP_FUNCTION(cl_get_kernel_info)
{
	zval *zid = NULL;
	cl_kernel kernel = NULL;
	long name = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|l", &zid, &name) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(kernel, cl_kernel, &zid, -1,
	                    "cl_kernel", phpcl_le_kernel());

	if (ZEND_NUM_ARGS() == 2) {
		_get_kernel_info_by_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, kernel, (cl_int)name);
	} else {
		_get_kernel_info_all(INTERNAL_FUNCTION_PARAM_PASSTHRU, kernel);
	}
}

/* }}} */
/* {{{ resource cl_kernel cl_get_kernel_info(resource cl_program program, string name) */

PHP_FUNCTION(cl_create_kernel)
{
	cl_int errcode = CL_SUCCESS;
	zval *zprogram = NULL;
	phpcl_program_t *prg = NULL;
	char *kernel_name = NULL;
	int kernel_name_len = 0;
	cl_kernel kernel = NULL;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "rs", &zprogram, &kernel_name,
	                          &kernel_name_len) == FAILURE) {
		return;
	}

	ZEND_FETCH_RESOURCE(prg, phpcl_program_t *, &zprogram, -1,
	                    "cl_kernel", phpcl_le_program());

	kernel = clCreateKernel(prg->program, kernel_name, &errcode);
	if (kernel) {
		ZEND_REGISTER_RESOURCE(return_value, kernel, phpcl_le_kernel());
	}
	if (errcode != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
		                 "%s", phpcl_errstr(errcode));
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
