/**
 * The OpenCL PHP extension
 *
 * @package     php-opencl
 * @author      Ryusuke SEKIYAMA <rsky0711@gmail.com>
 * @copyright   2012 Ryusuke SEKIYAMA
 * @license     http://www.opensource.org/licenses/mit-license.php  MIT License
 */

#include "device.h"

#define get_info_func ((phpcl_get_info_func_t)phpcl_get_device_info)
#define get_info_ex_func ((phpcl_get_info_ex_func_t)phpcl_get_device_info_ex)

/* {{{ globals */

static const phpcl_info_param_t device_info_params[] = {
	{ "type",                          CL_DEVICE_TYPE,                          INFO_TYPE_BITFIELD  },
	{ "vendor_id",                     CL_DEVICE_VENDOR_ID,                     INFO_TYPE_UINT      },
	{ "max_compute_units",             CL_DEVICE_MAX_COMPUTE_UNITS,             INFO_TYPE_UINT      },
	{ "max_work_item_dimensions",      CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,      INFO_TYPE_UINT      },
	{ "max_work_group_size",           CL_DEVICE_MAX_WORK_GROUP_SIZE,           INFO_TYPE_SIZE      },
	{ "max_work_item_sizes",           CL_DEVICE_MAX_WORK_ITEM_SIZES,           INFO_TYPE_CALLBACK  },
	{ "preferred_vector_width_char",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR,   INFO_TYPE_SIZE      },
	{ "preferred_vector_width_short",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,  INFO_TYPE_SIZE      },
	{ "preferred_vector_width_int",    CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT,    INFO_TYPE_SIZE      },
	{ "preferred_vector_width_long",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG,   INFO_TYPE_SIZE      },
	{ "preferred_vector_width_float",  CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,  INFO_TYPE_SIZE      },
	{ "preferred_vector_width_double", CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, INFO_TYPE_SIZE      },
#ifdef CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF /* OpenCL 1.1 */
	{ "preferred_vector_width_half",   CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF,   INFO_TYPE_UINT      },
#endif
#ifdef CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR /* OpenCL 1.1 */
	{ "native_vector_width_char",      CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR,      INFO_TYPE_UINT      },
	{ "native_vector_width_short",     CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT,     INFO_TYPE_UINT      },
	{ "native_vector_width_int",       CL_DEVICE_NATIVE_VECTOR_WIDTH_INT,       INFO_TYPE_UINT      },
	{ "native_vector_width_long",      CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG,      INFO_TYPE_UINT      },
	{ "native_vector_width_float",     CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT,     INFO_TYPE_UINT      },
	{ "native_vector_width_double",    CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE,    INFO_TYPE_UINT      },
	{ "native_vector_width_half",      CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF,      INFO_TYPE_UINT      },
#endif
	{ "max_clock_frequency",           CL_DEVICE_MAX_CLOCK_FREQUENCY,           INFO_TYPE_UINT      },
	{ "address_bits",                  CL_DEVICE_ADDRESS_BITS,                  INFO_TYPE_UINT      },
	{ "max_read_image_args",           CL_DEVICE_MAX_READ_IMAGE_ARGS,           INFO_TYPE_UINT      },
	{ "max_write_image_args",          CL_DEVICE_MAX_WRITE_IMAGE_ARGS,          INFO_TYPE_UINT      },
	{ "max_mem_alloc_size",            CL_DEVICE_MAX_MEM_ALLOC_SIZE,            INFO_TYPE_ULONG     },
	{ "image2d_max_width",             CL_DEVICE_IMAGE2D_MAX_WIDTH,             INFO_TYPE_SIZE      },
	{ "image2d_max_height",            CL_DEVICE_IMAGE2D_MAX_HEIGHT,            INFO_TYPE_SIZE      },
	{ "image3d_max_width",             CL_DEVICE_IMAGE3D_MAX_WIDTH,             INFO_TYPE_SIZE      },
	{ "image3d_max_height",            CL_DEVICE_IMAGE3D_MAX_HEIGHT,            INFO_TYPE_SIZE      },
	{ "image3d_max_depth",             CL_DEVICE_IMAGE3D_MAX_DEPTH,             INFO_TYPE_SIZE      },
	{ "image_support",                 CL_DEVICE_IMAGE_SUPPORT,                 INFO_TYPE_BOOL      },
	{ "max_parameter_size",            CL_DEVICE_MAX_PARAMETER_SIZE,            INFO_TYPE_SIZE      },
	{ "max_samplers",                  CL_DEVICE_MAX_SAMPLERS,                  INFO_TYPE_UINT      },
	{ "mem_base_addr_align",           CL_DEVICE_MEM_BASE_ADDR_ALIGN,           INFO_TYPE_UINT      },
	{ "min_data_type_align_size",      CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE,      INFO_TYPE_UINT      },
	{ "single_fp_config",              CL_DEVICE_SINGLE_FP_CONFIG,              INFO_TYPE_BITFIELD  },
	{ "global_mem_cache_type",         CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,         INFO_TYPE_UINT      },
	{ "global_mem_cacheline_size",     CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE,     INFO_TYPE_UINT      },
	{ "global_mem_cache_size",         CL_DEVICE_GLOBAL_MEM_CACHE_SIZE,         INFO_TYPE_ULONG     },
	{ "global_mem_size",               CL_DEVICE_GLOBAL_MEM_SIZE,               INFO_TYPE_ULONG     },
	{ "max_constant_buffer_size",      CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE,      INFO_TYPE_ULONG     },
	{ "max_constant_args",             CL_DEVICE_MAX_CONSTANT_ARGS,             INFO_TYPE_UINT      },
	{ "local_mem_type",                CL_DEVICE_LOCAL_MEM_TYPE,                INFO_TYPE_UINT      },
	{ "local_mem_size",                CL_DEVICE_LOCAL_MEM_SIZE,                INFO_TYPE_ULONG     },
	{ "error_correction_support",      CL_DEVICE_ERROR_CORRECTION_SUPPORT,      INFO_TYPE_BOOL      },
	{ "profiling_timer_resolution",    CL_DEVICE_PROFILING_TIMER_RESOLUTION,    INFO_TYPE_SIZE      },
	{ "endian_little",                 CL_DEVICE_ENDIAN_LITTLE,                 INFO_TYPE_BOOL      },
	{ "available",                     CL_DEVICE_AVAILABLE,                     INFO_TYPE_BOOL      },
	{ "compiler_available",            CL_DEVICE_COMPILER_AVAILABLE,            INFO_TYPE_BOOL      },
	{ "execution_capabilities",        CL_DEVICE_EXECUTION_CAPABILITIES,        INFO_TYPE_BITFIELD  },
	{ "queue_properties",              CL_DEVICE_QUEUE_PROPERTIES,              INFO_TYPE_BITFIELD  },
	{ "name",                          CL_DEVICE_NAME,                          INFO_TYPE_STRING    },
	{ "vendor",                        CL_DEVICE_VENDOR,                        INFO_TYPE_STRING    },
	{ "driver_version",                CL_DRIVER_VERSION,                       INFO_TYPE_STRING    },
	{ "profile",                       CL_DEVICE_PROFILE,                       INFO_TYPE_STRING    },
	{ "version",                       CL_DEVICE_VERSION,                       INFO_TYPE_STRING    },
	{ "extensions",                    CL_DEVICE_EXTENSIONS,                    INFO_TYPE_STRING    },
	{ "platform",                      CL_DEVICE_PLATFORM,                      INFO_TYPE_PLATFORM  },
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
	{ "double_fp_config",              CL_DEVICE_DOUBLE_FP_CONFIG,              INFO_TYPE_BITFIELD  },
#endif
#ifdef CL_DEVICE_HALF_FP_CONFIG
	{ "half_fp_config",                CL_DEVICE_HALF_FP_CONFIG,                INFO_TYPE_BITFIELD  },
#endif
#ifdef CL_DEVICE_HOST_UNIFIED_MEMORY
	{ "host_unified_memory",           CL_DEVICE_HOST_UNIFIED_MEMORY,           INFO_TYPE_BOOL      },
#endif
#ifdef CL_DEVICE_OPENCL_C_VERSION
	{ "opencl_c_version",              CL_DEVICE_OPENCL_C_VERSION,              INFO_TYPE_STRING    },
#endif
	{ NULL, 0, 0 }
};

