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

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
