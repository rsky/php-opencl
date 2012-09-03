/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "sampler.h"
#include "params.h"

#define get_info ((phpcl_get_info_func_t)_get_sampler_info)
#define get_info_ex NULL

/* {{{ globals */

static const phpcl_info_param_t sampler_info_params[] = {
	{ NULL, 0, 0 }
};

/* }}} */
/* {{{ _get_sampler_info() */

static cl_int _get_sampler_info(cl_sampler sampler,
                                void *reserved __attribute__ ((unused)),
                                cl_sampler_info name,
                                size_t value_size,
                                void *value,
                                size_t *value_size_ret)
{
	return clGetSamplerInfo(sampler, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ _get_sampler_info_all() */

static void _get_sampler_info_all(
	INTERNAL_FUNCTION_PARAMETERS,
	cl_sampler sampler)
{
	const phpcl_info_param_t *param = sampler_info_params;
	char buf[32] = {0};

	array_init_size(return_value, 8);
	snprintf(buf, sizeof(buf), "%p", sampler);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(get_info, get_info_ex,
		                             sampler, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */
/* {{{ _get_sampler_info_by_name() */

static void _get_sampler_info_by_name(
	INTERNAL_FUNCTION_PARAMETERS,
	cl_sampler sampler, cl_int name)
{
	const phpcl_info_param_t *param = sampler_info_params;

	RETVAL_NULL();

	while (param->key != NULL) {
		if (param->name == name) {
			zval *entry = phpcl_get_info(get_info, get_info_ex,
			                             sampler, NULL, param TSRMLS_CC);
			if (entry) {
				RETVAL_ZVAL(entry, 0, 1);
			}
			return;
		}
		param++;
	}
}

/* }}} */
/* {{{ mixed cl_get_sampler_info(resource cl_sampler sampler[, int name]) */

PHPCL_FUNCTION(cl_get_sampler_info)
{
	zval *zid = NULL;
	cl_sampler sampler = NULL;
	long name = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|l", &zid, &name) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(sampler, cl_sampler, &zid, -1,
	                    "cl_sampler", phpcl_le_sampler());

	if (ZEND_NUM_ARGS() == 2) {
		_get_sampler_info_by_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, sampler, (cl_int)name);
	} else {
		_get_sampler_info_all(INTERNAL_FUNCTION_PARAM_PASSTHRU, sampler);
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