/* }}} */
/* {{{ phpcl_get_device_info() */

static cl_int phpcl_get_device_info(cl_device_id device,
                                    void *reserved __attribute__ ((unused)),
                                    cl_device_info name,
                                    size_t value_size,
                                    void *value,
                                    size_t *value_size_ret)
{
	return clGetDeviceInfo(device, name, value_size, value, value_size_ret);
}

/* }}} */
/* {{{ phpcl_get_device_info_ex() */

static zval *phpcl_get_device_info_ex(cl_device_id device,
                                      void *reserved,
                                      cl_device_info name TSRMLS_DC)
{
	cl_int err = CL_SUCCESS;
	zval *zinfo;
	MAKE_STD_ZVAL(zinfo);

	if (name == CL_DEVICE_MAX_WORK_ITEM_SIZES) {
		cl_uint max_work_item_dimensions = 0;
		size_t siz, *sizes;

		err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
		                      sizeof(cl_uint), &max_work_item_dimensions, NULL);
		if (err != CL_SUCCESS) {
			return zinfo;
		}

		siz = sizeof(size_t) * max_work_item_dimensions;
		sizes = ecalloc(max_work_item_dimensions, sizeof(size_t));
		if (!sizes) {
			return zinfo;
		}

		err = clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_ITEM_SIZES, siz, sizes, NULL);
		if (err == CL_SUCCESS) {
			cl_uint i;
			array_init_size(zinfo, max_work_item_dimensions);
			for (i = 0; i < max_work_item_dimensions; i++) {
				long lval = (long)sizes[i];
				add_next_index_long(zinfo, lval);
			}
		}

		efree(sizes);
	}

	return zinfo;
}

