/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "event.h"
#include "params.h"

#define get_info ((phpcl_get_info_func_t)_get_event_info)
#define get_info_ex NULL

/* {{{ globals */

static const phpcl_info_param_t event_info_params[] = {
	{ NULL, 0 }
};

/* }}} */
/* {{{ _get_event_info() */

static cl_int _get_event_info(cl_event event,
                              void *reserved __attribute__ ((unused)),
                              cl_event_info name,
                              size_t value_size,
                              void *value,
                              size_t *value_size_ret)
{
	return clGetEventInfo(event, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ _get_event_info_all() */

static void _get_event_info_all(
	INTERNAL_FUNCTION_PARAMETERS,
	cl_event event)
{
	const phpcl_info_param_t *param = event_info_params;
	char buf[32] = {0};

	array_init_size(return_value, 8);
	snprintf(buf, sizeof(buf), "%p", event);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(get_info, get_info_ex,
		                             event, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */
/* {{{ _get_event_info_by_name() */

static void _get_event_info_by_name(
	INTERNAL_FUNCTION_PARAMETERS,
	cl_event event, cl_int name)
{
	const phpcl_info_param_t *param = event_info_params;

	RETVAL_NULL();

	while (param->key != NULL) {
		if (param->name == name) {
			zval *entry = phpcl_get_info(get_info, get_info_ex,
			                             event, NULL, param TSRMLS_CC);
			if (entry) {
				RETVAL_ZVAL(entry, 0, 1);
			}
			return;
		}
		param++;
	}
}

/* }}} */
/* {{{ mixed cl_get_event_info(resource cl_event event[, int name]) */

PHPCL_FUNCTION(cl_get_event_info)
{
	zval *zid = NULL;
	cl_event event = NULL;
	long name = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|l", &zid, &name) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(event, cl_event, &zid, -1,
	                    "cl_event", phpcl_le_event());

	if (ZEND_NUM_ARGS() == 2) {
		_get_event_info_by_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, event, (cl_int)name);
	} else {
		_get_event_info_all(INTERNAL_FUNCTION_PARAM_PASSTHRU, event);
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