/* }}} */
/* {{{ phpcl_get_device_info_all() */

static void phpcl_get_device_info_all(INTERNAL_FUNCTION_PARAMETERS,
	cl_device_id device)
{
	const phpcl_info_param_t *param = device_info_params;
	char buf[32] = {0};

	array_init_size(return_value, 64);
	snprintf(buf, sizeof(buf), "%p", device);
	add_assoc_string(return_value, "id", buf, 1);

	while (param->key != NULL) {
		zval *entry = phpcl_get_info(get_info_func, get_info_ex_func,
		                             device, NULL, param TSRMLS_CC);
		if (entry) {
			add_assoc_zval(return_value, param->key, entry);
		} else {
			add_assoc_null(return_value, param->key);
		}
		param++;
	}
}

/* }}} */
/* {{{ phpcl_get_device_info_by_name() */

static void phpcl_get_device_info_by_name(INTERNAL_FUNCTION_PARAMETERS,
	cl_device_id device, cl_int name)
{
	const phpcl_info_param_t *param = device_info_params;

	RETVAL_NULL();

	while (param->key != NULL) {
		if (param->name == name) {
			zval *entry = phpcl_get_info(get_info_func, get_info_ex_func,
			                             device, NULL, param TSRMLS_CC);
			if (entry) {
				RETVAL_ZVAL(entry, 0, 1);
			}
			return;
		}
		param++;
	}
}

/* }}} */
/* {{{ array cl_get_device_ids([resource cl_platform_id platform[, int device_type]]) */

PHP_FUNCTION(cl_get_device_ids)
{
	zval *zid = NULL;
	cl_platform_id platform = NULL;
	long ltype = 0L;
	cl_device_type device_type = CL_DEVICE_TYPE_DEFAULT;

	cl_int err = CL_SUCCESS;
	cl_uint num_entries = 0;
	cl_device_id *devices = NULL;
	cl_uint index = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "|r!l", &zid, &ltype) == FAILURE) {
		return;
	}
	if (zid) {
		ZEND_FETCH_RESOURCE(platform, cl_platform_id, &zid, -1,
		                    "cl_platform_id", phpcl_le_platform());
	}
	if (ZEND_NUM_ARGS() == 2) {
		device_type = (cl_device_type)ltype;
	}

	err = clGetDeviceIDs(platform, device_type, 0, NULL, &num_entries);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetDeviceIDs() failed [%s]", phpcl_errstr(err));
		efree(devices);
		return;
	}

	devices = ecalloc(num_entries, sizeof(cl_device_id));
	if (!devices) {
		return;
	}

	err = clGetDeviceIDs(platform, device_type, num_entries, devices, NULL);
	if (err != CL_SUCCESS) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,
			"clGetDeviceIDs() failed [%s]", phpcl_errstr(err));
		efree(devices);
		return;
	}

	array_init_size(return_value, num_entries);

	for (index = 0; index < num_entries; index++) {
		cl_device_id device = devices[index];
		zval *entry;
		MAKE_STD_ZVAL(entry);
		ZEND_REGISTER_RESOURCE(entry, device, phpcl_le_device());
		add_next_index_zval(return_value, entry);
	}

	efree(devices);
}

/* }}} */
/* {{{ array cl_get_device_info(resource cl_device_id device[, int name]) */

PHP_FUNCTION(cl_get_device_info)
{
	zval *zid = NULL;
	cl_device_id device = NULL;
	long name = 0;

	RETVAL_FALSE;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC,
	                          "r|l", &zid, &name) == FAILURE) {
		return;
	}
	ZEND_FETCH_RESOURCE(device, cl_device_id, &zid, -1,
	                    "cl_device_id", phpcl_le_device());

	if (ZEND_NUM_ARGS() == 2) {
		phpcl_get_device_info_by_name(INTERNAL_FUNCTION_PARAM_PASSTHRU, device, (cl_int)name);
	} else {
		phpcl_get_device_info_all(INTERNAL_FUNCTION_PARAM_PASSTHRU, device);
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
